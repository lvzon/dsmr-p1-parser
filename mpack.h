/**
 * The MIT License (MIT)
 * 
 * Copyright (c) 2015-2016 Nicholas Fraser
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 */

/*
 * This is the MPack 0.8.2 amalgamation package.
 *
 * http://github.com/ludocode/mpack
 */

#ifndef MPACK_H
#define MPACK_H 1

#define MPACK_AMALGAMATED 1
#define MPACK_RELEASE_VERSION 1

#include "mpack-config.h"


/* mpack-platform.h */

/**
 * @file
 *
 * Abstracts all platform-specific code from MPack. This contains
 * implementations of standard C functions when libc is not available,
 * as well as wrappers to library functions.
 */

#ifndef MPACK_PLATFORM_H
#define MPACK_PLATFORM_H 1



/* Pre-include checks */

#if defined(_MSC_VER) && _MSC_VER < 1800 && !defined(__cplusplus)
#error "In Visual Studio 2012 and earlier, MPack must be compiled as C++. Enable the /Tp compiler flag."
#endif

#if defined(WIN32) && defined(MPACK_INTERNAL) && MPACK_INTERNAL
#define _CRT_SECURE_NO_WARNINGS 1
#endif



/* #include "mpack-config.h" */



/*
 * Now that the config is included, we define to 0 any of the configuration
 * options and other switches that aren't defined. This supports -Wundef
 * without us having to write "#if defined(X) && X" everywhere (and while
 * allowing configs to be pre-defined to 0.)
 */
#ifndef MPACK_READER
#define MPACK_READER 0
#endif
#ifndef MPACK_EXPECT
#define MPACK_EXPECT 0
#endif
#ifndef MPACK_NODE
#define MPACK_NODE 0
#endif
#ifndef MPACK_WRITER
#define MPACK_WRITER 0
#endif

#ifndef MPACK_STDLIB
#define MPACK_STDLIB 0
#endif
#ifndef MPACK_STDIO
#define MPACK_STDIO 0
#endif

#ifndef MPACK_DEBUG
#define MPACK_DEBUG 0
#endif
#ifndef MPACK_STRINGS
#define MPACK_STRINGS 1 /* default on unless explicitly disabled */
#endif
#ifndef MPACK_CUSTOM_ASSERT
#define MPACK_CUSTOM_ASSERT 0
#endif
#ifndef MPACK_CUSTOM_BREAK
#define MPACK_CUSTOM_BREAK 0
#endif

#ifndef MPACK_READ_TRACKING
#define MPACK_READ_TRACKING 0
#endif
#ifndef MPACK_WRITE_TRACKING
#define MPACK_WRITE_TRACKING 0
#endif
#ifndef MPACK_NO_TRACKING
#define MPACK_NO_TRACKING 0
#endif
#ifndef MPACK_OPTIMIZE_FOR_SIZE
#define MPACK_OPTIMIZE_FOR_SIZE 0
#endif

#ifndef MPACK_EMIT_INLINE_DEFS
#define MPACK_EMIT_INLINE_DEFS 0
#endif
#ifndef MPACK_AMALGAMATED
#define MPACK_AMALGAMATED 0
#endif
#ifndef MPACK_RELEASE_VERSION
#define MPACK_RELEASE_VERSION 0
#endif
#ifndef MPACK_INTERNAL
#define MPACK_INTERNAL 0
#endif
#ifndef MPACK_NO_BUILTINS
#define MPACK_NO_BUILTINS 0
#endif



/* System headers (based on configuration) */

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS 1
#endif
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS 1
#endif
#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS 1
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <limits.h>

#if MPACK_STDLIB
#include <string.h>
#include <stdlib.h>
#endif

#if MPACK_STDIO
#include <stdio.h>
#include <errno.h>
#endif



/*
 * Header configuration
 */

#ifdef __cplusplus
    #define MPACK_EXTERN_C_START extern "C" {
    #define MPACK_EXTERN_C_END   }
#else
    #define MPACK_EXTERN_C_START /* nothing */
    #define MPACK_EXTERN_C_END   /* nothing */
#endif

/* GCC versions from 4.6 to before 5.1 warn about defining a C99
 * non-static inline function before declaring it (see issue #20) */
#ifdef __GNUC__
    #if (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
        #ifdef __cplusplus
            #define MPACK_DECLARED_INLINE_WARNING_START \
                _Pragma ("GCC diagnostic push") \
                _Pragma ("GCC diagnostic ignored \"-Wmissing-declarations\"")
        #else
            #define MPACK_DECLARED_INLINE_WARNING_START \
                _Pragma ("GCC diagnostic push") \
                _Pragma ("GCC diagnostic ignored \"-Wmissing-prototypes\"")
        #endif
        #define MPACK_DECLARED_INLINE_WARNING_END \
            _Pragma ("GCC diagnostic pop")
    #endif
#endif
#ifndef MPACK_DECLARED_INLINE_WARNING_START
    #define MPACK_DECLARED_INLINE_WARNING_START /* nothing */
    #define MPACK_DECLARED_INLINE_WARNING_END /* nothing */
#endif

/* GCC versions before 4.8 warn about shadowing a function with a
 * variable that isn't a function or function pointer (like "index") */
#ifdef __GNUC__
    #if (__GNUC__ < 4) || (__GNUC__ == 4 && __GNUC_MINOR__ < 8)
        #define MPACK_WSHADOW_WARNING_START \
            _Pragma ("GCC diagnostic push") \
            _Pragma ("GCC diagnostic ignored \"-Wshadow\"")
        #define MPACK_WSHADOW_WARNING_END \
            _Pragma ("GCC diagnostic pop")
    #endif
#endif
#ifndef MPACK_WSHADOW_WARNING_START
    #define MPACK_WSHADOW_WARNING_START /* nothing */
    #define MPACK_WSHADOW_WARNING_END /* nothing */
#endif

#define MPACK_HEADER_START \
    MPACK_EXTERN_C_START \
    MPACK_WSHADOW_WARNING_START \
    MPACK_DECLARED_INLINE_WARNING_START

#define MPACK_HEADER_END \
    MPACK_DECLARED_INLINE_WARNING_END \
    MPACK_WSHADOW_WARNING_END \
    MPACK_EXTERN_C_END

MPACK_HEADER_START



/* Miscellaneous helper macros */

#define MPACK_UNUSED(var) ((void)(var))

#define MPACK_STRINGIFY_IMPL(arg) #arg
#define MPACK_STRINGIFY(arg) MPACK_STRINGIFY_IMPL(arg)



/*
 * Definition of inline macros.
 *
 * MPack does not use static inline in header files; only one non-inline definition
 * of each function should exist in the final build. This can reduce the binary size
 * in cases where the compiler cannot or chooses not to inline a function.
 * The addresses of functions should also compare equal across translation units
 * regardless of whether they are declared inline.
 *
 * The above requirements mean that the declaration and definition of non-trivial
 * inline functions must be separated so that the definitions will only
 * appear when necessary. In addition, three different linkage models need
 * to be supported:
 *
 *  - The C99 model, where a standalone function is emitted only if there is any
 *    `extern inline` or non-`inline` declaration (including the definition itself)
 *
 *  - The GNU model, where an `inline` definition emits a standalone function and an
 *    `extern inline` definition does not, regardless of other declarations
 *
 *  - The C++ model, where `inline` emits a standalone function with special
 *    (COMDAT) linkage
 *
 * The macros below wrap up everything above. All inline functions defined in header
 * files have a single non-inline definition emitted in the compilation of
 * mpack-platform.c. All inline declarations and definitions use the same MPACK_INLINE
 * specification to simplify the rules on when standalone functions are emitted.
 * Inline functions in source files are defined MPACK_STATIC_INLINE.
 *
 * Additional reading:
 *     http://www.greenend.org.uk/rjk/tech/inline.html
 */

#if defined(__cplusplus)
    // C++ rules
    // The linker will need COMDAT support to link C++ object files,
    // so we don't need to worry about emitting definitions from C++
    // translation units. If mpack-platform.c (or the amalgamation)
    // is compiled as C, its definition will be used, otherwise a
    // C++ definition will be used, and no other C files will emit
    // a defition.
    #define MPACK_INLINE inline

#elif defined(_MSC_VER)
    // MSVC 2013 always uses COMDAT linkage, but it doesn't treat
    // 'inline' as a keyword in C99 mode.
    #define MPACK_INLINE __inline
    #define MPACK_STATIC_INLINE static __inline

#elif defined(__GNUC__) && (defined(__GNUC_GNU_INLINE__) || \
        !defined(__GNUC_STDC_INLINE__) && !defined(__GNUC_GNU_INLINE__))
    // GNU rules
    #if MPACK_EMIT_INLINE_DEFS
        #define MPACK_INLINE inline
    #else
        #define MPACK_INLINE extern inline
    #endif

#else
    // C99 rules
    #if MPACK_EMIT_INLINE_DEFS
        #define MPACK_INLINE extern inline
    #else
        #define MPACK_INLINE inline
    #endif
#endif

#ifndef MPACK_STATIC_INLINE
#define MPACK_STATIC_INLINE static inline
#endif

#ifdef MPACK_OPTIMIZE_FOR_SPEED
    #error "You should define MPACK_OPTIMIZE_FOR_SIZE, not MPACK_OPTIMIZE_FOR_SPEED."
#endif



/* Some compiler-specific keywords and builtins */

#if !MPACK_NO_BUILTINS
    #if defined(__GNUC__) || defined(__clang__)
        #define MPACK_UNREACHABLE __builtin_unreachable()
        #define MPACK_NORETURN(fn) fn __attribute__((noreturn))
        #define MPACK_RESTRICT __restrict__
    #elif defined(_MSC_VER)
        #define MPACK_UNREACHABLE __assume(0)
        #define MPACK_NORETURN(fn) __declspec(noreturn) fn
        #define MPACK_RESTRICT __restrict
    #endif
#endif

#ifndef MPACK_RESTRICT
#ifdef __cplusplus
#define MPACK_RESTRICT /* nothing, unavailable in C++ */
#else
#define MPACK_RESTRICT restrict /* required in C99 */
#endif
#endif

#ifndef MPACK_UNREACHABLE
#define MPACK_UNREACHABLE ((void)0)
#endif
#ifndef MPACK_NORETURN
#define MPACK_NORETURN(fn) fn
#endif



/* Static assert */

#ifndef MPACK_STATIC_ASSERT
    #if defined(__cplusplus)
        #if __cplusplus >= 201103L
            #define MPACK_STATIC_ASSERT static_assert
        #endif
    #elif defined(__STDC_VERSION__)
        #if __STDC_VERSION__ >= 201112L
            #define MPACK_STATIC_ASSERT _Static_assert
        #endif
    #endif
#endif

#if !MPACK_NO_BUILTINS
    #ifndef MPACK_STATIC_ASSERT
        #if defined(__has_feature)
            #if __has_feature(cxx_static_assert)
                #define MPACK_STATIC_ASSERT static_assert
            #elif __has_feature(c_static_assert)
                #define MPACK_STATIC_ASSERT _Static_assert
            #endif
        #endif
    #endif

    #ifndef MPACK_STATIC_ASSERT
        #if defined(__GNUC__)
            #if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
                #ifndef __cplusplus
                    #if __GNUC__ >= 5
                    #define MPACK_IGNORE_PEDANTIC "GCC diagnostic ignored \"-Wpedantic\""
                    #else
                    #define MPACK_IGNORE_PEDANTIC "GCC diagnostic ignored \"-pedantic\""
                    #endif
                    #define MPACK_STATIC_ASSERT(expr, str) do { \
                        _Pragma ("GCC diagnostic push") \
                        _Pragma (MPACK_IGNORE_PEDANTIC) \
                        _Pragma ("GCC diagnostic ignored \"-Wc++-compat\"") \
                        _Static_assert(expr, str); \
                        _Pragma ("GCC diagnostic pop") \
                    } while (0)
                #endif
            #endif
        #endif
    #endif

    #ifndef MPACK_STATIC_ASSERT
        #ifdef _MSC_VER
            #if _MSC_VER >= 1600
                #define MPACK_STATIC_ASSERT static_assert
            #endif
        #endif
    #endif
#endif

#ifndef MPACK_STATIC_ASSERT
    #define MPACK_STATIC_ASSERT(expr, str) (MPACK_UNUSED(sizeof(char[1 - 2*!(expr)])))
#endif



/* _Generic */

#ifndef MPACK_HAS_GENERIC
    #if defined(__clang__) && defined(__has_feature)
        // With Clang we can test for _Generic support directly
        // and ignore C/C++ version
        #if __has_feature(c_generic_selections)
            #define MPACK_HAS_GENERIC 1
        #else
            #define MPACK_HAS_GENERIC 0
        #endif
    #endif
#endif

#ifndef MPACK_HAS_GENERIC
    #if defined(__STDC_VERSION__)
        #if __STDC_VERSION__ >= 201112L
            #if defined(__GNUC__) && !defined(__clang__)
                // GCC does not have full C11 support in GCC 4.7 and 4.8
                #if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 9)
                    #define MPACK_HAS_GENERIC 1
                #endif
            #else
                // We hope other compilers aren't lying about C11/_Generic support
                #define MPACK_HAS_GENERIC 1
            #endif
        #endif
    #endif
#endif

#ifndef MPACK_HAS_GENERIC
    #define MPACK_HAS_GENERIC 0
#endif



/*
 * Finite Math
 *
 * -ffinite-math-only, included in -ffast-math, breaks functions that
 * that check for non-finite real values such as isnan() and isinf().
 *
 * We should use this to trap errors when reading data that contains
 * non-finite reals. This isn't currently implemented.
 */

#ifndef MPACK_FINITE_MATH
#if defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__
#define MPACK_FINITE_MATH 1
#endif
#endif

#ifndef MPACK_FINITE_MATH
#define MPACK_FINITE_MATH 0
#endif



/*
 * Endianness checks
 *
 * These define MPACK_NHSWAP*() which swap network<->host byte
 * order when needed.
 *
 * We leave them undefined if we can't determine the endianness
 * at compile-time, in which case we fall back to bit-shifts.
 *
 * See the notes in mpack-common.h.
 */

#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__)
    #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        #define MPACK_NHSWAP16(x) (x)
        #define MPACK_NHSWAP32(x) (x)
        #define MPACK_NHSWAP64(x) (x)
    #elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

        #if !MPACK_NO_BUILTINS
            #if defined(__clang__)
                #ifdef __has_builtin
                    // Unlike the GCC builtins, the bswap builtins in Clang
                    // significantly improve ARM performance.
                    #if __has_builtin(__builtin_bswap16)
                        #define MPACK_NHSWAP16(x) __builtin_bswap16(x)
                    #endif
                    #if __has_builtin(__builtin_bswap32)
                        #define MPACK_NHSWAP32(x) __builtin_bswap32(x)
                    #endif
                    #if __has_builtin(__builtin_bswap64)
                        #define MPACK_NHSWAP64(x) __builtin_bswap64(x)
                    #endif
                #endif

            #elif defined(__GNUC__)

                // The GCC bswap builtins are apparently poorly optimized on older
                // versions of GCC, so we set a minimum version here just in case.
                //     http://hardwarebug.org/2010/01/14/beware-the-builtins/

                #if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5)
                    #define MPACK_NHSWAP64(x) __builtin_bswap64(x)
                #endif

                // __builtin_bswap16() was not implemented on all platforms
                // until GCC 4.8.0:
                //     https://gcc.gnu.org/bugzilla/show_bug.cgi?id=52624
                //
                // The 16- and 32-bit versions in GCC significantly reduce performance
                // on ARM with little effect on code size so we don't use them.

            #endif
        #endif
    #endif

#elif defined(_MSC_VER) && defined(_WIN32) && !MPACK_NO_BUILTINS

    // On Windows, we assume x86 and x86_64 are always little-endian.
    // We make no assumptions about ARM even though all current
    // Windows Phone devices are little-endian in case Microsoft's
    // compiler is ever used with a big-endian ARM device.

    #if defined(_M_IX86) || defined(_M_X64) || defined(_M_AMD64)
        #define MPACK_NHSWAP16(x) _byteswap_ushort(x)
        #define MPACK_NHSWAP32(x) _byteswap_ulong(x)
        #define MPACK_NHSWAP64(x) _byteswap_uint64(x)
    #endif

#endif

#if defined(__FLOAT_WORD_ORDER__) && defined(__BYTE_ORDER__)

    // We check where possible that the float byte order matches the
    // integer byte order. This is extremely unlikely to fail, but
    // we check anyway just in case.
    //
    // (The static assert is placed in float/double encoders instead
    // of here because our static assert fallback doesn't work at
    // file scope)

    #define MPACK_CHECK_FLOAT_ORDER() \
        MPACK_STATIC_ASSERT(__FLOAT_WORD_ORDER__ == __BYTE_ORDER__, \
            "float byte order does not match int byte order! float/double " \
            "encoding is not properly implemented on this platform.")

#endif

#ifndef MPACK_CHECK_FLOAT_ORDER
    #define MPACK_CHECK_FLOAT_ORDER() /* nothing */
#endif


/*
 * Here we define mpack_assert() and mpack_break(). They both work like a normal
 * assertion function in debug mode, causing a trap or abort. However, on some platforms
 * you can safely resume execution from mpack_break(), whereas mpack_assert() is
 * always fatal.
 *
 * In release mode, mpack_assert() is converted to an assurance to the compiler
 * that the expression cannot be false (via e.g. __assume() or __builtin_unreachable())
 * to improve optimization where supported. There is thus no point in "safely" handling
 * the case of this being false. Writing mpack_assert(0) rarely makes sense (except
 * possibly as a default handler in a switch) since the compiler will throw away any
 * code after it. If at any time an mpack_assert() is not true, the behaviour is
 * undefined. This also means the expression is evaluated even in release.
 *
 * mpack_break() on the other hand is compiled to nothing in release. It is
 * used in situations where we want to highlight a programming error as early as
 * possible (in the debugger), but we still handle the situation safely if it
 * happens in release to avoid producing incorrect results (such as in
 * MPACK_WRITE_TRACKING.) It does not take an expression to test because it
 * belongs in a safe-handling block after its failing condition has been tested.
 *
 * If stdio is available, we can add a format string describing the error, and
 * on some compilers we can declare it noreturn to get correct results from static
 * analysis tools. Note that the format string and arguments are not evaluated unless
 * the assertion is hit.
 *
 * Note that any arguments to mpack_assert() beyond the first are only evaluated
 * if the expression is false (and are never evaluated in release.)
 *
 * mpack_assert_fail() and mpack_break_hit() are defined separately
 * because assert is noreturn and break isn't. This distinction is very
 * important for static analysis tools to give correct results.
 */

