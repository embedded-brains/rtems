/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CCtypeReqTolower
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

#include <rtems/test.h>

/**
 * @defgroup CCtypeReqTolower spec:/c/ctype/req/tolower
 *
 * @{
 */

typedef enum {
  CCtypeReqTolower_Pre_Char_Uppercase,
  CCtypeReqTolower_Pre_Char_Other,
  CCtypeReqTolower_Pre_Char_NA
} CCtypeReqTolower_Pre_Char;

typedef enum {
  CCtypeReqTolower_Post_Result_Lowercase,
  CCtypeReqTolower_Post_Result_Unchanged,
  CCtypeReqTolower_Post_Result_NA
} CCtypeReqTolower_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Char_NA : 1;
  uint8_t Post_Result : 2;
} CCtypeReqTolower_Entry;

/**
 * @brief Test context for spec:/c/ctype/req/tolower test case.
 */
typedef struct {
  /**
   * @brief This member specifies the characters to test.
   */
  const int *chars;

  /**
   * @brief This member specifies the number of characters to test.
   */
  size_t n;

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
    CCtypeReqTolower_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CCtypeReqTolower_Context;

static CCtypeReqTolower_Context
  CCtypeReqTolower_Instance;

static const char * const CCtypeReqTolower_PreDesc_Char[] = {
  "Uppercase",
  "Other",
  "NA"
};

static const char * const * const CCtypeReqTolower_PreDesc[] = {
  CCtypeReqTolower_PreDesc_Char,
  NULL
};

static const int uppercase_chars[] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
};

static const int other_chars[] = {
  -1, /* EOF */
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
  0x40, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60,
  0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d,
  0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a,
  0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
  0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
  0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
  0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
  0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
  0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
  0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
  0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
  0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
  0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
  0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
  0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
  0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
  0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
  0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

static void CCtypeReqTolower_Pre_Char_Prepare(
  CCtypeReqTolower_Context *ctx,
  CCtypeReqTolower_Pre_Char state
)
{
  switch ( state ) {
    case CCtypeReqTolower_Pre_Char_Uppercase: {
      /*
       * While the character specified by `c` is an uppercase letter.
       */
      ctx->chars = uppercase_chars;
      ctx->n = RTEMS_ARRAY_SIZE( uppercase_chars );
      break;
    }

    case CCtypeReqTolower_Pre_Char_Other: {
      /*
       * While the character specified by `c` is not an uppercase letter.
       */
      ctx->chars = other_chars;
      ctx->n = RTEMS_ARRAY_SIZE( other_chars );
      break;
    }

    case CCtypeReqTolower_Pre_Char_NA:
      break;
  }
}

static void CCtypeReqTolower_Post_Result_Check(
  CCtypeReqTolower_Context    *ctx,
  CCtypeReqTolower_Post_Result state
)
{
  switch ( state ) {
    case CCtypeReqTolower_Post_Result_Lowercase: {
      /*
       * The return value of the function shall be the corresponding lowercase
       * character.
       */
      size_t i;

      for ( i = 0; i < ctx->n; ++i ) {
        T_eq_int( tolower( ctx->chars[ i ] ), ctx->chars[ i ] + ( 'a' - 'A' ) );
      }
      break;
    }

    case CCtypeReqTolower_Post_Result_Unchanged: {
      /*
       * The return value of the function shall be the original character.
       */
      size_t i;

      for ( i = 0; i < ctx->n; ++i ) {
        T_eq_int( tolower( ctx->chars[ i ] ), ctx->chars[ i ] );
      }
      break;
    }

    case CCtypeReqTolower_Post_Result_NA:
      break;
  }
}

static void CCtypeReqTolower_Action( void )
{
  /* The action is performed in the post-condition checks */
}

static const CCtypeReqTolower_Entry
CCtypeReqTolower_Entries[] = {
  { 0, 0, CCtypeReqTolower_Post_Result_Lowercase },
  { 0, 0, CCtypeReqTolower_Post_Result_Unchanged }
};

static const uint8_t
CCtypeReqTolower_Map[] = {
  0, 1
};

static size_t CCtypeReqTolower_Scope( void *arg, char *buf, size_t n )
{
  CCtypeReqTolower_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CCtypeReqTolower_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CCtypeReqTolower_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CCtypeReqTolower_Scope,
  .initial_context = &CCtypeReqTolower_Instance
};

static inline CCtypeReqTolower_Entry CCtypeReqTolower_PopEntry(
  CCtypeReqTolower_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CCtypeReqTolower_Entries[
    CCtypeReqTolower_Map[ index ]
  ];
}

static void CCtypeReqTolower_TestVariant( CCtypeReqTolower_Context *ctx )
{
  CCtypeReqTolower_Pre_Char_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CCtypeReqTolower_Action();
  CCtypeReqTolower_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CCtypeReqTolower( void )
 */
T_TEST_CASE_FIXTURE( CCtypeReqTolower, &CCtypeReqTolower_Fixture )
{
  CCtypeReqTolower_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CCtypeReqTolower_Pre_Char_Uppercase;
    ctx->Map.pcs[ 0 ] < CCtypeReqTolower_Pre_Char_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CCtypeReqTolower_PopEntry( ctx );
    CCtypeReqTolower_TestVariant( ctx );
  }
}

/** @} */
