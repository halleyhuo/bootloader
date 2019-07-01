/**********************************************************
 *
 * File :
 *      crc.h
 *
 * Description:
 *      CRC functions.
 *
 */

#ifndef __CRC_H__
#define __CRC_H__

#include "stm32f1xx_hal.h"

uint32_t crc32( const uint8_t *buf, uint32_t size);

#endif /*__CRC_H__*/

