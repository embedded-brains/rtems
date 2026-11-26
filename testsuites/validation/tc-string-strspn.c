/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringReqStrspn
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include <rtems/test.h>

/**
 * @defgroup CStringReqStrspn spec:/c/string/req/strspn
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * The string referenced by the `s2` parameter is followed by the first
 * character of the string referenced by the `s1` parameter which does not
 * occur in it. A segment of a proper prefix or of the whole string holds each
 * character of the string referenced by the `s2` parameter. A character which
 * occurs follows the segment and the end of the string. Where the string
 * referenced by the `s2` parameter holds a character greater than 127, the
 * character which does not occur differs from it in the most significant bit
 * only.
 *
 * @{
 */

typedef enum {
  CStringReqStrspn_Pre_Accept_Empty,
  CStringReqStrspn_Pre_Accept_NonEmpty,
  CStringReqStrspn_Pre_Accept_NA
} CStringReqStrspn_Pre_Accept;

typedef enum {
  CStringReqStrspn_Pre_HighByte_Yes,
  CStringReqStrspn_Pre_HighByte_No,
  CStringReqStrspn_Pre_HighByte_NA
} CStringReqStrspn_Pre_HighByte;

typedef enum {
  CStringReqStrspn_Pre_Segment_Empty,
  CStringReqStrspn_Pre_Segment_None,
  CStringReqStrspn_Pre_Segment_Part,
  CStringReqStrspn_Pre_Segment_Whole,
  CStringReqStrspn_Pre_Segment_NA
} CStringReqStrspn_Pre_Segment;

typedef enum {
  CStringReqStrspn_Post_Result_Zero,
  CStringReqStrspn_Post_Result_Index,
  CStringReqStrspn_Post_Result_Length,
  CStringReqStrspn_Post_Result_NA
} CStringReqStrspn_Post_Result;

typedef enum {
  CStringReqStrspn_Post_S1_Nop,
  CStringReqStrspn_Post_S1_NA
} CStringReqStrspn_Post_S1;

typedef enum {
  CStringReqStrspn_Post_S2_Nop,
  CStringReqStrspn_Post_S2_NA
} CStringReqStrspn_Post_S2;

typedef enum {
  CStringReqStrspn_Post_Errno_Nop,
  CStringReqStrspn_Post_Errno_NA
} CStringReqStrspn_Post_Errno;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Accept_NA : 1;
  uint16_t Pre_HighByte_NA : 1;
  uint16_t Pre_Segment_NA : 1;
  uint16_t Post_Result : 2;
  uint16_t Post_S1 : 1;
  uint16_t Post_S2 : 1;
  uint16_t Post_Errno : 1;
} CStringReqStrspn_Entry;

typedef enum {
  SEGMENT_EMPTY,
  SEGMENT_NONE,
  SEGMENT_PART,
  SEGMENT_WHOLE
} Segment;

/**
 * @brief Test context for spec:/c/string/req/strspn test case.
 */
typedef struct {
  /**
   * @brief This member specifies the length of the string referenced by the
   *   `s2` parameter.
   */
  size_t accept_length;

  /**
   * @brief This member is true, if the string referenced by the `s2` parameter
   *   holds a character with a value greater than 127.
   */
  bool high;

  /**
   * @brief This member specifies the first character of the string referenced
   *   by the `s2` parameter, if the member high is true.
   */
  unsigned char high_byte;

  /**
   * @brief This member specifies the maximum initial segment.
   */
  Segment segment;

  /**
   * @brief This member contains the index of the first character of the string
   *   referenced by the `s1` parameter which does not occur in the string
   *   referenced by the `s2` parameter.
   */
  size_t stop;

  /**
   * @brief This member contains the length of the string referenced by the
   *   `s1` parameter.
   */
  size_t length;

  /**
   * @brief This member provides the string referenced by the `s1` parameter.
   */
  unsigned char s1_buf[ 16 ];

  /**
   * @brief This member provides the string referenced by the `s2` parameter.
   */
  unsigned char s2_buf[ 8 ];

  /**
   * @brief This member contains the content of the memory which holds the
   *   string referenced by the `s1` parameter before the strspn() call.
   */
  unsigned char s1_before[ 16 ];

  /**
   * @brief This member contains the content of the memory which holds the
   *   string referenced by the `s2` parameter before the strspn() call.
   */
  unsigned char s2_before[ 8 ];

  /**
   * @brief This member contains the return value of the strspn() call.
   */
  size_t retval;

  /**
   * @brief This member contains the value of errno after the strspn() call.
   */
  int errno_value;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 3 ];

    /**
     * @brief This member defines the representative index for the next action.
     */
    size_t rep;

    /**
     * @brief If this member is true, then the current run of the
     *   representatives should be skipped.
     */
    bool skip_rep;

    /**
     * @brief This member records the representatives which ran.
     */
    bool rep_ran[ 15 ];

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
    CStringReqStrspn_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringReqStrspn_Context;

