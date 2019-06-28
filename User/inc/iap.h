/**********************************************************
 *
 * File :
 *      iap.h
 *
 * Description:
 *
 */

#ifndef __IAP_H__
#define __IAP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

#define IAP_FLAG_ADDR               0x08004000
#define APP_ADDR                    0x08005000

/* IAP returns type*/
typedef int32_t                     IapRetVal;

#define IAP_OK                      0
#define IAP_ERROR                   -1

/* IAP flag type*/
typedef uint8_t                     IapFlagType;

#define IAP_FLAG_RUN_APP            0
#define IAP_FLAG_UPDATE_APP         0xFF



IapFlagType IAP_ReadFlag(void);

void IAP_WriteFlag(IapFlagType flag);

IapRetVal IAP_RunApp(void);

IapRetVal IAP_UpdateApp(void);

#ifdef __cplusplus
}
#endif

#endif //__IAP_H__
