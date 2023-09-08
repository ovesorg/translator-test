#ifndef __CRC16_H__
#define __CRC16_H__


#define	CRC_16_POLYNOMIALS	0xa001

uint16_t Check_CRC16(uint8_t* pchMsg,uint8_t wDataLen);
uint8_t CRCVerification(can_receive_message_struct* message);
uint8_t CRC16Verification(uint8_t* pchMsg,uint8_t wDataLen);
#endif /*__CRC16_H__*/


