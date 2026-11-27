/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsObjectReqGetClassicName
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

#include <rtems.h>
#include <rtems/score/objectimpl.h>

#include "ts-config.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsObjectReqGetClassicName \
 *   spec:/rtems/object/req/get-classic-name
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsObjectReqGetClassicName_Pre_API_Zero,
  RtemsObjectReqGetClassicName_Pre_API_Valid,
  RtemsObjectReqGetClassicName_Pre_API_Invalid,
  RtemsObjectReqGetClassicName_Pre_API_NA
} RtemsObjectReqGetClassicName_Pre_API;

typedef enum {
  RtemsObjectReqGetClassicName_Pre_Class_Zero,
  RtemsObjectReqGetClassicName_Pre_Class_ValidNoObjects,
  RtemsObjectReqGetClassicName_Pre_Class_ValidMaybeObjects,
  RtemsObjectReqGetClassicName_Pre_Class_Invalid,
  RtemsObjectReqGetClassicName_Pre_Class_NA
} RtemsObjectReqGetClassicName_Pre_Class;

typedef enum {
  RtemsObjectReqGetClassicName_Pre_Node_Zero,
  RtemsObjectReqGetClassicName_Pre_Node_Valid,
  RtemsObjectReqGetClassicName_Pre_Node_Invalid,
  RtemsObjectReqGetClassicName_Pre_Node_NA
} RtemsObjectReqGetClassicName_Pre_Node;

typedef enum {
  RtemsObjectReqGetClassicName_Pre_Index_Zero,
  RtemsObjectReqGetClassicName_Pre_Index_Valid,
  RtemsObjectReqGetClassicName_Pre_Index_Invalid,
  RtemsObjectReqGetClassicName_Pre_Index_NA
} RtemsObjectReqGetClassicName_Pre_Index;

typedef enum {
  RtemsObjectReqGetClassicName_Pre_NameType_Integer,
  RtemsObjectReqGetClassicName_Pre_NameType_String,
  RtemsObjectReqGetClassicName_Pre_NameType_NA
} RtemsObjectReqGetClassicName_Pre_NameType;

typedef enum {
  RtemsObjectReqGetClassicName_Pre_Name_Valid,
  RtemsObjectReqGetClassicName_Pre_Name_Null,
  RtemsObjectReqGetClassicName_Pre_Name_NA
} RtemsObjectReqGetClassicName_Pre_Name;

typedef enum {
  RtemsObjectReqGetClassicName_Post_Status_Ok,
  RtemsObjectReqGetClassicName_Post_Status_InvAddr,
  RtemsObjectReqGetClassicName_Post_Status_InvId,
  RtemsObjectReqGetClassicName_Post_Status_NA
} RtemsObjectReqGetClassicName_Post_Status;

typedef enum {
  RtemsObjectReqGetClassicName_Post_Name_Set,
  RtemsObjectReqGetClassicName_Post_Name_Zero,
  RtemsObjectReqGetClassicName_Post_Name_NA
} RtemsObjectReqGetClassicName_Post_Name;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_API_NA : 1;
  uint16_t Pre_Class_NA : 1;
  uint16_t Pre_Node_NA : 1;
  uint16_t Pre_Index_NA : 1;
  uint16_t Pre_NameType_NA : 1;
  uint16_t Pre_Name_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Name : 2;
} RtemsObjectReqGetClassicName_Entry;

/**
 * @brief Test context for spec:/rtems/object/req/get-classic-name test case.
 */
typedef struct {
  /**
   * @brief This member provides an object information to manipulate.
   */
  Objects_Information *information;

  /**
   * @brief This member contains the original object maximum identifier.
   */
  Objects_Id maximum_id;

  /**
   * @brief This member provides the object of the executing thread.
   */
  Objects_Control *object;

  /**
   * @brief This member provides the object referenced by the `name` parameter.
   */
  rtems_name name_value;

  /**
   * @brief This member provides the `id` parameter value.
   */
  rtems_id id;

  /**
   * @brief This member provides the `name` parameter value.
   */
  rtems_name *name;

  /**
   * @brief This member contains the return status.
   */
  rtems_status_code status;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 6 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 6 ];

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
    bool rep_ran[ 27 ];

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
    RtemsObjectReqGetClassicName_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsObjectReqGetClassicName_Context;

static RtemsObjectReqGetClassicName_Context
  RtemsObjectReqGetClassicName_Instance;

