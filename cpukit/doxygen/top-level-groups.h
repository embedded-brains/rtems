/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplDoxygen
 *
 * @brief This header file defines high level Doxygen groups.
 */

/*
 * Copyright (C) 2019, 2023 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @defgroup RTEMSAPI API
 *
 * @brief This group contains the Application Programming Interfaces (APIs) of
 *   RTEMS.
 */

/**
 * @defgroup RTEMSAPISystemLibrary System Library
 *
 * @ingroup RTEMSAPI
 *
 * @brief This group contains the system library APIs of RTEMS.
 */

/**
 * @defgroup RTEMSDeviceDrivers Device Drivers
 *
 * @brief This group contains the device drivers.
 */

/**
 * @defgroup RTEMSDeviceConsole Console Drivers
 *
 * @ingroup RTEMSDeviceDrivers
 *
 * @brief This group contains the console drivers.
 */

/**
 * @defgroup RTEMSImpl Implementation
 *
 * @brief This group contains the RTEMS implementation components.
 */

/**
 * @defgroup RTEMSImplDoxygen Doxygen Support
 *
 * @ingroup RTEMSImpl
 */

/**
 * @defgroup RTEMSImplFreeBSDKernel FreeBSD Kernel Space Support
 *
 * @ingroup RTEMSImpl
 *
 * @brief This group contains the interfaces used by FreeBSD kernel space
 *   components ported to RTEMS.
 */

/**
 * @defgroup RTEMSTestSuites Test Suites
 *
 * @brief This group contains the test suites.
 */

/**
 * @defgroup RTEMSTestSuitesFilesystem Filesystem Test Suites
 *
 * @ingroup RTEMSTestSuites
 *
 * @brief This group contains the test suites related to filesystem tests.
 */

/**
 * @defgroup RTEMSTestSuitesMembench Memory Benchmark Test Suites
 *
 * @ingroup RTEMSTestSuites
 *
 * @brief This group contains the test suites related to memory benchmarks.
 */

/**
 * @defgroup RTEMSTestSuitesUnit Unit Test Suites
 *
 * @ingroup RTEMSTestSuites
 *
 * @brief This group contains the test suites related to unit tests.
 */

/**
 * @defgroup RTEMSTestSuitesValidation Validation Test Suites
 *
 * @ingroup RTEMSTestSuites
 *
 * @brief This group contains the test suites related to validation tests.
 */

/**
 * @file cpukit/libc/ctype/ctype_.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the __set_ctype()
 *   function.
 */

/**
 * @file cpukit/libc/ctype/ctype_.h
 *
 * @ingroup libcsupport
 *
 * @brief This header file provides internal declarations used by the
 *   implementation of the character classification and conversion functions.
 */

/**
 * @file cpukit/libc/search/qsort.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the qsort()
 *   function.
 */

/**
 * @file cpukit/libc/stdlib/arc4random.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the arc4random()
 *   and arc4random_buf() functions.
 */

/**
 * @file cpukit/libc/stdlib/arc4random.h
 *
 * @ingroup libcsupport
 *
 * @brief This header file provides internal helper functions and macros
 *   used by the implementation of the arc4random() and arc4random_buf()
 *   functions.
 */

/**
 * @file cpukit/libc/stdlib/chacha_private.h
 *
 * @ingroup libcsupport
 *
 * @brief This header file provides the internal implementation of the
 *   ChaCha20 stream cipher used by the arc4random() implementation.
 */

/**
 * @file cpukit/libc/stdlib/env.h
 *
 * @ingroup libcsupport
 *
 * @brief This header file declares the environ variable and the __findenv()
 *   function used by the implementation of the environment variable
 *   functions.
 */

/**
 * @file cpukit/libc/stdlib/getenv.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the __findenv()
 *   and getenv() functions.
 */

/**
 * @file cpukit/libc/stdlib/reallocarray.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the reallocarray()
 *   function.
 */

/**
 * @file cpukit/libc/stdlib/setenv.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the putenv(),
 *   setenv(), and unsetenv() functions.
 */

/**
 * @file cpukit/libc/stdlib/strtol.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the strtol()
 *   function.
 */

/**
 * @file cpukit/libc/stdlib/strtoll.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the strtoll()
 *   function.
 */

