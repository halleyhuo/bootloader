/**********************************************************
 *
 * File :
 *      flash.c
 *
 * Description:
 *
 */

#include "stm32f1xx_hal.h"
#include "flash.h"


#define GET_PAGE_ADDR(addr) (((((addr) - FLASH_BASE) / PAGE_SIZE) * PAGE_SIZE) + FLASH_BASE)

FlashRetVal FLASH_Erase(uint32_t beginAddr, uint32_t size)
{
    uint32_t                    pageNums;
    HAL_StatusTypeDef           status;
    FLASH_EraseInitTypeDef      flashErase;
    uint32_t                    errorPage;

    HAL_FLASH_Unlock();

    pageNums = size / PAGE_SIZE;
    pageNums += (size % PAGE_SIZE == 0) ? 0 : 1;

    flashErase.TypeErase        = FLASH_TYPEERASE_PAGES;
    flashErase.PageAddress      = GET_PAGE_ADDR(beginAddr);
    flashErase.NbPages          = pageNums;

    status = HAL_FLASHEx_Erase(&flashErase, &errorPage);

    HAL_FLASH_Lock();

    if(status == HAL_OK)
        return FLASH_OK;

    return FLASH_ERROR;
}


FlashRetVal FLASH_Write(uint32_t addr, uint8_t * data, uint32_t dataLen)
{
    uint16_t                *pHalfWord;
    uint32_t                lenHalfWord;
    uint16_t                i;


    /* Check address */
    if((addr < FLASH_BASE) || (addr >= FLASH_BASE + 1024 * FLASH_SIZE))   // invaild address
        return FLASH_INVALID_ADDRESS;

    /* Check parameters */
    if(data == NULL)
    {
        return FLASH_PARAMETERS_ERR;
    }

    HAL_FLASH_Unlock();

    /* Write data into flash in halfword mode */
    pHalfWord = (uint16_t *)data;
    lenHalfWord = dataLen / 2;
    lenHalfWord += (dataLen % 2 == 0) ? 0 : 1;

    for(i = 0; i < lenHalfWord; i++)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, (addr + i * 2), pHalfWord[i]);
    }

    HAL_FLASH_Lock();

    return FLASH_OK;
}


FlashRetVal FLASH_WriteHalfWord(uint32_t addr, uint16_t data)
{
    HAL_StatusTypeDef               status;
    FlashRetVal                     retVal;

    HAL_FLASH_Unlock();

    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr, data);
    switch(status)
    {
        case HAL_OK:
            retVal = FLASH_OK;
            break;

        case HAL_ERROR:
        case HAL_BUSY:
        case HAL_TIMEOUT:
        default:
            retVal = FLASH_ERROR;
            break;
    }

    HAL_FLASH_Lock();

    return retVal;
}

FlashRetVal FLASH_WriteWord(uint32_t addr, uint32_t data)
{

    HAL_StatusTypeDef               status;
    FlashRetVal                     retVal;

    HAL_FLASH_Unlock();

    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, data);
    switch(status)
    {
        case HAL_OK:
            retVal = FLASH_OK;
            break;

        case HAL_ERROR:
        case HAL_BUSY:
        case HAL_TIMEOUT:
        default:
            retVal = FLASH_ERROR;
            break;
    }

    HAL_FLASH_Lock();

    return retVal;
}


FlashRetVal FLASH_Read(uint32_t addr, uint8_t * data, uint32_t len)
{
    int32_t                     ret = FLASH_OK;
    uint32_t                    i;

    for(i = 0; i < len; i++,  data++)
    {
        *data = *(volatile uint8_t *)(addr + i);
    }

    return ret;
}

FlashRetVal FLASH_ReadByte(uint32_t addr, uint8_t * data)
{
    return FLASH_Read(addr, data, 1);
}

FlashRetVal FLASH_ReadHalfWord(uint32_t addr, uint16_t * data)
{
    return FLASH_Read(addr, (uint8_t *)data, 2);
}

FlashRetVal FLASH_ReadWord(uint32_t addr, uint32_t * data)
{
    return FLASH_Read(addr, (uint8_t *)data, 4);
}

