/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup DevGrlibReqApbuartOutputCharPolled
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

#include <string.h>
#include <grlib/apbuart.h>
#include <rtems/bspIo.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup DevGrlibReqApbuartOutputCharPolled \
 *   spec:/dev/grlib/req/apbuart-output-char-polled
 *
 * @ingroup TestsuitesBspsValidationBsp0
 *
 * @{
 */

typedef enum {
  DevGrlibReqApbuartOutputCharPolled_Pre_Request_Char,
  DevGrlibReqApbuartOutputCharPolled_Pre_Request_Negative,
  DevGrlibReqApbuartOutputCharPolled_Pre_Request_Try,
  DevGrlibReqApbuartOutputCharPolled_Pre_Request_Drain,
  DevGrlibReqApbuartOutputCharPolled_Pre_Request_DrainTry,
  DevGrlibReqApbuartOutputCharPolled_Pre_Request_NA
} DevGrlibReqApbuartOutputCharPolled_Pre_Request;

typedef enum {
  DevGrlibReqApbuartOutputCharPolled_Pre_FifoEmpty_Yes,
  DevGrlibReqApbuartOutputCharPolled_Pre_FifoEmpty_No,
  DevGrlibReqApbuartOutputCharPolled_Pre_FifoEmpty_NA
} DevGrlibReqApbuartOutputCharPolled_Pre_FifoEmpty;

typedef enum {
  DevGrlibReqApbuartOutputCharPolled_Pre_ShiftEmpty_Yes,
  DevGrlibReqApbuartOutputCharPolled_Pre_ShiftEmpty_No,
  DevGrlibReqApbuartOutputCharPolled_Pre_ShiftEmpty_NA
} DevGrlibReqApbuartOutputCharPolled_Pre_ShiftEmpty;

typedef enum {
  DevGrlibReqApbuartOutputCharPolled_Post_Status_Zero,
  DevGrlibReqApbuartOutputCharPolled_Post_Status_MinusOne,
  DevGrlibReqApbuartOutputCharPolled_Post_Status_NA
} DevGrlibReqApbuartOutputCharPolled_Post_Status;

typedef enum {
  DevGrlibReqApbuartOutputCharPolled_Post_Action_Store,
  DevGrlibReqApbuartOutputCharPolled_Post_Action_WaitStore,
  DevGrlibReqApbuartOutputCharPolled_Post_Action_Wait,
  DevGrlibReqApbuartOutputCharPolled_Post_Action_None,
  DevGrlibReqApbuartOutputCharPolled_Post_Action_NA
} DevGrlibReqApbuartOutputCharPolled_Post_Action;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Request_NA : 1;
  uint16_t Pre_FifoEmpty_NA : 1;
  uint16_t Pre_ShiftEmpty_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Action : 3;
} DevGrlibReqApbuartOutputCharPolled_Entry;

/**
 * @brief Test context for spec:/dev/grlib/req/apbuart-output-char-polled test
 *   case.
 */
typedef struct {
  /**
   * @brief This member provides the APBUART register block.
   */
  apbuart regs;

  /**
   * @brief This member contains the count of \_IO_Relax() calls.
   */
  uint32_t io_relax_counter;

  /**
   * @brief This member specifies the second parameter of the action.
   */
  int c;

  /**
   * @brief This member contains the expected value of the data register.
   */
  uint32_t expected;

  /**
   * @brief This member contains the return value of the action.
   */
  int status;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 3 ];

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
    DevGrlibReqApbuartOutputCharPolled_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} DevGrlibReqApbuartOutputCharPolled_Context;

static DevGrlibReqApbuartOutputCharPolled_Context
  DevGrlibReqApbuartOutputCharPolled_Instance;

static const char *const DevGrlibReqApbuartOutputCharPolled_PreDesc_Request[] =
  { "Char", "Negative", "Try", "Drain", "DrainTry", "NA" };

static const char *const
  DevGrlibReqApbuartOutputCharPolled_PreDesc_FifoEmpty[] =
    { "Yes", "No", "NA" };

static const char *const
  DevGrlibReqApbuartOutputCharPolled_PreDesc_ShiftEmpty[] =
    { "Yes", "No", "NA" };

