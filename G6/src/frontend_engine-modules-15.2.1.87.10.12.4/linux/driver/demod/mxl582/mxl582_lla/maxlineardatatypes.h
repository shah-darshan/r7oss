/*
* Copyright (c) 2011-2013 MaxLinear, Inc. All rights reserved
*
* License type: GPLv2
*
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free Software
* Foundation.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*
* This program may alternatively be licensed under a proprietary license from
* MaxLinear, Inc.
*
* See terms and conditions defined in file 'LICENSE.txt', which is part of this
* source code package.
*/


#ifndef __MAXLINEAR_DATA_TYPES_H__
#define __MAXLINEAR_DATA_TYPES_H__

/*****************************************************************************************
    Include Header Files
    (No absolute paths - paths handled by make file)
*****************************************************************************************/

#ifndef _MSC_VER
//#include <stdint.h>
#endif



#include <linux/types.h>



/*****************************************************************************************
    Macros
*****************************************************************************************/

/*****************************************************************************************
    User-Defined Types (Typedefs)
*****************************************************************************************/

/* Basic datatypes are defined below.
 * If you do not wish to use them, or you already have
 * them defined, please add MXL_DO_NOT_DEFINE_BASIC_DATATYPES
 * to compilation defines
 */
#ifndef MXL_DO_NOT_DEFINE_BASIC_DATATYPES

#ifdef _MSC_VER
#ifndef __midl

typedef unsigned __int8     UINT8;
typedef unsigned __int16    UINT16;
typedef unsigned __int32    UINT32;
typedef unsigned __int64    UINT64;

typedef __int8              SINT8;
typedef __int16             SINT16;

#else
typedef small               SINT8;
typedef short               SINT16;

#endif

typedef __int32             SINT32;
typedef __int64             SINT64;

#else

typedef uint8_t             UINT8;
typedef uint16_t            UINT16;
typedef uint32_t            UINT32;
typedef uint64_t            UINT64;

typedef int8_t              SINT8;
typedef int16_t             SINT16;
typedef int32_t             SINT32;
typedef int64_t             SINT64;
#endif

typedef float               REAL32;
typedef double              REAL64;

#else
#include "MxLWare_HYDRA_OEM_Defines.h"
#endif

#define MXL_MIN(x, y)                   (((x) < (y)) ? (x) : (y))
#define MXL_MAX(x, y)                   (((x) >= (y)) ? (x) : (y))
#define MXL_ABS(x)                      (((x) >= 0) ? (x) : -(x))
#define MXL_DIV_ROUND(x, y)             (((x) + ((y)/2)) / (y))

typedef UINT32 (*MXL_CALLBACK_FN_T)(UINT8 devId, UINT32 callbackType, void * callbackPayload);

typedef enum
{
  MXL_SUCCESS = 0,
  MXL_FAILURE = 1,
  MXL_INVALID_PARAMETER,
  MXL_NOT_INITIALIZED,
  MXL_ALREADY_INITIALIZED,
  MXL_NOT_SUPPORTED,
  MXL_NOT_READY
} MXL_STATUS_E;

typedef enum
{
  MXL_DISABLE = 0,
  MXL_ENABLE  = 1,

  MXL_FALSE = 0,
  MXL_TRUE  = 1,

  MXL_INVALID = 0,
  MXL_VALID   = 1,

  MXL_NO      = 0,
  MXL_YES     = 1,

  MXL_OFF     = 0,
  MXL_ON      = 1
} MXL_BOOL_E;


/*****************************************************************************************
    Global Variable Declarations
*****************************************************************************************/

/*****************************************************************************************
    Prototypes
*****************************************************************************************/

#endif /* __MAXLINEAR_DATA_TYPES_H__ */
