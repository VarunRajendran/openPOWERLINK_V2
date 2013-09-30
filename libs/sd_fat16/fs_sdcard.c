/**
********************************************************************************
\file  fs_sdcard.c

\brief  Contains code for the SD card FLASH functionality.

This is support file to provide support for reading and writing to a SD FLASH
espicially to read the CDC file during runtime.

\ingroup module_demo
*******************************************************************************/
/***************************** Include Files *********************************/

#include "EplInc.h"
#if (EPL_CDC_ON_SD != FALSE)

#include "fs_sdcard.h"
#ifdef XPAR_PS7_SD_0_S_AXI_BASEADDR

#include <stdio.h>
#include "xstatus.h"


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
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------


static FATFS fatfs;

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
 \brief  Init routine for SD flash

 This function initializes the controller for the SD FLASH interface

 \return     - XST_SUCCESS if the controller initializes correctly
 *           - XST_FAILURE if the controller fails to initializes correctly

 \ingroup module_demo
 */
//------------------------------------------------------------------------------
INT sd_fs_init(void)
{
    FRESULT Ret;

    /* Register volume work area, initialize device */
    Ret = f_mount(0, &fatfs);

    if (Ret != FR_OK) {
        printf("SD: failed to mount file system %d\n", Ret);
        return XST_FAILURE;
    }

    return XST_SUCCESS;

}
//------------------------------------------------------------------------------
/**
 \brief  Open File

 This function opens a file to read/write on SD FLASH

 \param      pFile_p        pointer to recieve the FILE structure
                            to the file to be opened
 \param      strFilename_p  Name of the file to be opened
 \param      bMode_p        FA_READ - open for reading
                            FA_WRITE - open for read/write

 \return     - XST_SUCCESS if the controller opens file correctly
 *           - XST_FAILURE if the controller fails to open correctly

 \ingroup module_demo
 */
//------------------------------------------------------------------------------
INT sd_open(FIL *pFile_p, char *strFilename_p, BYTE bMode_p)
{
    FRESULT Ret;

    Ret = f_open(pFile_p, strFilename_p, bMode_p);
    if (Ret)
    {
        printf("SD: Unable to open file %s: %d\n", strFilename_p, Ret);
        return XST_FAILURE;
    }

    return XST_SUCCESS;
}
//------------------------------------------------------------------------------
/**
 \brief  Read File

 This function Reads the specified number of bytes from a file on SD FLASH

 \param      pFile_p        pointer to the FILE structure to the file to be read
 \param      pBuffer_p      Buffer to receive the read data
 \param      uiCount_p      Number of bytes to read
 \param      pReadNum_p     Number of bytes read

 \return     - XST_SUCCESS if the controller reads correctly
 *           - XST_FAILURE if the controller fails to read

 \ingroup module_demo
 */
//------------------------------------------------------------------------------
INT sd_read(FIL *pFile_p, void *pBuffer_p, UINT32 uiCount_p, UINT32 *pReadNum_p)
{
    FRESULT Ret;

    Ret = f_read(pFile_p, pBuffer_p, uiCount_p, pReadNum_p);
    if (Ret)
    {
        printf("*** ERROR: f_read returned %d\r\n", Ret);
        return XST_FAILURE;
    }

    return XST_SUCCESS;
}

//------------------------------------------------------------------------------
/**
 \brief  Write File

 This function writes the specified number of bytes to a file on SD FLASH

 \param      pFile_p        pointer to the FILE structure to the file to write
 \param      pBuffer_p      Buffer to contaning data to write
 \param      uiCount_p      Number of bytes to write
 \param      pWriteNum_p     Number of bytes written

 \return     - XST_SUCCESS if the controller writes correctly
 *           - XST_FAILURE if the controller fails to write

 \ingroup module_demo
 */
//------------------------------------------------------------------------------
#if !_FS_READONLY
INT sd_write(FIL *pFile_p, void *pBuffer_p, UINT32 uiCount_p, UINT32 *pWriteNum_p)
{
    FRESULT Ret;

    Ret = f_write(pFile_p, pBuffer_p, uiCount_p, pWriteNum_p);
    if (Ret) {
        printf("*** ERROR: f_write returned %d\r\n", Ret);
        return XST_FAILURE;
    }

    return XST_SUCCESS;
}
#endif /* _FS_READONLY */

//------------------------------------------------------------------------------
/**
 \brief  Get size of file

 This function retuns the size of a file on SD FLASH

 \param      pFile_p        pointer to the FILE structure to the file to write

 \return     Size of File

 \ingroup module_demo
 */
//------------------------------------------------------------------------------
UINT32 sd_get_fsize(FIL *pFile_p)
{
    return f_size(pFile_p);
}

//------------------------------------------------------------------------------
/**
 \brief  Close file

 This function closes the file

 \param      pFile_p        pointer to the FILE structure to the file to write

 \return     Size of File

 \ingroup module_demo
 */
//------------------------------------------------------------------------------
void sd_close(FIL *pFile_p)
{
    f_close(pFile_p);
    return;
}

#endif // EPL_CDC_ON_SD
#endif // XPAR_PS7_SD_0_S_AXI_BASEADDR
