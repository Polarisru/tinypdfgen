#include <ctype.h>
#include <string.h>
#include "utils.h"

//------------------------------------------------------------------------------
// UTILS_Min - find minimal value
//------------------------------------------------------------------------------
uint32_t UTILS_Min(uint32_t a, uint32_t b)
{
  if (a<b)
    return a;
  return b;
}

//------------------------------------------------------------------------------
// UTILS_Max - find maximal value
//------------------------------------------------------------------------------
uint32_t UTILS_Max(uint32_t a, uint32_t b)
{
  if (a>b)
    return a;
  return b;
}

//------------------------------------------------------------------------------
// UTILS_MyRound - round procedure
//------------------------------------------------------------------------------
float UTILS_MyRound(float inp)
{
  float temp;
  int temp2;
  
  temp2 = (int)(inp*100);
  if (temp2%10>=5)
    temp2 = (temp2/10) + 1;
  else
    temp2 = temp2/10;

  temp = (float)temp2/10;

  return temp;
}

//------------------------------------------------------------------------------
// UTILS_StrUpr - convert string to upper case
//------------------------------------------------------------------------------
char *UTILS_StrUpr(char *s)
{
  char *str=s;

  while (*str)
  {
    *str = toupper(*str);
    str++;
  }
  
  return s;
}

//------------------------------------------------------------------------------
// UTILS_StrUpr - convert string to upper case, length is limited
//------------------------------------------------------------------------------
char *UTILS_StrUprLen(char *s, uint8_t len)
{
  #define UTILS_MAX_STR_LEN (16)
  
  static char str[UTILS_MAX_STR_LEN + 1];
  uint8_t pos = 0;

  while ((*s)&&(len>0)&&(pos<UTILS_MAX_STR_LEN))
  {
    str[pos] = toupper(*s++);
    pos++;
    len--;
  }
  str[pos] = 0;
  
  return str;
}

//------------------------------------------------------------------------------
// UTILS_StrLwr - convert string to lower case
//------------------------------------------------------------------------------
char *UTILS_StrLwr(char *s)
{
  char *str=s;

  while (*str)
  {
    *str = tolower(*str);
    str++;
  }
  
  return s;
}

//------------------------------------------------------------------------------
// UTILS_ToHex - convert byte to HEX string
//------------------------------------------------------------------------------
char *UTILS_ToHex(uint8_t byte)
{
  static char str[3];
 
  if ((byte>>4)>9)
    str[0] = (byte>>4) - 10 + 'A';
  else
    str[0] = (byte>>4) + '0';
  if ((byte&0x0F)>9)
    str[1] = (byte&0x0F) - 10 + 'A';
  else
    str[1] = (byte&0x0F) + '0';
  str[2] = 0;
  
  return str;
}

//------------------------------------------------------------------------------
// UTILS_ToInt - convert one HEX char to uint8_t
//------------------------------------------------------------------------------
uint8_t UTILS_ToInt(char data)
{
  uint8_t res = 0;

  if ((data>=0x30)&&(data<=0x39))
    res = data-0x30;
  else
  if ((data>=0x41)&&(data<=0x46))
    res = data-0x37;

  return res;
}

//------------------------------------------------------------------------------
// UTILS_Hex2Int - convert HEX to uint8_t
//------------------------------------------------------------------------------
uint8_t UTILS_Hex2Int(char *str)
{
  uint8_t ret;
  
  // convert to a single 8 bit result
  ret = (uint8_t)UTILS_ToInt(*str++)*16;
  ret += (uint8_t)UTILS_ToInt(*str++);
  return ret;
}

//------------------------------------------------------------------------------
// UTILS_StrPos - find char position in the string
//------------------------------------------------------------------------------
int UTILS_StrPos(const char *string, char c)
{
  int i;

  i = strlen(string);
  for (;i>=0;i--)
  {
    if (string[i]==c)
      return i;
  }
  
  return -1;
}

//------------------------------------------------------------------------------
// UTILS_ParserGetIndex  - search index in string array
//------------------------------------------------------------------------------
int UTILS_ParserGetIndex(const TStringArray str_array, char *str)
{
  int i;

  UTILS_StrUpr(str);

  i = 0;
  while (str_array[i][0]!='\0')
  {
    if (!strcmp(str_array[i], str))
      return i; // strings are equal
    i++;
  }
  return -1;
}