static CStringReqStrspn_Context CStringReqStrspn_Instance;

static const char *const CStringReqStrspn_PreDesc_Accept[] =
  { "Empty", "NonEmptyOne", "NonEmptyTwo", "NonEmptyThree", "NA" };

static const char *const CStringReqStrspn_PreDesc_HighByte[] =
  { "YesMin", "YesMid", "YesMax", "No", "NA" };

static const char *const CStringReqStrspn_PreDesc_Segment[] =
  { "Empty", "None", "Part", "Whole", "NA" };

static const char *const *const CStringReqStrspn_PreDesc[] = {
  CStringReqStrspn_PreDesc_Accept,
  CStringReqStrspn_PreDesc_HighByte,
  CStringReqStrspn_PreDesc_Segment,
  NULL
};

typedef enum {
  CStringReqStrspn_Pre_Accept_NonEmpty_One,
  CStringReqStrspn_Pre_Accept_NonEmpty_Two,
  CStringReqStrspn_Pre_Accept_NonEmpty_Three
} CStringReqStrspn_Pre_Accept_NonEmpty_Rep;

typedef enum {
  CStringReqStrspn_Pre_HighByte_Yes_Min,
  CStringReqStrspn_Pre_HighByte_Yes_Mid,
  CStringReqStrspn_Pre_HighByte_Yes_Max
} CStringReqStrspn_Pre_HighByte_Yes_Rep;

static const uint8_t CStringReqStrspn_PreRepOffset_Accept[] = { 0, 1, 4, 5 };

static const uint8_t CStringReqStrspn_PreRepOffset_HighByte[] = { 0, 3, 4, 5 };

static const uint8_t CStringReqStrspn_PreRepOffset_Segment[] =
  { 0, 1, 2, 3, 4, 5 };

static const uint8_t *const CStringReqStrspn_PreRepOffset[] = {
  CStringReqStrspn_PreRepOffset_Accept,
  CStringReqStrspn_PreRepOffset_HighByte,
  CStringReqStrspn_PreRepOffset_Segment
};

static size_t CStringReqStrspn_GetRepresentativeCountOf(
  const CStringReqStrspn_Context *ctx,
  size_t                          index
)
{
  const uint8_t *offsets;
  size_t         state;

  offsets = CStringReqStrspn_PreRepOffset[ index ];
  state = ctx->Map.pcs[ index ];

  return offsets[ state + 1 ] - offsets[ state ];
}

static size_t CStringReqStrspn_GetRepresentative(
  const CStringReqStrspn_Context *ctx,
  size_t                          index
)
{
  size_t count;

  count = CStringReqStrspn_GetRepresentativeCountOf( ctx, index );

  if ( ctx->Map.rep < count ) {
    return ctx->Map.rep;
  }

  return count - 1;
}

static size_t CStringReqStrspn_GetRepresentativeCount(
  const CStringReqStrspn_Context *ctx
)
{
  size_t count;
  size_t i;

  count = 1;

  for ( i = 0; i < 3; ++i ) {
    size_t count_of_condition;

    count_of_condition = CStringReqStrspn_GetRepresentativeCountOf( ctx, i );

    if ( count_of_condition > count ) {
      count = count_of_condition;
    }
  }

  return count;
}