static const char *const RtemsObjectReqGetClassicName_PreDesc_API[] =
  { "Zero", "Valid", "InvalidAboveLast", "InvalidMax", "NA" };

static const char *const RtemsObjectReqGetClassicName_PreDesc_Class[] = {
  "Zero",
  "ValidNoObjects",
  "ValidMaybeObjects",
  "InvalidAboveLast",
  "InvalidMax",
  "NA"
};

static const char *const RtemsObjectReqGetClassicName_PreDesc_Node[] =
  { "Zero", "Valid", "InvalidTwo", "InvalidMax", "NA" };

static const char *const RtemsObjectReqGetClassicName_PreDesc_Index[] =
  { "Zero", "Valid", "InvalidAboveMaximum", "InvalidMax", "NA" };

static const char *const RtemsObjectReqGetClassicName_PreDesc_NameType[] =
  { "Integer", "String", "NA" };

static const char *const RtemsObjectReqGetClassicName_PreDesc_Name[] =
  { "Valid", "Null", "NA" };

static const char *const *const RtemsObjectReqGetClassicName_PreDesc[] = {
  RtemsObjectReqGetClassicName_PreDesc_API,
  RtemsObjectReqGetClassicName_PreDesc_Class,
  RtemsObjectReqGetClassicName_PreDesc_Node,
  RtemsObjectReqGetClassicName_PreDesc_Index,
  RtemsObjectReqGetClassicName_PreDesc_NameType,
  RtemsObjectReqGetClassicName_PreDesc_Name,
  NULL
};

typedef enum {
  RtemsObjectReqGetClassicName_Pre_API_Invalid_AboveLast,
  RtemsObjectReqGetClassicName_Pre_API_Invalid_Max
} RtemsObjectReqGetClassicName_Pre_API_Invalid_Rep;

typedef enum {
  RtemsObjectReqGetClassicName_Pre_Class_Invalid_AboveLast,
  RtemsObjectReqGetClassicName_Pre_Class_Invalid_Max
} RtemsObjectReqGetClassicName_Pre_Class_Invalid_Rep;

typedef enum {
  RtemsObjectReqGetClassicName_Pre_Node_Invalid_Two,
  RtemsObjectReqGetClassicName_Pre_Node_Invalid_Max
} RtemsObjectReqGetClassicName_Pre_Node_Invalid_Rep;

typedef enum {
  RtemsObjectReqGetClassicName_Pre_Index_Invalid_AboveMaximum,
  RtemsObjectReqGetClassicName_Pre_Index_Invalid_Max
} RtemsObjectReqGetClassicName_Pre_Index_Invalid_Rep;

static const uint8_t RtemsObjectReqGetClassicName_PreRepOffset_API[] =
  { 0, 1, 2, 4, 5 };

static const uint8_t RtemsObjectReqGetClassicName_PreRepOffset_Class[] =
  { 0, 1, 2, 3, 5, 6 };

static const uint8_t RtemsObjectReqGetClassicName_PreRepOffset_Node[] =
  { 0, 1, 2, 4, 5 };

static const uint8_t RtemsObjectReqGetClassicName_PreRepOffset_Index[] =
  { 0, 1, 2, 4, 5 };

static const uint8_t RtemsObjectReqGetClassicName_PreRepOffset_NameType[] =
  { 0, 1, 2, 3 };

static const uint8_t RtemsObjectReqGetClassicName_PreRepOffset_Name[] =
  { 0, 1, 2, 3 };

static const uint8_t *const RtemsObjectReqGetClassicName_PreRepOffset[] = {
  RtemsObjectReqGetClassicName_PreRepOffset_API,
  RtemsObjectReqGetClassicName_PreRepOffset_Class,
  RtemsObjectReqGetClassicName_PreRepOffset_Node,
  RtemsObjectReqGetClassicName_PreRepOffset_Index,
  RtemsObjectReqGetClassicName_PreRepOffset_NameType,
  RtemsObjectReqGetClassicName_PreRepOffset_Name
};

static size_t RtemsObjectReqGetClassicName_GetRepresentativeCountOf(
  const RtemsObjectReqGetClassicName_Context *ctx,
  size_t                                      index
)
{
  const uint8_t *offsets;
  size_t         state;

  offsets = RtemsObjectReqGetClassicName_PreRepOffset[ index ];
  state = ctx->Map.pcs[ index ];

  return offsets[ state + 1 ] - offsets[ state ];
}

