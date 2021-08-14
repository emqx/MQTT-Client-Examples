#ifndef __GB2312_H__
#define __GB2312_H__


void UTF_8ToUnicode(unsigned short* pOut, char *pText);
void UnicodeToGB2312(char* pOut, unsigned short uData);
void Gb2312ToUnicode(unsigned short* pOut, char *pGb2312Word);
void UnicodeToUTF_8(char* pOut, unsigned short* pText);
int GB2312ToUTF_8_LEN(char *pText, int pLen);
void GB2312ToUTF_8(char *pOut,char *pText, int pLen);
int UTF_8ToGB2312_LEN(char *pText, int pLen);
void UTF_8ToGB2312(char *pOut, char *pText, int pLen);
int Gb2312ToUtf8(char *pGb2312, int len, char **ppUtf8);

#endif