//------------------------------------------------------------------------------
// UTILS_StrRemoveSpaces - remove leading and trailing spaces
//------------------------------------------------------------------------------
char *UTILS_StrRemoveSpaces(char *string)
{
  char *start;
  uint8_t i;

  //parse it - remove leading spaces
  start = string;
  i = 0;
  while (string[i]&&isspace(string[i++]))
    start++;
  //remove trailing spaces
  //i = strlen(string)-1;
  //while (string[i]&&isspace(string[i]))
  //  string[i--] = 0x00;
  return start;
}

//------------------------------------------------------------------------------
// UTILS_StrRemoveTrailing - remove trailing spaces in string
//------------------------------------------------------------------------------
void UTILS_StrRemoveTrailing(char *string)
{
  uint8_t i;

  //remove trailing spaces
  i = strlen(string)-1;
  while (string[i]&&isspace(string[i]))
    string[i--] = 0x00;
}

//------------------------------------------------------------------------------
// UTILS_StrIsEmpty - is string empty?
//------------------------------------------------------------------------------
int UTILS_StrIsEmpty(char *string)
{
  char *str;

  str = string;
  while ((*str)&&isspace(*(str++)));
  return (*str==0);
}

//------------------------------------------------------------------------------
// UTILS_CheckPassword - check password if it's complex
//------------------------------------------------------------------------------
bool UTILS_CheckPassword(char *pass)
{
  bool haveBigLetter = false;
  bool haveDigit = false;
  
  while (*pass)
  {
    if ((isalpha(*pass))&&(isupper(*pass)))
      haveBigLetter = true;
    if (isdigit(*pass))
      haveDigit = true;
    pass++;
  }
  
  return (haveBigLetter & haveDigit);
}

//------------------------------------------------------------------------------
// UTILS_SwapBuff - swap buffer
//------------------------------------------------------------------------------
void UTILS_SwapBuff(uint8_t *buff, int count)
{
  uint8_t tmp;
  int i;

  for (i=0; i<count/2; i++)
  {
    tmp = buff[i];
    buff[i] = buff[count-i-1];
    buff[count-i-1] = tmp;
  }
}

//------------------------------------------------------------------------------
// UTILS_UnstuffBits - get bits from an array
//------------------------------------------------------------------------------
int UTILS_UnstuffBits(uint8_t *resp, int start, int size)
{                                   
   int byte_idx_stx;                                        
   uint8_t bit_idx_stx, bit_idx;
   int ret, byte_idx;

   byte_idx_stx = start/8;
   bit_idx_stx = start - byte_idx_stx*8;

   if (size<(8 - bit_idx_stx))       // in 1 byte
   {
     return ((resp[byte_idx_stx] >> bit_idx_stx) & ((1<<size) - 1));        
   }
   
   ret =  (resp[byte_idx_stx] >> bit_idx_stx) & ((1<<(8 - bit_idx_stx)) - 1);
   bit_idx = 8 - bit_idx_stx;
   size -= bit_idx;

   byte_idx = 1;
   while (size>8)
   {
     ret |= resp[byte_idx_stx + byte_idx] << (bit_idx);
     size -= 8;
     bit_idx += 8;
     byte_idx ++;
   }
   
  
   if (size>0)
   {
     ret |= (resp[byte_idx_stx + byte_idx] & ((1<<size) - 1)) << bit_idx;
   }

   return ret;
}

//------------------------------------------------------------------------------
// UTILS_Utf8Strlen - calculate string length in UTF8 format
//------------------------------------------------------------------------------
int UTILS_Utf8Strlen(char *str)
{
  uint8_t c;
  int i, len, q;
  
  len = strlen(str);
  for (q=0, i=0; i < len; i++, q++)
  {
    c = (uint8_t)str[i];
    if (c<=127) 
      i+=0;
    else if ((c & 0xE0) == 0xC0) 
      i+=1;
    else if ((c & 0xF0) == 0xE0) 
      i+=2;
    else if ((c & 0xF8) == 0xF0) 
      i+=3;
    else return 0;//invalid utf8
  }
  
  return q;
}
