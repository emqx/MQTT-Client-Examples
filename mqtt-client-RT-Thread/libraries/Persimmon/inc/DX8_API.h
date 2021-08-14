#ifndef __DX8_API_H__
#define __DX8_API_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define DX8_SUCCESS 0x00
#define DX8_BUS_ERR 0x01

#define DX8_NO_INIT 0x02
#define DX8_RESET_ERR 0x03
#define DX8_CHIP_INFO_ERR 0x04
#define DX8_RESPONSE_LEN_ERR 0x05
#define DX8_TRANSFER_CRC_ERR 0x06

#define DX8_STATUS_DATERR 0x07
#define DX8_STATUS_RIGHTERR 0x08
#define DX8_STATUS_CMDERR 0x09
#define DX8_STATUS_TIMEOUT 0x0a
#define DX8_SLEEP_ERR 0x0b
#define DX8_WAKEUP_ERR 0x0c

#define DX8_VERIFY_PIN_ERR 0x0d
#define DX8_VERIFY_KEY_ERR 0x0e

#define DX8_WRITE_ZONE_LEN_ERR 0x0f
#define DX8_READ_ZONE_LEN_ERR 0x10

#define DX8_CRYPTO_KEY_ERR 0x11
#define DX8_CRYPTO_LEN_ERR 0x12

#define DX8_ERASE_EEPROM_ERR 0x30
#define DX8_CHECK_EEPROM_ERR 0x31

//-----------------------------------------------------------------------------------------------
//  extern function, need to be implementated by user according to CPU and OS
//-----------------------------------------------------------------------------------------------
extern unsigned char dxif_transfer(unsigned char *buf, unsigned short len);

//-----------------------------------------------------------------------------------------------
//  Data Structure definition
//-----------------------------------------------------------------------------------------------
typedef struct
{
    unsigned char Type;
    unsigned char PageSize;
    unsigned char ZoneNum;
    unsigned short ZoneSize;
    unsigned char SN[8];
    unsigned char UID[7];
    unsigned char MID[6];
    unsigned char ZoneMode[16];
    unsigned char Fuse;
} CHIP_INFO;

extern CHIP_INFO dx8_info;     // user can access directly
extern unsigned int srand_cnt; // reference can be used for host rand() function

//----------------------------------------------------------------------------------------------------------------
// system function
//----------------------------------------------------------------------------------------------------------------
char *DX8_Version(void);                          // return DX8_API version, customID and company
void DX8_SetDevAddress(unsigned char DevAddress); //default is 0x60, DX8-C2 should change to 0x20

unsigned char DX8_Reset(void);
unsigned char DX8_Sleep(void);
unsigned char DX8_Wakeup(void);
unsigned char DX8_GetRandom(unsigned char *random, unsigned char len);
unsigned char DX8_VerifyPin(unsigned char *seed, unsigned char *buf);

//----------------------------------------------------------------------------------------------------------------
// host authentication
//----------------------------------------------------------------------------------------------------------------
unsigned char DX8_HostAuth(unsigned char *challenge, unsigned char len, unsigned char *response);
void Lib_HostAuth(unsigned char *challenge, unsigned char len, unsigned char *buf, unsigned char *response);

//----------------------------------------------------------------------------------------------------------------
// write and read memory
//----------------------------------------------------------------------------------------------------------------
unsigned char DX8_VerifyZone(unsigned char zone, unsigned char *seed, unsigned char *buf);
unsigned char DX8_WriteZone(unsigned char zone, unsigned short address, unsigned short len, unsigned char *buf);
unsigned char DX8_ReadZone(unsigned char zone, unsigned short address, unsigned short len, unsigned char *buf);

#ifdef __cplusplus
}
#endif

#endif
