#pragma once

#pragma once
#if defined(__GNUC__) || defined(__clang__)
#  define ION_NODISCARD(msg) nodiscard(msg), gnu::warn_unused_result
#elif defined(_MSC_VER)
#  define ION_NODISCARD(msg) nodiscard(msg)
#else
#  define ION_NODISCARD(msg) nodiscard
#endif