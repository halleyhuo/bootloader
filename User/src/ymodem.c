/**********************************************************
 *
 * File :
 *		YModem.c
 *
 * Description:
 *		YModem protocal (use statemachine)
 *
 */

#include "stm32f1xx_hal.h"
#include "ymodem.h"
#include "usart.h"
#include "flash.h"

typedef enum
{
    WAIT_FILE_DESC,
    WAIT_FIRST_DATA,
    WAIT_DATA,
    WAIT_LAST_STX_DATA,
    WAIT_LAST_SOH_DATA,
    WAIT_EOT,
    WAIT_END
} ReceiveState;


/*
 * Frame type
 */
typedef uint8_t                 FrameType;

#define FRAME_SOH               0x01    /* start of 128-byte data packet */
#define FRAME_STX               0x02    /* start of 1024-byte data packet */
#define FRAME_EOT               0x04    /* end of transmission */
#define FRAME_ACK               0x06    /* acknowledge */
#define FRAME_NAK               0x15    /* negative acknowledge */
#define FRAME_CAN               0x18    /* aborts transfer */
#define FRAME_C                 0x43    /* 'C' == 0x43, start transmission */
#define FRAME_ABORT1            0x41
#define FRAME_ABORT2            0x61


/*
 * Receive & Send Timeout
 */
#define RECEIVE_TIMEOUT         5000
#define SEND_TIMEOUT            1000

/*
 * Frame size
 */

#define STX_DATA_SIZE           1024
#define SOH_DATA_SIZE           128
#define FRAME_DATA_SIZE         1024
#define FRAME_HEADER_SIZE       3
#define FRAME_TAILOR_SIZE       2

/*
 * File Description
 */

#define FILE_NAME_LENGTH        50
#define FILE_SIZE_STR_LENGTH    9


static uint8_t      gFrameBuf[FRAME_DATA_SIZE + FRAME_HEADER_SIZE+ FRAME_TAILOR_SIZE];

/*
 * 
 */
static YmodemRetVal YmodemRecvByte(uint8_t *data, uint32_t timeout)
{
    UsartRetVal             usartRetVal;
    YmodemRetVal            retVal = YMODEM_ERROR;


    usartRetVal = USART_Receive(data, 1, timeout);
    if(usartRetVal == USART_OK)
    {
        retVal = YMODEM_OK;
    }
    else if(usartRetVal == USART_TIMEOUT)
    {
        retVal = YMODEM_TIMEOUT;
    }

    return retVal;
}


static YmodemRetVal YmodemSendByte(uint8_t data)
{
    YmodemRetVal            retVal;
    UsartRetVal             usartRetVal;

    usartRetVal = USART_SendData(&data, 1, SEND_TIMEOUT);

    if(usartRetVal == USART_OK)
    {
        retVal = YMODEM_OK;
    }
    else if(usartRetVal == USART_TIMEOUT)
    {
        retVal = YMODEM_TIMEOUT;
    }
    else
    {
        retVal = YMODEM_ERROR;
    }

    return retVal;
}

static int32_t YmodemAllocReceiveAddr(uint32_t recvAddr, uint32_t size)
{
    int32_t                 ret = -1;
    FlashRetVal             flashRetVal;

    flashRetVal = FLASH_Erase(recvAddr, size);
    if(flashRetVal == FLASH_OK)
    {
        ret = 0;
    }

    return ret;
}

static YmodemRetVal YmodemSaveData(uint32_t addr, uint8_t *data, uint32_t dataLen)
{
    FlashRetVal             flashRetVal;
    uint32_t                i;


    if(data == NULL)
        return YMODEM_PARAM_ERR;

    for(i = 0; i < dataLen; i += 4, data += 4, addr += 4)
    {
        flashRetVal = FLASH_WriteWord(addr, *(uint32_t*)data);
        if(flashRetVal == FLASH_ERROR)
        {
            return YMODEM_ERROR;
        }
    }

    return YMODEM_OK;
}


static YmodemRetVal YmodemReceiveFrame(FrameType * frameType, uint8_t * buf, uint32_t timeout)
{
    YmodemRetVal                retVal;
    uint8_t                     oneByte;
    uint32_t                    i;


    /* Check parameters invalid */
    if(frameType == NULL || buf == NULL)
    {
        return YMODEM_PARAM_ERR;
    }

    retVal = YmodemRecvByte(&oneByte, timeout);

    if(retVal != YMODEM_OK)
    {
        return retVal;
    }

    *frameType = oneByte;

    switch(oneByte)
    {
        case FRAME_SOH:
        {
            *buf = FRAME_SOH;

            for(i = 1; i < (SOH_DATA_SIZE + FRAME_HEADER_SIZE + FRAME_TAILOR_SIZE); i++)
            {
                retVal = YmodemRecvByte(&oneByte, timeout);

                if(retVal != YMODEM_OK)
                {
                    return retVal;
                }

                *(buf + i) = oneByte;
            }
            break;
        }

        case FRAME_STX:
        {
            *buf = FRAME_SOH;

            for(i = 1; i < (STX_DATA_SIZE + FRAME_HEADER_SIZE + FRAME_TAILOR_SIZE); i++)
            {
                retVal = YmodemRecvByte(&oneByte, timeout);

                if(retVal != YMODEM_OK)
                {
                    return retVal;
                }

                *(buf + i) = oneByte;
            }
            break;
        }

        case FRAME_EOT:
        {
            break;
        }

        case FRAME_ABORT1:
        case FRAME_ABORT2:
        {
            return YMODEM_ABORT;
        }
    }

    return retVal;
}