static size_t RtemsObjectReqGetClassicName_GetRepresentative(
  const RtemsObjectReqGetClassicName_Context *ctx,
  size_t                                      index
)
{
  size_t count;

  count = RtemsObjectReqGetClassicName_GetRepresentativeCountOf( ctx, index );

  if ( ctx->Map.rep < count ) {
    return ctx->Map.rep;
  }

  return count - 1;
}

static size_t RtemsObjectReqGetClassicName_GetRepresentativeCount(
  const RtemsObjectReqGetClassicName_Context *ctx
)
{
  size_t count;
  size_t i;

  count = 1;

  for ( i = 0; i < 6; ++i ) {
    size_t count_of_condition;

    count_of_condition = RtemsObjectReqGetClassicName_GetRepresentativeCountOf(
      ctx,
      i
    );

    if ( count_of_condition > count ) {
      count = count_of_condition;
    }
  }

  return count;
}

static RtemsObjectReqGetClassicName_Pre_API_Invalid_Rep
RtemsObjectReqGetClassicName_Pre_API_Invalid_GetRepresentative(
  const RtemsObjectReqGetClassicName_Context *ctx
)
{
  return (
    RtemsObjectReqGetClassicName_Pre_API_Invalid_Rep
  ) RtemsObjectReqGetClassicName_GetRepresentative( ctx, 0 );
}

static RtemsObjectReqGetClassicName_Pre_Class_Invalid_Rep
RtemsObjectReqGetClassicName_Pre_Class_Invalid_GetRepresentative(
  const RtemsObjectReqGetClassicName_Context *ctx
)
{
  return (
    RtemsObjectReqGetClassicName_Pre_Class_Invalid_Rep
  ) RtemsObjectReqGetClassicName_GetRepresentative( ctx, 1 );
}

static RtemsObjectReqGetClassicName_Pre_Node_Invalid_Rep
RtemsObjectReqGetClassicName_Pre_Node_Invalid_GetRepresentative(
  const RtemsObjectReqGetClassicName_Context *ctx
)
{
  return (
    RtemsObjectReqGetClassicName_Pre_Node_Invalid_Rep
  ) RtemsObjectReqGetClassicName_GetRepresentative( ctx, 2 );
}

static RtemsObjectReqGetClassicName_Pre_Index_Invalid_Rep
RtemsObjectReqGetClassicName_Pre_Index_Invalid_GetRepresentative(
  const RtemsObjectReqGetClassicName_Context *ctx
)
{
  return (
    RtemsObjectReqGetClassicName_Pre_Index_Invalid_Rep
  ) RtemsObjectReqGetClassicName_GetRepresentative( ctx, 3 );
}

static const uint8_t RtemsObjectReqGetClassicName_PreRepBase[] =
  { 0, 5, 11, 16, 21, 24 };

static const uint8_t RtemsObjectReqGetClassicName_PreRepStates[] =
  { 3, 4, 3, 3, 2, 2 };

static void RtemsObjectReqGetClassicName_ResetRepresentatives(
  RtemsObjectReqGetClassicName_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 27; ++i ) {
    ctx->Map.rep_ran[ i ] = false;
  }
}

static void RtemsObjectReqGetClassicName_MarkRepresentatives(
  RtemsObjectReqGetClassicName_Context *ctx
)
{
  size_t i;

  if ( ctx->Map.skip_rep ) {
    return;
  }

  for ( i = 0; i < 6; ++i ) {
    size_t rep;

    rep = RtemsObjectReqGetClassicName_PreRepBase[ i ] +
          RtemsObjectReqGetClassicName_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
          RtemsObjectReqGetClassicName_GetRepresentative( ctx, i );
    ctx->Map.rep_ran[ rep ] = true;
  }
}

static void RtemsObjectReqGetClassicName_CheckRepresentatives(
  const RtemsObjectReqGetClassicName_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 6; ++i ) {
    const uint8_t *offsets;
    const bool    *ran;
    size_t         state;

    offsets = RtemsObjectReqGetClassicName_PreRepOffset[ i ];
    ran = &ctx->Map.rep_ran[ RtemsObjectReqGetClassicName_PreRepBase[ i ] ];

    for (
      state = 0; state < RtemsObjectReqGetClassicName_PreRepStates[ i ];
      ++state
    ) {
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
            RtemsObjectReqGetClassicName_PreDesc[ i ][ k ]
          );
        }
      }
    }
  }
}

