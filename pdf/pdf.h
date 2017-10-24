#ifndef _PDF_H
#define _PDF_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "arc4.h"
#include "md5.h"

#define PDF_ID_LEN              16
#define PDF_PASSWD_LEN          32
#define PDF_ENCRYPT_KEY_MAX     16
#define PDF_PERMISSION_PAD      0xFFFFFFC0

#define PDF_ENABLE_READ         0
#define PDF_ENABLE_PRINT        4
#define PDF_ENABLE_EDIT_ALL     8
#define PDF_ENABLE_COPY         16
#define PDF_ENABLE_EDIT         32

#define PDF_ENCRYPT_R2          2
#define PDF_ENCRYPT_R3          3

#define PDF_LINES_NUMBER        45
#define PDF_TEXT_XOFFS          50
#define PDF_TEXT_YOFFS          705
#define PDF_TEXT_HEIGHT         15
#define PDF_TEXT_TOPY           21
#define PDF_PAGE_HEIGHT         842

#define PDF_BIT_PAGE            0x8000
#define PDF_MAX_BLOCKLEN        0x7FFF

#define PDF_ERR_NONE            0
#define PDF_ERR_FILE            1
#define PDF_ERR_LONGBLOCK       2
#define PDF_ERR_MAXNUM          3


enum PDF_TITLES
{
  PDF_TITLE_A,
  PDF_TITLE_T
};

enum PDF_OBJECTS
{
  PDF_OBJECT_ZERO,
  PDF_OBJECT_ROOT,
  PDF_OBJECT_PAGES,
  PDF_OBJECT_RESOURCES,
  PDF_OBJECT_FONT,
  PDF_OBJECT_ENC,
  PDF_OBJECT_INFO,
  PDF_OBJECT_HEADER,
  PDF_OBJECT_PAGE,
  PDF_OBJECT_NUMPAGE,
  PDF_OBJECT_XREF,
  PDF_OBJECT_TRAILER,
  PDF_OBJECT_END,
  PDF_OBJECT_ONEPAGE,
  PDF_OBJECT_TEXT,
  PDF_OBJECT_GRAPH,
  PDF_OBJECT_EMBFONT
  //PDF_OBJECT_LAST
};

enum PDF_OBJNUMS
{
  PDF_OBJNUM_ZERO,
  PDF_OBJNUM_ROOT,
  PDF_OBJNUM_PAGES,
  PDF_OBJNUM_RESOURCES,
  PDF_OBJNUM_FONT1,
  PDF_OBJNUM_FONT2,
  PDF_OBJNUM_ENC,
  PDF_OBJNUM_INFO,
  PDF_OBJNUM_HEADER,
  PDF_OBJNUM_FONTDESCR,
  PDF_OBJNUM_FONTWIDTHS,
  PDF_OBJNUM_EMBFONT,
  PDF_OBJNUM_LAST
};

typedef struct
{
  uint8_t   mode;

  /* key_len must be a multiple of 8, and between 40 to 128 */
  uint8_t          key_len;

  /* owner-password (not encrypted) */
  uint8_t          owner_passwd[PDF_PASSWD_LEN];

  /* user-password (not encrypted) */
  uint8_t          user_passwd[PDF_PASSWD_LEN];

  /* owner-password (encrypted) */
  uint8_t          owner_key[PDF_PASSWD_LEN];

  /* user-password (encrypted) */
  uint8_t          user_key[PDF_PASSWD_LEN];

  uint32_t         permission;
  uint8_t          encrypt_id[PDF_ID_LEN];
  uint8_t          encryption_key[MD5_KEY_LEN + 5];
  uint8_t          md5_encryption_key[MD5_KEY_LEN];
  ARC4_Ctx_Rec  arc4ctx;
} TPDFEncryptRec;

typedef struct
{
  uint16_t X;
  uint16_t Y;
  uint8_t Size;
  char *Text;
} TPdfText;

typedef struct
{
  uint16_t Position;
} TPdfXref;

extern FILE* PDF_Start(char *name, char *title, char *author);
extern uint8_t PDF_Write(FILE *fdst, uint8_t type, void *data);
extern uint8_t PDF_AddPage(FILE *fd);
extern uint8_t PDF_AddText(FILE *fd, uint16_t x, uint16_t y, char *text);
extern uint8_t PDF_Finish(FILE *fd);

#endif