/**
 * Convert character to uint8
 */
static uint8_t Char2Uint(char hexChar)
{
    uint8_t             val = 0;


    if(hexChar >= '0' && hexChar <= '9')
    {
        val = hexChar - '0';
    }

    return val;
}

/**
 * 
 */
static YmodemRetVal ParseFileDescription(uint8_t * content, uint8_t contentLen, char * fileName, uint32_t * fileSize)
{
    uint8_t                 strFileSize[FILE_SIZE_STR_LENGTH] = {0};
    uint8_t                 *pTemp;
    uint32_t                num;


    /* Check Parameters */
    if(content == NULL || fileName == NULL || fileSize == NULL)
        return YMODEM_PARAM_ERR;

    /* Init Parameteres if needed */
    *fileSize = 0;

    /* Init local varibles */
    pTemp = strFileSize;
    num = 0;

    /* Get file name */
    while(1)
    {
        *fileName =  *content;

        if(*fileName == '\0')
            break;

        fileName++;
        content++;
    }

    content++;          /* skip '\0'*/

    /* Get file size */
    while(1)
    {
        *pTemp = *content;

        if(*pTemp == ' ')
            break;

        num *= 10;
        num += Char2Uint(*pTemp);

        pTemp++;
        content++;
    }

    *fileSize = num;
	
	return YMODEM_OK;
}

YmodemRetVal YmodemReceive(uint32_t addrSaveData)
{
    ReceiveState            recvState;
    YmodemRetVal            recvRetVal;
    FrameType               frameType;
    uint8_t                 *dataContent;                   /* pointer to frame content */
    uint32_t                dataContentLen;                 /* frame content length */
    char                    fileName[FILE_NAME_LENGTH];
    uint32_t                fileSize;
    uint32_t                recveivedFileSize;
    uint8_t                 bRecvFinish;



    recvState = WAIT_FILE_DESC;
    bRecvFinish = 0;

    while(!bRecvFinish)
    {

        recvRetVal = YmodemReceiveFrame(&frameType, gFrameBuf, RECEIVE_TIMEOUT);

        if(recvRetVal == YMODEM_ABORT)
        {
            return YMODEM_ABORT;
        }

        switch(recvState)
        {
            case WAIT_FILE_DESC:
            {
                if(recvRetVal == YMODEM_TIMEOUT)
                {
                    YmodemSendByte(FRAME_C);
                }
                else if(recvRetVal == YMODEM_OK)
                {
                    if(frameType == FRAME_SOH)
                    {
                        YmodemRetVal          parseRetVal;


                        dataContent = gFrameBuf + FRAME_HEADER_SIZE;
                        dataContentLen = SOH_DATA_SIZE;

                        parseRetVal = ParseFileDescription(dataContent, dataContentLen, fileName, &fileSize);
                        if(parseRetVal == YMODEM_OK)
                        {
                            YmodemSendByte(FRAME_ACK);
                            YmodemSendByte(FRAME_C);

                            YmodemAllocReceiveAddr(addrSaveData, fileSize);

                            recvState = WAIT_DATA;
                            recveivedFileSize = 0;
                        }
                    }
                }

                break;
            }

            case WAIT_DATA:
            {
                if(recvRetVal == YMODEM_TIMEOUT)
                {
                    YmodemSendByte(FRAME_NAK);
                }
                else if(recvRetVal == YMODEM_OK)
                {
                    uint8_t             saveData = 0;       /* Save data if the frame type is correct */
                    uint32_t            remainDataSize = 0;


                    remainDataSize = fileSize - recveivedFileSize;

                    if(frameType == FRAME_SOH)
                    {
                        dataContentLen = remainDataSize > SOH_DATA_SIZE ? SOH_DATA_SIZE : remainDataSize;
                        saveData = 1;
                    }
                    else if(frameType == FRAME_STX)
                    {
                        dataContentLen = remainDataSize > STX_DATA_SIZE ? STX_DATA_SIZE : remainDataSize;
                        saveData = 1;
                    }
                    
                    if(saveData)
                    {
                        YmodemRetVal          saveRetVal;


                        dataContent = gFrameBuf + FRAME_HEADER_SIZE;

                        saveRetVal = YmodemSaveData(addrSaveData, dataContent, dataContentLen);

                        if(saveRetVal == YMODEM_OK)
                        {
                            YmodemSendByte(FRAME_ACK);

                            recveivedFileSize += dataContentLen;
                            addrSaveData += dataContentLen;

                            if(recveivedFileSize >= fileSize)
                            {
                                recvState = WAIT_EOT;
                            }
                        }
                        else
                        {
                            /*
                             * Send NAK ?
                             */
                        }
                    }
                }
                break;
            }

            case WAIT_EOT:
            {
            
                if(recvRetVal == YMODEM_TIMEOUT)
                {
                    YmodemSendByte(FRAME_NAK);
                }
                else if(recvRetVal == YMODEM_OK)
                {
                    if(frameType == FRAME_EOT)
                    {
                        YmodemSendByte(FRAME_ACK);
                        YmodemSendByte(FRAME_C);

                        recvState = WAIT_END;
                    }
                }
                break;
            }

            case WAIT_END:
            {
                if(recvRetVal == YMODEM_TIMEOUT)
                {
                    YmodemSendByte(FRAME_NAK);
                }
                else if(recvRetVal == YMODEM_OK)
                {
                    if(frameType == FRAME_SOH)
                    {
                        YmodemSendByte(FRAME_ACK);

                        bRecvFinish = 1;
                    }
                }
                break;
            }

            default:
                break;
        }
    }

    return YMODEM_OK;
}
