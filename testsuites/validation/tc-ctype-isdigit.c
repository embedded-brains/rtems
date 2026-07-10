/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CCtypeReqIsdigit
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
 * @defgroup CCtypeReqIsdigit spec:/c/ctype/req/isdigit
 *
 * @{
 */

typedef enum {
  CCtypeReqIsdigit_Pre_Char_Digit,
  CCtypeReqIsdigit_Pre_Char_NonDigit,
  CCtypeReqIsdigit_Pre_Char_NA
} CCtypeReqIsdigit_Pre_Char;

typedef enum {
  CCtypeReqIsdigit_Post_Result_NonZero,
  CCtypeReqIsdigit_Post_Result_Zero,
  CCtypeReqIsdigit_Post_Result_NA
} CCtypeReqIsdigit_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Char_NA : 1;
  uint8_t Post_Result : 2;
} CCtypeReqIsdigit_Entry;

/**
 * @brief Test context for spec:/c/ctype/req/isdigit test case.
 */
typedef struct {
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
    CCtypeReqIsdigit_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CCtypeReqIsdigit_Context;

static CCtypeReqIsdigit_Context
  CCtypeReqIsdigit_Instance;

static const char * const CCtypeReqIsdigit_PreDesc_Char[] = {
  "Digit",
  "NonDigit",
  "NA"
};

static const char * const * const CCtypeReqIsdigit_PreDesc[] = {
  CCtypeReqIsdigit_PreDesc_Char,
  NULL
};

static const int digits[] = {
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
};

static const int non_digits[] = {
  '\0', 1, 2, 3, 4, 5, 6, '\a', '\b', '\t',
  '\n', '\v', '\f', '\r', 14, 15, 16, 17, 18, 19,
  20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
  30, 31, ' ', '!', '\"', '#', '$', '%', '&', '\'',
  '(', ')', '*', '+', ',', '-', '.', '/', ':', ';',
  '<', '=', '>', '?', '@', 'A', 'B', 'C', 'D', 'E',
  'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
  'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
  'Z', '[', '\\', ']', '^', '_', '`', 'a', 'b', 'c',
  'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
  'x', 'y', 'z', '{', '|', '}', '~', 127, 128, 129,
  130, 131, 132, 133, 134, 135, 136, 137, 138, 139,
  140, 141, 142, 143, 144, 145, 146, 147, 148, 149,
  150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
  160, 161, 162, 163, 164, 165, 166, 167, 168, 169,
  170, 171, 172, 173, 174, 175, 176, 177, 178, 179,
  180, 181, 182, 183, 184, 185, 186, 187, 188, 189,
  190, 191, 192, 193, 194, 195, 196, 197, 198, 199,
  200, 201, 202, 203, 204, 205, 206, 207, 208, 209,
  210, 211, 212, 213, 214, 215, 216, 217, 218, 219,
  220, 221, 222, 223, 224, 225, 226, 227, 228, 229,
  230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
  240, 241, 242, 243, 244, 245, 246, 247, 248, 249,
  250, 251, 252, 253, 254, 255
};

static void CCtypeReqIsdigit_Pre_Char_Prepare(
  CCtypeReqIsdigit_Pre_Char state
)
{
  switch ( state ) {
    case CCtypeReqIsdigit_Pre_Char_Digit: {
      /*
       * While the character specified by `c` is a digit.
       */
      /* Handled by the post-condition loop */
      break;
    }

    case CCtypeReqIsdigit_Pre_Char_NonDigit: {
      /*
       * While the character specified by `c` is not a digit.
       */
      /* Handled by the post-condition loop */
      break;
    }

    case CCtypeReqIsdigit_Pre_Char_NA:
      break;
  }
}

static void CCtypeReqIsdigit_Post_Result_Check(
  CCtypeReqIsdigit_Post_Result state
)
{
  switch ( state ) {
    case CCtypeReqIsdigit_Post_Result_NonZero: {
      /*
       * The return value of the function shall be non-zero.
       */
      size_t i;

      for ( i = 0; i < RTEMS_ARRAY_SIZE( digits ); ++i ) {
        T_ne_int( isdigit( digits[ i ] ), 0 );
      }
      break;
    }

    case CCtypeReqIsdigit_Post_Result_Zero: {
      /*
       * The return value of the function shall be zero.
       */
      size_t i;

      for ( i = 0; i < RTEMS_ARRAY_SIZE( non_digits ); ++i ) {
        T_eq_int( isdigit( non_digits[ i ] ), 0 );
      }
      break;
    }

    case CCtypeReqIsdigit_Post_Result_NA:
      break;
  }
}

static void CCtypeReqIsdigit_Action( void )
{
  /* The action is performed in the post-condition loops */
}

static const CCtypeReqIsdigit_Entry
CCtypeReqIsdigit_Entries[] = {
  { 0, 0, CCtypeReqIsdigit_Post_Result_NonZero },
  { 0, 0, CCtypeReqIsdigit_Post_Result_Zero }
};

static const uint8_t
CCtypeReqIsdigit_Map[] = {
  0, 1
};

static size_t CCtypeReqIsdigit_Scope( void *arg, char *buf, size_t n )
{
  CCtypeReqIsdigit_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CCtypeReqIsdigit_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CCtypeReqIsdigit_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CCtypeReqIsdigit_Scope,
  .initial_context = &CCtypeReqIsdigit_Instance
};

static inline CCtypeReqIsdigit_Entry CCtypeReqIsdigit_PopEntry(
  CCtypeReqIsdigit_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CCtypeReqIsdigit_Entries[
    CCtypeReqIsdigit_Map[ index ]
  ];
}

static void CCtypeReqIsdigit_TestVariant( CCtypeReqIsdigit_Context *ctx )
{
  CCtypeReqIsdigit_Pre_Char_Prepare( ctx->Map.pcs[ 0 ] );
  CCtypeReqIsdigit_Action();
  CCtypeReqIsdigit_Post_Result_Check( ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CCtypeReqIsdigit( void )
 */
T_TEST_CASE_FIXTURE( CCtypeReqIsdigit, &CCtypeReqIsdigit_Fixture )
{
  CCtypeReqIsdigit_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CCtypeReqIsdigit_Pre_Char_Digit;
    ctx->Map.pcs[ 0 ] < CCtypeReqIsdigit_Pre_Char_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CCtypeReqIsdigit_PopEntry( ctx );
    CCtypeReqIsdigit_TestVariant( ctx );
  }
}

/** @} */