#if MPACK_DEBUG
    MPACK_NORETURN(void mpack_assert_fail(const char* message));
    #if MPACK_STDIO
        MPACK_NORETURN(void mpack_assert_fail_format(const char* format, ...));
        #define mpack_assert_fail_at(line, file, expr, ...) \
                mpack_assert_fail_format("mpack assertion failed at " file ":" #line "\n" expr "\n" __VA_ARGS__)
    #else
        #define mpack_assert_fail_at(line, file, ...) \
                mpack_assert_fail("mpack assertion failed at " file ":" #line )
    #endif

    #define mpack_assert_fail_pos(line, file, expr, ...) mpack_assert_fail_at(line, file, expr, __VA_ARGS__)
    #define mpack_assert(expr, ...) ((!(expr)) ? mpack_assert_fail_pos(__LINE__, __FILE__, #expr, __VA_ARGS__) : (void)0)

    void mpack_break_hit(const char* message);
    #if MPACK_STDIO
        void mpack_break_hit_format(const char* format, ...);
        #define mpack_break_hit_at(line, file, ...) \
                mpack_break_hit_format("mpack breakpoint hit at " file ":" #line "\n" __VA_ARGS__)
    #else
        #define mpack_break_hit_at(line, file, ...) \
                mpack_break_hit("mpack breakpoint hit at " file ":" #line )
    #endif
    #define mpack_break_hit_pos(line, file, ...) mpack_break_hit_at(line, file, __VA_ARGS__)
    #define mpack_break(...) mpack_break_hit_pos(__LINE__, __FILE__, __VA_ARGS__)
#else
    #define mpack_assert(expr, ...) ((!(expr)) ? MPACK_UNREACHABLE, (void)0 : (void)0)
    #define mpack_break(...) ((void)0)
#endif



/* Wrap some needed libc functions */

#if MPACK_STDLIB
    #define mpack_memcmp memcmp
    #define mpack_memcpy memcpy
    #define mpack_memmove memmove
    #define mpack_memset memset
    #define mpack_strlen strlen

    #if defined(MPACK_UNIT_TESTS) && MPACK_INTERNAL && defined(__GNUC__)
        // make sure we don't use the stdlib directly during development
        #undef memcmp
        #undef memcpy
        #undef memmove
        #undef memset
        #undef strlen
        #undef malloc
        #undef free
        #pragma GCC poison memcmp
        #pragma GCC poison memcpy
        #pragma GCC poison memmove
        #pragma GCC poison memset
        #pragma GCC poison strlen
        #pragma GCC poison malloc
        #pragma GCC poison free
    #endif

#elif defined(__GNUC__) && !MPACK_NO_BUILTINS
    // there's not always a builtin memmove for GCC,
    // and we don't have a way to test for it
    #define mpack_memcmp __builtin_memcmp
    #define mpack_memcpy __builtin_memcpy
    #define mpack_memset __builtin_memset
    #define mpack_strlen __builtin_strlen

#elif defined(__clang__) && defined(__has_builtin) && !MPACK_NO_BUILTINS
    #if __has_builtin(__builtin_memcmp)
    #define mpack_memcmp __builtin_memcmp
    #endif
    #if __has_builtin(__builtin_memcpy)
    #define mpack_memcpy __builtin_memcpy
    #endif
    #if __has_builtin(__builtin_memmove)
    #define mpack_memmove __builtin_memmove
    #endif
    #if __has_builtin(__builtin_memset)
    #define mpack_memset __builtin_memset
    #endif
    #if __has_builtin(__builtin_strlen)
    #define mpack_strlen __builtin_strlen
    #endif
#endif

#ifndef mpack_memcmp
int mpack_memcmp(const void* s1, const void* s2, size_t n);
#endif
#ifndef mpack_memcpy
void* mpack_memcpy(void* MPACK_RESTRICT s1, const void* MPACK_RESTRICT s2, size_t n);
#endif
#ifndef mpack_memmove
void* mpack_memmove(void* s1, const void* s2, size_t n);
#endif
#ifndef mpack_memset
void* mpack_memset(void* s, int c, size_t n);
#endif
#ifndef mpack_strlen
size_t mpack_strlen(const char* s);
#endif



/* Debug logging */
#if 0
    #define mpack_log(...) printf(__VA_ARGS__);
#else
    #define mpack_log(...) ((void)0)
#endif



/* Make sure our configuration makes sense */
#if defined(MPACK_MALLOC) && !defined(MPACK_FREE)
    #error "MPACK_MALLOC requires MPACK_FREE."
#endif
#if !defined(MPACK_MALLOC) && defined(MPACK_FREE)
    #error "MPACK_FREE requires MPACK_MALLOC."
#endif
#if MPACK_READ_TRACKING && !defined(MPACK_READER)
    #error "MPACK_READ_TRACKING requires MPACK_READER."
#endif
#if MPACK_WRITE_TRACKING && !defined(MPACK_WRITER)
    #error "MPACK_WRITE_TRACKING requires MPACK_WRITER."
#endif
#ifndef MPACK_MALLOC
    #if MPACK_STDIO
        #error "MPACK_STDIO requires preprocessor definitions for MPACK_MALLOC and MPACK_FREE."
    #endif
    #if MPACK_READ_TRACKING
        #error "MPACK_READ_TRACKING requires preprocessor definitions for MPACK_MALLOC and MPACK_FREE."
    #endif
    #if MPACK_WRITE_TRACKING
        #error "MPACK_WRITE_TRACKING requires preprocessor definitions for MPACK_MALLOC and MPACK_FREE."
    #endif
#endif



/* Implement realloc if unavailable */
#ifdef MPACK_MALLOC
    #ifdef MPACK_REALLOC
        MPACK_INLINE void* mpack_realloc(void* old_ptr, size_t used_size, size_t new_size) {
            MPACK_UNUSED(used_size);
            return MPACK_REALLOC(old_ptr, new_size);
        }
    #else
        void* mpack_realloc(void* old_ptr, size_t used_size, size_t new_size);
    #endif
#endif



/**
 * @}
 */

MPACK_HEADER_END

#endif


/* mpack-common.h */

/**
 * @file
 *
 * Defines types and functions shared by the MPack reader and writer.
 */

#ifndef MPACK_COMMON_H
#define MPACK_COMMON_H 1

/* #include "mpack-platform.h" */

MPACK_HEADER_START



/**
 * @defgroup common Common Elements
 *
 * Contains types, constants and functions shared by both the encoding
 * and decoding portions of MPack.
 *
 * @{
 */

/* Version information */

#define MPACK_VERSION_MAJOR 0  /**< The major version number of MPack. */
#define MPACK_VERSION_MINOR 8  /**< The minor version number of MPack. */
#define MPACK_VERSION_PATCH 2  /**< The patch version number of MPack. */

/** A number containing the version number of MPack for comparison purposes. */
#define MPACK_VERSION ((MPACK_VERSION_MAJOR * 10000) + \
        (MPACK_VERSION_MINOR * 100) + MPACK_VERSION_PATCH)

/** A macro to test for a minimum version of MPack. */
#define MPACK_VERSION_AT_LEAST(major, minor, patch) \
        (MPACK_VERSION >= (((major) * 10000) + ((minor) * 100) + (patch)))

/** @cond */
#if (MPACK_VERSION_PATCH > 0)
#define MPACK_VERSION_STRING_BASE \
        MPACK_STRINGIFY(MPACK_VERSION_MAJOR) "." \
        MPACK_STRINGIFY(MPACK_VERSION_MINOR) "." \
        MPACK_STRINGIFY(MPACK_VERSION_PATCH)
#else
#define MPACK_VERSION_STRING_BASE \
        MPACK_STRINGIFY(MPACK_VERSION_MAJOR) "." \
        MPACK_STRINGIFY(MPACK_VERSION_MINOR)
#endif
/** @endcond */

/**
 * @def MPACK_VERSION_STRING
 * @hideinitializer
 *
 * A string containing the MPack version.
 */
#if MPACK_RELEASE_VERSION
#define MPACK_VERSION_STRING MPACK_VERSION_STRING_BASE
#else
#define MPACK_VERSION_STRING MPACK_VERSION_STRING_BASE "dev"
#endif

/**
 * @def MPACK_LIBRARY_STRING
 * @hideinitializer
 *
 * A string describing MPack, containing the library name, version and debug mode.
 */
#if MPACK_DEBUG
#define MPACK_LIBRARY_STRING "MPack " MPACK_VERSION_STRING "-debug"
#else
#define MPACK_LIBRARY_STRING "MPack " MPACK_VERSION_STRING
#endif

/** @cond */
/**
 * @def MPACK_MAXIMUM_TAG_SIZE
 *
 * The maximum encoded size of a tag in bytes, as of the "new" MessagePack spec.
 */
#define MPACK_MAXIMUM_TAG_SIZE 9
/** @endcond */



/**
 * Error states for MPack objects.
 *
 * When a reader, writer, or tree is in an error state, all subsequent calls
 * are ignored and their return values are nil/zero. You should check whether
 * the source is in an error state before using such values.
 */
typedef enum mpack_error_t {
    mpack_ok = 0,        /**< No error. */
    mpack_error_io = 2,  /**< The reader or writer failed to fill or flush, or some other file or socket error occurred. */
    mpack_error_invalid, /**< The data read is not valid MessagePack. */
    mpack_error_type,    /**< The type or value range did not match what was expected by the caller. */
    mpack_error_too_big, /**< A read or write was bigger than the maximum size allowed for that operation. */
    mpack_error_memory,  /**< An allocation failure occurred. */
    mpack_error_bug,     /**< The MPack API was used incorrectly. (This will always assert in debug mode.) */
    mpack_error_data,    /**< The contained data is not valid. */
} mpack_error_t;

/**
 * Converts an MPack error to a string. This function returns an empty
 * string when MPACK_DEBUG is not set.
 */
const char* mpack_error_to_string(mpack_error_t error);

/**
 * Defines the type of a MessagePack tag.
 */
typedef enum mpack_type_t {
    mpack_type_nil = 1, /**< A null value. */
    mpack_type_bool,    /**< A boolean (true or false.) */
    mpack_type_float,   /**< A 32-bit IEEE 754 floating point number. */
    mpack_type_double,  /**< A 64-bit IEEE 754 floating point number. */
    mpack_type_int,     /**< A 64-bit signed integer. */
    mpack_type_uint,    /**< A 64-bit unsigned integer. */
    mpack_type_str,     /**< A string. */
    mpack_type_bin,     /**< A chunk of binary data. */
    mpack_type_ext,     /**< A typed MessagePack extension object containing a chunk of binary data. */
    mpack_type_array,   /**< An array of MessagePack objects. */
    mpack_type_map,     /**< An ordered map of key/value pairs of MessagePack objects. */
} mpack_type_t;

/**
 * Converts an MPack type to a string. This function returns an empty
 * string when MPACK_DEBUG is not set.
 */
const char* mpack_type_to_string(mpack_type_t type);

/**
 * An MPack tag is a MessagePack object header. It is a variant type representing
 * any kind of object, and includes the value of that object when it is not a
 * compound type (i.e. boolean, integer, float.)
 *
 * If the type is compound (str, bin, ext, array or map), the embedded data is
 * stored separately.
 */
typedef struct mpack_tag_t {
    mpack_type_t type; /**< The type of value. */

    int8_t exttype; /**< The extension type if the type is @ref mpack_type_ext. */

    /** The value for non-compound types. */
    union
    {
        bool     b; /**< The value if the type is bool. */
        float    f; /**< The value if the type is float. */
        double   d; /**< The value if the type is double. */
        int64_t  i; /**< The value if the type is signed int. */
        uint64_t u; /**< The value if the type is unsigned int. */
        uint32_t l; /**< The number of bytes if the type is str, bin or ext. */

        /** The element count if the type is an array, or the number of
            key/value pairs if the type is map. */
        uint32_t n;
    } v;
} mpack_tag_t;

/** Generates a nil tag. */
MPACK_INLINE mpack_tag_t mpack_tag_nil(void) {
    mpack_tag_t ret;
    mpack_memset(&ret, 0, sizeof(ret));
    ret.type = mpack_type_nil;
    return ret;
}

/** Generates a bool tag. */
MPACK_INLINE mpack_tag_t mpack_tag_bool(bool value) {
    mpack_tag_t ret;
    mpack_memset(&ret, 0, sizeof(ret));
    ret.type = mpack_type_bool;
    ret.v.b = value;
    return ret;
}

/** Generates a bool tag with value true. */
MPACK_INLINE mpack_tag_t mpack_tag_true(void) {
    mpack_tag_t ret;
    mpack_memset(&ret, 0, sizeof(ret));
    ret.type = mpack_type_bool;
    ret.v.b = true;
    return ret;
}

/** Generates a bool tag with value false. */
MPACK_INLINE mpack_tag_t mpack_tag_false(void) {
    mpack_tag_t ret;
    mpack_memset(&ret, 0, sizeof(ret));
    ret.type = mpack_type_bool;
    ret.v.b = false;
    return ret;
}

/** Generates a signed int tag. */
MPACK_INLINE mpack_tag_t mpack_tag_int(int64_t value) {
    mpack_tag_t ret;
    mpack_memset(&ret, 0, sizeof(ret));
    ret.type = mpack_type_int;
    ret.v.i = value;
    return ret;
}

/** Generates an unsigned int tag. */
MPACK_INLINE mpack_tag_t mpack_tag_uint(uint64_t value) {
    mpack_tag_t ret;
    mpack_memset(&ret, 0, sizeof(ret));
    ret.type = mpack_type_uint;
    ret.v.u = value;
    return ret;
}

/** Generates a float tag. */
MPACK_INLINE mpack_tag_t mpack_tag_float(float value) {
    mpack_tag_t ret;
    mpack_memset(&ret, 0, sizeof(ret));
    ret.type = mpack_type_float;
    ret.v.f = value;
    return ret;
}

/** Generates a double tag. */
MPACK_INLINE mpack_tag_t mpack_tag_double(double value) {
    mpack_tag_t ret;
    mpack_memset(&ret, 0, sizeof(ret));
    ret.type = mpack_type_double;
    ret.v.d = value;
    return ret;
}

/** Generates an array tag. */
MPACK_INLINE mpack_tag_t mpack_tag_array(int32_t count) {
    mpack_tag_t ret;
    mpack_memset(&ret, 0, sizeof(ret));
    ret.type = mpack_type_array;
    ret.v.n = count;
    return ret;
}

/** Generates a map tag. */
MPACK_INLINE mpack_tag_t mpack_tag_map(int32_t count) {
    mpack_tag_t ret;
    mpack_memset(&ret, 0, sizeof(ret));
    ret.type = mpack_type_map;
    ret.v.n = count;
    return ret;
}

/** Generates a str tag. */
MPACK_INLINE mpack_tag_t mpack_tag_str(int32_t length) {
    mpack_tag_t ret;
    mpack_memset(&ret, 0, sizeof(ret));
    ret.type = mpack_type_str;
    ret.v.l = length;
    return ret;
}

/** Generates a bin tag. */
MPACK_INLINE mpack_tag_t mpack_tag_bin(int32_t length) {
    mpack_tag_t ret;
    mpack_memset(&ret, 0, sizeof(ret));
    ret.type = mpack_type_bin;
    ret.v.l = length;
    return ret;
}

/** Generates an ext tag. */
MPACK_INLINE mpack_tag_t mpack_tag_ext(int8_t exttype, int32_t length) {
    mpack_tag_t ret;
    mpack_memset(&ret, 0, sizeof(ret));
    ret.type = mpack_type_ext;
    ret.exttype = exttype;
    ret.v.l = length;
    return ret;
}

/**
 * Compares two tags with an arbitrary fixed ordering. Returns 0 if the tags are
 * equal, a negative integer if left comes before right, or a positive integer
 * otherwise.
 *
 * \warning The ordering is not guaranteed to be preserved across MPack versions; do
 * not rely on it in persistent data.
 *
 * \warning Floating point numbers are compared bit-for-bit, not using the language's
 * operator==. This means that NaNs with matching representation will compare equal.
 * This behaviour is up for debate; see comments in the definition of mpack_tag_cmp().
 *
 * See mpack_tag_equal() for more information on when tags are considered equal.
 */
int mpack_tag_cmp(mpack_tag_t left, mpack_tag_t right);

/**
 * Compares two tags for equality. Tags are considered equal if the types are compatible
 * and the values (for non-compound types) are equal.
 *
 * The field width of variable-width fields is ignored (and in fact is not stored
 * in a tag), and positive numbers in signed integers are considered equal to their
 * unsigned counterparts. So for example the value 1 stored as a positive fixint
 * is equal to the value 1 stored in a 64-bit unsigned integer field.
 *
 * The "extension type" of an extension object is considered part of the value
 * and much match exactly.
 *
 * \warning Floating point numbers are compared bit-for-bit, not using the language's
 * operator==. This means that NaNs with matching representation will compare equal.
 * This behaviour is up for debate; see comments in the definition of mpack_tag_cmp().
 */
MPACK_INLINE bool mpack_tag_equal(mpack_tag_t left, mpack_tag_t right) {
    return mpack_tag_cmp(left, right) == 0;
}

/**
 * @}
 */



/** @cond */

/*
 * Helpers to perform unaligned network-endian loads and stores
 * at arbitrary addresses. Byte-swapping builtins are used if they
 * are available and if they improve performance.
 *
 * These will remain available in the public API so feel free to
 * use them for other purposes, but they are undocumented.
 */

MPACK_INLINE uint8_t mpack_load_u8(const char* p) {
    return (uint8_t)p[0];
}

MPACK_INLINE uint16_t mpack_load_u16(const char* p) {
    #ifdef MPACK_NHSWAP16
    uint16_t val;
    mpack_memcpy(&val, p, sizeof(val));
    return MPACK_NHSWAP16(val);
    #else
    return (uint16_t)((((uint16_t)(uint8_t)p[0]) << 8) |
           ((uint16_t)(uint8_t)p[1]));
    #endif
}

MPACK_INLINE uint32_t mpack_load_u32(const char* p) {
    #ifdef MPACK_NHSWAP32
    uint32_t val;
    mpack_memcpy(&val, p, sizeof(val));
    return MPACK_NHSWAP32(val);
    #else
    return (((uint32_t)(uint8_t)p[0]) << 24) |
           (((uint32_t)(uint8_t)p[1]) << 16) |
           (((uint32_t)(uint8_t)p[2]) <<  8) |
            ((uint32_t)(uint8_t)p[3]);
    #endif
}

MPACK_INLINE uint64_t mpack_load_u64(const char* p) {
    #ifdef MPACK_NHSWAP64
    uint64_t val;
    mpack_memcpy(&val, p, sizeof(val));
    return MPACK_NHSWAP64(val);
    #else
    return (((uint64_t)(uint8_t)p[0]) << 56) |
           (((uint64_t)(uint8_t)p[1]) << 48) |
           (((uint64_t)(uint8_t)p[2]) << 40) |
           (((uint64_t)(uint8_t)p[3]) << 32) |
           (((uint64_t)(uint8_t)p[4]) << 24) |
           (((uint64_t)(uint8_t)p[5]) << 16) |
           (((uint64_t)(uint8_t)p[6]) <<  8) |
            ((uint64_t)(uint8_t)p[7]);
    #endif
}

MPACK_INLINE void mpack_store_u8(char* p, uint8_t val) {
    uint8_t* u = (uint8_t*)p;
    u[0] = val;
}

MPACK_INLINE void mpack_store_u16(char* p, uint16_t val) {
    #ifdef MPACK_NHSWAP16
    val = MPACK_NHSWAP16(val);
    mpack_memcpy(p, &val, sizeof(val));
    #else
    uint8_t* u = (uint8_t*)p;
    u[0] = (uint8_t)((val >> 8) & 0xFF);
    u[1] = (uint8_t)( val       & 0xFF);
    #endif
}

MPACK_INLINE void mpack_store_u32(char* p, uint32_t val) {
    #ifdef MPACK_NHSWAP32
    val = MPACK_NHSWAP32(val);
    mpack_memcpy(p, &val, sizeof(val));
    #else
    uint8_t* u = (uint8_t*)p;
    u[0] = (uint8_t)((val >> 24) & 0xFF);
    u[1] = (uint8_t)((val >> 16) & 0xFF);
    u[2] = (uint8_t)((val >>  8) & 0xFF);
    u[3] = (uint8_t)( val        & 0xFF);
    #endif
}

MPACK_INLINE void mpack_store_u64(char* p, uint64_t val) {
    #ifdef MPACK_NHSWAP64
    val = MPACK_NHSWAP64(val);
    mpack_memcpy(p, &val, sizeof(val));
    #else
    uint8_t* u = (uint8_t*)p;
    u[0] = (uint8_t)((val >> 56) & 0xFF);
    u[1] = (uint8_t)((val >> 48) & 0xFF);
    u[2] = (uint8_t)((val >> 40) & 0xFF);
    u[3] = (uint8_t)((val >> 32) & 0xFF);
    u[4] = (uint8_t)((val >> 24) & 0xFF);
    u[5] = (uint8_t)((val >> 16) & 0xFF);
    u[6] = (uint8_t)((val >>  8) & 0xFF);
    u[7] = (uint8_t)( val        & 0xFF);
    #endif
}

MPACK_INLINE int8_t  mpack_load_i8 (const char* p) {return (int8_t) mpack_load_u8 (p);}
MPACK_INLINE int16_t mpack_load_i16(const char* p) {return (int16_t)mpack_load_u16(p);}
MPACK_INLINE int32_t mpack_load_i32(const char* p) {return (int32_t)mpack_load_u32(p);}
MPACK_INLINE int64_t mpack_load_i64(const char* p) {return (int64_t)mpack_load_u64(p);}
MPACK_INLINE void mpack_store_i8 (char* p, int8_t  val) {mpack_store_u8 (p, (uint8_t) val);}
MPACK_INLINE void mpack_store_i16(char* p, int16_t val) {mpack_store_u16(p, (uint16_t)val);}
MPACK_INLINE void mpack_store_i32(char* p, int32_t val) {mpack_store_u32(p, (uint32_t)val);}
MPACK_INLINE void mpack_store_i64(char* p, int64_t val) {mpack_store_u64(p, (uint64_t)val);}

MPACK_INLINE float mpack_load_float(const char* p) {
    MPACK_CHECK_FLOAT_ORDER();
    union {
        float f;
        uint32_t u;
    } v;
    v.u = mpack_load_u32(p);
    return v.f;
}

MPACK_INLINE double mpack_load_double(const char* p) {
    MPACK_CHECK_FLOAT_ORDER();
    union {
        double d;
        uint64_t u;
    } v;
    v.u = mpack_load_u64(p);
    return v.d;
}

MPACK_INLINE void mpack_store_float(char* p, float value) {
    MPACK_CHECK_FLOAT_ORDER();
    union {
        float f;
        uint32_t u;
    } v;
    v.f = value;
    mpack_store_u32(p, v.u);
}

MPACK_INLINE void mpack_store_double(char* p, double value) {
    MPACK_CHECK_FLOAT_ORDER();
    union {
        double d;
        uint64_t u;
    } v;
    v.d = value;
    mpack_store_u64(p, v.u);
}

/** @endcond */



/** @cond */

// Sizes in bytes for the various possible tags
#define MPACK_TAG_SIZE_FIXUINT  1
#define MPACK_TAG_SIZE_U8       2
#define MPACK_TAG_SIZE_U16      3
#define MPACK_TAG_SIZE_U32      5
#define MPACK_TAG_SIZE_U64      9
#define MPACK_TAG_SIZE_FIXINT   1
#define MPACK_TAG_SIZE_I8       2
#define MPACK_TAG_SIZE_I16      3
#define MPACK_TAG_SIZE_I32      5
#define MPACK_TAG_SIZE_I64      9
#define MPACK_TAG_SIZE_FLOAT    5
#define MPACK_TAG_SIZE_DOUBLE   9
#define MPACK_TAG_SIZE_FIXARRAY 1
#define MPACK_TAG_SIZE_ARRAY16  3
#define MPACK_TAG_SIZE_ARRAY32  5
#define MPACK_TAG_SIZE_FIXMAP   1
#define MPACK_TAG_SIZE_MAP16    3
#define MPACK_TAG_SIZE_MAP32    5
#define MPACK_TAG_SIZE_FIXSTR   1
#define MPACK_TAG_SIZE_STR8     2
#define MPACK_TAG_SIZE_STR16    3
#define MPACK_TAG_SIZE_STR32    5
#define MPACK_TAG_SIZE_BIN8     2
#define MPACK_TAG_SIZE_BIN16    3
#define MPACK_TAG_SIZE_BIN32    5
#define MPACK_TAG_SIZE_FIXEXT1  2
#define MPACK_TAG_SIZE_FIXEXT2  2
#define MPACK_TAG_SIZE_FIXEXT4  2
#define MPACK_TAG_SIZE_FIXEXT8  2
#define MPACK_TAG_SIZE_FIXEXT16 2
#define MPACK_TAG_SIZE_EXT8     3
#define MPACK_TAG_SIZE_EXT16    4
#define MPACK_TAG_SIZE_EXT32    6

/** @endcond */



#if MPACK_READ_TRACKING || MPACK_WRITE_TRACKING
/* Tracks the write state of compound elements (maps, arrays, */
/* strings, binary blobs and extension types) */
/** @cond */

typedef struct mpack_track_element_t {
    mpack_type_t type;
    uint64_t left; // we need 64-bit because (2 * INT32_MAX) elements can be stored in a map
} mpack_track_element_t;

typedef struct mpack_track_t {
    size_t count;
    size_t capacity;
    mpack_track_element_t* elements;
} mpack_track_t;

#if MPACK_INTERNAL
mpack_error_t mpack_track_init(mpack_track_t* track);
mpack_error_t mpack_track_grow(mpack_track_t* track);
mpack_error_t mpack_track_push(mpack_track_t* track, mpack_type_t type, uint64_t count);
mpack_error_t mpack_track_pop(mpack_track_t* track, mpack_type_t type);
mpack_error_t mpack_track_element(mpack_track_t* track, bool read);
mpack_error_t mpack_track_peek_element(mpack_track_t* track, bool read);
mpack_error_t mpack_track_bytes(mpack_track_t* track, bool read, uint64_t count);
mpack_error_t mpack_track_str_bytes_all(mpack_track_t* track, bool read, uint64_t count);
mpack_error_t mpack_track_check_empty(mpack_track_t* track);
mpack_error_t mpack_track_destroy(mpack_track_t* track, bool cancel);
#endif

/** @endcond */
#endif



#if MPACK_INTERNAL
/** @cond */



/* Miscellaneous string functions */

/**
 * Returns true if the given UTF-8 string is valid.
 */
bool mpack_utf8_check(const char* str, size_t bytes);

/**
 * Returns true if the given UTF-8 string is valid and contains no null characters.
 */
bool mpack_utf8_check_no_null(const char* str, size_t bytes);

/**
 * Returns true if the given string has no null bytes.
 */
bool mpack_str_check_no_null(const char* str, size_t bytes);



/** @endcond */
#endif



MPACK_HEADER_END

#endif


/* mpack-writer.h */

/**
 * @file
 *
 * Declares the MPack Writer.
 */

#ifndef MPACK_WRITER_H
#define MPACK_WRITER_H 1

/* #include "mpack-common.h" */

MPACK_HEADER_START

#if MPACK_WRITER

#if MPACK_WRITE_TRACKING
struct mpack_track_t;
#endif

/**
 * @defgroup writer Write API
 *
 * The MPack Write API encodes structured data of a fixed (hardcoded) schema to MessagePack.
 *
 * @{
 */

/**
 * @def MPACK_WRITER_MINIMUM_BUFFER_SIZE
 *
 * The minimum buffer size for a writer with a flush function.
 */
#define MPACK_WRITER_MINIMUM_BUFFER_SIZE 32

/**
 * A buffered MessagePack encoder.
 *
 * The encoder wraps an existing buffer and, optionally, a flush function.
 * This allows efficiently encoding to an in-memory buffer or to a stream.
 *
 * All write operations are synchronous; they will block until the
 * data is fully written, or an error occurs.
 */
typedef struct mpack_writer_t mpack_writer_t;

/**
 * The MPack writer's flush function to flush the buffer to the output stream.
 * It should flag an appropriate error on the writer if flushing fails (usually
 * mpack_error_io or mpack_error_memory.)
 *
 * The specified context for callbacks is at writer->context.
 */
typedef void (*mpack_writer_flush_t)(mpack_writer_t* writer, const char* buffer, size_t count);

/**
 * An error handler function to be called when an error is flagged on
 * the writer.
 *
 * The error handler will only be called once on the first error flagged;
 * any subsequent writes and errors are ignored, and the writer is
 * permanently in that error state.
 *
 * MPack is safe against non-local jumps out of error handler callbacks.
 * This means you are allowed to longjmp or throw an exception (in C++,
 * Objective-C, or with SEH) out of this callback.
 *
 * Bear in mind when using longjmp that local non-volatile variables that
 * have changed are undefined when setjmp() returns, so you can't put the
 * writer on the stack in the same activation frame as the setjmp without
 * declaring it volatile.
 *
 * You must still eventually destroy the writer. It is not destroyed
 * automatically when an error is flagged. It is safe to destroy the
 * writer within this error callback, but you will either need to perform
 * a non-local jump, or store something in your context to identify
 * that the writer is destroyed since any future accesses to it cause
 * undefined behavior.
 */
typedef void (*mpack_writer_error_t)(mpack_writer_t* writer, mpack_error_t error);

/**
 * A teardown function to be called when the writer is destroyed.
 */
typedef void (*mpack_writer_teardown_t)(mpack_writer_t* writer);

/* Hide internals from documentation */
/** @cond */

struct mpack_writer_t {
    mpack_writer_flush_t flush;       /* Function to write bytes to the output stream */
    mpack_writer_error_t error_fn;    /* Function to call on error */
    mpack_writer_teardown_t teardown; /* Function to teardown the context on destroy */
    void* context;                    /* Context for writer callbacks */

    char* buffer;         /* Byte buffer */
    size_t size;          /* Size of the buffer */
    size_t used;          /* How many bytes have been written into the buffer */
    mpack_error_t error;  /* Error state */

    #if MPACK_WRITE_TRACKING
    mpack_track_t track; /* Stack of map/array/str/bin/ext writes */
    #endif

    #ifdef MPACK_MALLOC
    /* Reserved. You can use this space to allocate a custom
     * context in order to reduce heap allocations. */
    void* reserved[2];
    #endif
};

#if MPACK_WRITE_TRACKING
void mpack_writer_track_push(mpack_writer_t* writer, mpack_type_t type, uint64_t count);
void mpack_writer_track_pop(mpack_writer_t* writer, mpack_type_t type);
void mpack_writer_track_element(mpack_writer_t* writer);
void mpack_writer_track_bytes(mpack_writer_t* writer, size_t count);
#else
MPACK_INLINE void mpack_writer_track_push(mpack_writer_t* writer, mpack_type_t type, uint64_t count) {
    MPACK_UNUSED(writer);
    MPACK_UNUSED(type);
    MPACK_UNUSED(count);
}
MPACK_INLINE void mpack_writer_track_pop(mpack_writer_t* writer, mpack_type_t type) {
    MPACK_UNUSED(writer);
    MPACK_UNUSED(type);
}
MPACK_INLINE void mpack_writer_track_element(mpack_writer_t* writer) {
    MPACK_UNUSED(writer);
}
MPACK_INLINE void mpack_writer_track_bytes(mpack_writer_t* writer, size_t count) {
    MPACK_UNUSED(writer);
    MPACK_UNUSED(count);
}
#endif

/** @endcond */

/**
 * @name Lifecycle Functions
 * @{
 */

/**
 * Initializes an MPack writer with the given buffer. The writer
 * does not assume ownership of the buffer.
 *
 * Trying to write past the end of the buffer will result in mpack_error_too_big
 * unless a flush function is set with mpack_writer_set_flush(). To use the data
 * without flushing, call mpack_writer_buffer_used() to determine the number of
 * bytes written.
 *
 * @param writer The MPack writer.
 * @param buffer The buffer into which to write MessagePack data.
 * @param size The size of the buffer.
 */
void mpack_writer_init(mpack_writer_t* writer, char* buffer, size_t size);

#ifdef MPACK_MALLOC
/**
 * Initializes an MPack writer using a growable buffer.
 *
 * The data is placed in the given data pointer if and when the writer
 * is destroyed without error. The data pointer is NULL during writing,
 * and will remain NULL if an error occurs.
 *
 * The allocated data must be freed with MPACK_FREE() (or simply free()
 * if MPack's allocator hasn't been customized.)
 *
 * @throws mpack_error_memory if the buffer fails to grow when
 * flushing.
 *
 * @param writer The MPack writer.
 * @param data Where to place the allocated data.
 * @param size Where to write the size of the data.
 */
void mpack_writer_init_growable(mpack_writer_t* writer, char** data, size_t* size);
#endif

/**
 * Initializes an MPack writer directly into an error state. Use this if you
 * are writing a wrapper to mpack_writer_init() which can fail its setup.
 */
void mpack_writer_init_error(mpack_writer_t* writer, mpack_error_t error);

#if MPACK_STDIO
/**
 * Initializes an MPack writer that writes to a file.
 *
 * @throws mpack_error_memory if allocation fails
 * @throws mpack_error_io if the file cannot be opened
 */
void mpack_writer_init_file(mpack_writer_t* writer, const char* filename);
#endif

/** @cond */

#define mpack_writer_init_stack_line_ex(line, writer) \
    char mpack_buf_##line[MPACK_STACK_SIZE]; \
    mpack_writer_init(writer, mpack_buf_##line, sizeof(mpack_buf_##line))

#define mpack_writer_init_stack_line(line, writer) \
    mpack_writer_init_stack_line_ex(line, writer)

/*
 * Initializes an MPack writer using stack space as a buffer. A flush function
 * should be added to the writer to flush the buffer.
 *
 * This is currently undocumented since it's not entirely useful on its own.
 */

#define mpack_writer_init_stack(writer) \
    mpack_writer_init_stack_line(__LINE__, (writer))

/** @endcond */

/**
 * Cleans up the MPack writer, flushing and closing the underlying stream,
 * if any. Returns the final error state of the writer.
 *
 * No flushing is performed if the writer is in an error state. The attached
 * teardown function is called whether or not the writer is in an error state.
 *
 * This will assert in tracking mode if the writer is not in an error
 * state and has any unclosed compound types. If you want to cancel
 * writing in the middle of a document, you need to flag an error on
 * the writer before destroying it (such as mpack_error_data).
 *
 * Note that a writer may raise an error and call your error handler during
 * the final flush. It is safe to longjmp or throw out of this error handler,
 * but if you do, the writer will not be destroyed, and the teardown function
 * will not be called. You can still get the writer's error state, and you
 * must call mpack_writer_destroy again. (The second call is guaranteed not
 * to call your error handler again since the writer is already in an error
 * state.)
 *
 * @see mpack_writer_set_error_handler
 * @see mpack_writer_set_flush
 * @see mpack_writer_set_teardown
 * @see mpack_writer_flag_error
 * @see mpack_error_data
 */
mpack_error_t mpack_writer_destroy(mpack_writer_t* writer);

/**
 * @}
 */

/**
 * @name Callbacks
 * @{
 */

/**
 * Sets the custom pointer to pass to the writer callbacks, such as flush
 * or teardown.
 *
 * @param writer The MPack writer.
 * @param context User data to pass to the writer callbacks.
 */
MPACK_INLINE void mpack_writer_set_context(mpack_writer_t* writer, void* context) {
    writer->context = context;
}

/**
 * Sets the flush function to write out the data when the buffer is full.
 *
 * If no flush function is used, trying to write past the end of the
 * buffer will result in mpack_error_too_big.
 *
 * This should normally be used with mpack_writer_set_context() to register
 * a custom pointer to pass to the flush function.
 *
 * @param writer The MPack writer.
 * @param flush The function to write out data from the buffer.
 */
void mpack_writer_set_flush(mpack_writer_t* writer, mpack_writer_flush_t flush);

/**
 * Sets the error function to call when an error is flagged on the writer.
 *
 * This should normally be used with mpack_writer_set_context() to register
 * a custom pointer to pass to the error function.
 *
 * See the definition of mpack_writer_error_t for more information about
 * what you can do from an error callback.
 *
 * @see mpack_writer_error_t
 * @param writer The MPack writer.
 * @param error_fn The function to call when an error is flagged on the writer.
 */
MPACK_INLINE void mpack_writer_set_error_handler(mpack_writer_t* writer, mpack_writer_error_t error_fn) {
    writer->error_fn = error_fn;
}

/**
 * Sets the teardown function to call when the writer is destroyed.
 *
 * This should normally be used with mpack_writer_set_context() to register
 * a custom pointer to pass to the teardown function.
 *
 * @param writer The MPack writer.
 * @param teardown The function to call when the writer is destroyed.
 */
MPACK_INLINE void mpack_writer_set_teardown(mpack_writer_t* writer, mpack_writer_teardown_t teardown) {
    writer->teardown = teardown;
}

/**
 * @}
 */

/**
 * @name Core Writer Functions
 * @{
 */

/**
 * Returns the number of bytes currently stored in the buffer. This
 * may be less than the total number of bytes written if bytes have
 * been flushed to an underlying stream.
 */
MPACK_INLINE size_t mpack_writer_buffer_used(mpack_writer_t* writer) {
    return writer->used;
}

/**
 * Returns the amount of space left in the buffer. This may be reset
 * after a write if bytes are flushed to an underlying stream.
 */
MPACK_INLINE size_t mpack_writer_buffer_left(mpack_writer_t* writer) {
    return writer->size - writer->used;
}

/**
 * Places the writer in the given error state, calling the error callback if one
 * is set.
 *
 * This allows you to externally flag errors, for example if you are validating
 * data as you write it, or if you want to cancel writing in the middle of a
 * document. (The writer will assert if you try to destroy it without error and
 * with unclosed compound types. In this case you should flag mpack_error_data
 * before destroying it.)
 *
 * If the writer is already in an error state, this call is ignored and no
 * error callback is called.
 *
 * @see mpack_writer_destroy
 * @see mpack_error_data
 */
void mpack_writer_flag_error(mpack_writer_t* writer, mpack_error_t error);

/**
 * Queries the error state of the MPack writer.
 *
 * If a writer is in an error state, you should discard all data since the
 * last time the error flag was checked. The error flag cannot be cleared.
 */
MPACK_INLINE mpack_error_t mpack_writer_error(mpack_writer_t* writer) {
    return writer->error;
}

/**
 * Writes a MessagePack object header (an MPack Tag.)
 *
 * If the value is a map, array, string, binary or extension type, the
 * containing elements or bytes must be written separately and the
 * appropriate finish function must be called (as though one of the
 * mpack_start_*() functions was called.)
 *
 * @see mpack_write_bytes()
 * @see mpack_finish_map()
 * @see mpack_finish_array()
 * @see mpack_finish_str()
 * @see mpack_finish_bin()
 * @see mpack_finish_ext()
 * @see mpack_finish_type()
 */
void mpack_write_tag(mpack_writer_t* writer, mpack_tag_t tag);

/**
 * @}
 */

/**
 * @name Integers
 * @{
 */

/** Writes an 8-bit integer in the most efficient packing available. */
void mpack_write_i8(mpack_writer_t* writer, int8_t value);

/** Writes a 16-bit integer in the most efficient packing available. */
void mpack_write_i16(mpack_writer_t* writer, int16_t value);

/** Writes a 32-bit integer in the most efficient packing available. */
void mpack_write_i32(mpack_writer_t* writer, int32_t value);

/** Writes a 64-bit integer in the most efficient packing available. */
void mpack_write_i64(mpack_writer_t* writer, int64_t value);

/** Writes an integer in the most efficient packing available. */
MPACK_INLINE void mpack_write_int(mpack_writer_t* writer, int64_t value) {
    mpack_write_i64(writer, value);
}

/** Writes an 8-bit unsigned integer in the most efficient packing available. */
void mpack_write_u8(mpack_writer_t* writer, uint8_t value);

/** Writes an 16-bit unsigned integer in the most efficient packing available. */
void mpack_write_u16(mpack_writer_t* writer, uint16_t value);

/** Writes an 32-bit unsigned integer in the most efficient packing available. */
void mpack_write_u32(mpack_writer_t* writer, uint32_t value);

/** Writes an 64-bit unsigned integer in the most efficient packing available. */
void mpack_write_u64(mpack_writer_t* writer, uint64_t value);

/** Writes an unsigned integer in the most efficient packing available. */
MPACK_INLINE void mpack_write_uint(mpack_writer_t* writer, uint64_t value) {
    mpack_write_u64(writer, value);
}

/**
 * @}
 */

/**
 * @name Other Basic Types
 * @{
 */

/** Writes a float. */
void mpack_write_float(mpack_writer_t* writer, float value);

/** Writes a double. */
void mpack_write_double(mpack_writer_t* writer, double value);

/** Writes a boolean. */
void mpack_write_bool(mpack_writer_t* writer, bool value);

/** Writes a boolean with value true. */
void mpack_write_true(mpack_writer_t* writer);

/** Writes a boolean with value false. */
void mpack_write_false(mpack_writer_t* writer);

/** Writes a nil. */
void mpack_write_nil(mpack_writer_t* writer);

/** Write a pre-encoded messagepack object */
void mpack_write_object_bytes(mpack_writer_t* writer, const char* data, size_t bytes);

/**
 * @}
 */

/**
 * @name Map and Array Functions
 * @{
 */

/**
 * Opens an array.
 *
 * `count` elements must follow, and mpack_finish_array() must be called
 * when done.
 *
 * @see mpack_finish_array()
 */
void mpack_start_array(mpack_writer_t* writer, uint32_t count);

/**
 * Opens a map.
 *
 * `count * 2` elements must follow, and mpack_finish_map() must be called
 * when done.
 *
 * Remember that while map elements in MessagePack are implicitly ordered,
 * they are not ordered in JSON. If you need elements to be read back
 * in the order they are written, consider use an array instead.
 *
 * @see mpack_finish_map()
 */
void mpack_start_map(mpack_writer_t* writer, uint32_t count);

/**
 * Finishes writing an array.
 *
 * This should be called only after a corresponding call to mpack_start_array()
 * and after the array contents are written.
 *
 * This will track writes to ensure that the correct number of elements are written.
 *
 * @see mpack_start_array()
 */
MPACK_INLINE void mpack_finish_array(mpack_writer_t* writer) {
    mpack_writer_track_pop(writer, mpack_type_array);
}

/**
 * Finishes writing a map.
 *
 * This should be called only after a corresponding call to mpack_start_map()
 * and after the map contents are written.
 *
 * This will track writes to ensure that the correct number of elements are written.
 *
 * @see mpack_start_map()
 */
MPACK_INLINE void mpack_finish_map(mpack_writer_t* writer) {
    mpack_writer_track_pop(writer, mpack_type_map);
}

/**
 * @}
 */

/**
 * @name Data Helpers
 * @{
 */

/**
 * Writes a string.
 *
 * To stream a string in chunks, use mpack_start_str() instead.
 *
 * MPack does not care about the underlying encoding, but UTF-8 is highly
 * recommended, especially for compatibility with JSON. You should consider
 * calling mpack_write_utf8() instead, especially if you will be reading
 * it back as UTF-8.
 *
 * You should not call mpack_finish_str() after calling this; this
 * performs both start and finish.
 */
void mpack_write_str(mpack_writer_t* writer, const char* str, uint32_t length);

/**
 * Writes a string, ensuring that it is valid UTF-8.
 *
 * This does not accept any UTF-8 variant such as Modified UTF-8, CESU-8 or
 * WTF-8. Only pure UTF-8 is allowed.
 *
 * You should not call mpack_finish_str() after calling this; this
 * performs both start and finish.
 *
 * @throws mpack_error_invalid if the string is not valid UTF-8
 */
void mpack_write_utf8(mpack_writer_t* writer, const char* str, uint32_t length);

/**
 * Writes a null-terminated string. (The null-terminator is not written.)
 *
 * MPack does not care about the underlying encoding, but UTF-8 is highly
 * recommended, especially for compatibility with JSON. You should consider
 * calling mpack_write_utf8_cstr() instead, especially if you will be reading
 * it back as UTF-8.
 *
 * You should not call mpack_finish_str() after calling this; this
 * performs both start and finish.
 */
void mpack_write_cstr(mpack_writer_t* writer, const char* cstr);

/**
 * Writes a null-terminated string, or a nil node if the given cstr pointer
 * is NULL. (The null-terminator is not written.)
 *
 * MPack does not care about the underlying encoding, but UTF-8 is highly
 * recommended, especially for compatibility with JSON. You should consider
 * calling mpack_write_utf8_cstr_or_nil() instead, especially if you will
 * be reading it back as UTF-8.
 *
 * You should not call mpack_finish_str() after calling this; this
 * performs both start and finish.
 */
void mpack_write_cstr_or_nil(mpack_writer_t* writer, const char* cstr);

/**
 * Writes a null-terminated string, ensuring that it is valid UTF-8. (The
 * null-terminator is not written.)
 *
 * This does not accept any UTF-8 variant such as Modified UTF-8, CESU-8 or
 * WTF-8. Only pure UTF-8 is allowed.
 *
 * You should not call mpack_finish_str() after calling this; this
 * performs both start and finish.
 *
 * @throws mpack_error_invalid if the string is not valid UTF-8
 */
void mpack_write_utf8_cstr(mpack_writer_t* writer, const char* cstr);

/**
 * Writes a null-terminated string ensuring that it is valid UTF-8, or
 * writes nil if the given cstr pointer is NULL. (The null-terminator
 * is not written.)
 *
 * This does not accept any UTF-8 variant such as Modified UTF-8, CESU-8 or
 * WTF-8. Only pure UTF-8 is allowed.
 *
 * You should not call mpack_finish_str() after calling this; this
 * performs both start and finish.
 *
 * @throws mpack_error_invalid if the string is not valid UTF-8
 */
void mpack_write_utf8_cstr_or_nil(mpack_writer_t* writer, const char* cstr);

/**
 * Writes a binary blob.
 *
 * To stream a binary blob in chunks, use mpack_start_bin() instead.
 *
 * You should not call mpack_finish_bin() after calling this; this
 * performs both start and finish.
 */
void mpack_write_bin(mpack_writer_t* writer, const char* data, uint32_t count);

/**
 * Writes an extension type.
 *
 * To stream an extension blob in chunks, use mpack_start_ext() instead.
 *
 * Extension types [0, 127] are available for application-specific types. Extension
 * types [-128, -1] are reserved for future extensions of MessagePack.
 *
 * You should not call mpack_finish_ext() after calling this; this
 * performs both start and finish.
 */
void mpack_write_ext(mpack_writer_t* writer, int8_t exttype, const char* data, uint32_t count);

/**
 * @}
 */

/**
 * @name Chunked Data Functions
 * @{
 */

/**
 * Opens a string. `count` bytes should be written with calls to
 * mpack_write_bytes(), and mpack_finish_str() should be called
 * when done.
 *
 * To write an entire string at once, use mpack_write_str() or
 * mpack_write_cstr() instead.
 *
 * MPack does not care about the underlying encoding, but UTF-8 is highly
 * recommended, especially for compatibility with JSON.
 */
void mpack_start_str(mpack_writer_t* writer, uint32_t count);

/**
 * Opens a binary blob. `count` bytes should be written with calls to
 * mpack_write_bytes(), and mpack_finish_bin() should be called
 * when done.
 */
void mpack_start_bin(mpack_writer_t* writer, uint32_t count);

/**
 * Opens an extension type. `count` bytes should be written with calls
 * to mpack_write_bytes(), and mpack_finish_ext() should be called
 * when done.
 *
 * Extension types [0, 127] are available for application-specific types. Extension
 * types [-128, -1] are reserved for future extensions of MessagePack.
 */
void mpack_start_ext(mpack_writer_t* writer, int8_t exttype, uint32_t count);

/**
 * Writes a portion of bytes for a string, binary blob or extension type which
 * was opened by mpack_write_tag() or one of the mpack_start_*() functions.
 *
 * This can be called multiple times to write the data in chunks, as long as
 * the total amount of bytes written matches the count given when the compound
 * type was started.
 *
 * The corresponding mpack_finish_*() function must be called when done.
 *
 * To write an entire string, binary blob or extension type at
 * once, use one of the mpack_write_*() functions instead.
 *
 * @see mpack_write_tag()
 * @see mpack_start_str()
 * @see mpack_start_bin()
 * @see mpack_start_ext()
 * @see mpack_finish_str()
 * @see mpack_finish_bin()
 * @see mpack_finish_ext()
 * @see mpack_finish_type()
 */
void mpack_write_bytes(mpack_writer_t* writer, const char* data, size_t count);

/**
 * Finishes writing a string.
 *
 * This should be called only after a corresponding call to mpack_start_str()
 * and after the string bytes are written with mpack_write_bytes().
 *
 * This will track writes to ensure that the correct number of elements are written.
 *
 * @see mpack_start_str()
 * @see mpack_write_bytes()
 */
MPACK_INLINE void mpack_finish_str(mpack_writer_t* writer) {
    mpack_writer_track_pop(writer, mpack_type_str);
}

/**
 * Finishes writing a binary blob.
 *
 * This should be called only after a corresponding call to mpack_start_bin()
 * and after the binary bytes are written with mpack_write_bytes().
 *
 * This will track writes to ensure that the correct number of bytes are written.
 *
 * @see mpack_start_bin()
 * @see mpack_write_bytes()
 */
MPACK_INLINE void mpack_finish_bin(mpack_writer_t* writer) {
    mpack_writer_track_pop(writer, mpack_type_bin);
}

/**
 * Finishes writing an extended type binary data blob.
 *
 * This should be called only after a corresponding call to mpack_start_bin()
 * and after the binary bytes are written with mpack_write_bytes().
 *
 * This will track writes to ensure that the correct number of bytes are written.
 *
 * @see mpack_start_ext()
 * @see mpack_write_bytes()
 */
MPACK_INLINE void mpack_finish_ext(mpack_writer_t* writer) {
    mpack_writer_track_pop(writer, mpack_type_ext);
}

/**
 * Finishes writing the given compound type.
 *
 * This will track writes to ensure that the correct number of elements
 * or bytes are written.
 *
 * This can be called with the appropriate type instead the corresponding
 * mpack_finish_*() function if you want to finish a dynamic type.
 */
MPACK_INLINE void mpack_finish_type(mpack_writer_t* writer, mpack_type_t type) {
    mpack_writer_track_pop(writer, type);
}

/**
 * @}
 */

/**
 * @}
 */

#endif

MPACK_HEADER_END

#if MPACK_WRITER && MPACK_HAS_GENERIC

/**
 * @addtogroup writer
 * @{
 */

/**
 * @name Core Writer Functions
 * @{
 */

/**
 * @def mpack_write(writer, value)
 *
 * Type-generic writer for primitive types.
 *
 * The compiler will dispatch to an appropriate write function based
 * on the type of the @a value parameter.
 *
 * @note This requires C11 `_Generic` support, or C++. This is implemented
 * as a set of inline overloads in C++ mode if `_Generic` is not supported.
 *
 * @warning In C11, the indentifiers `true`, `false` and `NULL` are
 * all of type `int`, not `bool` or `void*`! They will emit unexpected
 * types when passed uncast, so be careful when using them.
 */
#define mpack_write(writer, value) \
    _Generic(((void)0, value),                      \
              int8_t: mpack_write_i8,               \
             int16_t: mpack_write_i16,              \
             int32_t: mpack_write_i32,              \
             int64_t: mpack_write_i64,              \
             uint8_t: mpack_write_u8,               \
            uint16_t: mpack_write_u16,              \
            uint32_t: mpack_write_u32,              \
            uint64_t: mpack_write_u64,              \
                bool: mpack_write_bool,             \
               float: mpack_write_float,            \
              double: mpack_write_double,           \
              char *: mpack_write_cstr_or_nil,      \
        const char *: mpack_write_cstr_or_nil       \
    )(writer, value)

/**
 * @def mpack_write_kv(writer, key, value)
 *
 * Type-generic writer for key-value pairs of null-terminated string
 * keys and primitive values.
 * a cstr string.
 *
 * @warning @a writer may be evaluated multiple times.
 *
 * @param writer The writer.
 * @param key A null-terminated C string.
 * @param value A primitive type supported by mpack_write().
 */
#define mpack_write_kv(writer, key, value) do {     \
    mpack_write_cstr(writer, key);                  \
    mpack_write(writer, value);                     \
} while (0)

/**
 * @}
 */

/**
 * @}
 */
#elif defined(__cplusplus)

/* C++ generic write for primitive values */

MPACK_INLINE void mpack_write(mpack_writer_t* writer, int8_t value) {
    mpack_write_i8(writer, value);
}

MPACK_INLINE void mpack_write(mpack_writer_t* writer, int16_t value) {
    mpack_write_i16(writer, value);
}

MPACK_INLINE void mpack_write(mpack_writer_t* writer, int32_t value) {
    mpack_write_i32(writer, value);
}

MPACK_INLINE void mpack_write(mpack_writer_t* writer, int64_t value) {
    mpack_write_i64(writer, value);
}

MPACK_INLINE void mpack_write(mpack_writer_t* writer, uint8_t value) {
    mpack_write_u8(writer, value);
}

MPACK_INLINE void mpack_write(mpack_writer_t* writer, uint16_t value) {
    mpack_write_u16(writer, value);
}

MPACK_INLINE void mpack_write(mpack_writer_t* writer, uint32_t value) {
    mpack_write_u32(writer, value);
}

MPACK_INLINE void mpack_write(mpack_writer_t* writer, uint64_t value) {
    mpack_write_u64(writer, value);
}

MPACK_INLINE void mpack_write(mpack_writer_t* writer, bool value) {
    mpack_write_bool(writer, value);
}

MPACK_INLINE void mpack_write(mpack_writer_t* writer, float value) {
    mpack_write_float(writer, value);
}

MPACK_INLINE void mpack_write(mpack_writer_t* writer, double value) {
    mpack_write_double(writer, value);
}

MPACK_INLINE void mpack_write(mpack_writer_t* writer, char *value) {
    mpack_write_cstr_or_nil(writer, value);
}

MPACK_INLINE void mpack_write(mpack_writer_t* writer, const char *value) {
    mpack_write_cstr_or_nil(writer, value);
}

/* C++ generic write for key-value pairs */

MPACK_INLINE void mpack_write_kv(mpack_writer_t* writer, const char *key, int8_t value) {
    mpack_write_cstr(writer, key);
    mpack_write_i8(writer, value);
}

MPACK_INLINE void mpack_write_kv(mpack_writer_t* writer, const char *key, int16_t value) {
    mpack_write_cstr(writer, key);
    mpack_write_i16(writer, value);
}

MPACK_INLINE void mpack_write_kv(mpack_writer_t* writer, const char *key, int32_t value) {
    mpack_write_cstr(writer, key);
    mpack_write_i32(writer, value);
}

MPACK_INLINE void mpack_write_kv(mpack_writer_t* writer, const char *key, int64_t value) {
    mpack_write_cstr(writer, key);
    mpack_write_i64(writer, value);
}

MPACK_INLINE void mpack_write_kv(mpack_writer_t* writer, const char *key, uint8_t value) {
    mpack_write_cstr(writer, key);
    mpack_write_u8(writer, value);
}

MPACK_INLINE void mpack_write_kv(mpack_writer_t* writer, const char *key, uint16_t value) {
    mpack_write_cstr(writer, key);
    mpack_write_u16(writer, value);
}

MPACK_INLINE void mpack_write_kv(mpack_writer_t* writer, const char *key, uint32_t value) {
    mpack_write_cstr(writer, key);
    mpack_write_u32(writer, value);
}

MPACK_INLINE void mpack_write_kv(mpack_writer_t* writer, const char *key, uint64_t value) {
    mpack_write_cstr(writer, key);
    mpack_write_u64(writer, value);
}

MPACK_INLINE void mpack_write_kv(mpack_writer_t* writer, const char *key, bool value) {
    mpack_write_cstr(writer, key);
    mpack_write_bool(writer, value);
}

MPACK_INLINE void mpack_write_kv(mpack_writer_t* writer, const char *key, float value) {
    mpack_write_cstr(writer, key);
    mpack_write_float(writer, value);
}

MPACK_INLINE void mpack_write_kv(mpack_writer_t* writer, const char *key, double value) {
    mpack_write_cstr(writer, key);
    mpack_write_double(writer, value);
}

MPACK_INLINE void mpack_write_kv(mpack_writer_t* writer, const char *key, char *value) {
    mpack_write_cstr(writer, key);
    mpack_write_cstr_or_nil(writer, value);
}

MPACK_INLINE void mpack_write_kv(mpack_writer_t* writer, const char *key, const char *value) {
    mpack_write_cstr(writer, key);
    mpack_write_cstr_or_nil(writer, value);
}
#endif /* __cplusplus */

#endif

/* mpack-reader.h */

/**
 * @file
 *
 * Declares the core MPack Tag Reader.
 */

#ifndef MPACK_READER_H
#define MPACK_READER_H 1

/* #include "mpack-common.h" */

MPACK_HEADER_START

#if MPACK_READER

#if MPACK_READ_TRACKING
struct mpack_track_t;
#endif

/**
 * @defgroup reader Core Reader API
 *
 * The MPack Core Reader API contains functions for imperatively reading
 * dynamically typed data from a MessagePack stream. This forms the basis
 * of the Expect API.
 *
 * @{
 */

/**
 * @def MPACK_READER_MINIMUM_BUFFER_SIZE
 *
 * The minimum buffer size for a reader with a fill function.
 */
#define MPACK_READER_MINIMUM_BUFFER_SIZE 32

/**
 * A buffered MessagePack decoder.
 *
 * The decoder wraps an existing buffer and, optionally, a fill function.
 * This allows efficiently decoding data from existing memory buffers, files,
 * streams, etc.
 *
 * All read operations are synchronous; they will block until the
 * requested data is fully read, or an error occurs.
 *
 * This structure is opaque; its fields should not be accessed outside
 * of MPack.
 */
typedef struct mpack_reader_t mpack_reader_t;

/**
 * The MPack reader's fill function. It should fill the buffer as
 * much as possible, returning the number of bytes put into the buffer.
 *
 * In case of error, it should flag an appropriate error on the reader.
 */
typedef size_t (*mpack_reader_fill_t)(mpack_reader_t* reader, char* buffer, size_t count);

/**
 * The MPack reader's skip function. It should discard the given number
 * of bytes from the source (for example by seeking forward.)
 *
 * In case of error, it should flag an appropriate error on the reader.
 */
typedef void (*mpack_reader_skip_t)(mpack_reader_t* reader, size_t count);

/**
 * An error handler function to be called when an error is flagged on
 * the reader.
 *
 * The error handler will only be called once on the first error flagged;
 * any subsequent reads and errors are ignored, and the reader is
 * permanently in that error state.
 *
 * MPack is safe against non-local jumps out of error handler callbacks.
 * This means you are allowed to longjmp or throw an exception (in C++,
 * Objective-C, or with SEH) out of this callback.
 *
 * Bear in mind when using longjmp that local non-volatile variables that
 * have changed are undefined when setjmp() returns, so you can't put the
 * reader on the stack in the same activation frame as the setjmp without
 * declaring it volatile.
 *
 * You must still eventually destroy the reader. It is not destroyed
 * automatically when an error is flagged. It is safe to destroy the
 * reader within this error callback, but you will either need to perform
 * a non-local jump, or store something in your context to identify
 * that the reader is destroyed since any future accesses to it cause
 * undefined behavior.
 */
typedef void (*mpack_reader_error_t)(mpack_reader_t* reader, mpack_error_t error);

/**
 * A teardown function to be called when the reader is destroyed.
 */
typedef void (*mpack_reader_teardown_t)(mpack_reader_t* reader);

/* Hide internals from documentation */
/** @cond */

struct mpack_reader_t {
    void* context;                    /* Context for reader callbacks */
    mpack_reader_fill_t fill;         /* Function to read bytes into the buffer */
    mpack_reader_error_t error_fn;    /* Function to call on error */
    mpack_reader_teardown_t teardown; /* Function to teardown the context on destroy */

    // The skip function may be unused, but we don't preproc it
    // out on MPACK_OPTIMIZE_FOR_SIZE in case calling code is compiled
    // with different optimization options. MPACK_OPTIMIZE_FOR_SIZE
    // should never be used in header files.
    mpack_reader_skip_t skip;         /* Function to skip bytes from the source */

    char* buffer;       /* Byte buffer */
    size_t size;        /* Size of the buffer, or zero if it's const */
    size_t left;        /* How many bytes are left in the buffer */
    size_t pos;         /* Position within the buffer */
    mpack_error_t error;  /* Error state */

    #if MPACK_READ_TRACKING
    mpack_track_t track; /* Stack of map/array/str/bin/ext reads */
    #endif
};

/** @endcond */

/**
 * @name Lifecycle Functions
 * @{
 */

/**
 * Initializes an MPack reader with the given buffer. The reader does
 * not assume ownership of the buffer, but the buffer must be writeable
 * if a fill function will be used to refill it.
 *
 * @param reader The MPack reader.
 * @param buffer The buffer with which to read MessagePack data.
 * @param size The size of the buffer.
 * @param count The number of bytes already in the buffer.
 */
void mpack_reader_init(mpack_reader_t* reader, char* buffer, size_t size, size_t count);

/**
 * Initializes an MPack reader directly into an error state. Use this if you
 * are writing a wrapper to mpack_reader_init() which can fail its setup.
 */
void mpack_reader_init_error(mpack_reader_t* reader, mpack_error_t error);

/**
 * Initializes an MPack reader to parse a pre-loaded contiguous chunk of data. The
 * reader does not assume ownership of the data.
 *
 * @param reader The MPack reader.
 * @param data The data to parse.
 * @param count The number of bytes pointed to by data.
 */
void mpack_reader_init_data(mpack_reader_t* reader, const char* data, size_t count);

#if MPACK_STDIO
/**
 * Initializes an MPack reader that reads from a file.
 */
void mpack_reader_init_file(mpack_reader_t* reader, const char* filename);
#endif

/**
 * @def mpack_reader_init_stack(reader)
 * @hideinitializer
 *
 * Initializes an MPack reader using stack space as a buffer. A fill function
 * should be added to the reader to fill the buffer.
 *
 * @see mpack_reader_set_fill
 */

/** @cond */
#define mpack_reader_init_stack_line_ex(line, reader) \
    char mpack_buf_##line[MPACK_STACK_SIZE]; \
    mpack_reader_init((reader), mpack_buf_##line, sizeof(mpack_buf_##line), 0)

#define mpack_reader_init_stack_line(line, reader) \
    mpack_reader_init_stack_line_ex(line, reader)
/** @endcond */

#define mpack_reader_init_stack(reader) \
    mpack_reader_init_stack_line(__LINE__, (reader))

/**
 * Cleans up the MPack reader, ensuring that all compound elements
 * have been completely read. Returns the final error state of the
 * reader.
 *
 * This will assert in tracking mode if the reader is not in an error
 * state and has any incomplete reads. If you want to cancel reading
 * in the middle of a document, you need to flag an error on the reader
 * before destroying it (such as mpack_error_data).
 *
 * @see mpack_read_tag()
 * @see mpack_reader_flag_error()
 * @see mpack_error_data
 */
mpack_error_t mpack_reader_destroy(mpack_reader_t* reader);

/**
 * @}
 */

/**
 * @name Callbacks
 * @{
 */

/**
 * Sets the custom pointer to pass to the reader callbacks, such as fill
 * or teardown.
 *
 * @param reader The MPack reader.
 * @param context User data to pass to the reader callbacks.
 */
MPACK_INLINE void mpack_reader_set_context(mpack_reader_t* reader, void* context) {
    reader->context = context;
}

/**
 * Sets the fill function to refill the data buffer when it runs out of data.
 *
 * If no fill function is used, truncated MessagePack data results in
 * mpack_error_invalid (since the buffer is assumed to contain a
 * complete MessagePack object.)
 *
 * If a fill function is used, truncated MessagePack data usually
 * results in mpack_error_io (since the fill function fails to get
 * the missing data.)
 *
 * This should normally be used with mpack_reader_set_context() to register
 * a custom pointer to pass to the fill function.
 *
 * @param reader The MPack reader.
 * @param fill The function to fetch additional data into the buffer.
 */
void mpack_reader_set_fill(mpack_reader_t* reader, mpack_reader_fill_t fill);

/**
 * Sets the skip function to discard bytes from the source stream.
 *
 * It's not necessary to implement this function. If the stream is not
 * seekable, don't set a skip callback. The reader will fall back to
 * using the fill function instead.
 *
 * This should normally be used with mpack_reader_set_context() to register
 * a custom pointer to pass to the skip function.
 *
 * The skip function is ignored in size-optimized builds to reduce code
 * size. Data will be skipped with the fill function when necessary.
 *
 * @param reader The MPack reader.
 * @param skip The function to discard bytes from the source stream.
 */
void mpack_reader_set_skip(mpack_reader_t* reader, mpack_reader_skip_t skip);

/**
 * Sets the error function to call when an error is flagged on the reader.
 *
 * This should normally be used with mpack_reader_set_context() to register
 * a custom pointer to pass to the error function.
 *
 * See the definition of mpack_reader_error_t for more information about
 * what you can do from an error callback.
 *
 * @see mpack_reader_error_t
 * @param reader The MPack reader.
 * @param error_fn The function to call when an error is flagged on the reader.
 */
MPACK_INLINE void mpack_reader_set_error_handler(mpack_reader_t* reader, mpack_reader_error_t error_fn) {
    reader->error_fn = error_fn;
}

/**
 * Sets the teardown function to call when the reader is destroyed.
 *
 * This should normally be used with mpack_reader_set_context() to register
 * a custom pointer to pass to the teardown function.
 *
 * @param reader The MPack reader.
 * @param teardown The function to call when the reader is destroyed.
 */
MPACK_INLINE void mpack_reader_set_teardown(mpack_reader_t* reader, mpack_reader_teardown_t teardown) {
    reader->teardown = teardown;
}

/**
 * @}
 */

/**
 * @name Core Reader Functions
 * @{
 */

/**
 * Queries the error state of the MPack reader.
 *
 * If a reader is in an error state, you should discard all data since the
 * last time the error flag was checked. The error flag cannot be cleared.
 */
MPACK_INLINE mpack_error_t mpack_reader_error(mpack_reader_t* reader) {
    return reader->error;
}

/**
 * Places the reader in the given error state, calling the error callback if one
 * is set.
 *
 * This allows you to externally flag errors, for example if you are validating
 * data as you read it.
 *
 * If the reader is already in an error state, this call is ignored and no
 * error callback is called.
 */
void mpack_reader_flag_error(mpack_reader_t* reader, mpack_error_t error);

/**
 * Places the reader in the given error state if the given error is not mpack_ok,
 * returning the resulting error state of the reader.
 *
 * This allows you to externally flag errors, for example if you are validating
 * data as you read it.
 *
 * If the given error is mpack_ok or if the reader is already in an error state,
 * this call is ignored and the actual error state of the reader is returned.
 */
MPACK_INLINE mpack_error_t mpack_reader_flag_if_error(mpack_reader_t* reader, mpack_error_t error) {
    if (error != mpack_ok)
        mpack_reader_flag_error(reader, error);
    return mpack_reader_error(reader);
}

/**
 * Returns bytes left in the reader's buffer.
 *
 * If you are done reading MessagePack data but there is other interesting data
 * following it, the reader may have buffered too much data. The number of bytes
 * remaining in the buffer and a pointer to the position of those bytes can be
 * queried here.
 *
 * If you know the length of the MPack chunk beforehand, it's better to instead
 * have your fill function limit the data it reads so that the reader does not
 * have extra data. In this case you can simply check that this returns zero.
 *
 * Returns 0 if the reader is in an error state.
 *
 * @param reader The MPack reader from which to query remaining data.
 * @param data [out] A pointer to the remaining data, or NULL.
 * @return The number of bytes remaining in the buffer.
 */
size_t mpack_reader_remaining(mpack_reader_t* reader, const char** data);

/**
 * Reads a MessagePack object header (an MPack tag.)
 *
 * If an error occurs, the reader is placed in an error state and a
 * nil tag is returned. If the reader is already in an error state,
 * a nil tag is returned.
 *
 * If the type is compound (i.e. is a map, array, string, binary or
 * extension type), additional reads are required to get the contained
 * data, and the corresponding done function must be called when done.
 *
 * @note Maps in JSON are unordered, so it is recommended not to expect
 * a specific ordering for your map values in case your data is converted
 * to/from JSON.
 * 
 * @see mpack_read_bytes()
 * @see mpack_done_array()
 * @see mpack_done_map()
 * @see mpack_done_str()
 * @see mpack_done_bin()
 * @see mpack_done_ext()
 */
mpack_tag_t mpack_read_tag(mpack_reader_t* reader);

/**
 * Parses the next MessagePack object header (an MPack tag) without
 * advancing the reader.
 *
 * If an error occurs, the reader is placed in an error state and a
 * nil tag is returned. If the reader is already in an error state,
 * a nil tag is returned.
 *
 * @note Maps in JSON are unordered, so it is recommended not to expect
 * a specific ordering for your map values in case your data is converted
 * to/from JSON.
 *
 * @see mpack_read_tag()
 * @see mpack_discard()
 */
mpack_tag_t mpack_peek_tag(mpack_reader_t* reader);

/**
 * @}
 */

/**
 * @name String and Data Functions
 * @{
 */

/**
 * Skips bytes from the underlying stream. This is used only to
 * skip the contents of a string, binary blob or extension object.
 */
void mpack_skip_bytes(mpack_reader_t* reader, size_t count);

/**
 * Reads bytes from a string, binary blob or extension object, copying
 * them into the given buffer.
 *
 * A str, bin or ext must have been opened by a call to mpack_read_tag()
 * which yielded one of these types, or by a call to an expect function
 * such as mpack_expect_str() or mpack_expect_bin().
 *
 * If an error occurs, the buffer contents are undefined.
 *
 * This can be called multiple times for a single str, bin or ext
 * to read the data in chunks. The total data read must add up
 * to the size of the object.
 *
 * @param reader The MPack reader
 * @param p The buffer in which to copy the bytes
 * @param count The number of bytes to read
 */
void mpack_read_bytes(mpack_reader_t* reader, char* p, size_t count);

/**
 * Reads bytes from a string, ensures that the string is valid UTF-8,
 * and copies the bytes into the given buffer.
 *
 * A string must have been opened by a call to mpack_read_tag() which
 * yielded a string, or by a call to an expect function such as
 * mpack_expect_str().
 *
 * The given byte count must match the complete size of the string as
 * returned by the tag or expect function. You must ensure that the
 * buffer fits the data.
 *
 * This does not accept any UTF-8 variant such as Modified UTF-8, CESU-8 or
 * WTF-8. Only pure UTF-8 is allowed.
 *
 * If an error occurs, the buffer contents are undefined.
 *
 * Unlike mpack_read_bytes(), this cannot be used to read the data in
 * chunks (since this might split a character's UTF-8 bytes, and the
 * reader does not keep track of the UTF-8 decoding state between reads.)
 *
 * @throws mpack_error_type if the string contains invalid UTF-8.
 */
void mpack_read_utf8(mpack_reader_t* reader, char* p, size_t byte_count);

/**
 * Reads bytes from a string, ensures that the string contains no NUL
 * bytes, copies the bytes into the given buffer and adds a null-terminator.
 *
 * A string must have been opened by a call to mpack_read_tag() which
 * yielded a string, or by a call to an expect function such as
 * mpack_expect_str().
 *
 * The given byte count must match the size of the string as returned
 * by the tag or expect function. The string will only be copied if
 * the buffer is large enough to store it.
 *
 * If an error occurs, the buffer will contain an empty string.
 *
 * @note If you know the object will be a string before reading it,
 * it is highly recommended to use mpack_expect_cstr() instead.
 * Alternatively you could use mpack_peek_tag() and call
 * mpack_expect_cstr() if it's a string.
 *
 * @throws mpack_error_too_big if the string plus null-terminator is larger than the given buffer size
 * @throws mpack_error_type if the string contains a null byte.
 *
 * @see mpack_peek_tag()
 * @see mpack_expect_cstr()
 * @see mpack_expect_utf8_cstr()
 */
void mpack_read_cstr(mpack_reader_t* reader, char* buf, size_t buffer_size, size_t byte_count);

/**
 * Reads bytes from a string, ensures that the string is valid UTF-8
 * with no NUL bytes, copies the bytes into the given buffer and adds a
 * null-terminator.
 *
 * A string must have been opened by a call to mpack_read_tag() which
 * yielded a string, or by a call to an expect function such as
 * mpack_expect_str().
 *
 * The given byte count must match the size of the string as returned
 * by the tag or expect function. The string will only be copied if
 * the buffer is large enough to store it.
 *
 * This does not accept any UTF-8 variant such as Modified UTF-8, CESU-8 or
 * WTF-8. Only pure UTF-8 is allowed, but without the NUL character, since
 * it cannot be represented in a null-terminated string.
 *
 * If an error occurs, the buffer will contain an empty string.
 *
 * @note If you know the object will be a string before reading it,
 * it is highly recommended to use mpack_expect_utf8_cstr() instead.
 * Alternatively you could use mpack_peek_tag() and call
 * mpack_expect_utf8_cstr() if it's a string.
 *
 * @throws mpack_error_too_big if the string plus null-terminator is larger than the given buffer size
 * @throws mpack_error_type if the string contains invalid UTF-8 or a null byte.
 *
 * @see mpack_peek_tag()
 * @see mpack_expect_utf8_cstr()
 */
void mpack_read_utf8_cstr(mpack_reader_t* reader, char* buf, size_t buffer_size, size_t byte_count);

#ifdef MPACK_MALLOC
/** @cond */
// This can optionally add a null-terminator, but it does not check
// whether the data contains null bytes. This must be done separately
// in a cstring read function (possibly as part of a UTF-8 check.)
char* mpack_read_bytes_alloc_impl(mpack_reader_t* reader, size_t count, bool null_terminated);
/** @endcond */

/**
 * Reads bytes from a string, binary blob or extension object, allocating
 * storage for them and returning the allocated pointer.
 *
 * The allocated string must be freed with MPACK_FREE() (or simply free()
 * if MPack's allocator hasn't been customized.)
 *
 * Returns NULL if any error occurs, or if count is zero.
 */
MPACK_INLINE char* mpack_read_bytes_alloc(mpack_reader_t* reader, size_t count) {
    return mpack_read_bytes_alloc_impl(reader, count, false);
}
#endif

/**
 * Reads bytes from a string, binary blob or extension object in-place in
 * the buffer. This can be used to avoid copying the data.
 *
 * A str, bin or ext must have been opened by a call to mpack_read_tag()
 * which yielded one of these types, or by a call to an expect function
 * such as mpack_expect_str() or mpack_expect_bin().
 *
 * If the bytes are from a string, the string is not null-terminated! Use
 * mpack_read_cstr() to copy the string into a buffer and add a null-terminator.
 *
 * The returned pointer is invalidated on the next read, or when the buffer
 * is destroyed.
 *
 * The reader will move data around in the buffer if needed to ensure that
 * the pointer can always be returned, so this should only be used if
 * count is very small compared to the buffer size. If you need to check
 * whether a small size is reasonable (for example you intend to handle small and
 * large sizes differently), you can call mpack_should_read_bytes_inplace().
 *
 * This can be called multiple times for a single str, bin or ext
 * to read the data in chunks. The total data read must add up
 * to the size of the object.
 *
 * NULL is returned if the reader is in an error state.
 *
 * @throws mpack_error_too_big if the requested size is larger than the buffer size
 *
 * @see mpack_should_read_bytes_inplace()
 */
const char* mpack_read_bytes_inplace(mpack_reader_t* reader, size_t count);

/**
 * Reads bytes from a string in-place in the buffer and ensures they are
 * valid UTF-8. This can be used to avoid copying the data.
 *
 * A string must have been opened by a call to mpack_read_tag() which
 * yielded a string, or by a call to an expect function such as
 * mpack_expect_str().
 *
 * The string is not null-terminated! Use mpack_read_utf8_cstr() to
 * copy the string into a buffer and add a null-terminator.
 *
 * The returned pointer is invalidated on the next read, or when the buffer
 * is destroyed.
 *
 * The reader will move data around in the buffer if needed to ensure that
 * the pointer can always be returned, so this should only be used if
 * count is very small compared to the buffer size. If you need to check
 * whether a small size is reasonable (for example you intend to handle small and
 * large sizes differently), you can call mpack_should_read_bytes_inplace().
 *
 * This does not accept any UTF-8 variant such as Modified UTF-8, CESU-8 or
 * WTF-8. Only pure UTF-8 is allowed.
 *
 * Unlike mpack_read_bytes_inplace(), this cannot be used to read the data in
 * chunks (since this might split a character's UTF-8 bytes, and the
 * reader does not keep track of the UTF-8 decoding state between reads.)
 *
 * NULL is returned if the reader is in an error state.
 *
 * @throws mpack_error_type if the string contains invalid UTF-8
 * @throws mpack_error_too_big if the requested size is larger than the buffer size
 *
 * @see mpack_should_read_bytes_inplace()
 */
const char* mpack_read_utf8_inplace(mpack_reader_t* reader, size_t count);

/**
 * Returns true if it's a good idea to read the given number of bytes
 * in-place.
 *
 * If the read will be larger than some small fraction of the buffer size,
 * this will return false to avoid shuffling too much data back and forth
 * in the buffer.
 *
 * Use this if you're expecting arbitrary size data, and you want to read
 * in-place for the best performance when possible but will fall back to
 * a normal read if the data is too large.
 *
 * @see mpack_read_bytes_inplace()
 */
MPACK_INLINE bool mpack_should_read_bytes_inplace(mpack_reader_t* reader, size_t count) {
    return (reader->size == 0 || count <= reader->size / 32);
}

/**
 * @}
 */

/**
 * @name Core Reader Functions
 * @{
 */

#if MPACK_READ_TRACKING
/**
 * Finishes reading the given type.
 *
 * This will track reads to ensure that the correct number of elements
 * or bytes are read.
 */
void mpack_done_type(mpack_reader_t* reader, mpack_type_t type);
#else
MPACK_INLINE void mpack_done_type(mpack_reader_t* reader, mpack_type_t type) {
    MPACK_UNUSED(reader);
    MPACK_UNUSED(type);
}
#endif

/**
 * Finishes reading an array.
 *
 * This will track reads to ensure that the correct number of elements are read.
 */
MPACK_INLINE void mpack_done_array(mpack_reader_t* reader) {
    mpack_done_type(reader, mpack_type_array);
}

/**
 * @fn mpack_done_map(mpack_reader_t* reader)
 *
 * Finishes reading a map.
 *
 * This will track reads to ensure that the correct number of elements are read.
 */
MPACK_INLINE void mpack_done_map(mpack_reader_t* reader) {
    mpack_done_type(reader, mpack_type_map);
}

/**
 * @fn mpack_done_str(mpack_reader_t* reader)
 *
 * Finishes reading a string.
 *
 * This will track reads to ensure that the correct number of bytes are read.
 */
MPACK_INLINE void mpack_done_str(mpack_reader_t* reader) {
    mpack_done_type(reader, mpack_type_str);
}

/**
 * @fn mpack_done_bin(mpack_reader_t* reader)
 *
 * Finishes reading a binary data blob.
 *
 * This will track reads to ensure that the correct number of bytes are read.
 */
MPACK_INLINE void mpack_done_bin(mpack_reader_t* reader) {
    mpack_done_type(reader, mpack_type_bin);
}

/**
 * @fn mpack_done_ext(mpack_reader_t* reader)
 *
 * Finishes reading an extended type binary data blob.
 *
 * This will track reads to ensure that the correct number of bytes are read.
 */
MPACK_INLINE void mpack_done_ext(mpack_reader_t* reader) {
    mpack_done_type(reader, mpack_type_ext);
}

/**
 * Reads and discards the next object. This will read and discard all
 * contained data as well if it is a compound type.
 */
void mpack_discard(mpack_reader_t* reader);

/**
 * @}
 */

#if MPACK_STDIO
/**
 * @name Debugging Functions
 * @{
 */

/**
 * Converts a blob of MessagePack to pseudo-JSON for debugging purposes
 * and pretty-prints it to the given file.
 */
void mpack_print_file(const char* data, size_t len, FILE* file);

#ifndef MPACK_GCOV
/**
 * Converts a blob of MessagePack to pseudo-JSON for debugging purposes
 * and pretty-prints it to stdout.
 */
MPACK_INLINE void mpack_print(const char* data, size_t len) {
    mpack_print_file(data, len, stdout);
}
#endif

/**
 * @}
 */
#endif

/**
 * @}
 */



#if MPACK_INTERNAL

bool mpack_reader_ensure_straddle(mpack_reader_t* reader, size_t count);

// Ensures there are at least count bytes left in the buffer. This will
// flag an error if there is not enough data, and will assert if there
// is a fill function and count is larger than the buffer size. Returns
// true if there are enough bytes, false otherwise. Error handling must
// be done separately! The reader cannot be in an error state when this
// is called.
MPACK_INLINE bool mpack_reader_ensure(mpack_reader_t* reader, size_t count) {
    mpack_assert(count != 0, "cannot ensure zero bytes!");
    mpack_assert(count <= MPACK_READER_MINIMUM_BUFFER_SIZE,
            "cannot ensure %i bytes, this is more than the minimum buffer size %i!", count,
            (int)count, (int)MPACK_READER_MINIMUM_BUFFER_SIZE);
    mpack_assert(reader->error == mpack_ok, "reader cannot be in an error state!");

    if (count <= reader->left)
        return true;
    return mpack_reader_ensure_straddle(reader, count);
}

void mpack_read_native_big(mpack_reader_t* reader, char* p, size_t count);

// Reads count bytes into p, deferring to mpack_read_native_big() if more
// bytes are needed than are available in the buffer.
MPACK_INLINE void mpack_read_native(mpack_reader_t* reader, char* p, size_t count) {
    mpack_assert(count == 0 || p != NULL, "data pointer for %i bytes is NULL", (int)count);

    if (count > reader->left) {
        mpack_read_native_big(reader, p, count);
    } else {
        mpack_memcpy(p, reader->buffer + reader->pos, count);
        reader->pos += count;
        reader->left -= count;
    }
}

#if MPACK_READ_TRACKING
#define MPACK_READER_TRACK(reader, error_expr) \
    (((reader)->error == mpack_ok) ? mpack_reader_flag_if_error((reader), (error_expr)) : (reader)->error)
#else
#define MPACK_READER_TRACK(reader, error_expr) (MPACK_UNUSED(reader), mpack_ok)
#endif

MPACK_INLINE mpack_error_t mpack_reader_track_element(mpack_reader_t* reader) {
    return MPACK_READER_TRACK(reader, mpack_track_element(&reader->track, true));
}

MPACK_INLINE mpack_error_t mpack_reader_track_peek_element(mpack_reader_t* reader) {
    return MPACK_READER_TRACK(reader, mpack_track_peek_element(&reader->track, true));
}

MPACK_INLINE mpack_error_t mpack_reader_track_bytes(mpack_reader_t* reader, uint64_t count) {
    MPACK_UNUSED(count);
    return MPACK_READER_TRACK(reader, mpack_track_bytes(&reader->track, true, count));
}

MPACK_INLINE mpack_error_t mpack_reader_track_str_bytes_all(mpack_reader_t* reader, uint64_t count) {
    MPACK_UNUSED(count);
    return MPACK_READER_TRACK(reader, mpack_track_str_bytes_all(&reader->track, true, count));
}

#endif



#endif

MPACK_HEADER_END

#endif


/* mpack-expect.h */

/**
 * @file
 *
 * Declares the MPack static Expect API.
 */

#ifndef MPACK_EXPECT_H
#define MPACK_EXPECT_H 1

/* #include "mpack-reader.h" */

MPACK_HEADER_START

#if MPACK_EXPECT

#if !MPACK_READER
#error "MPACK_EXPECT requires MPACK_READER."
#endif

/**
 * @defgroup expect Expect API
 *
 * The MPack Expect API allows you to easily read MessagePack data when you
 * expect it to follow a predefined schema.
 *
 * See @ref docs/expect.md for examples.
 *
 * The main purpose of the Expect API is convenience, so the API is lax. It
 * automatically converts between similar types where there is no loss of
 * precision.
 *
 * When using any of the expect functions, if the type or value of what was
 * read does not match what is expected, @ref mpack_error_type is raised.
 *
 * @{
 */

/**
 * @name Basic Number Functions
 * @{
 */

/**
 * Reads an 8-bit unsigned integer.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in an 8-bit unsigned int.
 *
 * Returns zero if an error occurs.
 */
uint8_t mpack_expect_u8(mpack_reader_t* reader);

/**
 * Reads a 16-bit unsigned integer.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in a 16-bit unsigned int.
 *
 * Returns zero if an error occurs.
 */
uint16_t mpack_expect_u16(mpack_reader_t* reader);

/**
 * Reads a 32-bit unsigned integer.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in a 32-bit unsigned int.
 *
 * Returns zero if an error occurs.
 */
uint32_t mpack_expect_u32(mpack_reader_t* reader);

/**
 * Reads a 64-bit unsigned integer.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in a 64-bit unsigned int.
 *
 * Returns zero if an error occurs.
 */
uint64_t mpack_expect_u64(mpack_reader_t* reader);

/**
 * Reads an 8-bit signed integer.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in an 8-bit signed int.
 *
 * Returns zero if an error occurs.
 */
int8_t mpack_expect_i8(mpack_reader_t* reader);

/**
 * Reads a 16-bit signed integer.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in a 16-bit signed int.
 *
 * Returns zero if an error occurs.
 */
int16_t mpack_expect_i16(mpack_reader_t* reader);

/**
 * Reads a 32-bit signed integer.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in a 32-bit signed int.
 *
 * Returns zero if an error occurs.
 */
int32_t mpack_expect_i32(mpack_reader_t* reader);

/**
 * Reads a 64-bit signed integer.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in a 64-bit signed int.
 *
 * Returns zero if an error occurs.
 */
int64_t mpack_expect_i64(mpack_reader_t* reader);

/**
 * Reads a number, returning the value as a float. The underlying value can be an
 * integer, float or double; the value is converted to a float.
 *
 * @note Reading a double or a large integer with this function can incur a
 * loss of precision.
 *
 * @throws mpack_error_type if the underlying value is not a float, double or integer.
 */
float mpack_expect_float(mpack_reader_t* reader);

/**
 * Reads a number, returning the value as a double. The underlying value can be an
 * integer, float or double; the value is converted to a double.
 *
 * @note Reading a very large integer with this function can incur a
 * loss of precision.
 *
 * @throws mpack_error_type if the underlying value is not a float, double or integer.
 */
double mpack_expect_double(mpack_reader_t* reader);

/**
 * Reads a float. The underlying value must be a float, not a double or an integer.
 * This ensures no loss of precision can occur.
 *
 * @throws mpack_error_type if the underlying value is not a float.
 */
float mpack_expect_float_strict(mpack_reader_t* reader);

/**
 * Reads a double. The underlying value must be a float or double, not an integer.
 * This ensures no loss of precision can occur.
 *
 * @throws mpack_error_type if the underlying value is not a float or double.
 */
double mpack_expect_double_strict(mpack_reader_t* reader);

/**
 * @}
 */

/**
 * @name Ranged Number Functions
 * @{
 */

/**
 * Reads an 8-bit unsigned integer, ensuring that it falls within the given range.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in an 8-bit unsigned int.
 *
 * Returns min_value if an error occurs.
 */
uint8_t mpack_expect_u8_range(mpack_reader_t* reader, uint8_t min_value, uint8_t max_value);

/**
 * Reads a 16-bit unsigned integer, ensuring that it falls within the given range.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in a 16-bit unsigned int.
 *
 * Returns min_value if an error occurs.
 */
uint16_t mpack_expect_u16_range(mpack_reader_t* reader, uint16_t min_value, uint16_t max_value);

/**
 * Reads a 32-bit unsigned integer, ensuring that it falls within the given range.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in a 32-bit unsigned int.
 *
 * Returns min_value if an error occurs.
 */
uint32_t mpack_expect_u32_range(mpack_reader_t* reader, uint32_t min_value, uint32_t max_value);

/**
 * Reads a 64-bit unsigned integer, ensuring that it falls within the given range.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in a 64-bit unsigned int.
 *
 * Returns min_value if an error occurs.
 */
uint64_t mpack_expect_u64_range(mpack_reader_t* reader, uint64_t min_value, uint64_t max_value);

/**
 * Reads an unsigned integer, ensuring that it falls within the given range.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in an unsigned int.
 *
 * Returns min_value if an error occurs.
 */
MPACK_INLINE unsigned int mpack_expect_uint_range(mpack_reader_t* reader, unsigned int min_value, unsigned int max_value) {
    // This should be true at compile-time, so this just wraps the 32-bit
    // function. We fallback to 64-bit if for some reason sizeof(int) isn't 4.
    if (sizeof(unsigned int) == 4)
        return (unsigned int)mpack_expect_u32_range(reader, (uint32_t)min_value, (uint32_t)max_value);
    return (unsigned int)mpack_expect_u64_range(reader, min_value, max_value);
}

/**
 * Reads an 8-bit unsigned integer, ensuring that it is at most @a max_value.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in an 8-bit unsigned int.
 *
 * Returns 0 if an error occurs.
 */
MPACK_INLINE uint8_t mpack_expect_u8_max(mpack_reader_t* reader, uint8_t max_value) {
    return mpack_expect_u8_range(reader, 0, max_value);
}

/**
 * Reads a 16-bit unsigned integer, ensuring that it is at most @a max_value.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in a 16-bit unsigned int.
 *
 * Returns 0 if an error occurs.
 */
MPACK_INLINE uint16_t mpack_expect_u16_max(mpack_reader_t* reader, uint16_t max_value) {
    return mpack_expect_u16_range(reader, 0, max_value);
}

/**
 * Reads a 32-bit unsigned integer, ensuring that it is at most @a max_value.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in a 32-bit unsigned int.
 *
 * Returns 0 if an error occurs.
 */
MPACK_INLINE uint32_t mpack_expect_u32_max(mpack_reader_t* reader, uint32_t max_value) {
    return mpack_expect_u32_range(reader, 0, max_value);
}

/**
 * Reads a 64-bit unsigned integer, ensuring that it is at most @a max_value.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in a 64-bit unsigned int.
 *
 * Returns 0 if an error occurs.
 */
MPACK_INLINE uint64_t mpack_expect_u64_max(mpack_reader_t* reader, uint64_t max_value) {
    return mpack_expect_u64_range(reader, 0, max_value);
}

/**
 * Reads an unsigned integer, ensuring that it is at most @a max_value.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in an unsigned int.
 *
 * Returns 0 if an error occurs.
 */
MPACK_INLINE unsigned int mpack_expect_uint_max(mpack_reader_t* reader, unsigned int max_value) {
    return mpack_expect_uint_range(reader, 0, max_value);
}

/**
 * Reads an 8-bit signed integer, ensuring that it falls within the given range.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in an 8-bit signed int.
 *
 * Returns min_value if an error occurs.
 */
int8_t mpack_expect_i8_range(mpack_reader_t* reader, int8_t min_value, int8_t max_value);

/**
 * Reads a 16-bit signed integer, ensuring that it falls within the given range.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in a 16-bit signed int.
 *
 * Returns min_value if an error occurs.
 */
int16_t mpack_expect_i16_range(mpack_reader_t* reader, int16_t min_value, int16_t max_value);

/**
 * Reads a 32-bit signed integer, ensuring that it falls within the given range.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in a 32-bit signed int.
 *
 * Returns min_value if an error occurs.
 */
int32_t mpack_expect_i32_range(mpack_reader_t* reader, int32_t min_value, int32_t max_value);

/**
 * Reads a 64-bit signed integer, ensuring that it falls within the given range.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in a 64-bit signed int.
 *
 * Returns min_value if an error occurs.
 */
int64_t mpack_expect_i64_range(mpack_reader_t* reader, int64_t min_value, int64_t max_value);

/**
 * Reads a signed integer, ensuring that it falls within the given range.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in a signed int.
 *
 * Returns min_value if an error occurs.
 */
MPACK_INLINE int mpack_expect_int_range(mpack_reader_t* reader, int min_value, int max_value) {
    // This should be true at compile-time, so this just wraps the 32-bit
    // function. We fallback to 64-bit if for some reason sizeof(int) isn't 4.
    if (sizeof(int) == 4)
        return (int)mpack_expect_i32_range(reader, (int32_t)min_value, (int32_t)max_value);
    return (int)mpack_expect_i64_range(reader, min_value, max_value);
}

/**
 * Reads an 8-bit signed integer, ensuring that it is at least zero and at
 * most @a max_value.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in an 8-bit signed int.
 *
 * Returns 0 if an error occurs.
 */
MPACK_INLINE int8_t mpack_expect_i8_max(mpack_reader_t* reader, int8_t max_value) {
    return mpack_expect_i8_range(reader, 0, max_value);
}

/**
 * Reads a 16-bit signed integer, ensuring that it is at least zero and at
 * most @a max_value.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in a 16-bit signed int.
 *
 * Returns 0 if an error occurs.
 */
MPACK_INLINE int16_t mpack_expect_i16_max(mpack_reader_t* reader, int16_t max_value) {
    return mpack_expect_i16_range(reader, 0, max_value);
}

/**
 * Reads a 32-bit signed integer, ensuring that it is at least zero and at
 * most @a max_value.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in a 32-bit signed int.
 *
 * Returns 0 if an error occurs.
 */
MPACK_INLINE int32_t mpack_expect_i32_max(mpack_reader_t* reader, int32_t max_value) {
    return mpack_expect_i32_range(reader, 0, max_value);
}

/**
 * Reads a 64-bit signed integer, ensuring that it is at least zero and at
 * most @a max_value.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in a 64-bit signed int.
 *
 * Returns 0 if an error occurs.
 */
MPACK_INLINE int64_t mpack_expect_i64_max(mpack_reader_t* reader, int64_t max_value) {
    return mpack_expect_i64_range(reader, 0, max_value);
}

/**
 * Reads an int, ensuring that it is at least zero and at most @a max_value.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in a signed int.
 *
 * Returns 0 if an error occurs.
 */
MPACK_INLINE int mpack_expect_int_max(mpack_reader_t* reader, int max_value) {
    return mpack_expect_int_range(reader, 0, max_value);
}

/**
 * Reads a number, ensuring that it falls within the given range and returning
 * the value as a float. The underlying value can be an integer, float or
 * double; the value is converted to a float.
 *
 * @note Reading a double or a large integer with this function can incur a
 * loss of precision.
 *
 * @throws mpack_error_type if the underlying value is not a float, double or integer.
 */
float mpack_expect_float_range(mpack_reader_t* reader, float min_value, float max_value);

/**
 * Reads a number, ensuring that it falls within the given range and returning
 * the value as a double. The underlying value can be an integer, float or
 * double; the value is converted to a double.
 *
 * @note Reading a very large integer with this function can incur a
 * loss of precision.
 *
 * @throws mpack_error_type if the underlying value is not a float, double or integer.
 */
double mpack_expect_double_range(mpack_reader_t* reader, double min_value, double max_value);

/**
 * @}
 */



// These are additional Basic Number functions that wrap inline range functions.

/**
 * @name Basic Number Functions
 * @{
 */

/**
 * Reads an unsigned int.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in an unsigned int.
 *
 * Returns zero if an error occurs.
 */
MPACK_INLINE unsigned int mpack_expect_uint(mpack_reader_t* reader) {

    // This should be true at compile-time, so this just wraps the 32-bit function.
    if (sizeof(unsigned int) == 4)
        return (unsigned int)mpack_expect_u32(reader);

    // Otherwise we wrap the max function to ensure it fits.
    return (unsigned int)mpack_expect_u64_max(reader, UINT_MAX);

}

/**
 * Reads a signed int.
 *
 * The underlying type may be an integer type of any size and signedness,
 * as long as the value can be represented in a signed int.
 *
 * Returns zero if an error occurs.
 */
MPACK_INLINE int mpack_expect_int(mpack_reader_t* reader) {

    // This should be true at compile-time, so this just wraps the 32-bit function.
    if (sizeof(int) == 4)
        return (int)mpack_expect_i32(reader);

    // Otherwise we wrap the range function to ensure it fits.
    return (int)mpack_expect_i64_range(reader, INT_MIN, INT_MAX);

}

/**
 * @}
 */



/**
 * @name Matching Number Functions
 * @{
 */

/**
 * Reads an unsigned integer, ensuring that it exactly matches the given value.
 *
 * mpack_error_type is raised if the value is not representable as an unsigned
 * integer or if it does not exactly match the given value.
 */
void mpack_expect_uint_match(mpack_reader_t* reader, uint64_t value);

/**
 * Reads a signed integer, ensuring that it exactly matches the given value.
 *
 * mpack_error_type is raised if the value is not representable as a signed
 * integer or if it does not exactly match the given value.
 */
void mpack_expect_int_match(mpack_reader_t* reader, int64_t value);

/**
 * @name Other Basic Types
 * @{
 */

/**
 * Reads a nil, raising @ref mpack_error_type if the value is not nil.
 */
void mpack_expect_nil(mpack_reader_t* reader);

/**
 * Reads a boolean.
 *
 * @note Integers will raise mpack_error_type; the value must be strictly a boolean.
 */
bool mpack_expect_bool(mpack_reader_t* reader);

/**
 * Reads a boolean, raising @ref mpack_error_type if its value is not @c true.
 */
void mpack_expect_true(mpack_reader_t* reader);

/**
 * Reads a boolean, raising @ref mpack_error_type if its value is not @c false.
 */
void mpack_expect_false(mpack_reader_t* reader);


/**
 * @}
 */

/**
 * @name Compound Types
 * @{
 */

/**
 * Reads the start of a map, returning its element count.
 *
 * A number of values follow equal to twice the element count of the map,
 * alternating between keys and values. @ref mpack_done_map() must be called
 * once all elements have been read.
 *
 * @note Maps in JSON are unordered, so it is recommended not to expect
 * a specific ordering for your map values in case your data is converted
 * to/from JSON.
 *
 * @warning This call is dangerous! It does not have a size limit, and it
 * does not have any way of checking whether there is enough data in the
 * message (since the data could be coming from a stream.) When looping
 * through the map's contents, you must check for errors on each iteration
 * of the loop. Otherwise an attacker could craft a message declaring a map
 * of a billion elements which would throw your parsing code into an
 * infinite loop! You should strongly consider using mpack_expect_map_max()
 * with a safe maximum size instead.
 *
 * @throws mpack_error_type if the value is not a map.
 */
uint32_t mpack_expect_map(mpack_reader_t* reader);

/**
 * Reads the start of a map with a number of elements in the given range, returning
 * its element count.
 *
 * A number of values follow equal to twice the element count of the map,
 * alternating between keys and values. @ref mpack_done_map() must be called
 * once all elements have been read.
 *
 * @note Maps in JSON are unordered, so it is recommended not to expect
 * a specific ordering for your map values in case your data is converted
 * to/from JSON.
 *
 * min_count is returned if an error occurs.
 *
 * @throws mpack_error_type if the value is not a map or if its size does
 * not fall within the given range.
 */
uint32_t mpack_expect_map_range(mpack_reader_t* reader, uint32_t min_count, uint32_t max_count);

/**
 * Reads the start of a map with a number of elements at most @a max_count,
 * returning its element count.
 *
 * A number of values follow equal to twice the element count of the map,
 * alternating between keys and values. @ref mpack_done_map() must be called
 * once all elements have been read.
 *
 * @note Maps in JSON are unordered, so it is recommended not to expect
 * a specific ordering for your map values in case your data is converted
 * to/from JSON.
 *
 * Zero is returned if an error occurs.
 *
 * @throws mpack_error_type if the value is not a map or if its size is
 * greater than max_count.
 */
MPACK_INLINE uint32_t mpack_expect_map_max(mpack_reader_t* reader, uint32_t max_count) {
    return mpack_expect_map_range(reader, 0, max_count);
}

/**
 * Reads the start of a map of the exact size given.
 *
 * A number of values follow equal to twice the element count of the map,
 * alternating between keys and values. @ref mpack_done_map() must be called
 * once all elements have been read.
 *
 * @note Maps in JSON are unordered, so it is recommended not to expect
 * a specific ordering for your map values in case your data is converted
 * to/from JSON.
 *
 * @throws mpack_error_type if the value is not a map or if its size
 * does not match the given count.
 */
void mpack_expect_map_match(mpack_reader_t* reader, uint32_t count);

/**
 * Reads a nil node or the start of a map, returning whether a map was
 * read and placing its number of key/value pairs in count.
 *
 * If a map was read, a number of values follow equal to twice the element count
 * of the map, alternating between keys and values. @ref mpack_done_map() should
 * also be called once all elements have been read (only if a map was read.)
 *
 * @note Maps in JSON are unordered, so it is recommended not to expect
 * a specific ordering for your map values in case your data is converted
 * to/from JSON.
 *
 * @warning This call is dangerous! It does not have a size limit, and it
 * does not have any way of checking whether there is enough data in the
 * message (since the data could be coming from a stream.) When looping
 * through the map's contents, you must check for errors on each iteration
 * of the loop. Otherwise an attacker could craft a message declaring a map
 * of a billion elements which would throw your parsing code into an
 * infinite loop! You should strongly consider using mpack_expect_map_max_or_nil()
 * with a safe maximum size instead.
 *
 * @returns @c true if a map was read successfully; @c false if nil was read
 *     or an error occured.
 * @throws mpack_error_type if the value is not a nil or map.
 */
bool mpack_expect_map_or_nil(mpack_reader_t* reader, uint32_t* count);

/**
 * Reads a nil node or the start of a map with a number of elements at most
 * max_count, returning whether a map was read and placing its number of
 * key/value pairs in count.
 *
 * If a map was read, a number of values follow equal to twice the element count
 * of the map, alternating between keys and values. @ref mpack_done_map() should
 * anlso be called once all elements have been read (only if a map was read.)
 *
 * @note Maps in JSON are unordered, so it is recommended not to expect
 * a specific ordering for your map values in case your data is converted
 * to/from JSON. Consider using mpack_expect_key_cstr() or mpack_expect_key_uint()
 * to switch on the key; see @ref docs/expect.md for examples.
 *
 * @returns @c true if a map was read successfully; @c false if nil was read
 *     or an error occured.
 * @throws mpack_error_type if the value is not a nil or map.
 */
bool mpack_expect_map_max_or_nil(mpack_reader_t* reader, uint32_t max_count, uint32_t* count);

/**
 * Reads the start of an array, returning its element count.
 *
 * A number of values follow equal to the element count of the array.
 * @ref mpack_done_array() must be called once all elements have been read.
 *
 * @warning This call is dangerous! It does not have a size limit, and it
 * does not have any way of checking whether there is enough data in the
 * message (since the data could be coming from a stream.) When looping
 * through the array's contents, you must check for errors on each iteration
 * of the loop. Otherwise an attacker could craft a message declaring an array
 * of a billion elements which would throw your parsing code into an
 * infinite loop! You should strongly consider using mpack_expect_array_max()
 * with a safe maximum size instead.
 */
uint32_t mpack_expect_array(mpack_reader_t* reader);

/**
 * Reads the start of an array with a number of elements in the given range,
 * returning its element count.
 *
 * A number of values follow equal to the element count of the array.
 * @ref mpack_done_array() must be called once all elements have been read.
 *
 * min_count is returned if an error occurs.
 *
 * @throws mpack_error_type if the value is not an array or if its size does
 * not fall within the given range.
 */
uint32_t mpack_expect_array_range(mpack_reader_t* reader, uint32_t min_count, uint32_t max_count);

/**
 * Reads the start of an array with a number of elements at most @a max_count,
 * returning its element count.
 *
 * A number of values follow equal to the element count of the array.
 * @ref mpack_done_array() must be called once all elements have been read.
 *
 * Zero is returned if an error occurs.
 *
 * @throws mpack_error_type if the value is not an array or if its size is
 * greater than max_count.
 */
MPACK_INLINE uint32_t mpack_expect_array_max(mpack_reader_t* reader, uint32_t max_count) {
    return mpack_expect_array_range(reader, 0, max_count);
}

/**
 * Reads the start of an array of the exact size given.
 *
 * A number of values follow equal to the element count of the array.
 * @ref mpack_done_array() must be called once all elements have been read.
 *
 * @throws mpack_error_type if the value is not an array or if its size does
 * not match the given count.
 */
void mpack_expect_array_match(mpack_reader_t* reader, uint32_t count);

/**
 * Reads a nil node or the start of an array, returning whether an array was
 * read and placing its number of elements in count.
 *
 * If an array was read, a number of values follow equal to the element count
 * of the array. @ref mpack_done_array() should also be called once all elements
 * have been read (only if an array was read.)
 *
 * @warning This call is dangerous! It does not have a size limit, and it
 * does not have any way of checking whether there is enough data in the
 * message (since the data could be coming from a stream.) When looping
 * through the array's contents, you must check for errors on each iteration
 * of the loop. Otherwise an attacker could craft a message declaring an array
 * of a billion elements which would throw your parsing code into an
 * infinite loop! You should strongly consider using mpack_expect_array_max_or_nil()
 * with a safe maximum size instead.
 *
 * @returns @c true if an array was read successfully; @c false if nil was read
 *     or an error occured.
 * @throws mpack_error_type if the value is not a nil or array.
 */
bool mpack_expect_array_or_nil(mpack_reader_t* reader, uint32_t* count);

/**
 * Reads a nil node or the start of an array with a number of elements at most
 * max_count, returning whether an array was read and placing its number of
 * key/value pairs in count.
 *
 * If an array was read, a number of values follow equal to the element count
 * of the array. @ref mpack_done_array() should also be called once all elements
 * have been read (only if an array was read.)
 *
 * @returns @c true if an array was read successfully; @c false if nil was read
 *     or an error occured.
 * @throws mpack_error_type if the value is not a nil or array.
 */
bool mpack_expect_array_max_or_nil(mpack_reader_t* reader, uint32_t max_count, uint32_t* count);

#ifdef MPACK_MALLOC
/**
 * @hideinitializer
 *
 * Reads the start of an array and allocates storage for it, placing its
 * size in out_count. A number of objects follow equal to the element count
 * of the array. You must call @ref mpack_done_array() when done (even
 * if the element count is zero.)
 *
 * If an error occurs, NULL is returned and the reader is placed in an
 * error state.
 *
 * If the count is zero, NULL is returned. This does not indicate error.
 * You should not check the return value for NULL to check for errors; only
 * check the reader's error state.
 *
 * The allocated array must be freed with MPACK_FREE() (or simply free()
 * if MPack's allocator hasn't been customized.)
 *
 * @throws mpack_error_type if the value is not an array or if its size is
 * greater than max_count.
 */
#define mpack_expect_array_alloc(reader, Type, max_count, out_count) \
    ((Type*)mpack_expect_array_alloc_impl(reader, sizeof(Type), max_count, out_count, false))

/**
 * @hideinitializer
 *
 * Reads a nil node or the start of an array and allocates storage for it,
 * placing its size in out_count. A number of objects follow equal to the element
 * count of the array if a non-empty array was read.
 *
 * If an error occurs, NULL is returned and the reader is placed in an
 * error state.
 *
 * If a nil node was read, NULL is returned. If an empty array was read,
 * mpack_done_array() is called automatically and NULL is returned. These
 * do not indicate error. You should not check the return value for NULL
 * to check for errors; only check the reader's error state.
 *
 * The allocated array must be freed with MPACK_FREE() (or simply free()
 * if MPack's allocator hasn't been customized.)
 *
 * @warning You must call @ref mpack_done_array() if and only if a non-zero
 * element count is read. This function does not differentiate between nil
 * and an empty array.
 *
 * @throws mpack_error_type if the value is not an array or if its size is
 * greater than max_count.
 */
#define mpack_expect_array_or_nil_alloc(reader, Type, max_count, out_count) \
    ((Type*)mpack_expect_array_alloc_impl(reader, sizeof(Type), max_count, out_count, true))
#endif

/**
 * @}
 */

/** @cond */
#ifdef MPACK_MALLOC
void* mpack_expect_array_alloc_impl(mpack_reader_t* reader,
        size_t element_size, uint32_t max_count, uint32_t* out_count, bool allow_nil);
#endif
/** @endcond */


/**
 * @name String Functions
 * @{
 */

/**
 * Reads the start of a string, returning its size in bytes.
 *
 * The bytes follow and must be read separately with mpack_read_bytes()
 * or mpack_read_bytes_inplace(). mpack_done_str() must be called
 * once all bytes have been read.
 *
 * NUL bytes are allowed in the string, and no encoding checks are done.
 *
 * mpack_error_type is raised if the value is not a string.
 */
uint32_t mpack_expect_str(mpack_reader_t* reader);

/**
 * Reads a string of at most the given size, writing it into the
 * given buffer and returning its size in bytes.
 *
 * This does not add a null-terminator! Use mpack_expect_cstr() to
 * add a null-terminator.
 *
 * NUL bytes are allowed in the string, and no encoding checks are done.
 */
size_t mpack_expect_str_buf(mpack_reader_t* reader, char* buf, size_t bufsize);

/**
 * Reads a string into the given buffer, ensuring it is a valid UTF-8 string
 * and returning its size in bytes.
 *
 * This does not add a null-terminator! Use mpack_expect_utf8_cstr() to
 * add a null-terminator.
 *
 * This does not accept any UTF-8 variant such as Modified UTF-8, CESU-8 or
 * WTF-8. Only pure UTF-8 is allowed.
 *
 * NUL bytes are allowed in the string (as they are in UTF-8.)
 *
 * Raises mpack_error_too_big if there is not enough room for the string.
 * Raises mpack_error_type if the value is not a string or is not a valid UTF-8 string.
 */
size_t mpack_expect_utf8(mpack_reader_t* reader, char* buf, size_t bufsize);

/**
 * Reads the start of a string, raising an error if its length is not
 * at most the given number of bytes (not including any null-terminator.)
 *
 * The bytes follow and must be read separately with mpack_read_bytes()
 * or mpack_read_bytes_inplace(). @ref mpack_done_str() must be called
 * once all bytes have been read.
 *
 * @throws mpack_error_type If the value is not a string.
 * @throws mpack_error_too_big If the string's length in bytes is larger than the given maximum size.
 */
MPACK_INLINE uint32_t mpack_expect_str_max(mpack_reader_t* reader, uint32_t maxsize) {
    uint32_t length = mpack_expect_str(reader);
    if (length > maxsize) {
        mpack_reader_flag_error(reader, mpack_error_too_big);
        return 0;
    }
    return length;
}

/**
 * Reads the start of a string, raising an error if its length is not
 * exactly the given number of bytes (not including any null-terminator.)
 *
 * The bytes follow and must be read separately with mpack_read_bytes()
 * or mpack_read_bytes_inplace(). @ref mpack_done_str() must be called
 * once all bytes have been read.
 *
 * mpack_error_type is raised if the value is not a string or if its
 * length does not match.
 */
MPACK_INLINE void mpack_expect_str_length(mpack_reader_t* reader, uint32_t count) {
    if (mpack_expect_str(reader) != count)
        mpack_reader_flag_error(reader, mpack_error_type);
}

/**
 * Reads a string, ensuring it exactly matches the given string.
 *
 * Remember that maps are unordered in JSON. Don't use this for map keys
 * unless the map has only a single key!
 */
void mpack_expect_str_match(mpack_reader_t* reader, const char* str, size_t length);

/**
 * Reads a string into the given buffer, ensures it has no null bytes,
 * and adds a null-terminator at the end.
 *
 * Raises mpack_error_too_big if there is not enough room for the string and null-terminator.
 * Raises mpack_error_type if the value is not a string or contains a null byte.
 */
void mpack_expect_cstr(mpack_reader_t* reader, char* buf, size_t size);

/**
 * Reads a string into the given buffer, ensures it is a valid UTF-8 string
 * without NUL characters, and adds a null-terminator at the end.
 *
 * This does not accept any UTF-8 variant such as Modified UTF-8, CESU-8 or
 * WTF-8. Only pure UTF-8 is allowed, but without the NUL character, since
 * it cannot be represented in a null-terminated string.
 *
 * Raises mpack_error_too_big if there is not enough room for the string and null-terminator.
 * Raises mpack_error_type if the value is not a string or is not a valid UTF-8 string.
 */
void mpack_expect_utf8_cstr(mpack_reader_t* reader, char* buf, size_t size);

#ifdef MPACK_MALLOC
/**
 * Reads a string with the given total maximum size (including space for a
 * null-terminator), allocates storage for it, ensures it has no null-bytes,
 * and adds a null-terminator at the end. You assume ownership of the
 * returned pointer if reading succeeds.
 *
 * The allocated string must be freed with MPACK_FREE() (or simply free()
 * if MPack's allocator hasn't been customized.)
 *
 * @throws mpack_error_too_big if the string plus null-terminator is larger than the given maxsize.
 * @throws mpack_error_invalid if the value is not a string or contains a null byte.
 */
char* mpack_expect_cstr_alloc(mpack_reader_t* reader, size_t maxsize);

/**
 * Reads a string with the given total maximum size (including space for a
 * null-terminator), allocates storage for it, ensures it is valid UTF-8
 * with no null-bytes, and adds a null-terminator at the end. You assume
 * ownership of the returned pointer if reading succeeds.
 *
 * The length in bytes of the string, not including the null-terminator,
 * will be written to size.
 *
 * This does not accept any UTF-8 variant such as Modified UTF-8, CESU-8 or
 * WTF-8. Only pure UTF-8 is allowed, but without the NUL character, since
 * it cannot be represented in a null-terminated string.
 *
 * The allocated string must be freed with MPACK_FREE() (or simply free()
 * if MPack's allocator hasn't been customized.)
 * if you want a null-terminator.
 *
 * @throws mpack_error_too_big if the string plus null-terminator is larger
 *     than the given maxsize.
 * @throws mpack_error_invalid if the value is not a string or contains
 *     invalid UTF-8 or a null byte.
 */
char* mpack_expect_utf8_cstr_alloc(mpack_reader_t* reader, size_t maxsize);
#endif

/**
 * Reads a string, ensuring it exactly matches the given null-terminated
 * string.
 *
 * Remember that maps are unordered in JSON. Don't use this for map keys
 * unless the map has only a single key!
 */
MPACK_INLINE void mpack_expect_cstr_match(mpack_reader_t* reader, const char* str) {
    if (mpack_strlen(str) > UINT32_MAX) {
        mpack_reader_flag_error(reader, mpack_error_type);
        return;
    }
    mpack_expect_str_match(reader, str, mpack_strlen(str));
}

/**
 * @}
 */

/**
 * @name Binary Data / Extension Functions
 * @{
 */

/**
 * Reads the start of a binary blob, returning its size in bytes.
 *
 * The bytes follow and must be read separately with mpack_read_bytes()
 * or mpack_read_bytes_inplace(). @ref mpack_done_bin() must be called
 * once all bytes have been read.
 *
 * mpack_error_type is raised if the value is not a binary blob.
 */
uint32_t mpack_expect_bin(mpack_reader_t* reader);

/**
 * Reads the start of a binary blob, raising an error if its length is not
 * at most the given number of bytes.
 *
 * The bytes follow and must be read separately with mpack_read_bytes()
 * or mpack_read_bytes_inplace(). @ref mpack_done_bin() must be called
 * once all bytes have been read.
 *
 * mpack_error_type is raised if the value is not a binary blob or if its
 * length does not match.
 */
MPACK_INLINE uint32_t mpack_expect_bin_max(mpack_reader_t* reader, uint32_t maxsize) {
    uint32_t length = mpack_expect_bin(reader);
    if (length > maxsize) {
        mpack_reader_flag_error(reader, mpack_error_type);
        return 0;
    }
    return length;
}

/**
 * Reads the start of a binary blob, raising an error if its length is not
 * exactly the given number of bytes.
 *
 * The bytes follow and must be read separately with mpack_read_bytes()
 * or mpack_read_bytes_inplace(). @ref mpack_done_bin() must be called
 * once all bytes have been read.
 *
 * mpack_error_type is raised if the value is not a binary blob or if its
 * length does not match.
 */
MPACK_INLINE void mpack_expect_bin_size(mpack_reader_t* reader, uint32_t count) {
    if (mpack_expect_bin(reader) != count)
        mpack_reader_flag_error(reader, mpack_error_type);
}

/**
 * Reads a binary blob into the given buffer, returning its size in bytes.
 *
 * For compatibility, this will accept if the underlying type is string or
 * binary (since in MessagePack 1.0, strings and binary data were combined
 * under the "raw" type which became string in 1.1.)
 */
size_t mpack_expect_bin_buf(mpack_reader_t* reader, char* buf, size_t size);

/**
 * Reads a binary blob with the given total maximum size, allocating storage for it.
 */
char* mpack_expect_bin_alloc(mpack_reader_t* reader, size_t maxsize, size_t* size);

/**
 * @}
 */

/**
 * @name Special Functions
 * @{
 */

/**
 * Reads a MessagePack object header (an MPack tag), expecting it to exactly
 * match the given tag.
 *
 * If the type is compound (i.e. is a map, array, string, binary or
 * extension type), additional reads are required to get the contained
 * data, and the corresponding done function must be called when done.
 *
 * @throws mpack_error_type if the tag does not match
 *
 * @see mpack_read_bytes()
 * @see mpack_done_array()
 * @see mpack_done_map()
 * @see mpack_done_str()
 * @see mpack_done_bin()
 * @see mpack_done_ext()
 */
void mpack_expect_tag(mpack_reader_t* reader, mpack_tag_t tag);

/**
 * Expects a string matching one of the strings in the given array,
 * returning its array index.
 *
 * If the value does not match any of the given strings,
 * @ref mpack_error_type is flagged. Use mpack_expect_enum_optional()
 * if you want to allow other values than the given strings.
 *
 * If any error occurs or the reader is in an error state, @a count
 * is returned.
 *
 * This can be used to quickly parse a string into an enum when the
 * enum values range from 0 to @a count-1. If the last value in the
 * enum is a special "count" value, it can be passed as the count,
 * and the return value can be cast directly to the enum type.
 *
 * @code{.c}
 * typedef enum           { APPLE ,  BANANA ,  ORANGE , COUNT} fruit_t;
 * const char* fruits[] = {"apple", "banana", "orange"};
 *
 * fruit_t fruit = (fruit_t)mpack_expect_enum(reader, fruits, COUNT);
 * @endcode
 *
 * See @ref docs/expect.md for more examples.
 *
 * The maximum string length is the size of the buffer (strings are read in-place.)
 *
 * @param reader The reader
 * @param strings An array of expected strings of length count
 * @param count The number of strings
 * @return The index of the matched string, or @a count in case of error
 */
size_t mpack_expect_enum(mpack_reader_t* reader, const char* strings[], size_t count);

/**
 * Expects a string matching one of the strings in the given array
 * returning its array index, or @a count if no strings match.
 *
 * If the value is not a string, or it does not match any of the
 * given strings, @a count is returned and no error is flagged.
 *
 * If any error occurs or the reader is in an error state, @a count
 * is returned.
 *
 * This can be used to quickly parse a string into an enum when the
 * enum values range from 0 to @a count-1. If the last value in the
 * enum is a special "count" value, it can be passed as the count,
 * and the return value can be cast directly to the enum type.
 *
 * @code{.c}
 * typedef enum           { APPLE ,  BANANA ,  ORANGE , COUNT} fruit_t;
 * const char* fruits[] = {"apple", "banana", "orange"};
 *
 * fruit_t fruit = (fruit_t)mpack_expect_enum_optional(reader, fruits, COUNT);
 * @endcode
 *
 * See @ref docs/expect.md for more examples.
 *
 * The maximum string length is the size of the buffer (strings are read in-place.)
 *
 * @param reader The reader
 * @param strings An array of expected strings of length count
 * @param count The number of strings
 *
 * @return The index of the matched string, or @a count if it does not
 * match or an error occurs
 */
size_t mpack_expect_enum_optional(mpack_reader_t* reader, const char* strings[], size_t count);

/**
 * Expects an unsigned integer map key between 0 and count-1, marking it
 * as found in the given bool array and returning it.
 *
 * This is a helper for switching among int keys in a map. It is
 * typically used with an enum to define the key values. It should
 * be called in the expression of a switch() statement. See @ref
 * docs/expect.md for an example.
 *
 * The found array must be cleared before expecting the first key. If the
 * flag for a given key is already set when found (i.e. the map contains a
 * duplicate key), mpack_error_invalid is flagged.
 *
 * If the key is not a non-negative integer, or if the key is @a count or
 * larger, @a count is returned and no error is flagged. If you want an error
 * on unrecognized keys, flag an error in the default case in your switch;
 * otherwise you must call mpack_discard() to discard its content.
 *
 * @param reader The reader
 * @param found An array of bool flags of length count
 * @param count The number of values in the found array, and one more than the
 *              maximum allowed key
 *
 * @see @ref docs/expect.md
 */
size_t mpack_expect_key_uint(mpack_reader_t* reader, bool found[], size_t count);

/**
 * Expects a string map key matching one of the strings in the given key list,
 * marking it as found in the given bool array and returning its index.
 *
 * This is a helper for switching among string keys in a map. It is
 * typically used with an enum with names matching the strings in the
 * array to define the key indices. It should be called in the expression
 * of a switch() statement. See @ref docs/expect.md for an example.
 *
 * The found array must be cleared before expecting the first key. If the
 * flag for a given key is already set when found (i.e. the map contains a
 * duplicate key), mpack_error_invalid is flagged.
 *
 * If the key is unrecognized, count is returned and no error is flagged. If
 * you want an error on unrecognized keys, flag an error in the default case
 * in your switch; otherwise you must call mpack_discard() to discard its content.
 *
 * The maximum key length is the size of the buffer (keys are read in-place.)
 *
 * @param reader The reader
 * @param keys An array of expected string keys of length count
 * @param found An array of bool flags of length count
 * @param count The number of values in the keys and found arrays
 *
 * @see @ref docs/expect.md
 */
size_t mpack_expect_key_cstr(mpack_reader_t* reader, const char* keys[],
        bool found[], size_t count);

/**
 * @}
 */

/**
 * @}
 */

#endif

MPACK_HEADER_END

#endif



/* mpack-node.h */

/**
 * @file
 *
 * Declares the MPack dynamic Node API.
 */

#ifndef MPACK_NODE_H
#define MPACK_NODE_H 1

/* #include "mpack-reader.h" */

MPACK_HEADER_START

#if MPACK_NODE

/**
 * @defgroup node Node API
 *
 * The MPack Node API allows you to parse a chunk of MessagePack data
 * in-place into a dynamically typed data structure.
 *
 * @{
 */

/**
 * A handle to node data in a parsed MPack tree.
 *
 * Nodes represent either primitive values or compound types. If a
 * node is a compound type, it contains a pointer to its child nodes,
 * or a pointer to its underlying data.
 *
 * Nodes are immutable.
 *
 * @note @ref mpack_node_t is a handle, not the node data itself. It
 *     is passed by value in the Node API.
 */
typedef struct mpack_node_t mpack_node_t;

/**
 * The storage for nodes in an MPack tree.
 *
 * You only need to use this if you intend to provide your own storage
 * for nodes instead of letting the tree allocate it.
 */
typedef struct mpack_node_data_t mpack_node_data_t;

/**
 * An MPack tree parsed from a blob of MessagePack.
 *
 * The tree contains a single root node which contains all parsed data.
 * The tree and its nodes are immutable.
 */
typedef struct mpack_tree_t mpack_tree_t;

/**
 * An error handler function to be called when an error is flagged on
 * the tree.
 *
 * The error handler will only be called once on the first error flagged;
 * any subsequent node reads and errors are ignored, and the tree is
 * permanently in that error state.
 *
 * MPack is safe against non-local jumps out of error handler callbacks.
 * This means you are allowed to longjmp or throw an exception (in C++,
 * Objective-C, or with SEH) out of this callback.
 *
 * Bear in mind when using longjmp that local non-volatile variables that
 * have changed are undefined when setjmp() returns, so you can't put the
 * tree on the stack in the same activation frame as the setjmp without
 * declaring it volatile.
 *
 * You must still eventually destroy the tree. It is not destroyed
 * automatically when an error is flagged. It is safe to destroy the
 * tree within this error callback, but you will either need to perform
 * a non-local jump, or store something in your context to identify
 * that the tree is destroyed since any future accesses to it cause
 * undefined behavior.
 */
typedef void (*mpack_tree_error_t)(mpack_tree_t* tree, mpack_error_t error);

/**
 * A teardown function to be called when the tree is destroyed.
 */
typedef void (*mpack_tree_teardown_t)(mpack_tree_t* tree);



/* Hide internals from documentation */
/** @cond */

struct mpack_node_t {
    mpack_node_data_t* data;
    mpack_tree_t* tree;
};

struct mpack_node_data_t {
    mpack_type_t type;

    /*
     * The element count if the type is an array;
     * the number of key/value pairs if the type is map;
     * or the number of bytes if the type is str, bin or ext.
     */
    uint32_t len;

    union
    {
        bool     b; /* The value if the type is bool. */
        float    f; /* The value if the type is float. */
        double   d; /* The value if the type is double. */
        int64_t  i; /* The value if the type is signed int. */
        uint64_t u; /* The value if the type is unsigned int. */
        const char* bytes; /* The byte pointer for str, bin and ext */
        mpack_node_data_t* children; /* The children for map or array */
    } value;
};

typedef struct mpack_tree_page_t {
    struct mpack_tree_page_t* next;
    mpack_node_data_t nodes[1]; // variable size
} mpack_tree_page_t;

struct mpack_tree_t {
    mpack_tree_error_t error_fn;    /* Function to call on error */
    mpack_tree_teardown_t teardown; /* Function to teardown the context on destroy */
    void* context;                  /* Context for tree callbacks */

    mpack_node_data_t nil_node; /* a nil node to be returned in case of error */
    mpack_error_t error;

    size_t node_count;
    size_t size;

    mpack_node_data_t* root;

    #ifdef MPACK_MALLOC
    mpack_tree_page_t* next;
    #endif
};

// internal functions

MPACK_INLINE mpack_node_t mpack_node(mpack_tree_t* tree, mpack_node_data_t* data) {
    mpack_node_t node;
    node.data = data;
    node.tree = tree;
    return node;
}

MPACK_INLINE mpack_node_data_t* mpack_node_child(mpack_node_t node, size_t child) {
    return node.data->value.children + child;
}

MPACK_INLINE mpack_node_t mpack_tree_nil_node(mpack_tree_t* tree) {
    return mpack_node(tree, &tree->nil_node);
}

/** @endcond */



/**
 * @name Tree Functions
 * @{
 */

#ifdef MPACK_MALLOC
/**
 * Initializes a tree by parsing the given data buffer. The tree must be destroyed
 * with mpack_tree_destroy(), even if parsing fails.
 *
 * The tree will allocate pages of nodes as needed, and free them when destroyed.
 *
 * Any string or blob data types reference the original data, so the data
 * pointer must remain valid until after the tree is destroyed.
 */
void mpack_tree_init(mpack_tree_t* tree, const char* data, size_t length);
#endif

/**
 * Initializes a tree by parsing the given data buffer, using the given
 * node data pool to store the results.
 *
 * If the data does not fit in the pool, mpack_error_too_big will be flagged
 * on the tree.
 *
 * The tree must be destroyed with mpack_tree_destroy(), even if parsing fails.
 */
void mpack_tree_init_pool(mpack_tree_t* tree, const char* data, size_t length, mpack_node_data_t* node_pool, size_t node_pool_count);

/**
 * Initializes an MPack tree directly into an error state. Use this if you
 * are writing a wrapper to mpack_tree_init() which can fail its setup.
 */
void mpack_tree_init_error(mpack_tree_t* tree, mpack_error_t error);

#if MPACK_STDIO
/**
 * Initializes a tree by reading and parsing the given file. The tree must be
 * destroyed with mpack_tree_destroy(), even if parsing fails.
 *
 * The file is opened, loaded fully into memory, and closed before this call
 * returns.
 *
 * @param tree The tree to initialize
 * @param filename The filename passed to fopen() to read the file
 * @param max_bytes The maximum size of file to load, or 0 for unlimited size.
 */
void mpack_tree_init_file(mpack_tree_t* tree, const char* filename, size_t max_bytes);
#endif

/**
 * Returns the root node of the tree, if the tree is not in an error state.
 * Returns a nil node otherwise.
 */
mpack_node_t mpack_tree_root(mpack_tree_t* tree);

/**
 * Returns the error state of the tree.
 */
MPACK_INLINE mpack_error_t mpack_tree_error(mpack_tree_t* tree) {
    return tree->error;
}

/**
 * Returns the number of bytes used in the buffer when the tree was
 * parsed. If there is something in the buffer after the MessagePack
 * object (such as another object), this can be used to find it.
 *
 * This is zero if an error occurred during tree parsing (since the
 * portion of the data that the first complete object occupies cannot
 * be determined if the data is invalid or corrupted.)
 */
MPACK_INLINE size_t mpack_tree_size(mpack_tree_t* tree) {
    return tree->size;
}

/**
 * Destroys the tree.
 */
mpack_error_t mpack_tree_destroy(mpack_tree_t* tree);

/**
 * Sets the custom pointer to pass to the tree callbacks, such as teardown.
 *
 * @param tree The MPack tree.
 * @param context User data to pass to the tree callbacks.
 */
MPACK_INLINE void mpack_tree_set_context(mpack_tree_t* tree, void* context) {
    tree->context = context;
}

/**
 * Sets the error function to call when an error is flagged on the tree.
 *
 * This should normally be used with mpack_tree_set_context() to register
 * a custom pointer to pass to the error function.
 *
 * See the definition of mpack_tree_error_t for more information about
 * what you can do from an error callback.
 *
 * @see mpack_tree_error_t
 * @param tree The MPack tree.
 * @param error_fn The function to call when an error is flagged on the tree.
 */
MPACK_INLINE void mpack_tree_set_error_handler(mpack_tree_t* tree, mpack_tree_error_t error_fn) {
    tree->error_fn = error_fn;
}

/**
 * Sets the teardown function to call when the tree is destroyed.
 *
 * This should normally be used with mpack_tree_set_context() to register
 * a custom pointer to pass to the teardown function.
 *
 * @param tree The MPack tree.
 * @param teardown The function to call when the tree is destroyed.
 */
MPACK_INLINE void mpack_tree_set_teardown(mpack_tree_t* tree, mpack_tree_teardown_t teardown) {
    tree->teardown = teardown;
}

/**
 * Places the tree in the given error state, calling the error callback if one
 * is set.
 *
 * This allows you to externally flag errors, for example if you are validating
 * data as you read it.
 *
 * If the tree is already in an error state, this call is ignored and no
 * error callback is called.
 */
void mpack_tree_flag_error(mpack_tree_t* tree, mpack_error_t error);

/**
 * Places the node's tree in the given error state, calling the error callback
 * if one is set.
 *
 * This allows you to externally flag errors, for example if you are validating
 * data as you read it.
 *
 * If the tree is already in an error state, this call is ignored and no
 * error callback is called.
 */
void mpack_node_flag_error(mpack_node_t node, mpack_error_t error);

/**
 * @}
 */

/**
 * @name Node Core Functions
 * @{
 */

/**
 * Returns the error state of the node's tree.
 */
MPACK_INLINE mpack_error_t mpack_node_error(mpack_node_t node) {
    return mpack_tree_error(node.tree);
}

/**
 * Returns a tag describing the given node, or a nil tag if the
 * tree is in an error state.
 */
mpack_tag_t mpack_node_tag(mpack_node_t node);

#if MPACK_STDIO
/**
 * Converts a node to pseudo-JSON for debugging purposes
 * and pretty-prints it to the given file.
 */
void mpack_node_print_file(mpack_node_t node, FILE* file);

#ifndef MPACK_GCOV
/**
 * Converts a node to pseudo-JSON for debugging purposes
 * and pretty-prints it to stdout.
 */
MPACK_INLINE void mpack_node_print(mpack_node_t node) {
    mpack_node_print_file(node, stdout);
}
#endif
#endif

/**
 * @}
 */

/**
 * @name Node Primitive Value Functions
 * @{
 */

/**
 * Returns the type of the node.
 */
MPACK_INLINE mpack_type_t mpack_node_type(mpack_node_t node) {
    if (mpack_node_error(node) != mpack_ok)
        return mpack_type_nil;
    return node.data->type;
}

/**
 * Checks if the given node is of nil type, raising mpack_error_type otherwise.
 */
MPACK_INLINE void mpack_node_nil(mpack_node_t node) {
    if (mpack_node_error(node) != mpack_ok)
        return;
    if (node.data->type != mpack_type_nil)
        mpack_node_flag_error(node, mpack_error_type);
}

/**
 * Returns the bool value of the node. If this node is not of the correct
 * type, false is returned and mpack_error_type is raised.
 */
MPACK_INLINE bool mpack_node_bool(mpack_node_t node) {
    if (mpack_node_error(node) != mpack_ok)
        return false;

    if (node.data->type == mpack_type_bool)
        return node.data->value.b;

    mpack_node_flag_error(node, mpack_error_type);
    return false;
}

/**
 * Checks if the given node is of bool type with value true, raising
 * mpack_error_type otherwise.
 */
MPACK_INLINE void mpack_node_true(mpack_node_t node) {
    if (mpack_node_bool(node) != true)
        mpack_node_flag_error(node, mpack_error_type);
}

/**
 * Checks if the given node is of bool type with value false, raising
 * mpack_error_type otherwise.
 */
MPACK_INLINE void mpack_node_false(mpack_node_t node) {
    if (mpack_node_bool(node) != false)
        mpack_node_flag_error(node, mpack_error_type);
}

/**
 * Returns the 8-bit unsigned value of the node. If this node is not
 * of a compatible type, mpack_error_type is raised and zero is returned.
 */
MPACK_INLINE uint8_t mpack_node_u8(mpack_node_t node) {
    if (mpack_node_error(node) != mpack_ok)
        return 0;

    if (node.data->type == mpack_type_uint) {
        if (node.data->value.u <= UINT8_MAX)
            return (uint8_t)node.data->value.u;
    } else if (node.data->type == mpack_type_int) {
        if (node.data->value.i >= 0 && node.data->value.i <= UINT8_MAX)
            return (uint8_t)node.data->value.i;
    }

    mpack_node_flag_error(node, mpack_error_type);
    return 0;
}

/**
 * Returns the 8-bit signed value of the node. If this node is not
 * of a compatible type, mpack_error_type is raised and zero is returned.
 */
MPACK_INLINE int8_t mpack_node_i8(mpack_node_t node) {
    if (mpack_node_error(node) != mpack_ok)
        return 0;

    if (node.data->type == mpack_type_uint) {
        if (node.data->value.u <= INT8_MAX)
            return (int8_t)node.data->value.u;
    } else if (node.data->type == mpack_type_int) {
        if (node.data->value.i >= INT8_MIN && node.data->value.i <= INT8_MAX)
            return (int8_t)node.data->value.i;
    }

    mpack_node_flag_error(node, mpack_error_type);
    return 0;
}

/**
 * Returns the 16-bit unsigned value of the node. If this node is not
 * of a compatible type, mpack_error_type is raised and zero is returned.
 */
MPACK_INLINE uint16_t mpack_node_u16(mpack_node_t node) {
    if (mpack_node_error(node) != mpack_ok)
        return 0;

    if (node.data->type == mpack_type_uint) {
        if (node.data->value.u <= UINT16_MAX)
            return (uint16_t)node.data->value.u;
    } else if (node.data->type == mpack_type_int) {
        if (node.data->value.i >= 0 && node.data->value.i <= UINT16_MAX)
            return (uint16_t)node.data->value.i;
    }

    mpack_node_flag_error(node, mpack_error_type);
    return 0;
}

/**
 * Returns the 16-bit signed value of the node. If this node is not
 * of a compatible type, mpack_error_type is raised and zero is returned.
 */
MPACK_INLINE int16_t mpack_node_i16(mpack_node_t node) {
    if (mpack_node_error(node) != mpack_ok)
        return 0;

    if (node.data->type == mpack_type_uint) {
        if (node.data->value.u <= INT16_MAX)
            return (int16_t)node.data->value.u;
    } else if (node.data->type == mpack_type_int) {
        if (node.data->value.i >= INT16_MIN && node.data->value.i <= INT16_MAX)
            return (int16_t)node.data->value.i;
    }

    mpack_node_flag_error(node, mpack_error_type);
    return 0;
}

/**
 * Returns the 32-bit unsigned value of the node. If this node is not
 * of a compatible type, mpack_error_type is raised and zero is returned.
 */
MPACK_INLINE uint32_t mpack_node_u32(mpack_node_t node) {
    if (mpack_node_error(node) != mpack_ok)
        return 0;

    if (node.data->type == mpack_type_uint) {
        if (node.data->value.u <= UINT32_MAX)
            return (uint32_t)node.data->value.u;
    } else if (node.data->type == mpack_type_int) {
        if (node.data->value.i >= 0 && node.data->value.i <= UINT32_MAX)
            return (uint32_t)node.data->value.i;
    }

    mpack_node_flag_error(node, mpack_error_type);
    return 0;
}

/**
 * Returns the 32-bit signed value of the node. If this node is not
 * of a compatible type, mpack_error_type is raised and zero is returned.
 */
MPACK_INLINE int32_t mpack_node_i32(mpack_node_t node) {
    if (mpack_node_error(node) != mpack_ok)
        return 0;

    if (node.data->type == mpack_type_uint) {
        if (node.data->value.u <= INT32_MAX)
            return (int32_t)node.data->value.u;
    } else if (node.data->type == mpack_type_int) {
        if (node.data->value.i >= INT32_MIN && node.data->value.i <= INT32_MAX)
            return (int32_t)node.data->value.i;
    }

    mpack_node_flag_error(node, mpack_error_type);
    return 0;
}

/**
 * Returns the 64-bit unsigned value of the node. If this node is not
 * of a compatible type, mpack_error_type is raised, and zero is returned.
 */
MPACK_INLINE uint64_t mpack_node_u64(mpack_node_t node) {
    if (mpack_node_error(node) != mpack_ok)
        return 0;

    if (node.data->type == mpack_type_uint) {
        return node.data->value.u;
    } else if (node.data->type == mpack_type_int) {
        if (node.data->value.i >= 0)
            return (uint64_t)node.data->value.i;
    }

    mpack_node_flag_error(node, mpack_error_type);
    return 0;
}

/**
 * Returns the 64-bit signed value of the node. If this node is not
 * of a compatible type, mpack_error_type is raised and zero is returned.
 */
MPACK_INLINE int64_t mpack_node_i64(mpack_node_t node) {
    if (mpack_node_error(node) != mpack_ok)
        return 0;

    if (node.data->type == mpack_type_uint) {
        if (node.data->value.u <= (uint64_t)INT64_MAX)
            return (int64_t)node.data->value.u;
    } else if (node.data->type == mpack_type_int) {
        return node.data->value.i;
    }

    mpack_node_flag_error(node, mpack_error_type);
    return 0;
}

/**
 * Returns the unsigned int value of the node.
 *
 * Returns zero if an error occurs.
 *
 * @throws mpack_error_type If the node is not an integer type or does not fit in the range of an unsigned int
 */
MPACK_INLINE unsigned int mpack_node_uint(mpack_node_t node) {

    // This should be true at compile-time, so this just wraps the 32-bit function.
    if (sizeof(unsigned int) == 4)
        return (unsigned int)mpack_node_u32(node);

    // Otherwise we use u64 and check the range.
    uint64_t val = mpack_node_u64(node);
    if (val <= UINT_MAX)
        return (unsigned int)val;

    mpack_node_flag_error(node, mpack_error_type);
    return 0;
}

/**
 * Returns the int value of the node.
 *
 * Returns zero if an error occurs.
 *
 * @throws mpack_error_type If the node is not an integer type or does not fit in the range of an int
 */
MPACK_INLINE int mpack_node_int(mpack_node_t node) {

    // This should be true at compile-time, so this just wraps the 32-bit function.
    if (sizeof(int) == 4)
        return (int)mpack_node_i32(node);

    // Otherwise we use i64 and check the range.
    int64_t val = mpack_node_i64(node);
    if (val >= INT_MIN && val <= INT_MAX)
        return (int)val;

    mpack_node_flag_error(node, mpack_error_type);
    return 0;
}

/**
 * Returns the float value of the node. The underlying value can be an
 * integer, float or double; the value is converted to a float.
 *
 * @note Reading a double or a large integer with this function can incur a
 * loss of precision.
 *
 * @throws mpack_error_type if the underlying value is not a float, double or integer.
 */
MPACK_INLINE float mpack_node_float(mpack_node_t node) {
    if (mpack_node_error(node) != mpack_ok)
        return 0.0f;

    if (node.data->type == mpack_type_uint)
        return (float)node.data->value.u;
    else if (node.data->type == mpack_type_int)
        return (float)node.data->value.i;
    else if (node.data->type == mpack_type_float)
        return node.data->value.f;
    else if (node.data->type == mpack_type_double)
        return (float)node.data->value.d;

    mpack_node_flag_error(node, mpack_error_type);
    return 0.0f;
}

/**
 * Returns the double value of the node. The underlying value can be an
 * integer, float or double; the value is converted to a double.
 *
 * @note Reading a very large integer with this function can incur a
 * loss of precision.
 *
 * @throws mpack_error_type if the underlying value is not a float, double or integer.
 */
MPACK_INLINE double mpack_node_double(mpack_node_t node) {
    if (mpack_node_error(node) != mpack_ok)
        return 0.0;

    if (node.data->type == mpack_type_uint)
        return (double)node.data->value.u;
    else if (node.data->type == mpack_type_int)
        return (double)node.data->value.i;
    else if (node.data->type == mpack_type_float)
        return (double)node.data->value.f;
    else if (node.data->type == mpack_type_double)
        return node.data->value.d;

    mpack_node_flag_error(node, mpack_error_type);
    return 0.0;
}

/**
 * Returns the float value of the node. The underlying value must be a float,
 * not a double or an integer. This ensures no loss of precision can occur.
 *
 * @throws mpack_error_type if the underlying value is not a float.
 */
MPACK_INLINE float mpack_node_float_strict(mpack_node_t node) {
    if (mpack_node_error(node) != mpack_ok)
        return 0.0f;

    if (node.data->type == mpack_type_float)
        return node.data->value.f;

    mpack_node_flag_error(node, mpack_error_type);
    return 0.0f;
}

/**
 * Returns the double value of the node. The underlying value must be a float
 * or double, not an integer. This ensures no loss of precision can occur.
 *
 * @throws mpack_error_type if the underlying value is not a float or double.
 */
MPACK_INLINE double mpack_node_double_strict(mpack_node_t node) {
    if (mpack_node_error(node) != mpack_ok)
        return 0.0;

    if (node.data->type == mpack_type_float)
        return (double)node.data->value.f;
    else if (node.data->type == mpack_type_double)
        return node.data->value.d;

    mpack_node_flag_error(node, mpack_error_type);
    return 0.0;
}

/**
 * @}
 */

/**
 * @name Node String and Data Functions
 * @{
 */

/**
 * Checks that the given node contains a valid UTF-8 string.
 *
 * If the string is invalid, this flags an error, which would cause subsequent calls
 * to mpack_node_str() to return NULL and mpack_node_strlen() to return zero. So you
 * can check the node for error immediately after calling this, or you can call those
 * functions to use the data anyway and check for errors later.
 *
 * @throws mpack_error_type If this node is not a string or does not contain valid UTF-8.
 *
 * @param node The string node to test
 *
 * @see mpack_node_str()
 * @see mpack_node_strlen()
 */
void mpack_node_check_utf8(mpack_node_t node);

/**
 * Checks that the given node contains a valid UTF-8 string with no NUL bytes.
 *
 * This does not check that the string has a null-terminator! It only checks whether
 * the string could safely be represented as a C-string by appending a null-terminator.
 * (If the string does already contain a null-terminator, this will flag an error.)
 *
 * This is performed automatically by other UTF-8 cstr helper functions. Only
 * call this if you will do something else with the data directly, but you still
 * want to ensure it will be valid as a UTF-8 C-string.
 *
 * @throws mpack_error_type If this node is not a string, does not contain valid UTF-8,
 *     or contains a NUL byte.
 *
 * @param node The string node to test
 *
 * @see mpack_node_str()
 * @see mpack_node_strlen()
 * @see mpack_node_copy_utf8_cstr()
 * @see mpack_node_utf8_cstr_alloc()
 */
void mpack_node_check_utf8_cstr(mpack_node_t node);

/**
 * Returns the extension type of the given ext node.
 *
 * This returns zero if the tree is in an error state.
 */
MPACK_INLINE int8_t mpack_node_exttype(mpack_node_t node) {
    if (mpack_node_error(node) != mpack_ok)
        return 0;

    // the exttype of an ext node is stored in the byte preceding the data
    if (node.data->type == mpack_type_ext)
        return (int8_t)*(node.data->value.bytes - 1);

    mpack_node_flag_error(node, mpack_error_type);
    return 0;
}

/**
 * Returns the length of the given str, bin or ext node.
 *
 * This returns zero if the tree is in an error state.
 */
MPACK_INLINE uint32_t mpack_node_data_len(mpack_node_t node) {
    if (mpack_node_error(node) != mpack_ok)
        return 0;

    mpack_type_t type = node.data->type;
    if (type == mpack_type_str || type == mpack_type_bin || type == mpack_type_ext)
        return (uint32_t)node.data->len;

    mpack_node_flag_error(node, mpack_error_type);
    return 0;
}

/**
 * Returns the length in bytes of the given string node. This does not
 * include any null-terminator.
 *
 * This returns zero if the tree is in an error state.
 */
MPACK_INLINE size_t mpack_node_strlen(mpack_node_t node) {
    if (mpack_node_error(node) != mpack_ok)
        return 0;

    if (node.data->type == mpack_type_str)
        return (size_t)node.data->len;

    mpack_node_flag_error(node, mpack_error_type);
    return 0;
}

/**
 * Returns a pointer to the data contained by this node, ensuring it is a string.
 *
 * @note Strings are not null-terminated! Use one of the cstr functions
 * to get a null-terminated string.
 *
 * The pointer is valid as long as the data backing the tree is valid.
 *
 * If this node is not a string, @ref mpack_error_type is raised and @c NULL is returned.
 *
 * @see mpack_node_copy_cstr()
 * @see mpack_node_cstr_alloc()
 * @see mpack_node_utf8_cstr_alloc()
 */
MPACK_INLINE const char* mpack_node_str(mpack_node_t node) {
    if (mpack_node_error(node) != mpack_ok)
        return NULL;

    mpack_type_t type = node.data->type;
    if (type == mpack_type_str)
        return node.data->value.bytes;

    mpack_node_flag_error(node, mpack_error_type);
    return NULL;
}

/**
 * Returns a pointer to the data contained by this node.
 *
 * @note Strings are not null-terminated! Use one of the cstr functions
 * to get a null-terminated string.
 *
 * The pointer is valid as long as the data backing the tree is valid.
 *
 * If this node is not of a str, bin or map, mpack_error_type is raised, and
 * @c NULL is returned.
 *
 * @see mpack_node_copy_cstr()
 * @see mpack_node_cstr_alloc()
 * @see mpack_node_utf8_cstr_alloc()
 */
MPACK_INLINE const char* mpack_node_data(mpack_node_t node) {
    if (mpack_node_error(node) != mpack_ok)
        return NULL;

    mpack_type_t type = node.data->type;
    if (type == mpack_type_str || type == mpack_type_bin || type == mpack_type_ext)
        return node.data->value.bytes;

    mpack_node_flag_error(node, mpack_error_type);
    return NULL;
}

/**
 * Copies the bytes contained by this node into the given buffer, returning the
 * number of bytes in the node.
 *
 * @throws mpack_error_type If this node is not a str, bin or ext type
 * @throws mpack_error_too_big If the string does not fit in the given buffer
 *
 * @param node The string node from which to copy data
 * @param buffer A buffer in which to copy the node's bytes
 * @param bufsize The size of the given buffer
 *
 * @return The number of bytes in the node, or zero if an error occurs.
 */
size_t mpack_node_copy_data(mpack_node_t node, char* buffer, size_t bufsize);

/**
 * Checks that the given node contains a valid UTF-8 string and copies the
 * string into the given buffer, returning the number of bytes in the string.
 *
 * @throws mpack_error_type If this node is not a string
 * @throws mpack_error_too_big If the string does not fit in the given buffer
 *
 * @param node The string node from which to copy data
 * @param buffer A buffer in which to copy the node's bytes
 * @param bufsize The size of the given buffer
 *
 * @return The number of bytes in the node, or zero if an error occurs.
 */
size_t mpack_node_copy_utf8(mpack_node_t node, char* buffer, size_t bufsize);

/**
 * Checks that the given node contains a string with no NUL bytes, copies the string
 * into the given buffer, and adds a null terminator.
 *
 * If this node is not of a string type, mpack_error_type is raised. If the string
 * does not fit, mpack_error_data is raised.
 *
 * If any error occurs, the buffer will contain an empty null-terminated string.
 *
 * @param node The string node from which to copy data
 * @param buffer A buffer in which to copy the node's string
 * @param size The size of the given buffer
 */
void mpack_node_copy_cstr(mpack_node_t node, char* buffer, size_t size);

/**
 * Checks that the given node contains a valid UTF-8 string with no NUL bytes,
 * copies the string into the given buffer, and adds a null terminator.
 *
 * If this node is not of a string type, mpack_error_type is raised. If the string
 * does not fit, mpack_error_data is raised.
 *
 * If any error occurs, the buffer will contain an empty null-terminated string.
 *
 * @param node The string node from which to copy data
 * @param buffer A buffer in which to copy the node's string
 * @param size The size of the given buffer
 */
void mpack_node_copy_utf8_cstr(mpack_node_t node, char* buffer, size_t size);

#ifdef MPACK_MALLOC
/**
 * Allocates a new chunk of data using MPACK_MALLOC with the bytes
 * contained by this node.
 *
 * The allocated data must be freed with MPACK_FREE() (or simply free()
 * if MPack's allocator hasn't been customized.)
 *
 * @throws mpack_error_type If this node is not a str, bin or ext type
 * @throws mpack_error_too_big If the size of the data is larger than the
 *     given maximum size
 * @throws mpack_error_memory If an allocation failure occurs
 *
 * @param node The node from which to allocate and copy data
 * @param maxsize The maximum size to allocate
 *
 * @return The allocated data, or NULL if any error occurs.
 */
char* mpack_node_data_alloc(mpack_node_t node, size_t maxsize);

/**
 * Allocates a new null-terminated string using MPACK_MALLOC with the string
 * contained by this node.
 *
 * The allocated string must be freed with MPACK_FREE() (or simply free()
 * if MPack's allocator hasn't been customized.)
 *
 * @throws mpack_error_type If this node is not a string or contains NUL bytes
 * @throws mpack_error_too_big If the size of the string plus null-terminator
 *     is larger than the given maximum size
 * @throws mpack_error_memory If an allocation failure occurs
 *
 * @param node The node from which to allocate and copy string data
 * @param maxsize The maximum size to allocate, including the null-terminator
 *
 * @return The allocated string, or NULL if any error occurs.
 */
char* mpack_node_cstr_alloc(mpack_node_t node, size_t maxsize);

/**
 * Allocates a new null-terminated string using MPACK_MALLOC with the UTF-8
 * string contained by this node.
 *
 * The allocated string must be freed with MPACK_FREE() (or simply free()
 * if MPack's allocator hasn't been customized.)
 *
 * @throws mpack_error_type If this node is not a string, is not valid UTF-8,
 *     or contains NUL bytes
 * @throws mpack_error_too_big If the size of the string plus null-terminator
 *     is larger than the given maximum size
 * @throws mpack_error_memory If an allocation failure occurs
 *
 * @param node The node from which to allocate and copy string data
 * @param maxsize The maximum size to allocate, including the null-terminator
 *
 * @return The allocated string, or NULL if any error occurs.
 */
char* mpack_node_utf8_cstr_alloc(mpack_node_t node, size_t maxsize);
#endif

/**
 * Searches the given string array for a string matching the given
 * node and returns its index.
 *
 * If the node does not match any of the given strings,
 * @ref mpack_error_type is flagged. Use mpack_node_enum_optional()
 * if you want to allow values other than the given strings.
 *
 * If any error occurs or if the tree is in an error state, @a count
 * is returned.
 *
 * This can be used to quickly parse a string into an enum when the
 * enum values range from 0 to @a count-1. If the last value in the
 * enum is a special "count" value, it can be passed as the count,
 * and the return value can be cast directly to the enum type.
 *
 * @code{.c}
 * typedef enum           { APPLE ,  BANANA ,  ORANGE , COUNT} fruit_t;
 * const char* fruits[] = {"apple", "banana", "orange"};
 *
 * fruit_t fruit = (fruit_t)mpack_node_enum(node, fruits, COUNT);
 * @endcode
 *
 * @param node The node
 * @param strings An array of expected strings of length count
 * @param count The number of strings
 * @return The index of the matched string, or @a count in case of error
 */
size_t mpack_node_enum(mpack_node_t node, const char* strings[], size_t count);

/**
 * Searches the given string array for a string matching the given node,
 * returning its index or @a count if no strings match.
 *
 * If the value is not a string, or it does not match any of the
 * given strings, @a count is returned and no error is flagged.
 *
 * If any error occurs or if the tree is in an error state, @a count
 * is returned.
 *
 * This can be used to quickly parse a string into an enum when the
 * enum values range from 0 to @a count-1. If the last value in the
 * enum is a special "count" value, it can be passed as the count,
 * and the return value can be cast directly to the enum type.
 *
 * @code{.c}
 * typedef enum           { APPLE ,  BANANA ,  ORANGE , COUNT} fruit_t;
 * const char* fruits[] = {"apple", "banana", "orange"};
 *
 * fruit_t fruit = (fruit_t)mpack_node_enum_optional(node, fruits, COUNT);
 * @endcode
 *
 * @param node The node
 * @param strings An array of expected strings of length count
 * @param count The number of strings
 * @return The index of the matched string, or @a count in case of error
 */
size_t mpack_node_enum_optional(mpack_node_t node, const char* strings[], size_t count);

/**
 * @}
 */

/**
 * @name Compound Node Functions
 * @{
 */

/**
 * Returns the length of the given array node. Raises mpack_error_type
 * and returns 0 if the given node is not an array.
 */
MPACK_INLINE size_t mpack_node_array_length(mpack_node_t node) {
    if (mpack_node_error(node) != mpack_ok)
        return 0;

    if (node.data->type != mpack_type_array) {
        mpack_node_flag_error(node, mpack_error_type);
        return 0;
    }

    return (size_t)node.data->len;
}

/**
 * Returns the node in the given array at the given index. If the node
 * is not an array, mpack_error_type is raised and a nil node is returned.
 * If the given index is out of bounds, mpack_error_data is raised and
 * a nil node is returned.
 */
MPACK_INLINE mpack_node_t mpack_node_array_at(mpack_node_t node, size_t index) {
    if (mpack_node_error(node) != mpack_ok)
        return mpack_tree_nil_node(node.tree);

    if (node.data->type != mpack_type_array) {
        mpack_node_flag_error(node, mpack_error_type);
        return mpack_tree_nil_node(node.tree);
    }

    if (index >= node.data->len) {
        mpack_node_flag_error(node, mpack_error_data);
        return mpack_tree_nil_node(node.tree);
    }

    return mpack_node(node.tree, mpack_node_child(node, index));
}

/**
 * Returns the number of key/value pairs in the given map node. Raises
 * mpack_error_type and returns 0 if the given node is not a map.
 */
MPACK_INLINE size_t mpack_node_map_count(mpack_node_t node) {
    if (mpack_node_error(node) != mpack_ok)
        return 0;

    if (node.data->type != mpack_type_map) {
        mpack_node_flag_error(node, mpack_error_type);
        return 0;
    }

    return node.data->len;
}

// internal node map lookup
MPACK_INLINE mpack_node_t mpack_node_map_at(mpack_node_t node, size_t index, size_t offset) {
    if (mpack_node_error(node) != mpack_ok)
        return mpack_tree_nil_node(node.tree);

    if (node.data->type != mpack_type_map) {
        mpack_node_flag_error(node, mpack_error_type);
        return mpack_tree_nil_node(node.tree);
    }

    if (index >= node.data->len) {
        mpack_node_flag_error(node, mpack_error_data);
        return mpack_tree_nil_node(node.tree);
    }

    return mpack_node(node.tree, mpack_node_child(node, index * 2 + offset));
}

/**
 * Returns the key node in the given map at the given index.
 *
 * A nil node is returned in case of error.
 *
 * @throws mpack_error_type if the node is not a map
 * @throws mpack_error_data if the given index is out of bounds
 */
MPACK_INLINE mpack_node_t mpack_node_map_key_at(mpack_node_t node, size_t index) {
    return mpack_node_map_at(node, index, 0);
}

/**
 * Returns the value node in the given map at the given index.
 *
 * A nil node is returned in case of error.
 *
 * @throws mpack_error_type if the node is not a map
 * @throws mpack_error_data if the given index is out of bounds
 */
MPACK_INLINE mpack_node_t mpack_node_map_value_at(mpack_node_t node, size_t index) {
    return mpack_node_map_at(node, index, 1);
}

/**
 * Returns the value node in the given map for the given integer key.
 *
 * The key must exist within the map. Use mpack_node_map_int_optional() to
 * check for optional keys.
 *
 * The key must be unique. An error is flagged if the node has multiple
 * entries with the given key.
 *
 * @throws mpack_error_type If the node is not a map
 * @throws mpack_error_data If the node does not contain exactly one entry with the given key
 *
 * @return The value node for the given key, or a nil node in case of error
 */
mpack_node_t mpack_node_map_int(mpack_node_t node, int64_t num);

/**
 * Returns the value node in the given map for the given integer key, or a nil
 * node if the map does not contain the given key.
 *
 * The key must be unique. An error is flagged if the node has multiple
 * entries with the given key.
 *
 * @throws mpack_error_type If the node is not a map
 * @throws mpack_error_data If the node contains more than one entry with the given key
 *
 * @return The value node for the given key, or a nil node in case of error
 */
mpack_node_t mpack_node_map_int_optional(mpack_node_t node, int64_t num);

/**
 * Returns the value node in the given map for the given unsigned integer key.
 *
 * The key must exist within the map. Use mpack_node_map_uint_optional() to
 * check for optional keys.
 *
 * The key must be unique. An error is flagged if the node has multiple
 * entries with the given key.
 *
 * @throws mpack_error_type If the node is not a map
 * @throws mpack_error_data If the node does not contain exactly one entry with the given key
 *
 * @return The value node for the given key, or a nil node in case of error
 */
mpack_node_t mpack_node_map_uint(mpack_node_t node, uint64_t num);

/**
 * Returns the value node in the given map for the given unsigned integer
 * key, or a nil node if the map does not contain the given key.
 *
 * The key must be unique. An error is flagged if the node has multiple
 * entries with the given key.
 *
 * @throws mpack_error_type If the node is not a map
 * @throws mpack_error_data If the node contains more than one entry with the given key
 *
 * @return The value node for the given key, or a nil node in case of error
 */
mpack_node_t mpack_node_map_uint_optional(mpack_node_t node, uint64_t num);

/**
 * Returns the value node in the given map for the given string key.
 *
 * The key must exist within the map. Use mpack_node_map_str_optional() to
 * check for optional keys.
 *
 * The key must be unique. An error is flagged if the node has multiple
 * entries with the given key.
 *
 * @throws mpack_error_type If the node is not a map
 * @throws mpack_error_data If the node does not contain exactly one entry with the given key
 *
 * @return The value node for the given key, or a nil node in case of error
 */
mpack_node_t mpack_node_map_str(mpack_node_t node, const char* str, size_t length);

/**
 * Returns the value node in the given map for the given string key, or a nil
 * node if the map does not contain the given key.
 *
 * The key must be unique. An error is flagged if the node has multiple
 * entries with the given key.
 *
 * @throws mpack_error_type If the node is not a map
 * @throws mpack_error_data If the node contains more than one entry with the given key
 *
 * @return The value node for the given key, or a nil node in case of error
 */
mpack_node_t mpack_node_map_str_optional(mpack_node_t node, const char* str, size_t length);

/**
 * Returns the value node in the given map for the given null-terminated
 * string key.
 *
 * The key must exist within the map. Use mpack_node_map_cstr_optional() to
 * check for optional keys.
 *
 * The key must be unique. An error is flagged if the node has multiple
 * entries with the given key.
 *
 * @throws mpack_error_type If the node is not a map
 * @throws mpack_error_data If the node does not contain exactly one entry with the given key
 *
 * @return The value node for the given key, or a nil node in case of error
 */
mpack_node_t mpack_node_map_cstr(mpack_node_t node, const char* cstr);

/**
 * Returns the value node in the given map for the given null-terminated
 * string key, or a nil node if the map does not contain the given key.
 *
 * The key must be unique. An error is flagged if the node has multiple
 * entries with the given key.
 *
 * @throws mpack_error_type If the node is not a map
 * @throws mpack_error_data If the node contains more than one entry with the given key
 *
 * @return The value node for the given key, or a nil node in case of error
 */
mpack_node_t mpack_node_map_cstr_optional(mpack_node_t node, const char* cstr);

/**
 * Returns true if the given node map contains exactly one entry with the
 * given integer key.
 *
 * The key must be unique. An error is flagged if the node has multiple
 * entries with the given key.
 *
 * @throws mpack_error_type If the node is not a map
 * @throws mpack_error_data If the node contains more than one entry with the given key
 */
bool mpack_node_map_contains_int(mpack_node_t node, int64_t num);

/**
 * Returns true if the given node map contains exactly one entry with the
 * given unsigned integer key.
 *
 * The key must be unique. An error is flagged if the node has multiple
 * entries with the given key.
 *
 * @throws mpack_error_type If the node is not a map
 * @throws mpack_error_data If the node contains more than one entry with the given key
 */
bool mpack_node_map_contains_uint(mpack_node_t node, uint64_t num);

/**
 * Returns true if the given node map contains exactly one entry with the
 * given string key.
 *
 * The key must be unique. An error is flagged if the node has multiple
 * entries with the given key.
 *
 * @throws mpack_error_type If the node is not a map
 * @throws mpack_error_data If the node contains more than one entry with the given key
 */
bool mpack_node_map_contains_str(mpack_node_t node, const char* str, size_t length);

/**
 * Returns true if the given node map contains exactly one entry with the
 * given null-terminated string key.
 *
 * The key must be unique. An error is flagged if the node has multiple
 * entries with the given key.
 *
 * @throws mpack_error_type If the node is not a map
 * @throws mpack_error_data If the node contains more than one entry with the given key
 */
bool mpack_node_map_contains_cstr(mpack_node_t node, const char* cstr);

/**
 * @}
 */

/**
 * @}
 */

#endif

MPACK_HEADER_END

#endif


#endif

