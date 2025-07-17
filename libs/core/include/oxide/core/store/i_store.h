#pragma once

#include <charconv>               // for std::from_chars
#include <cstdint>
#include <filesystem>
#include <expected>
#include <string>
#include <string_view>
#include <vector>
#include <system_error>           // for std::errc

#include <oxide/core/export.h>

namespace oxide::core::store {

/// --------------------------------------------------------------------------
/// Thread-safety: neither IStore nor ITransaction are thread-safe.
/// Concurrent access must be synchronized externally.
/// Keys in path-strings must match `[A-Za-z_][A-Za-z0-9_]*`.
/// No quoting/escaping is supported; invalid segments yield PathSyntax.
/// --------------------------------------------------------------------------

enum class OXIDE_CORE_API Error : uint8_t {
    InvalidHandle,    // raw==0 or generation mismatch
    PathSyntax,       // malformed "[", non-digit index, etc.
    KeyNotFound,      // child key missing
    IndexOutOfRange,  // array index >= size
    TypeMismatch,     // wrong type for op
    IoFailure,        // serialization / filesystem error
};

struct OXIDE_CORE_API StoreHandle {
    uint64_t raw = 0;
    constexpr bool valid() const noexcept { return raw != 0; }
    bool operator==(StoreHandle o) const noexcept { return raw == o.raw; }
};

struct OXIDE_CORE_API FileStoreOptions {
    bool write_mmap  = false;
    bool use_journal = true;
};

struct OXIDE_CORE_API JsonStoreOptions {
    bool write_mmap     = false;
    bool use_journal    = true;
    bool allow_comments = false;
};

struct OXIDE_CORE_API TomlStoreOptions {
    bool write_mmap     = false;
    bool use_journal    = true;
    bool preserve_order = false;
    bool strict_types   = true;
};

class OXIDE_CORE_API IStore {
public:
    virtual ~IStore() = default;
    virtual std::expected<void,Error>
    open(std::filesystem::path const& path) = 0;
    virtual std::expected<void,Error>
    close() = 0;

    virtual std::expected<std::unique_ptr<class ITransaction>,Error>
    begin_transaction() = 0;
};

class OXIDE_CORE_API ITransaction {
public:
    virtual ~ITransaction() noexcept {
        if (!committed_) rollback_impl();
    }

    void rollback() noexcept {
        if (!committed_) {
            rollback_impl();
            committed_ = true;
        }
    }

    /// Indicates why root may be invalid (e.g. IoFailure).
    virtual std::expected<StoreHandle,Error> root() const = 0;

    virtual std::expected<bool,Error>
    get_bool   (StoreHandle h) const = 0;
    virtual std::expected<int64_t,Error>
    get_int    (StoreHandle h) const = 0;
    virtual std::expected<double,Error>
    get_double (StoreHandle h) const = 0;
    virtual std::expected<std::string,Error>
    get_string (StoreHandle h) const = 0;

    virtual std::expected<void,Error>
    set_bool   (StoreHandle h, bool   v) = 0;
    virtual std::expected<void,Error>
    set_int    (StoreHandle h, int64_t v) = 0;
    virtual std::expected<void,Error>
    set_double (StoreHandle h, double  v) = 0;
    virtual std::expected<void,Error>
    set_string (StoreHandle h, std::string_view v) = 0;

    virtual std::expected<StoreHandle,Error>
    make_array  (StoreHandle parent, std::string_view key) = 0;
    virtual std::expected<StoreHandle,Error>
    make_object (StoreHandle parent, std::string_view key) = 0;

    virtual std::expected<void,Error>
    make_bool   (StoreHandle parent, std::string_view key, bool   v) = 0;
    virtual std::expected<void,Error>
    make_int    (StoreHandle parent, std::string_view key, int64_t v) = 0;
    virtual std::expected<void,Error>
    make_double (StoreHandle parent, std::string_view key, double  v) = 0;
    virtual std::expected<void,Error>
    make_string (StoreHandle parent, std::string_view key, std::string_view v) = 0;

    virtual std::expected<void,Error>
    remove        (StoreHandle parent, std::string_view key) = 0;
    virtual std::expected<bool,Error>
    has           (StoreHandle parent, std::string_view key) const = 0;
    virtual std::expected<void,Error>
    erase_element (StoreHandle parent, size_t idx) = 0;
    virtual std::expected<bool,Error>
    has_element   (StoreHandle parent, size_t idx) const = 0;

    virtual std::expected<StoreHandle,Error>
    child   (StoreHandle parent, std::string_view key) const = 0;
    virtual std::expected<StoreHandle,Error>
    element (StoreHandle parent, size_t idx) const = 0;

    std::expected<void,Error> commit() {
        auto e = commit_impl();
        if (e) committed_ = true;
        return e;
    }

    std::expected<StoreHandle,Error>
    navigate(StoreHandle base, std::string_view path) const {
        if (!base.valid()) return std::unexpected(Error::InvalidHandle);
        StoreHandle cur = base;
        size_t i = 0, n = path.size();
        while (i < n) {
            if (path[i] == '.') { ++i; continue; }
            if (path[i] == '[') {
                ++i; size_t start = i;
                while (i < n && path[i] >= '0' && path[i] <= '9') ++i;
                if (i >= n || path[i] != ']') return std::unexpected(Error::PathSyntax);
                uint64_t idx = 0;
                auto [ptr, ec] = std::from_chars(path.data()+start, path.data()+i, idx);
                if (ec == std::errc::invalid_argument)   return std::unexpected(Error::PathSyntax);
                if (ec == std::errc::result_out_of_range) return std::unexpected(Error::IndexOutOfRange);
                auto next = element(cur, idx);
                if (!next) return next;
                cur = *next;
                ++i;
            } else {
                size_t j = i;
                while (j < n && path[j] != '.' && path[j] != '[') ++j;
                auto key = path.substr(i, j - i);
                auto next = child(cur, key);
                if (!next) return next;
                cur = *next;
                i = j;
            }
            if (!cur.valid()) return std::unexpected(Error::KeyNotFound);
        }
        return cur;
    }

    template<typename T>
    std::expected<T,Error>
    get(StoreHandle base, std::string_view path) const {
        auto h = navigate(base, path);
        if (!h) return std::unexpected(h.error());
        if constexpr (std::is_same_v<T,bool>)       return get_bool(*h);
        else if constexpr (std::is_same_v<T,int64_t>) return get_int(*h);
        else if constexpr (std::is_same_v<T,double>)  return get_double(*h);
        else if constexpr (std::is_same_v<T,std::string>) return get_string(*h);
        else static_assert(sizeof(T)==0, "Unsupported get<> type");
    }

private:
    virtual std::expected<void,Error> commit_impl() = 0;
    virtual void                   rollback_impl() noexcept = 0;
    bool committed_ = false;
};

// Factory functions—exact option types only.
OXIDE_CORE_API std::expected<std::unique_ptr<IStore>,Error>
makeJsonFileStore(std::filesystem::path const&, JsonStoreOptions);

OXIDE_CORE_API std::expected<std::unique_ptr<IStore>,Error>
makeTomlFileStore(std::filesystem::path const&, TomlStoreOptions);

OXIDE_CORE_API std::expected<std::unique_ptr<IStore>,Error>
makeSqliteStore(std::filesystem::path const&);

OXIDE_CORE_API std::expected<std::unique_ptr<IStore>,Error>
makeInMemoryStore();

}  // namespace oxide::core::store