static void RtemsObjectReqGetClassicName_Pre_API_Prepare(
  RtemsObjectReqGetClassicName_Context *ctx,
  RtemsObjectReqGetClassicName_Pre_API  state
)
{
  switch ( state ) {
    case RtemsObjectReqGetClassicName_Pre_API_Zero: {
      /*
       * While the API of the `id` parameter is zero.
       */
      /* Already prepared */
      break;
    }

    case RtemsObjectReqGetClassicName_Pre_API_Valid: {
      /*
       * While the API of the `id` parameter is associated with an object API.
       */
      ctx->id |= OBJECTS_CLASSIC_API << OBJECTS_API_START_BIT;
      break;
    }

    case RtemsObjectReqGetClassicName_Pre_API_Invalid: {
      /*
       * While the API of the `id` parameter is not equal to zero and is
       * associated with no object API.
       */
      switch (
        RtemsObjectReqGetClassicName_Pre_API_Invalid_GetRepresentative( ctx )
      ) {
        case RtemsObjectReqGetClassicName_Pre_API_Invalid_AboveLast: {
          ctx->id |= ( OBJECTS_APIS_LAST + 1 ) << OBJECTS_API_START_BIT;
          break;
        }

        case RtemsObjectReqGetClassicName_Pre_API_Invalid_Max: {
          ctx->id |= OBJECTS_API_VALID_BITS << OBJECTS_API_START_BIT;
          break;
        }
      }
      break;
    }

    case RtemsObjectReqGetClassicName_Pre_API_NA:
      break;
  }
}

static void RtemsObjectReqGetClassicName_Pre_Class_Prepare(
  RtemsObjectReqGetClassicName_Context  *ctx,
  RtemsObjectReqGetClassicName_Pre_Class state
)
{
  switch ( state ) {
    case RtemsObjectReqGetClassicName_Pre_Class_Zero: {
      /*
       * While the class of the `id` parameter is zero.
       */
      /* Already prepared */
      break;
    }

    case RtemsObjectReqGetClassicName_Pre_Class_ValidNoObjects: {
      /*
       * While the class of the `id` parameter is associated with an object
       * class which has no object configured.
       */
      ctx->id |= OBJECTS_RTEMS_TASKS << OBJECTS_CLASS_START_BIT;
      ctx->information->maximum_id = 0;
      break;
    }

    case RtemsObjectReqGetClassicName_Pre_Class_ValidMaybeObjects: {
      /*
       * While the class of the `id` parameter is associated with an object
       * class which has at least one object configured.
       */
      ctx->id |= OBJECTS_RTEMS_TASKS << OBJECTS_CLASS_START_BIT;
      break;
    }

    case RtemsObjectReqGetClassicName_Pre_Class_Invalid: {
      /*
       * While the class of the `id` parameter is not equal to zero and is
       * associated with no object class.
       */
      switch (
        RtemsObjectReqGetClassicName_Pre_Class_Invalid_GetRepresentative( ctx )
      ) {
        case RtemsObjectReqGetClassicName_Pre_Class_Invalid_AboveLast: {
          ctx->id |= ( OBJECTS_RTEMS_CLASSES_LAST + 1 )
                     << OBJECTS_CLASS_START_BIT;
          break;
        }

        case RtemsObjectReqGetClassicName_Pre_Class_Invalid_Max: {
          ctx->id |= OBJECTS_CLASS_VALID_BITS << OBJECTS_CLASS_START_BIT;
          break;
        }
      }
      break;
    }

    case RtemsObjectReqGetClassicName_Pre_Class_NA:
      break;
  }
}