static CStringReqStrspn_Pre_Accept_NonEmpty_Rep
CStringReqStrspn_Pre_Accept_NonEmpty_GetRepresentative(
  const CStringReqStrspn_Context *ctx
)
{
  return (
    CStringReqStrspn_Pre_Accept_NonEmpty_Rep
  ) CStringReqStrspn_GetRepresentative( ctx, 0 );
}

static CStringReqStrspn_Pre_HighByte_Yes_Rep
CStringReqStrspn_Pre_HighByte_Yes_GetRepresentative(
  const CStringReqStrspn_Context *ctx
)
{
  return (
    CStringReqStrspn_Pre_HighByte_Yes_Rep
  ) CStringReqStrspn_GetRepresentative( ctx, 1 );
}

static const uint8_t CStringReqStrspn_PreRepBase[] = { 0, 5, 10 };

static const uint8_t CStringReqStrspn_PreRepStates[] = { 2, 2, 4 };

static void CStringReqStrspn_ResetRepresentatives(
  CStringReqStrspn_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 15; ++i ) {
    ctx->Map.rep_ran[ i ] = false;
  }
}

static void CStringReqStrspn_MarkRepresentatives(
  CStringReqStrspn_Context *ctx
)
{
  size_t i;

  if ( ctx->Map.skip_rep ) {
    return;
  }

  for ( i = 0; i < 3; ++i ) {
    size_t rep;

    rep = CStringReqStrspn_PreRepBase[ i ] +
          CStringReqStrspn_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
          CStringReqStrspn_GetRepresentative( ctx, i );
    ctx->Map.rep_ran[ rep ] = true;
  }
}

static void CStringReqStrspn_CheckRepresentatives(
  const CStringReqStrspn_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 3; ++i ) {
    const uint8_t *offsets;
    const bool    *ran;
    size_t         state;

    offsets = CStringReqStrspn_PreRepOffset[ i ];
    ran = &ctx->Map.rep_ran[ CStringReqStrspn_PreRepBase[ i ] ];

    for ( state = 0; state < CStringReqStrspn_PreRepStates[ i ]; ++state ) {
      size_t k;
      bool   any;

      any = false;

      for ( k = offsets[ state ]; k < offsets[ state + 1 ]; ++k ) {
        any = any || ran[ k ];
      }

      if ( any ) {
        for ( k = offsets[ state ]; k < offsets[ state + 1 ]; ++k ) {
          T_quiet_true(
            ran[ k ],
            "representative %s did not run",
            CStringReqStrspn_PreDesc[ i ][ k ]
          );
        }
      }
    }
  }
}

typedef CStringReqStrspn_Context Context;

#define ACCEPT_MAX 3

static const unsigned char accept_low[ ACCEPT_MAX ] = { 'a', 'c', 'e' };

static const unsigned char accept_high[ ACCEPT_MAX ] = { 0xe2, 'c', 'e' };

static void CheckUnchanged(
  const unsigned char *actual,
  const unsigned char *expected,
  size_t               size
)
{
  size_t i;

  for ( i = 0; i < size; ++i ) {
    T_quiet_eq_u8( actual[ i ], expected[ i ] );
  }
}

static void Build( Context *ctx )
{
  const unsigned char *members;
  unsigned char        high[ ACCEPT_MAX ];
  unsigned char        reject;
  size_t               i;
  size_t               n;

  if ( ctx->high ) {
    memcpy( high, accept_high, sizeof( high ) );
    high[ 0 ] = ctx->high_byte;
    members = high;
    reject = 0x62;
  } else {
    members = accept_low;
    reject = 'b';
  }

  for ( i = 0; i < ctx->accept_length; ++i ) {
    ctx->s2_buf[ i ] = members[ i ];
  }

  ctx->s2_buf[ ctx->accept_length + 1 ] = reject;
  n = 0;

  switch ( ctx->segment ) {
    case SEGMENT_NONE:
      ctx->s1_buf[ n ] = reject;
      ++n;
      ctx->s1_buf[ n ] = members[ 0 ];
      ++n;
      break;
    case SEGMENT_PART:
    case SEGMENT_WHOLE:
      for ( i = 0; i < 2 * ctx->accept_length; ++i ) {
        ctx->s1_buf[ n ] = members[ i % ctx->accept_length ];
        ++n;
      }

      ctx->stop = n;

      if ( ctx->segment == SEGMENT_PART ) {
        ctx->s1_buf[ n ] = reject;
        ++n;
        ctx->s1_buf[ n ] = members[ 0 ];
        ++n;
      }

      break;
    default:
      break;
  }

  ctx->length = n;
  ctx->s1_buf[ n + 1 ] = members[ 0 ];
}

