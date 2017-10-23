#ifndef _UTILS_H
#define _UTILS_H

#include <stdint.h>
#include <stdbool.h>

typedef const char *TStringArray[];

extern uint32_t UTILS_Min(uint32_t a, uint32_t b);
extern uint32_t UTILS_Max(uint32_t a, uint32_t b);
extern float UTILS_MyRound(float inp);
extern char *UTILS_StrUpr(char *s);
extern char *UTILS_StrUprLen(char *s, uint8_t len);
extern char *UTILS_StrLwr(char *s);
extern char *UTILS_ToHex(uint8_t byte);
extern uint8_t UTILS_Hex2Int(char *str);
extern int UTILS_StrPos(const char *string, char c);
extern int UTILS_ParserGetIndex(const TStringArray str_array, char *str);
extern char *UTILS_StrRemoveSpaces(char *string);
extern void UTILS_StrRemoveTrailing(char *string);
extern int UTILS_StrIsEmpty(char *string);
extern bool UTILS_CheckPassword(char *pass);
extern void UTILS_SwapBuff(uint8_t *buff, int count);
extern int UTILS_UnstuffBits(uint8_t *resp, int start, int size);
extern int UTILS_Utf8Strlen(char *str);

#endif
