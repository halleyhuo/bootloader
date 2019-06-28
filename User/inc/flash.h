/**********************************************************
 *
 * File :
 *      flash.h
 *
 * Description:
 *
 */

#ifndef __FLASH_H__
#define __FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

/* Flash related values for stm32f103c8 */
#define PAGE_SIZE                   0x400       /* 1 Kbyte */
#define FLASH_SIZE                  0x10000     /* 64 KBytes */

/* Return value types for flash functions */
typedef int32_t                     FlashRetVal;

#define FLASH_OK                    0
#define FLASH_ERROR                 -1
#define FLASH_PARAMETERS_ERR        -2
#define FLASH_INVALID_ADDRESS       -3;


FlashRetVal FLASH_Erase(uint32_t beginAddr, uint32_t size);

FlashRetVal FLASH_Write(uint32_t addr, uint8_t * data, uint32_t dataLen);

FlashRetVal FLASH_WriteHalfWord(uint32_t addr, uint16_t data);

FlashRetVal FLASH_WriteWord(uint32_t addr, uint32_t data);

FlashRetVal FLASH_Read(uint32_t addr, uint8_t * data, uint32_t len);

FlashRetVal FLASH_ReadByte(uint32_t addr, uint8_t * data);

FlashRetVal FLASH_ReadHalfWord(uint32_t addr, uint16_t * data);

FlashRetVal FLASH_ReadWord(uint32_t addr, uint32_t * data);


#ifdef __cplusplus
}
#endif

#endif //__FLASH_H__

