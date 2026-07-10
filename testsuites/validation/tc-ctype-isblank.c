/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CCtypeReqIsblank
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ctype.h>
#include <stdio.h>

#include <rtems/test.h>

/**
 * @defgroup CCtypeReqIsblank spec:/c/ctype/req/isblank
 *
 * @{
 */

typedef enum {
  CCtypeReqIsblank_Pre_Character_Blank,
  CCtypeReqIsblank_Pre_Character_NonBlank,
  CCtypeReqIsblank_Pre_Character_NA
} CCtypeReqIsblank_Pre_Character;

typedef enum {
  CCtypeReqIsblank_Post_Result_NonZero,
  CCtypeReqIsblank_Post_Result_Zero,
  CCtypeReqIsblank_Post_Result_NA
} CCtypeReqIsblank_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Character_NA : 1;
  uint8_t Post_Result : 2;
} CCtypeReqIsblank_Entry;

/**
 * @brief Test context for spec:/c/ctype/req/isblank test case.
 */
typedef struct {
  /**
   * @brief This member specifies the characters to be tested.
   */
  const int *chars;

  /**
   * @brief This member specifies the number of characters to be tested.
   */
  size_t n;

  /**
   * @brief This member contains the return values.
   */
  int retval[ 257 ];

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 1 ];

    /**
     * @brief If this member is true, then the test action loop is executed.
     */
    bool in_action_loop;

    /**
     * @brief This member contains the next transition map index.
     */
    size_t index;

    /**
     * @brief This member contains the current transition map entry.
     */
    CCtypeReqIsblank_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CCtypeReqIsblank_Context;

static CCtypeReqIsblank_Context
  CCtypeReqIsblank_Instance;

static const char * const CCtypeReqIsblank_PreDesc_Character[] = {
  "Blank",
  "NonBlank",
  "NA"
};

static const char * const * const CCtypeReqIsblank_PreDesc[] = {
  CCtypeReqIsblank_PreDesc_Character,
  NULL
};

static const int blank_chars[] = {
  '\t', ' '
};

static const int non_blank_chars[] = {
  '\0', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\a',
  '\b', '\n', '\v', '\f', '\r', '\x0e', '\x0f',
  '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17',
  '\x18', '\x19', '\x1a', '\x1b', '\x1c', '\x1d', '\x1e', '\x1f',
  '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  ':', ';', '<', '=', '>', '?', '@',
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
  '[', '\\', ']', '^', '_', '`',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  '{', '|', '}', '~', '\x7f',
  EOF,
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
  0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
  0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
  0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
  0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
  0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
  0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
  256 /* Out of bounds */
};

static void CCtypeReqIsblank_Pre_Character_Prepare(
  CCtypeReqIsblank_Context      *ctx,
  CCtypeReqIsblank_Pre_Character state
)
{
  switch ( state ) {
    case CCtypeReqIsblank_Pre_Character_Blank: {
      /*
       * While the character specified by `c` is a blank character.
       */
      ctx->chars = blank_chars;
      ctx->n = RTEMS_ARRAY_SIZE( blank_chars );
      break;
    }

    case CCtypeReqIsblank_Pre_Character_NonBlank: {
      /*
       * While the character specified by `c` is not a blank character.
       */
      ctx->chars = non_blank_chars;
      ctx->n = RTEMS_ARRAY_SIZE( non_blank_chars );
      break;
    }

    case CCtypeReqIsblank_Pre_Character_NA:
      break;
  }
}

static void CCtypeReqIsblank_Post_Result_Check(
  CCtypeReqIsblank_Context    *ctx,
  CCtypeReqIsblank_Post_Result state
)
{
  switch ( state ) {
    case CCtypeReqIsblank_Post_Result_NonZero: {
      /*
       * The return value shall be non-zero.
       */
      size_t i;

      for ( i = 0; i < ctx->n; ++i ) {
        T_ne_int( ctx->retval[ i ], 0 );
      }
      break;
    }

    case CCtypeReqIsblank_Post_Result_Zero: {
      /*
       * The return value shall be zero.
       */
      size_t i;

      for ( i = 0; i < ctx->n; ++i ) {
        T_eq_int( ctx->retval[ i ], 0 );
      }
      break;
    }

    case CCtypeReqIsblank_Post_Result_NA:
      break;
  }
}

static void CCtypeReqIsblank_Action( CCtypeReqIsblank_Context *ctx )
{
  size_t i;

  for ( i = 0; i < ctx->n; ++i ) {
    ctx->retval[ i ] = isblank( ctx->chars[ i ] );
  }
}

static const CCtypeReqIsblank_Entry
CCtypeReqIsblank_Entries[] = {
  { 0, 0, CCtypeReqIsblank_Post_Result_NonZero },
  { 0, 0, CCtypeReqIsblank_Post_Result_Zero }
};

static const uint8_t
CCtypeReqIsblank_Map[] = {
  0, 1
};

static size_t CCtypeReqIsblank_Scope( void *arg, char *buf, size_t n )
{
  CCtypeReqIsblank_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CCtypeReqIsblank_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CCtypeReqIsblank_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CCtypeReqIsblank_Scope,
  .initial_context = &CCtypeReqIsblank_Instance
};

static inline CCtypeReqIsblank_Entry CCtypeReqIsblank_PopEntry(
  CCtypeReqIsblank_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CCtypeReqIsblank_Entries[
    CCtypeReqIsblank_Map[ index ]
  ];
}

static void CCtypeReqIsblank_TestVariant( CCtypeReqIsblank_Context *ctx )
{
  CCtypeReqIsblank_Pre_Character_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CCtypeReqIsblank_Action( ctx );
  CCtypeReqIsblank_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CCtypeReqIsblank( void )
 */
T_TEST_CASE_FIXTURE( CCtypeReqIsblank, &CCtypeReqIsblank_Fixture )
{
  CCtypeReqIsblank_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CCtypeReqIsblank_Pre_Character_Blank;
    ctx->Map.pcs[ 0 ] < CCtypeReqIsblank_Pre_Character_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CCtypeReqIsblank_PopEntry( ctx );
    CCtypeReqIsblank_TestVariant( ctx );
  }
}

/** @} */