static const char *const *const DevGrlibReqApbuartOutputCharPolled_PreDesc[] =
  { DevGrlibReqApbuartOutputCharPolled_PreDesc_Request,
    DevGrlibReqApbuartOutputCharPolled_PreDesc_FifoEmpty,
    DevGrlibReqApbuartOutputCharPolled_PreDesc_ShiftEmpty,
    NULL };

typedef DevGrlibReqApbuartOutputCharPolled_Context Context;

static void IORelaxHandler( void *arg )
{
  Context *ctx;

  ctx = arg;
  ++ctx->io_relax_counter;
  ctx->regs.status = APBUART_STATUS_TE | APBUART_STATUS_TS;
}

static void DevGrlibReqApbuartOutputCharPolled_Pre_Request_Prepare(
  DevGrlibReqApbuartOutputCharPolled_Context    *ctx,
  DevGrlibReqApbuartOutputCharPolled_Pre_Request state
)
{
  switch ( state ) {
    case DevGrlibReqApbuartOutputCharPolled_Pre_Request_Char: {
      /*
       * While the second parameter specifies a character and no flag.
       */
      ctx->c = 'c';
      ctx->expected = 'c';
      break;
    }

    case DevGrlibReqApbuartOutputCharPolled_Pre_Request_Negative: {
      /*
       * While the second parameter is a negative value.
       */
      ctx->c = (signed char) 0xe9;
      ctx->expected = 0xe9;
      break;
    }

    case DevGrlibReqApbuartOutputCharPolled_Pre_Request_Try: {
      /*
       * While the second parameter specifies a character and
       * RTEMS_OUTPUT_CHAR_TRY.
       */
      ctx->c = RTEMS_OUTPUT_CHAR_TRY | 'c';
      ctx->expected = 'c';
      break;
    }

    case DevGrlibReqApbuartOutputCharPolled_Pre_Request_Drain: {
      /*
       * While the second parameter specifies RTEMS_OUTPUT_CHAR_DRAIN and not
       * RTEMS_OUTPUT_CHAR_TRY.
       */
      ctx->c = RTEMS_OUTPUT_CHAR_DRAIN | 'c';
      ctx->expected = 0;
      break;
    }

    case DevGrlibReqApbuartOutputCharPolled_Pre_Request_DrainTry: {
      /*
       * While the second parameter specifies RTEMS_OUTPUT_CHAR_DRAIN and
       * RTEMS_OUTPUT_CHAR_TRY.
       */
      ctx->c = RTEMS_OUTPUT_CHAR_DRAIN | RTEMS_OUTPUT_CHAR_TRY | 'c';
      ctx->expected = 0;
      break;
    }

    case DevGrlibReqApbuartOutputCharPolled_Pre_Request_NA:
      break;
  }
}

static void DevGrlibReqApbuartOutputCharPolled_Pre_FifoEmpty_Prepare(
  DevGrlibReqApbuartOutputCharPolled_Context      *ctx,
  DevGrlibReqApbuartOutputCharPolled_Pre_FifoEmpty state
)
{
  switch ( state ) {
    case DevGrlibReqApbuartOutputCharPolled_Pre_FifoEmpty_Yes: {
      /*
       * While the transmitter FIFO is empty.
       */
      ctx->regs.status |= APBUART_STATUS_TE;
      break;
    }

    case DevGrlibReqApbuartOutputCharPolled_Pre_FifoEmpty_No: {
      /*
       * While the transmitter FIFO is not empty.
       */
      ctx->regs.status &= ~APBUART_STATUS_TE;
      break;
    }

    case DevGrlibReqApbuartOutputCharPolled_Pre_FifoEmpty_NA:
      break;
  }
}

static void DevGrlibReqApbuartOutputCharPolled_Pre_ShiftEmpty_Prepare(
  DevGrlibReqApbuartOutputCharPolled_Context       *ctx,
  DevGrlibReqApbuartOutputCharPolled_Pre_ShiftEmpty state
)
{
  switch ( state ) {
    case DevGrlibReqApbuartOutputCharPolled_Pre_ShiftEmpty_Yes: {
      /*
       * While the transmitter shift register is empty.
       */
      ctx->regs.status |= APBUART_STATUS_TS;
      break;
    }

    case DevGrlibReqApbuartOutputCharPolled_Pre_ShiftEmpty_No: {
      /*
       * While the transmitter shift register is not empty.
       */
      ctx->regs.status &= ~APBUART_STATUS_TS;
      break;
    }

    case DevGrlibReqApbuartOutputCharPolled_Pre_ShiftEmpty_NA:
      break;
  }
}