static void RtemsObjectReqGetClassicName_Pre_Node_Prepare(
  RtemsObjectReqGetClassicName_Context *ctx,
  RtemsObjectReqGetClassicName_Pre_Node state
)
{
  switch ( state ) {
    case RtemsObjectReqGetClassicName_Pre_Node_Zero: {
      /*
       * While the node of the `id` parameter is zero.
       */
      /* Already prepared */
      break;
    }

    case RtemsObjectReqGetClassicName_Pre_Node_Valid: {
      /*
       * While the node of the `id` parameter is associated with a node.
       */
      ctx->id |= 1 << OBJECTS_NODE_START_BIT;
      break;
    }

    case RtemsObjectReqGetClassicName_Pre_Node_Invalid: {
      /*
       * While the node of the `id` parameter is not equal to zero and is
       * associated with no node.
       */
      switch (
        RtemsObjectReqGetClassicName_Pre_Node_Invalid_GetRepresentative( ctx )
      ) {
        case RtemsObjectReqGetClassicName_Pre_Node_Invalid_Two: {
          ctx->id |= 2 << OBJECTS_NODE_START_BIT;
          break;
        }

        case RtemsObjectReqGetClassicName_Pre_Node_Invalid_Max: {
          ctx->id |= OBJECTS_NODE_VALID_BITS << OBJECTS_NODE_START_BIT;
          break;
        }
      }
      break;
    }

    case RtemsObjectReqGetClassicName_Pre_Node_NA:
      break;
  }
}

static void RtemsObjectReqGetClassicName_Pre_Index_Prepare(
  RtemsObjectReqGetClassicName_Context  *ctx,
  RtemsObjectReqGetClassicName_Pre_Index state
)
{
  switch ( state ) {
    case RtemsObjectReqGetClassicName_Pre_Index_Zero: {
      /*
       * While the index of the `id` parameter is zero.
       */
      /* Already prepared */
      break;
    }

    case RtemsObjectReqGetClassicName_Pre_Index_Valid: {
      /*
       * While the index of the `id` parameter is associated with an object.
       */
      ctx->id |= _Objects_Get_index( ctx->object->id )
                 << OBJECTS_INDEX_START_BIT;
      break;
    }

    case RtemsObjectReqGetClassicName_Pre_Index_Invalid: {
      /*
       * While the index of the `id` parameter is not equal to zero and is
       * associated with no object.
       */
      switch (
        RtemsObjectReqGetClassicName_Pre_Index_Invalid_GetRepresentative( ctx )
      ) {
        case RtemsObjectReqGetClassicName_Pre_Index_Invalid_AboveMaximum: {
          ctx->id |= ( _Objects_Get_index( ctx->maximum_id ) + 1 )
                     << OBJECTS_INDEX_START_BIT;
          break;
        }

        case RtemsObjectReqGetClassicName_Pre_Index_Invalid_Max: {
          ctx->id |= OBJECTS_INDEX_VALID_BITS << OBJECTS_INDEX_START_BIT;
          break;
        }
      }
      break;
    }

    case RtemsObjectReqGetClassicName_Pre_Index_NA:
      break;
  }
}

static void RtemsObjectReqGetClassicName_Pre_NameType_Prepare(
  RtemsObjectReqGetClassicName_Context     *ctx,
  RtemsObjectReqGetClassicName_Pre_NameType state
)
{
  switch ( state ) {
    case RtemsObjectReqGetClassicName_Pre_NameType_Integer: {
      /*
       * While the object class uses integer names.
       */
      ctx->information->name_length = 0;
      break;
    }

    case RtemsObjectReqGetClassicName_Pre_NameType_String: {
      /*
       * While the object class uses string names.
       */
      ctx->information->name_length = sizeof( rtems_name );
      break;
    }

    case RtemsObjectReqGetClassicName_Pre_NameType_NA:
      break;
  }
}

static void RtemsObjectReqGetClassicName_Pre_Name_Prepare(
  RtemsObjectReqGetClassicName_Context *ctx,
  RtemsObjectReqGetClassicName_Pre_Name state
)
{
  switch ( state ) {
    case RtemsObjectReqGetClassicName_Pre_Name_Valid: {
      /*
       * While the `name` parameter references an object of type rtems_name.
       */
      ctx->name = &ctx->name_value;
      break;
    }

    case RtemsObjectReqGetClassicName_Pre_Name_Null: {
      /*
       * While the `name` parameter is NULL.
       */
      ctx->name = NULL;
      break;
    }

    case RtemsObjectReqGetClassicName_Pre_Name_NA:
      break;
  }
}

