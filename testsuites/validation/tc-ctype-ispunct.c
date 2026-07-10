/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CCtypeReqIspunct
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
 * @defgroup CCtypeReqIspunct spec:/c/ctype/req/ispunct
 *
 * @{
 */

typedef enum {
  CCtypeReqIspunct_Pre_Char_Punct,
  CCtypeReqIspunct_Pre_Char_None,
  CCtypeReqIspunct_Pre_Char_NA
} CCtypeReqIspunct_Pre_Char;

typedef enum {
  CCtypeReqIspunct_Post_Result_NonZero,
  CCtypeReqIspunct_Post_Result_Zero,
  CCtypeReqIspunct_Post_Result_NA
} CCtypeReqIspunct_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Char_NA : 1;
  uint8_t Post_Result : 2;
} CCtypeReqIspunct_Entry;

/**
 * @brief Test context for spec:/c/ctype/req/ispunct test case.
 */
typedef struct {
  /**
   * @brief This member references the character array to test.
   */
  const int *chars;

  /**
   * @brief This member contains the number of characters to test.
   */
  size_t num_chars;

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
    CCtypeReqIspunct_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CCtypeReqIspunct_Context;

static CCtypeReqIspunct_Context
  CCtypeReqIspunct_Instance;

static const char * const CCtypeReqIspunct_PreDesc_Char[] = {
  "Punct",
  "None",
  "NA"
};

static const char * const * const CCtypeReqIspunct_PreDesc[] = {
  CCtypeReqIspunct_PreDesc_Char,
  NULL
};

static const int valid_punct[] = {
  '!', '\"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/', ':', ';', '<', '=', '>', '?', '@', '[', '\\', ']', '^', '_', '`', '{', '|', '}', '~'
};

static const int valid_none[] = {
  '\0', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\a', '\b', '\t', '\n', '\v', '\f', '\r', '\x0e', '\x0f', '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1a', '\x1b', '\x1c', '\x1d', '\x1e', '\x1f', ' ', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '\x7f', '\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87', '\x88', '\x89', '\x8a', '\x8b', '\x8c', '\x8d', '\x8e', '\x8f', '\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97', '\x98', '\x99', '\x9a', '\x9b', '\x9c', '\x9d', '\x9e', '\x9f', '\xa0', '\xa1', '\xa2', '\xa3', '\xa4', '\xa5', '\xa6', '\xa7', '\xa8', '\xa9', '\xaa', '\xab', '\xac', '\xad', '\xae', '\xaf', '\xb0', '\xb1', '\xb2', '\xb3', '\xb4', '\xb5', '\xb6', '\xb7', '\xb8', '\xb9', '\xba', '\xbb', '\xbc', '\xbd', '\xbe', '\xbf', '\xc0', '\xc1', '\xc2', '\xc3', '\xc4', '\xc5', '\xc6', '\xc7', '\xc8', '\xc9', '\xca', '\xcb', '\xcc', '\xcd', '\xce', '\xcf', '\xd0', '\xd1', '\xd2', '\xd3', '\xd4', '\xd5', '\xd6', '\xd7', '\xd8', '\xd9', '\xda', '\xdb', '\xdc', '\xdd', '\xde', '\xdf', '\xe0', '\xe1', '\xe2', '\xe3', '\xe4', '\xe5', '\xe6', '\xe7', '\xe8', '\xe9', '\xea', '\xeb', '\xec', '\xed', '\xee', '\xef', '\xf0', '\xf1', '\xf2', '\xf3', '\xf4', '\xf5', '\xf6', '\xf7', '\xf8', '\xf9', '\xfa', '\xfb', '\xfc', '\xfd', '\xfe', '\xff', -1
};

static void CCtypeReqIspunct_Pre_Char_Prepare(
  CCtypeReqIspunct_Context *ctx,
  CCtypeReqIspunct_Pre_Char state
)
{
  switch ( state ) {
    case CCtypeReqIspunct_Pre_Char_Punct: {
      /*
       * While the character specified by `c` is a punctuation character.
       */
      ctx->chars = valid_punct;
      ctx->num_chars = RTEMS_ARRAY_SIZE( valid_punct );
      break;
    }

    case CCtypeReqIspunct_Pre_Char_None: {
      /*
       * While the character specified by `c` is not a punctuation character.
       */
      ctx->chars = valid_none;
      ctx->num_chars = RTEMS_ARRAY_SIZE( valid_none );
      break;
    }

    case CCtypeReqIspunct_Pre_Char_NA:
      break;
  }
}

static void CCtypeReqIspunct_Post_Result_Check(
  CCtypeReqIspunct_Context    *ctx,
  CCtypeReqIspunct_Post_Result state
)
{
  switch ( state ) {
    case CCtypeReqIspunct_Post_Result_NonZero: {
      /*
       * The return value shall be non-zero.
       */
      size_t i;

      for ( i = 0; i < ctx->num_chars; ++i ) {
        T_ne_int( ispunct( ctx->chars[ i ] ), 0 );
      }
      break;
    }

    case CCtypeReqIspunct_Post_Result_Zero: {
      /*
       * The return value shall be zero.
       */
      size_t i;

      for ( i = 0; i < ctx->num_chars; ++i ) {
        T_eq_int( ispunct( ctx->chars[ i ] ), 0 );
      }
      break;
    }

    case CCtypeReqIspunct_Post_Result_NA:
      break;
  }
}

static void CCtypeReqIspunct_Action( void )
{
  /* The action is performed in the post-condition checks */
}

static const CCtypeReqIspunct_Entry
CCtypeReqIspunct_Entries[] = {
  { 0, 0, CCtypeReqIspunct_Post_Result_NonZero },
  { 0, 0, CCtypeReqIspunct_Post_Result_Zero }
};

static const uint8_t
CCtypeReqIspunct_Map[] = {
  0, 1
};

static size_t CCtypeReqIspunct_Scope( void *arg, char *buf, size_t n )
{
  CCtypeReqIspunct_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CCtypeReqIspunct_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CCtypeReqIspunct_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CCtypeReqIspunct_Scope,
  .initial_context = &CCtypeReqIspunct_Instance
};

static inline CCtypeReqIspunct_Entry CCtypeReqIspunct_PopEntry(
  CCtypeReqIspunct_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CCtypeReqIspunct_Entries[
    CCtypeReqIspunct_Map[ index ]
  ];
}

static void CCtypeReqIspunct_TestVariant( CCtypeReqIspunct_Context *ctx )
{
  CCtypeReqIspunct_Pre_Char_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CCtypeReqIspunct_Action();
  CCtypeReqIspunct_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CCtypeReqIspunct( void )
 */
T_TEST_CASE_FIXTURE( CCtypeReqIspunct, &CCtypeReqIspunct_Fixture )
{
  CCtypeReqIspunct_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CCtypeReqIspunct_Pre_Char_Punct;
    ctx->Map.pcs[ 0 ] < CCtypeReqIspunct_Pre_Char_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CCtypeReqIspunct_PopEntry( ctx );
    CCtypeReqIspunct_TestVariant( ctx );
  }
}

/** @} */