/**
 * @file cpukit/libc/stdlib/strtoul.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the strtoul()
 *   function.
 */

/**
 * @file cpukit/libc/stdlib/strtoull.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the strtoull()
 *   function.
 */

/**
 * @file cpukit/libc/string/explicit_bzero.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the
 *   explicit_bzero() function.
 */

/**
 * @file cpukit/libc/string/flsl.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the flsl()
 *   function.
 */

/**
 * @file cpukit/libc/string/local.h
 *
 * @ingroup libcsupport
 *
 * @brief This header file declares the __wcwidth() function and provides
 *   internal macros used by the implementation of the string functions.
 */

/**
 * @file cpukit/libc/string/memccpy.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the memccpy()
 *   function.
 */

/**
 * @file cpukit/libc/string/memchr.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the memchr()
 *   function.
 */

/**
 * @file cpukit/libc/string/memcmp.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the memcmp()
 *   function.
 */

/**
 * @file cpukit/libc/string/memcpy.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the memcpy()
 *   function.
 */

/**
 * @file cpukit/libc/string/memmove.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the memmove()
 *   function.
 */

/**
 * @file cpukit/libc/string/memset.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the memset()
 *   function.
 */

/**
 * @file cpukit/libc/string/stpcpy.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the stpcpy()
 *   function.
 */

/**
 * @file cpukit/libc/string/str-two-way.h
 *
 * @ingroup libcsupport
 *
 * @brief This header file provides the internal Two-Way string searching
 *   algorithm template used by the implementation of substring search
 *   functions such as strstr().
 */

/**
 * @file cpukit/libc/string/strcasecmp.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the strcasecmp()
 *   function.
 */

/**
 * @file cpukit/libc/string/strchr.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the strchr()
 *   function.
 */

/**
 * @file cpukit/libc/string/strcmp.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the strcmp()
 *   function.
 */

/**
 * @file cpukit/libc/string/strcspn.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the strcspn()
 *   function.
 */

/**
 * @file cpukit/libc/string/strdup.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the strdup()
 *   function.
 */

/**
 * @file cpukit/libc/string/strlcpy.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the strlcpy()
 *   function.
 */

/**
 * @file cpukit/libc/string/strlen.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the strlen()
 *   function.
 */

/**
 * @file cpukit/libc/string/strncasecmp.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the strncasecmp()
 *   function.
 */

/**
 * @file cpukit/libc/string/strncat.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the strncat()
 *   function.
 */

/**
 * @file cpukit/libc/string/strncmp.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the strncmp()
 *   function.
 */

/**
 * @file cpukit/libc/string/strncpy.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the strncpy()
 *   function.
 */

/**
 * @file cpukit/libc/string/strndup.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the strndup()
 *   function.
 */

/**
 * @file cpukit/libc/string/strnlen.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the strnlen()
 *   function.
 */

/**
 * @file cpukit/libc/string/strpbrk.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the strpbrk()
 *   function.
 */

/**
 * @file cpukit/libc/string/strrchr.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the strrchr()
 *   function.
 */

/**
 * @file cpukit/libc/string/strspn.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the strspn()
 *   function.
 */

/**
 * @file cpukit/libc/string/strstr.c
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of the strstr()
 *   function.
 */

/**
 * @defgroup RTEMSImplCompilerBuiltins Compiler Builtins
 *
 * @ingroup RTEMSImpl
 *
 * @brief This group contains the implementation of compiler builtins.
 */

/**
 * @file cpukit/compiler-rt/lib/builtins/bswapdi2.c
 *
 * @ingroup RTEMSImplCompilerBuiltins
 *
 * @brief This source file contains the implemention of a compiler builtin.
 */

/**
 * @file cpukit/compiler-rt/lib/builtins/bswapsi2.c
 *
 * @ingroup RTEMSImplCompilerBuiltins
 *
 * @brief This source file contains the implemention of a compiler builtin.
 */

/**
 * @file cpukit/compiler-rt/lib/builtins/clzsi2.c
 *
 * @ingroup RTEMSImplCompilerBuiltins
 *
 * @brief This source file contains the implemention of a compiler builtin.
 */