static void RtemsObjectReqGetClassicName_Post_Status_Check(
  RtemsObjectReqGetClassicName_Context    *ctx,
  RtemsObjectReqGetClassicName_Post_Status state
)
{
  switch ( state ) {
    case RtemsObjectReqGetClassicName_Post_Status_Ok: {
      /*
       * The return status of rtems_object_get_classic_name() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsObjectReqGetClassicName_Post_Status_InvAddr: {
      /*
       * The return status of rtems_object_get_classic_name() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsObjectReqGetClassicName_Post_Status_InvId: {
      /*
       * The return status of rtems_object_get_classic_name() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsObjectReqGetClassicName_Post_Status_NA:
      break;
  }
}

static void RtemsObjectReqGetClassicName_Post_Name_Check(
  RtemsObjectReqGetClassicName_Context  *ctx,
  RtemsObjectReqGetClassicName_Post_Name state
)
{
  switch ( state ) {
    case RtemsObjectReqGetClassicName_Post_Name_Set: {
      /*
       * The value of the object referenced by the `name` parameter shall be
       * the object name of the object identified by the `id` parameter.
       */
      T_eq_u32( ctx->name_value, TEST_RUNNER_NAME );
      break;
    }

    case RtemsObjectReqGetClassicName_Post_Name_Zero: {
      /*
       * The value of the object referenced by the `name` parameter shall be
       * zero.
       */
      T_eq_u32( ctx->name_value, 0 );
      break;
    }

    case RtemsObjectReqGetClassicName_Post_Name_NA:
      break;
  }
}

static void RtemsObjectReqGetClassicName_Setup(
  RtemsObjectReqGetClassicName_Context *ctx
)
{
  ctx->information = _Objects_Information_table[ OBJECTS_CLASSIC_API ]
                                               [ OBJECTS_RTEMS_TASKS ];
  ctx->maximum_id = ctx->information->maximum_id;
  ctx->object = &GetExecuting()->Object;
  T_eq_u32( ctx->object->name.name_u32, TEST_RUNNER_NAME );
}

static void RtemsObjectReqGetClassicName_Setup_Wrap( void *arg )
{
  RtemsObjectReqGetClassicName_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsObjectReqGetClassicName_Setup( ctx );
}

static void RtemsObjectReqGetClassicName_Teardown(
  RtemsObjectReqGetClassicName_Context *ctx
)
{
  ctx->information->maximum_id = ctx->maximum_id;
  ctx->information->name_length = 0;
}

static void RtemsObjectReqGetClassicName_Teardown_Wrap( void *arg )
{
  RtemsObjectReqGetClassicName_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsObjectReqGetClassicName_Teardown( ctx );
}

static void RtemsObjectReqGetClassicName_Prepare(
  RtemsObjectReqGetClassicName_Context *ctx
)
{
  ctx->id = 0;
  ctx->name_value = 0xffffffff;
  ctx->information->maximum_id = ctx->maximum_id;
  ctx->information->name_length = 0;
}

static void RtemsObjectReqGetClassicName_Action(
  RtemsObjectReqGetClassicName_Context *ctx
)
{
  ctx->status = rtems_object_get_classic_name( ctx->id, ctx->name );
}

/* clang-format off */

static const RtemsObjectReqGetClassicName_Entry
RtemsObjectReqGetClassicName_Entries[] = {
  { 1, 0, 0, 0, 0, 0, 0, RtemsObjectReqGetClassicName_Post_Status_NA,
    RtemsObjectReqGetClassicName_Post_Name_NA },
  { 1, 0, 0, 0, 0, 1, 0, RtemsObjectReqGetClassicName_Post_Status_InvId,
    RtemsObjectReqGetClassicName_Post_Name_Zero },
  { 1, 0, 0, 0, 0, 1, 0, RtemsObjectReqGetClassicName_Post_Status_InvAddr,
    RtemsObjectReqGetClassicName_Post_Name_NA },
  { 1, 0, 0, 0, 0, 1, 0, RtemsObjectReqGetClassicName_Post_Status_InvId,
    RtemsObjectReqGetClassicName_Post_Name_Zero },
  { 1, 0, 0, 0, 0, 1, 0, RtemsObjectReqGetClassicName_Post_Status_InvAddr,
    RtemsObjectReqGetClassicName_Post_Name_NA },
  { 0, 0, 0, 0, 0, 1, 0, RtemsObjectReqGetClassicName_Post_Status_InvId,
    RtemsObjectReqGetClassicName_Post_Name_Zero },
  { 0, 0, 0, 0, 0, 1, 0, RtemsObjectReqGetClassicName_Post_Status_InvAddr,
    RtemsObjectReqGetClassicName_Post_Name_NA },
  { 0, 0, 0, 0, 0, 0, 0, RtemsObjectReqGetClassicName_Post_Status_InvAddr,
    RtemsObjectReqGetClassicName_Post_Name_NA },
  { 0, 0, 0, 0, 0, 0, 0, RtemsObjectReqGetClassicName_Post_Status_InvId,
    RtemsObjectReqGetClassicName_Post_Name_Zero },
  { 1, 0, 0, 0, 0, 0, 0, RtemsObjectReqGetClassicName_Post_Status_InvId,
    RtemsObjectReqGetClassicName_Post_Name_Zero },
  { 1, 0, 0, 0, 0, 0, 0, RtemsObjectReqGetClassicName_Post_Status_InvAddr,
    RtemsObjectReqGetClassicName_Post_Name_NA },
  { 0, 0, 0, 0, 0, 0, 0, RtemsObjectReqGetClassicName_Post_Status_Ok,
    RtemsObjectReqGetClassicName_Post_Name_Set },
  { 0, 0, 0, 0, 1, 1, 0, RtemsObjectReqGetClassicName_Post_Status_InvId,
    RtemsObjectReqGetClassicName_Post_Name_Zero },
  { 0, 0, 0, 0, 1, 1, 0, RtemsObjectReqGetClassicName_Post_Status_InvAddr,
    RtemsObjectReqGetClassicName_Post_Name_NA }
};

