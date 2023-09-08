#include "main.h"





uint16_t Check_CRC16(uint8_t* pchMsg,uint8_t wDataLen)
{
    uint8_t i, chChar; uint16_t wCRC = 0xFFFF; 
    while (wDataLen--)
    {
        chChar = *pchMsg++;
        wCRC ^= (uint16_t) chChar;
        for (i = 0; i < 8; i++)
        {
            if (wCRC & 0x0001)
                wCRC = (wCRC >> 1) ^ CRC_16_POLYNOMIALS;
            else
                wCRC >>= 1;
        }
    }
    return wCRC;
}

uint8_t CRCVerification(can_receive_message_struct* message) 
{
    // 校验前六位数据
    uint16_t crc = Check_CRC16(message->rx_data, 6);

    // 获取第7、8位校验码
    uint8_t crc_high = message->rx_data[6];
    uint8_t crc_low = message->rx_data[7];

    // 比较校验结果
    if ((crc >> 8) == crc_high && (crc & 0xFF) == crc_low) {
        return 1; // 校验通过
    } else {
        return 0; // 校验失败
    }
}

uint8_t CRC16Verification(uint8_t* pchMsg,uint8_t wDataLen) 
{
    // 校验前六位数据
    uint16_t crc = Check_CRC16(pchMsg, wDataLen);

    // 获取第7、8位校验码
    uint8_t crc_high = pchMsg[wDataLen];
    uint8_t crc_low = pchMsg[wDataLen+1];

    // 比较校验结果
    if ((crc >> 8) == crc_high && (crc & 0xFF) == crc_low) {
        return 1; // 校验通过
    } else {
        return 0; // 校验失败
    }
}

