#ifndef _PDF_H
#define _PDF_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "arc4.h"
#include "md5.h"
#include "pdf_wrapper.h"

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

#define PDF_PAGE_HEIGHT         842

#define PDF_BIT_PAGE            0x8000
#define PDF_MAX_BLOCKLEN        0x7FFF

#define PDF_ERR_NONE            0
#define PDF_ERR_FILE            1
#define PDF_ERR_BUSY            2
#define PDF_ERR_LONGBLOCK       3
#define PDF_ERR_MAXNUM          4
#define PDF_ERR_HEADER          5
#define PDF_ERR_NOTSTARTED      6

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
  PDF_OBJNUM_FONTDESCR,
  PDF_OBJNUM_FONTWIDTHS,
  PDF_OBJNUM_EMBFONT,
  PDF_OBJNUM_IMAGE,
  PDF_OBJNUM_LAST
};

enum PDF_FONTS
{
  PDF_FONT_TIMES,
  PDF_FONT_TIMES_BOLD,
  PDF_FONT_TIMES_ITALIC,
  PDF_FONT_TIMES_BOLDITALIC,
  PDF_FONT_HELVETICA,
  PDF_FONT_HELVETICA_BOLD,
  PDF_FONT_HELVETICA_ITALIC,
  PDF_FONT_HELVETICA_BOLDITALIC,
  PDF_FONT_COURIER,
  PDF_FONT_COURIER_BOLD,
  PDF_FONT_COURIER_ITALIC,
  PDF_FONT_COURIER_BOLDITALIC,
  PDF_FONT_LAST
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

extern const char PDF_IMAGE_INSERT[];
extern const char PDF_FRAME_STRING[];

extern uint8_t PDF_Start(char *name, char *title, char *author);
extern uint8_t PDF_AddPage(bool has_header);
extern uint8_t PDF_AddStream(char *stream);
extern uint8_t PDF_AddText(uint16_t x, uint16_t y, char *text);
extern uint8_t PDF_AddTextToHeader(uint16_t x, uint16_t y, char *text);
extern void PDF_SetFont(uint8_t font);
extern uint8_t PDF_AddFrame(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
extern uint8_t PDF_Finish(void);
extern void PDF_Abort(void);

#endif
