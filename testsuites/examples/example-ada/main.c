/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2023, 2025 embedded brains GmbH & Co. KG
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
 * @brief This source file provides the application of the Ada example.
 */

#include <errno.h>
#include <pthread.h>
#include <signal.h>

#include "appl-config.h"

/* Defined by the -Mgnat_main option of gnatmake */
extern int gnat_main(int argc, char **argv, char **envp);

/*
 * Unfortunately, the Ada runtime library support for RTEMS uses signals.  This
 * is something which should be change if Ada is used in a serious application.
 * The sigaction() is used by __gnat_install_handler().  The pthread_kill() is
 * used by System.Task_Primitives.Operations.Abort_Task().
 */

int sigaction(int sig, const struct sigaction *restrict act, struct sigaction *restrict oact)
{
	(void)sig;
	(void)act;
	(void)oact;
	errno = EINVAL;
	return -1;
}

int pthread_kill(pthread_t thread, int sig)
{
	(void)thread;
	(void)sig;
	return EINVAL;
}

int main(void)
{
	return gnat_main(0, NULL, NULL);
}
