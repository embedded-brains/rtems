/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplGcov
 *
 * @brief This source file contains the implementation of _Gcov_Dump_info().
 */

/*
 * Copyright (C) 2021, 2026 embedded brains GmbH & Co. KG
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

#include <rtems/test-gcov.h>

#include <stdbool.h>
#include <string.h>

typedef struct {
  IO_Put_char put_char;
  void       *arg;
} Gcov_Context;

/*
 * The output path of the dump updates coverage counters.  The dump serializes
 * the information of one object file into this buffer before it outputs the
 * data, so the counters of an object file are consistent.
 */
#define GCOV_INFO_BUFFER_SIZE 32768

typedef struct {
  char   *begin;
  size_t  size;
  size_t  used;
  bool    overflow;
} Gcov_Buffer;

static char _Gcov_Info_buffer[ GCOV_INFO_BUFFER_SIZE ];

static void _Gcov_Dump( const void *data, unsigned length, void *arg )
{
  Gcov_Context *ctx;
  IO_Put_char   put_char;
  void         *ctx_arg;
  const char   *in;
  const void   *end;

  ctx = arg;
  in = data;
  end = in + length;
  put_char = ctx->put_char;
  ctx_arg = ctx->arg;

  while ( in != end ) {
    ( *put_char )( *in, ctx_arg );
    ++in;
  }
}

static void _Gcov_Filename( const char *filename, void *arg )
{
  __gcov_filename_to_gcfn( filename, _Gcov_Dump, arg );
}

static void _Gcov_Buffer_dump( const void *data, unsigned length, void *arg )
{
  Gcov_Buffer *buffer;

  buffer = arg;

  if ( length > buffer->size - buffer->used ) {
    buffer->overflow = true;
    return;
  }

  memcpy( buffer->begin + buffer->used, data, length );
  buffer->used += length;
}

static void _Gcov_Buffer_filename( const char *filename, void *arg )
{
  __gcov_filename_to_gcfn( filename, _Gcov_Buffer_dump, arg );
}

static void *_Gcov_Allocate( unsigned length, void *arg )
{
  (void) length;
  (void) arg;
  return NULL;
}

void _Gcov_Dump_info( IO_Put_char put_char, void *arg )
{
  Gcov_Context                   ctx;
  const struct gcov_info *const *item;

  ctx.put_char = put_char;
  ctx.arg = arg;

  RTEMS_LINKER_SET_FOREACH ( gcov_info, item ) {
    Gcov_Buffer buffer;

    buffer.begin = _Gcov_Info_buffer;
    buffer.size = sizeof( _Gcov_Info_buffer );
    buffer.used = 0;
    buffer.overflow = false;
    __gcov_info_to_gcda(
      *item,
      _Gcov_Buffer_filename,
      _Gcov_Buffer_dump,
      _Gcov_Allocate,
      &buffer
    );

    if ( buffer.overflow ) {
      __gcov_info_to_gcda(
        *item,
        _Gcov_Filename,
        _Gcov_Dump,
        _Gcov_Allocate,
        &ctx
      );
    } else {
      _Gcov_Dump( buffer.begin, (unsigned) buffer.used, &ctx );
    }
  }
}