static const uint8_t
RtemsObjectReqGetClassicName_Map[] = {
  11, 7, 8, 7, 1, 2, 1, 2, 5, 6, 0, 0, 5, 6, 0, 0, 1, 2, 1, 2, 5, 6, 0, 0, 5,
  6, 0, 0, 1, 2, 1, 2, 5, 6, 0, 0, 3, 4, 3, 4, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3,
  4, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1,
  2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1,
  2, 3, 4, 3, 4, 5, 6, 0, 0, 1, 2, 1, 2, 5, 6, 0, 0, 5, 6, 0, 0, 1, 2, 1, 2, 5,
  6, 0, 0, 5, 6, 0, 0, 1, 2, 1, 2, 5, 6, 0, 0, 5, 6, 0, 0, 1, 2, 1, 2, 5, 6, 0,
  0, 5, 6, 0, 0, 1, 2, 1, 2, 5, 6, 0, 0, 5, 6, 0, 0, 1, 2, 1, 2, 5, 6, 0, 0, 5,
  6, 0, 0, 1, 2, 1, 2, 5, 6, 0, 0, 5, 6, 0, 0, 12, 13, 0, 0, 5, 6, 0, 0, 5, 6,
  0, 0, 1, 2, 1, 2, 5, 6, 0, 0, 8, 7, 8, 7, 9, 10, 9, 10, 8, 7, 8, 7, 8, 7, 8,
  7, 11, 7, 8, 7, 8, 7, 8, 7, 8, 7, 8, 7, 9, 10, 9, 10, 8, 7, 8, 7, 5, 6, 0, 0,
  1, 2, 1, 2, 5, 6, 0, 0, 5, 6, 0, 0, 1, 2, 1, 2, 5, 6, 0, 0, 5, 6, 0, 0, 1, 2,
  1, 2, 5, 6, 0, 0, 5, 6, 0, 0, 1, 2, 1, 2, 5, 6, 0, 0, 5, 6, 0, 0, 1, 2, 1, 2,
  5, 6, 0, 0, 5, 6, 0, 0, 1, 2, 1, 2, 5, 6, 0, 0, 3, 4, 3, 4, 1, 2, 1, 2, 3, 4,
  3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 3, 4, 3, 4,
  3, 4, 3, 4, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4,
  3, 4, 1, 2, 1, 2, 3, 4, 3, 4, 5, 6, 0, 0, 1, 2, 1, 2, 5, 6, 0, 0, 5, 6, 0, 0,
  1, 2, 1, 2, 5, 6, 0, 0, 5, 6, 0, 0, 1, 2, 1, 2, 5, 6, 0, 0
};

/* clang-format on */

static size_t RtemsObjectReqGetClassicName_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  RtemsObjectReqGetClassicName_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    size_t pcd[ 6 ];
    size_t i;

    for ( i = 0; i < 6; ++i ) {
      pcd[ i ] =
        RtemsObjectReqGetClassicName_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
        RtemsObjectReqGetClassicName_GetRepresentative( ctx, i );
    }

    return T_get_scope( RtemsObjectReqGetClassicName_PreDesc, buf, n, pcd );
  }

  return 0;
}