static void DevGrlibReqApbuartOutputCharPolled_Post_Status_Check(
  DevGrlibReqApbuartOutputCharPolled_Context    *ctx,
  DevGrlibReqApbuartOutputCharPolled_Post_Status state
)
{
  switch ( state ) {
    case DevGrlibReqApbuartOutputCharPolled_Post_Status_Zero: {
      /*
       * The return value of apbuart_output_char_polled() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case DevGrlibReqApbuartOutputCharPolled_Post_Status_MinusOne: {
      /*
       * The return value of apbuart_output_char_polled() shall be minus one.
       */
      T_eq_int( ctx->status, -1 );
      break;
    }

    case DevGrlibReqApbuartOutputCharPolled_Post_Status_NA:
      break;
  }
}

static void DevGrlibReqApbuartOutputCharPolled_Post_Action_Check(
  DevGrlibReqApbuartOutputCharPolled_Context    *ctx,
  DevGrlibReqApbuartOutputCharPolled_Post_Action state
)
{
  switch ( state ) {
    case DevGrlibReqApbuartOutputCharPolled_Post_Action_Store: {
      /*
       * The apbuart_output_char_polled() function shall store the character
       * specified by `c` to the data register without a wait.
       */
      T_eq_int( ctx->io_relax_counter, 0 );
      T_eq_u32( ctx->regs.data, ctx->expected );
      break;
    }

    case DevGrlibReqApbuartOutputCharPolled_Post_Action_WaitStore: {
      /*
       * The apbuart_output_char_polled() function shall wait until the
       * transmitter FIFO is empty and then store the character specified by
       * `c` to the data register.
       */
      T_eq_int( ctx->io_relax_counter, 1 );
      T_eq_u32( ctx->regs.data, ctx->expected );
      break;
    }

    case DevGrlibReqApbuartOutputCharPolled_Post_Action_Wait: {
      /*
       * The apbuart_output_char_polled() function shall wait until the
       * transmitter FIFO and the transmitter shift register are empty and
       * store no character.
       */
      T_eq_int( ctx->io_relax_counter, 1 );
      T_eq_u32( ctx->regs.data, 0 );
      break;
    }

    case DevGrlibReqApbuartOutputCharPolled_Post_Action_None: {
      /*
       * The apbuart_output_char_polled() function shall neither wait nor store
       * a character.
       */
      T_eq_int( ctx->io_relax_counter, 0 );
      T_eq_u32( ctx->regs.data, 0 );
      break;
    }

    case DevGrlibReqApbuartOutputCharPolled_Post_Action_NA:
      break;
  }
}

static void DevGrlibReqApbuartOutputCharPolled_Setup(
  DevGrlibReqApbuartOutputCharPolled_Context *ctx
)
{
  SetIORelaxHandler( IORelaxHandler, ctx );
}

static void DevGrlibReqApbuartOutputCharPolled_Setup_Wrap( void *arg )
{
  DevGrlibReqApbuartOutputCharPolled_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  DevGrlibReqApbuartOutputCharPolled_Setup( ctx );
}

static void DevGrlibReqApbuartOutputCharPolled_Teardown( void )
{
  SetIORelaxHandler( NULL, NULL );
}

static void DevGrlibReqApbuartOutputCharPolled_Teardown_Wrap( void *arg )
{
  DevGrlibReqApbuartOutputCharPolled_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  DevGrlibReqApbuartOutputCharPolled_Teardown();
}

static void DevGrlibReqApbuartOutputCharPolled_Prepare(
  DevGrlibReqApbuartOutputCharPolled_Context *ctx
)
{
  ctx->io_relax_counter = 0;
  memset( &ctx->regs, 0, sizeof( ctx->regs ) );
}

static void DevGrlibReqApbuartOutputCharPolled_Action(
  DevGrlibReqApbuartOutputCharPolled_Context *ctx
)
{
  ctx->status = apbuart_output_char_polled( &ctx->regs, ctx->c );
}

/* clang-format off */