/**
 * @file cpukit/compiler-rt/lib/builtins/clzdi2.c
 *
 * @ingroup RTEMSImplCompilerBuiltins
 *
 * @brief This source file contains the implemention of a compiler builtin.
 */

/**
 * @file cpukit/compiler-rt/lib/builtins/ctzsi2.c
 *
 * @ingroup RTEMSImplCompilerBuiltins
 *
 * @brief This source file contains the implemention of a compiler builtin.
 */

/**
 * @file cpukit/compiler-rt/lib/builtins/ctzdi2.c
 *
 * @ingroup RTEMSImplCompilerBuiltins
 *
 * @brief This source file contains the implemention of a compiler builtin.
 */

/**
 * @file cpukit/compiler-rt/lib/builtins/divdi3.c
 *
 * @ingroup RTEMSImplCompilerBuiltins
 *
 * @brief This source file contains the implemention of a compiler builtin.
 */

/**
 * @file cpukit/compiler-rt/lib/builtins/ffsdi2.c
 *
 * @ingroup RTEMSImplCompilerBuiltins
 *
 * @brief This source file contains the implemention of a compiler builtin.
 */

/**
 * @file cpukit/compiler-rt/lib/builtins/ffssi2.c
 *
 * @ingroup RTEMSImplCompilerBuiltins
 *
 * @brief This source file contains the implemention of a compiler builtin.
 */

/**
 * @file cpukit/compiler-rt/lib/builtins/int_div_impl.inc
 *
 * @ingroup RTEMSImplCompilerBuiltins
 *
 * @brief This header file contains the implemention of compiler builtins.
 */

/**
 * @file cpukit/compiler-rt/lib/builtins/int_endianness.h
 *
 * @ingroup RTEMSImplCompilerBuiltins
 *
 * @brief This source file contains the implemention of a compiler builtin.
 */

/**
 * @file cpukit/compiler-rt/lib/builtins/int_lib.h
 *
 * @ingroup RTEMSImplCompilerBuiltins
 *
 * @brief This header file provides interfaces for compiler builtins.
 */

/**
 * @file cpukit/compiler-rt/lib/builtins/int_types.h
 *
 * @ingroup RTEMSImplCompilerBuiltins
 *
 * @brief This header file provides interfaces for compiler builtins.
 */

/**
 * @file cpukit/compiler-rt/lib/builtins/int_util.h
 *
 * @ingroup RTEMSImplCompilerBuiltins
 *
 * @brief This header file provides interfaces for compiler builtins.
 */

/**
 * @file cpukit/compiler-rt/lib/builtins/moddi3.c
 *
 * @ingroup RTEMSImplCompilerBuiltins
 *
 * @brief This source file contains the implemention of a compiler builtin.
 */

/**
 * @file cpukit/compiler-rt/lib/builtins/paritydi2.c
 *
 * @ingroup RTEMSImplCompilerBuiltins
 *
 * @brief This source file contains the implemention of a compiler builtin.
 */

/**
 * @file cpukit/compiler-rt/lib/builtins/paritysi2.c
 *
 * @ingroup RTEMSImplCompilerBuiltins
 *
 * @brief This source file contains the implemention of a compiler builtin.
 */

/**
 * @file cpukit/compiler-rt/lib/builtins/popcountdi2.c
 *
 * @ingroup RTEMSImplCompilerBuiltins
 *
 * @brief This source file contains the implemention of a compiler builtin.
 */

/**
 * @file cpukit/compiler-rt/lib/builtins/popcountsi2.c
 *
 * @ingroup RTEMSImplCompilerBuiltins
 *
 * @brief This source file contains the implemention of a compiler builtin.
 */

/**
 * @file cpukit/compiler-rt/lib/builtins/udivdi3.c
 *
 * @ingroup RTEMSImplCompilerBuiltins
 *
 * @brief This source file contains the implemention of a compiler builtin.
 */

/**
 * @file cpukit/compiler-rt/lib/builtins/udivmoddi4.c
 *
 * @ingroup RTEMSImplCompilerBuiltins
 *
 * @brief This source file contains the implemention of a compiler builtin.
 */

/**
 * @file cpukit/compiler-rt/lib/builtins/umoddi3.c
 *
 * @ingroup RTEMSImplCompilerBuiltins
 *
 * @brief This source file contains the implemention of a compiler builtin.
 */
