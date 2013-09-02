/**
********************************************************************************
\file   errhnducal-noosdual.c

\brief  Implementation of user CAL module for error handler

This module implements the user layer CAL functions of the error handler.
This implementation uses shared memory to share the error objects
between user and kernel part runninf on two different processors.

\ingroup module_errhnducal
*******************************************************************************/
/*------------------------------------------------------------------------------
Copyright (c) 2012 Kalycito Infotech Private Limited
              www.kalycito.com
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holders nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include <EplInc.h>

#include <errhnd.h>
#include <unistd.h>

#include <dualprocshm.h>

//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// global variable declaration
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static tErrHndObjects           *pLocalObjects_l;       ///< pointer to user error objects
static BYTE*                    pErrHndMem_l;

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief    Initialize error handler user CAL module

The function initializes the user layer CAL module of the error handler.

\param  pLocalObjects_p         Pointer to local error objects

\return Always returns kEplSuccessful

\ingroup module_errhnducal
*/
//------------------------------------------------------------------------------
tEplKernel errhnducal_init (tErrHndObjects *pLocalObjects_p)
{
    tDualprocReturn dualRet;
    tDualprocDrvInstance pInstance = dualprocshm_getInstance(kDualProcHost);
    UINT8*               pBase;
    UINT16               span;

    if(pInstance == NULL)
    {
        return kEplNoResource;
    }

    if (pErrHndMem_l != NULL)
        return kEplNoFreeInstance;

    dualRet = dualprocshm_getDynRes(pInstance, kDualprocResIdErrCount, &pBase, &span );
    if(dualRet != kDualprocSuccessful)
    {
        EPL_DBGLVL_ERROR_TRACE("%s() couldn't get Error counter buffer details (%d)\n",
                __func__, dualRet);
        return kEplNoResource;
    }

    if(span < sizeof(tErrHndObjects))
    {
        EPL_DBGLVL_ERROR_TRACE("%s: Error Handler Object Buffer too small\n",
                __func__);
        return kEplNoResource;
    }

    pErrHndMem_l = (BYTE*) pBase;

    pLocalObjects_l = pLocalObjects_p;

    return kEplSuccessful;
}

//------------------------------------------------------------------------------
/**
\brief    shutdown error handler user CAL module

The function is used to deinitialize and shutdown the user layer
CAL module of the error handler.

\ingroup module_errhnducal
*/
//------------------------------------------------------------------------------
void errhnducal_exit (void)
{
    if (pErrHndMem_l != NULL)
    {
        pErrHndMem_l = NULL;
    }
}

//------------------------------------------------------------------------------
/**
\brief    write an error handler object

The function writes an error handler object to the shared memory region used
by user and kernel modules.

\param  index_p             Index of object in object dictionary
\param  subIndex_p          Subindex of object
\param  pParam_p            Pointer to object in error handlers memory space

\return Returns a tEplKernel error code.

\ingroup module_errhnducal
*/
//------------------------------------------------------------------------------
tEplKernel errhnducal_writeErrorObject(UINT index_p, UINT subIndex_p, UINT32 *pParam_p)
{
    UINT    offset;

    UNUSED_PARAMETER(index_p);
    UNUSED_PARAMETER(subIndex_p);

    offset = (char *)pParam_p - (char *)pLocalObjects_l;
    *(UINT32*)(pErrHndMem_l + offset) = *pParam_p;

    TARGET_FLUSH_DCACHE((pErrHndMem_l + offset),sizeof(UINT32));
    return kEplSuccessful;
}

//------------------------------------------------------------------------------
/**
\brief    read an error handler object

The function reads an error handler object from the shared memory region used
by user and kernel modules.

\param  index_p             Index of object in object dictionary
\param  subIndex_p          Subindex of object
\param  pParam_p            Pointer to object in error handlers memory space

\return Returns a tEplKernel error code.

\ingroup module_errhnducal
*/
//------------------------------------------------------------------------------
tEplKernel errhnducal_readErrorObject(UINT index_p, UINT subIndex_p, UINT32* pParam_p)
{
    UINT    offset;

    UNUSED_PARAMETER(index_p);
    UNUSED_PARAMETER(subIndex_p);

    offset = (char *)pParam_p - (char *)pLocalObjects_l;

    TARGET_INVALIDATE_DCACHE((pErrHndMem_l + offset),sizeof(UINT32));
    *pParam_p = *(UINT32*)(pErrHndMem_l + offset);
    return kEplSuccessful;
}

//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//

