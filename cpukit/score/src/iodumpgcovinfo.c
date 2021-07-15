/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreIO
 *
 * @brief This source file contains the implementation of _IO_Dump_gcov_info().
 */

/*
 * Copyright (C) 2021 embedded brains GmbH (http://www.embedded-brains.de)
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/io.h>
#include <rtems/score/interr.h>
#include <rtems/linkersets.h>

#include <gcov.h>

RTEMS_LINKER_ROSET( gcov_info, const struct gcov_info * );

typedef struct {
  IO_Put_char put_char;
  void *arg;
} IO_Dump_gcov_context;

static void _IO_Gcov_filename( const char *f, void *arg )
{
  const IO_Dump_gcov_context *ctx;

  ctx = arg;
  _IO_Printf( ctx->put_char, ctx->arg, "Emitting gcda bytes for %s\n", f );
}

static void _IO_Gcov_dump( const void *d, unsigned n, void *arg )
{
  const IO_Dump_gcov_context *ctx;
  const unsigned char        *c;
  unsigned                    i;

  ctx = arg;
  c = d;

  for (i = 0; i < n; ++i) {
    _IO_Printf( ctx->put_char, ctx->arg, "%02x", c[ i ] );
  }
}

static void *_IO_Gcov_allocate( unsigned length, void *arg )
{
  (void) length;
  (void) arg;
  return NULL;
}

void _IO_Dump_gcov_info( IO_Put_char put_char, void *arg )
{
  IO_Dump_gcov_context            ctx;
  const struct gcov_info * const *item;

  ctx.put_char = put_char;
  ctx.arg = arg;

  RTEMS_LINKER_SET_FOREACH( gcov_info, item ) {
    __gcov_info_to_gcda(
      *item,
      _IO_Gcov_filename,
      _IO_Gcov_dump,
      _IO_Gcov_allocate,
      &ctx
    );
    ( *put_char )( '\n', arg );
  }
}