static void CStringReqStrspn_Pre_Accept_Prepare(
  CStringReqStrspn_Context   *ctx,
  CStringReqStrspn_Pre_Accept state
)
{
  switch ( state ) {
    case CStringReqStrspn_Pre_Accept_Empty: {
      /*
       * While the length of the string referenced by the `s2` parameter is
       * equal to zero.
       */
      ctx->accept_length = 0;
      break;
    }

    case CStringReqStrspn_Pre_Accept_NonEmpty: {
      /*
       * While the length of the string referenced by the `s2` parameter is
       * greater than zero.
       */
      switch (
        CStringReqStrspn_Pre_Accept_NonEmpty_GetRepresentative( ctx )
      ) {
        case CStringReqStrspn_Pre_Accept_NonEmpty_One: {
          ctx->accept_length = 1;
          break;
        }

        case CStringReqStrspn_Pre_Accept_NonEmpty_Two: {
          ctx->accept_length = 2;
          break;
        }

        case CStringReqStrspn_Pre_Accept_NonEmpty_Three: {
          ctx->accept_length = ACCEPT_MAX;
          break;
        }
      }
      break;
    }

    case CStringReqStrspn_Pre_Accept_NA:
      break;
  }
}

static void CStringReqStrspn_Pre_HighByte_Prepare(
  CStringReqStrspn_Context     *ctx,
  CStringReqStrspn_Pre_HighByte state
)
{
  switch ( state ) {
    case CStringReqStrspn_Pre_HighByte_Yes: {
      /*
       * While the string referenced by the `s2` parameter holds a character
       * with a value greater than 127, where the value is interpreted as type
       * `unsigned char`.
       */
      switch ( CStringReqStrspn_Pre_HighByte_Yes_GetRepresentative( ctx ) ) {
        case CStringReqStrspn_Pre_HighByte_Yes_Min: {
          ctx->high_byte = 0x80;
          break;
        }

        case CStringReqStrspn_Pre_HighByte_Yes_Mid: {
          ctx->high_byte = 0xe2;
          break;
        }

        case CStringReqStrspn_Pre_HighByte_Yes_Max: {
          ctx->high_byte = 0xff;
          break;
        }
      }

      ctx->high = true;
      break;
    }

    case CStringReqStrspn_Pre_HighByte_No: {
      /*
       * While the string referenced by the `s2` parameter holds no character
       * with a value greater than 127, where the value is interpreted as type
       * `unsigned char`.
       */
      ctx->high = false;
      break;
    }

    case CStringReqStrspn_Pre_HighByte_NA:
      break;
  }
}

static void CStringReqStrspn_Pre_Segment_Prepare(
  CStringReqStrspn_Context    *ctx,
  CStringReqStrspn_Pre_Segment state
)
{
  switch ( state ) {
    case CStringReqStrspn_Pre_Segment_Empty: {
      /*
       * While the length of the string referenced by the `s1` parameter is
       * equal to zero.
       */
      ctx->segment = SEGMENT_EMPTY;
      break;
    }

    case CStringReqStrspn_Pre_Segment_None: {
      /*
       * While the length of the string referenced by the `s1` parameter is
       * greater than zero, while the first character of the string referenced
       * by the `s1` parameter does not occur in the string referenced by the
       * `s2` parameter.
       */
      ctx->segment = SEGMENT_NONE;
      break;
    }

    case CStringReqStrspn_Pre_Segment_Part: {
      /*
       * While the first character of the string referenced by the `s1`
       * parameter occurs in the string referenced by the `s2` parameter, while
       * a character of the string referenced by the `s1` parameter does not
       * occur in the string referenced by the `s2` parameter.
       */
      ctx->segment = SEGMENT_PART;
      break;
    }

    case CStringReqStrspn_Pre_Segment_Whole: {
      /*
       * While the length of the string referenced by the `s1` parameter is
       * greater than zero, while each character of the string referenced by
       * the `s1` parameter occurs in the string referenced by the `s2`
       * parameter.
       */
      ctx->segment = SEGMENT_WHOLE;
      break;
    }

    case CStringReqStrspn_Pre_Segment_NA:
      break;
  }
}

