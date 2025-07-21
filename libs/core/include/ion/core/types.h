#pragma once

#pragma once
#if defined(__GNUC__) || defined(__clang__)
#  define OAT_NODISCARD(msg) [[nodiscard(msg), gnu::warn_unused_result]]
#elif defined(_MSC_VER)
#  define OAT_NODISCARD(msg) [[nodiscard(msg)]]
#else
#  define OAT_NODISCARD(msg) [[nodiscard]]
#endif