static const DevGrlibReqApbuartOutputCharPolled_Entry
DevGrlibReqApbuartOutputCharPolled_Entries[] = {
  { 0, 0, 0, 0, DevGrlibReqApbuartOutputCharPolled_Post_Status_Zero,
    DevGrlibReqApbuartOutputCharPolled_Post_Action_Store },
  { 0, 0, 0, 0, DevGrlibReqApbuartOutputCharPolled_Post_Status_MinusOne,
    DevGrlibReqApbuartOutputCharPolled_Post_Action_None },
  { 0, 0, 0, 0, DevGrlibReqApbuartOutputCharPolled_Post_Status_Zero,
    DevGrlibReqApbuartOutputCharPolled_Post_Action_WaitStore },
  { 0, 0, 0, 0, DevGrlibReqApbuartOutputCharPolled_Post_Status_Zero,
    DevGrlibReqApbuartOutputCharPolled_Post_Action_Wait },
  { 0, 0, 0, 0, DevGrlibReqApbuartOutputCharPolled_Post_Status_Zero,
    DevGrlibReqApbuartOutputCharPolled_Post_Action_None }
};

static const uint8_t
DevGrlibReqApbuartOutputCharPolled_Map[] = {
  0, 0, 2, 2, 0, 0, 2, 2, 0, 0, 1, 1, 4, 3, 3, 3, 4, 1, 1, 1
};

/* clang-format on */

static size_t DevGrlibReqApbuartOutputCharPolled_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  DevGrlibReqApbuartOutputCharPolled_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      DevGrlibReqApbuartOutputCharPolled_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture DevGrlibReqApbuartOutputCharPolled_Fixture = {
  .setup = DevGrlibReqApbuartOutputCharPolled_Setup_Wrap,
  .stop = NULL,
  .teardown = DevGrlibReqApbuartOutputCharPolled_Teardown_Wrap,
  .scope = DevGrlibReqApbuartOutputCharPolled_Scope,
  .initial_context = &DevGrlibReqApbuartOutputCharPolled_Instance
};

static inline DevGrlibReqApbuartOutputCharPolled_Entry
DevGrlibReqApbuartOutputCharPolled_PopEntry(
  DevGrlibReqApbuartOutputCharPolled_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return DevGrlibReqApbuartOutputCharPolled_Entries
    [ DevGrlibReqApbuartOutputCharPolled_Map[ index ] ];
}

static void DevGrlibReqApbuartOutputCharPolled_TestVariant(
  DevGrlibReqApbuartOutputCharPolled_Context *ctx
)
{
  DevGrlibReqApbuartOutputCharPolled_Pre_Request_Prepare(
    ctx,
    ctx->Map.pcs[ 0 ]
  );
  DevGrlibReqApbuartOutputCharPolled_Pre_FifoEmpty_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  DevGrlibReqApbuartOutputCharPolled_Pre_ShiftEmpty_Prepare(
    ctx,
    ctx->Map.pcs[ 2 ]
  );
  DevGrlibReqApbuartOutputCharPolled_Action( ctx );
  DevGrlibReqApbuartOutputCharPolled_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  DevGrlibReqApbuartOutputCharPolled_Post_Action_Check(
    ctx,
    ctx->Map.entry.Post_Action
  );
}

/**
 * @fn void T_case_body_DevGrlibReqApbuartOutputCharPolled( void )
 */
T_TEST_CASE_FIXTURE(
  DevGrlibReqApbuartOutputCharPolled,
  &DevGrlibReqApbuartOutputCharPolled_Fixture
)
{
  DevGrlibReqApbuartOutputCharPolled_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = DevGrlibReqApbuartOutputCharPolled_Pre_Request_Char;
    ctx->Map.pcs[ 0 ] < DevGrlibReqApbuartOutputCharPolled_Pre_Request_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = DevGrlibReqApbuartOutputCharPolled_Pre_FifoEmpty_Yes;
      ctx->Map.pcs[ 1 ] < DevGrlibReqApbuartOutputCharPolled_Pre_FifoEmpty_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] =
          DevGrlibReqApbuartOutputCharPolled_Pre_ShiftEmpty_Yes;
        ctx->Map.pcs[ 2 ] <
        DevGrlibReqApbuartOutputCharPolled_Pre_ShiftEmpty_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = DevGrlibReqApbuartOutputCharPolled_PopEntry( ctx );
        DevGrlibReqApbuartOutputCharPolled_Prepare( ctx );
        DevGrlibReqApbuartOutputCharPolled_TestVariant( ctx );
      }
    }
  }
}

/** @} */