static void CStringReqStrspn_Post_Result_Check(
  CStringReqStrspn_Context    *ctx,
  CStringReqStrspn_Post_Result state
)
{
  switch ( state ) {
    case CStringReqStrspn_Post_Result_Zero: {
      /*
       * The return value of strspn() shall be equal to zero.
       */
      T_eq_sz( ctx->retval, 0 );
      break;
    }

    case CStringReqStrspn_Post_Result_Index: {
      /*
       * The return value of strspn() shall be equal to the index of the first
       * character of the string referenced by the `s1` parameter which does
       * not occur in the string referenced by the `s2` parameter.
       */
      T_eq_sz( ctx->retval, ctx->stop );
      break;
    }

    case CStringReqStrspn_Post_Result_Length: {
      /*
       * The return value of strspn() shall be equal to the length of the
       * string referenced by the `s1` parameter.
       */
      T_eq_sz( ctx->retval, ctx->length );
      break;
    }

    case CStringReqStrspn_Post_Result_NA:
      break;
  }
}

static void CStringReqStrspn_Post_S1_Check(
  CStringReqStrspn_Context *ctx,
  CStringReqStrspn_Post_S1  state
)
{
  switch ( state ) {
    case CStringReqStrspn_Post_S1_Nop: {
      /*
       * The bytes of the string referenced by the `s1` parameter shall not be
       * modified.
       */
      CheckUnchanged( ctx->s1_buf, ctx->s1_before, sizeof( ctx->s1_buf ) );
      break;
    }

    case CStringReqStrspn_Post_S1_NA:
      break;
  }
}

static void CStringReqStrspn_Post_S2_Check(
  CStringReqStrspn_Context *ctx,
  CStringReqStrspn_Post_S2  state
)
{
  switch ( state ) {
    case CStringReqStrspn_Post_S2_Nop: {
      /*
       * The bytes of the string referenced by the `s2` parameter shall not be
       * modified.
       */
      CheckUnchanged( ctx->s2_buf, ctx->s2_before, sizeof( ctx->s2_buf ) );
      break;
    }

    case CStringReqStrspn_Post_S2_NA:
      break;
  }
}

static void CStringReqStrspn_Post_Errno_Check(
  CStringReqStrspn_Context   *ctx,
  CStringReqStrspn_Post_Errno state
)
{
  switch ( state ) {
    case CStringReqStrspn_Post_Errno_Nop: {
      /*
       * The value of errno shall not be modified.
       */
      T_eq_int( ctx->errno_value, -1 );
      break;
    }

    case CStringReqStrspn_Post_Errno_NA:
      break;
  }
}

static void CStringReqStrspn_Prepare( CStringReqStrspn_Context *ctx )
{
  memset( ctx->s1_buf, 0, sizeof( ctx->s1_buf ) );
  memset( ctx->s2_buf, 0, sizeof( ctx->s2_buf ) );
  ctx->stop = 0;
  ctx->length = 0;
  ctx->retval = SIZE_MAX;
}

static void CStringReqStrspn_Action( CStringReqStrspn_Context *ctx )
{
  Build( ctx );
  memcpy( ctx->s1_before, ctx->s1_buf, sizeof( ctx->s1_buf ) );
  memcpy( ctx->s2_before, ctx->s2_buf, sizeof( ctx->s2_buf ) );
  errno = -1;
  ctx->retval = strspn(
    (const char *) ctx->s1_buf,
    (const char *) ctx->s2_buf
  );
  ctx->errno_value = errno;
}

/* clang-format off */

