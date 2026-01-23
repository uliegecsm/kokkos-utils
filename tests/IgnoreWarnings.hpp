#ifndef KOKKOS_UTILS_TESTS_IGNOREWARNINGS_HPP
#define KOKKOS_UTILS_TESTS_IGNOREWARNINGS_HPP

#define DO_PRAGMA_(x) _Pragma (#x)

#if defined(__clang__)
    #define PRAGMA_DIAGNOSTIC_POP           _Pragma("clang diagnostic pop")
    #define PRAGMA_DIAGNOSTIC_PUSH          _Pragma("clang diagnostic push")
    #define PRAGMA_DIAGNOSTIC_IGNORED(what) DO_PRAGMA_(clang diagnostic ignored what)
#elif defined(__GNUC__) || defined(__GNUG__)
    #define PRAGMA_DIAGNOSTIC_POP           _Pragma("GCC diagnostic pop")
    #define PRAGMA_DIAGNOSTIC_PUSH          _Pragma("GCC diagnostic push")
    #define PRAGMA_DIAGNOSTIC_IGNORED(what) DO_PRAGMA_(GCC   diagnostic ignored what)
#else
    #error "Unsupported compiler."
#endif

#endif // KOKKOS_UTILS_TESTS_IGNOREWARNINGS_HPP
