/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2020, 2025 embedded brains GmbH & Co. KG
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
 * @file
 *
 * @ingroup RTEMSExamplesAda
 *
 * @brief This header file provides the configuration of the Ada example.
 */

#ifndef APPL_CONFIG_H
#define APPL_CONFIG_H

#include <rtems.h>

/**
 * @defgroup RTEMSExamplesAda Ada example
 *
 * @ingroup Examples
 *
 * @brief This group contains the Ada example application.
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define APPL_NAME "EXAMPLE ADA"

int main(void);

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_STACK_CHECKER_ENABLED

#define CONFIGURE_UNIFIED_WORK_AREAS

#define CONFIGURE_UNLIMITED_OBJECTS

#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 4

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* APPL_CONFIG_H */