static const CStringReqStrspn_Entry
CStringReqStrspn_Entries[] = {
  { 1, 0, 0, 0, CStringReqStrspn_Post_Result_NA, CStringReqStrspn_Post_S1_NA,
    CStringReqStrspn_Post_S2_NA, CStringReqStrspn_Post_Errno_NA },
  { 0, 0, 0, 0, CStringReqStrspn_Post_Result_Zero,
    CStringReqStrspn_Post_S1_Nop, CStringReqStrspn_Post_S2_Nop,
    CStringReqStrspn_Post_Errno_Nop },
  { 0, 0, 0, 0, CStringReqStrspn_Post_Result_Index,
    CStringReqStrspn_Post_S1_Nop, CStringReqStrspn_Post_S2_Nop,
    CStringReqStrspn_Post_Errno_Nop },
  { 0, 0, 0, 0, CStringReqStrspn_Post_Result_Length,
    CStringReqStrspn_Post_S1_Nop, CStringReqStrspn_Post_S2_Nop,
    CStringReqStrspn_Post_Errno_Nop }
};

static const uint8_t
CStringReqStrspn_Map[] = {
  0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 2, 3, 1, 1, 2, 3
};

/* clang-format on */

static size_t CStringReqStrspn_Scope( void *arg, char *buf, size_t n )
{
  CStringReqStrspn_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    size_t pcd[ 3 ];
    size_t i;

    for ( i = 0; i < 3; ++i ) {
      pcd[ i ] = CStringReqStrspn_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
                 CStringReqStrspn_GetRepresentative( ctx, i );
    }

    return T_get_scope( CStringReqStrspn_PreDesc, buf, n, pcd );
  }

  return 0;
}

static T_fixture CStringReqStrspn_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringReqStrspn_Scope,
  .initial_context = &CStringReqStrspn_Instance
};

static inline CStringReqStrspn_Entry CStringReqStrspn_PopEntry(
  CStringReqStrspn_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringReqStrspn_Entries[ CStringReqStrspn_Map[ index ] ];
}

static void CStringReqStrspn_TestVariant( CStringReqStrspn_Context *ctx )
{
  CStringReqStrspn_Pre_Accept_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringReqStrspn_Pre_HighByte_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStringReqStrspn_Pre_Segment_Prepare( ctx, ctx->Map.pcs[ 2 ] );

  if ( ctx->Map.skip_rep ) {
    return;
  }

  CStringReqStrspn_Action( ctx );
  CStringReqStrspn_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
  CStringReqStrspn_Post_S1_Check( ctx, ctx->Map.entry.Post_S1 );
  CStringReqStrspn_Post_S2_Check( ctx, ctx->Map.entry.Post_S2 );
  CStringReqStrspn_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
}

/**
 * @fn void T_case_body_CStringReqStrspn( void )
 */
T_TEST_CASE_FIXTURE( CStringReqStrspn, &CStringReqStrspn_Fixture )
{
  CStringReqStrspn_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  CStringReqStrspn_ResetRepresentatives( ctx );

  for (
    ctx->Map.pcs[ 0 ] = CStringReqStrspn_Pre_Accept_Empty;
    ctx->Map.pcs[ 0 ] < CStringReqStrspn_Pre_Accept_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CStringReqStrspn_Pre_HighByte_Yes;
      ctx->Map.pcs[ 1 ] < CStringReqStrspn_Pre_HighByte_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = CStringReqStrspn_Pre_Segment_Empty;
        ctx->Map.pcs[ 2 ] < CStringReqStrspn_Pre_Segment_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = CStringReqStrspn_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        for (
          ctx->Map.rep = 0;
          ctx->Map.rep < CStringReqStrspn_GetRepresentativeCount( ctx );
          ++ctx->Map.rep
        ) {
          ctx->Map.skip_rep = false;
          CStringReqStrspn_Prepare( ctx );
          CStringReqStrspn_TestVariant( ctx );

          CStringReqStrspn_MarkRepresentatives( ctx );
        }
      }
    }
  }

  CStringReqStrspn_CheckRepresentatives( ctx );
}

/** @} */
