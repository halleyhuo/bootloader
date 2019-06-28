/**********************************************************
 *
 * File :
 *      iap.c
 *
 * Description:
 *      IAP functions.
 *
 */


#include "stm32f1xx_hal.h"
#include "flash.h"
#include "iap.h"
#include "ymodem.h"
#include <string.h>

static uint32_t gJumpAddress;


typedef void (*pFunction)(void);

pFunction Jump_To_Application;


IapFlagType IAP_ReadFlag(void)
{
    uint8_t         flag = IAP_FLAG_UPDATE_APP;


    FLASH_Read(IAP_FLAG_ADDR, &flag, 1);

    return flag;
}

void IAP_WriteFlag(IapFlagType flag)
{
    FLASH_Write(IAP_FLAG_ADDR, &flag, 1);
}


IapRetVal IAP_RunApp(void)
{
    if (((*(__IO uint32_t*)APP_ADDR) & 0x2FFE0000 ) == 0x20000000)
    {
        gJumpAddress = *(__IO uint32_t*) (APP_ADDR + 4);
        Jump_To_Application = (pFunction) gJumpAddress;
        __set_MSP(*(__IO uint32_t*) APP_ADDR);
        Jump_To_Application();

        return IAP_OK;
    }
    else
    {
        return IAP_ERROR;
    }
}

IapRetVal IAP_UpdateApp(void)
{
    YmodemRetVal                ymodemRet;
    IapRetVal                   iapRetVal;

    ymodemRet = YmodemReceive(APP_ADDR);

    if(ymodemRet == YMODEM_OK)
        iapRetVal = IAP_OK;
    else
        iapRetVal = IAP_ERROR;

    return iapRetVal;
}