static T_fixture RtemsObjectReqGetClassicName_Fixture = {
  .setup = RtemsObjectReqGetClassicName_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsObjectReqGetClassicName_Teardown_Wrap,
  .scope = RtemsObjectReqGetClassicName_Scope,
  .initial_context = &RtemsObjectReqGetClassicName_Instance
};

static inline RtemsObjectReqGetClassicName_Entry
RtemsObjectReqGetClassicName_PopEntry(
  RtemsObjectReqGetClassicName_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsObjectReqGetClassicName_Entries
    [ RtemsObjectReqGetClassicName_Map[ index ] ];
}

static void RtemsObjectReqGetClassicName_SetPreConditionStates(
  RtemsObjectReqGetClassicName_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];

  if ( ctx->Map.entry.Pre_Index_NA ) {
    ctx->Map.pcs[ 3 ] = RtemsObjectReqGetClassicName_Pre_Index_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  if ( ctx->Map.entry.Pre_NameType_NA ) {
    ctx->Map.pcs[ 4 ] = RtemsObjectReqGetClassicName_Pre_NameType_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }

  ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
}

static void RtemsObjectReqGetClassicName_TestVariant(
  RtemsObjectReqGetClassicName_Context *ctx
)
{
  RtemsObjectReqGetClassicName_Pre_API_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsObjectReqGetClassicName_Pre_Class_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsObjectReqGetClassicName_Pre_Node_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsObjectReqGetClassicName_Pre_Index_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsObjectReqGetClassicName_Pre_NameType_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsObjectReqGetClassicName_Pre_Name_Prepare( ctx, ctx->Map.pcs[ 5 ] );

  if ( ctx->Map.skip_rep ) {
    return;
  }

  RtemsObjectReqGetClassicName_Action( ctx );
  RtemsObjectReqGetClassicName_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  RtemsObjectReqGetClassicName_Post_Name_Check(
    ctx,
    ctx->Map.entry.Post_Name
  );
}

/**
 * @fn void T_case_body_RtemsObjectReqGetClassicName( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsObjectReqGetClassicName,
  &RtemsObjectReqGetClassicName_Fixture
)
{
  RtemsObjectReqGetClassicName_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  RtemsObjectReqGetClassicName_ResetRepresentatives( ctx );

  for (
    ctx->Map.pci[ 0 ] = RtemsObjectReqGetClassicName_Pre_API_Zero;
    ctx->Map.pci[ 0 ] < RtemsObjectReqGetClassicName_Pre_API_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsObjectReqGetClassicName_Pre_Class_Zero;
      ctx->Map.pci[ 1 ] < RtemsObjectReqGetClassicName_Pre_Class_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsObjectReqGetClassicName_Pre_Node_Zero;
        ctx->Map.pci[ 2 ] < RtemsObjectReqGetClassicName_Pre_Node_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = RtemsObjectReqGetClassicName_Pre_Index_Zero;
          ctx->Map.pci[ 3 ] < RtemsObjectReqGetClassicName_Pre_Index_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] =
              RtemsObjectReqGetClassicName_Pre_NameType_Integer;
            ctx->Map.pci[ 4 ] < RtemsObjectReqGetClassicName_Pre_NameType_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = RtemsObjectReqGetClassicName_Pre_Name_Valid;
              ctx->Map.pci[ 5 ] < RtemsObjectReqGetClassicName_Pre_Name_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              ctx->Map.entry = RtemsObjectReqGetClassicName_PopEntry( ctx );

              if ( ctx->Map.entry.Skip ) {
                continue;
              }

              RtemsObjectReqGetClassicName_SetPreConditionStates( ctx );

              for (
                ctx->Map.rep = 0;
                ctx->Map.rep <
                RtemsObjectReqGetClassicName_GetRepresentativeCount( ctx );
                ++ctx->Map.rep
              ) {
                ctx->Map.skip_rep = false;
                RtemsObjectReqGetClassicName_Prepare( ctx );
                RtemsObjectReqGetClassicName_TestVariant( ctx );

                RtemsObjectReqGetClassicName_MarkRepresentatives( ctx );
              }
            }
          }
        }
      }
    }
  }

  RtemsObjectReqGetClassicName_CheckRepresentatives( ctx );
}

/** @} */
