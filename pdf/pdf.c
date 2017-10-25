#include <stdio.h>
#include <string.h>

#include "arc4.h"
#include "md5.h"
#include "pdf.h"
#include "pdf_conf.h"
#include "pdf_const.h"
#ifdef PDF_USE_EMBFONT
  #include "pdf_font.h"
#endif // PDF_USE_EMBFONT
#include "pdf_wrapper.h"

// --- PDF objects structure ---
//object 1 - root
//object 2 - pages, generate at the end
//object 3 - resources (fonts)
//object 4, 5 - internal fonts (Courier, Courier-Bold), not embedded!
//object 6 - encryption data
//object 7 - info
//object 8 - header of the pages (not modifiable), used only for audit trail file
//object 9 - font description (embedded font)
//object 10 - font widthes (embedded font)
//object 11 - font data (embedded font)
//object 12 and following - text blocks and pages

uint16_t PDF_XrefTable[PDF_MAX_NUM]; //big array to store objects positions

uint32_t PDF_CurrObject;  //current PDF object
uint16_t PDF_PageNum;     //number of the page
uint32_t PDF_LastObject;  //last saved PDF object
uint32_t PDF_XrefPos;     //current Xref position
TPDFEncryptRec PDF_EncryptRec;  //structure for encrypting the document

/** \brief Convert byte value to HEX string
 *
 * \param [in] byte Byte value to convert
 * \return String with HEX representation
 *
 */
char *PDF_ByteToHex(uint8_t byte)
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

/** \brief Build escaped string
 *
 * \param [out] str Output string
 * \param [in] key Input string
 * \return Length of new string
 *
 */
uint8_t PDF_EscString(uint8_t *str, uint8_t *key, uint16_t len)
{
  uint8_t i;
  uint8_t res = 0;

  for (i=0; i<len; i++)
  {
    switch (key[i])
    {
      case '(':
      case ')':
      case '\\':
        str[res++] = '\\';
        break;
    }
    str[res++] = key[i];
  }

  return res;
}

/** \brief Pad or truncate initial password to standard length
 *
 * \param
 * \param
 * \return
 *
 */
void PDF_PadOrTruncatePasswd(const char *pwd, uint8_t *new_pwd)
{
  uint8_t len = strlen(pwd);

  memset(new_pwd, 0x00, PDF_PASSWD_LEN);

  if (len >= PDF_PASSWD_LEN)
  {
    memcpy(new_pwd, (uint8_t *)pwd, PDF_PASSWD_LEN);
  } else
  {
    if (len > 0)
    {
      memcpy(new_pwd, (uint8_t *)pwd, len);
    }
    memcpy(new_pwd + len, PDF_PADDING_STRING, PDF_PASSWD_LEN - len);
  }
}

/** \brief Initialize encryption structure
 *
 * \param
 * \param
 * \return
 *
 */
void PDF_Encrypt_Init(TPDFEncryptRec *attr)
{
  memset(attr, 0, sizeof(TPDFEncryptRec));
  attr->mode = PDF_ENCRYPT_R3;
  attr->key_len = 5;
  PDF_PadOrTruncatePasswd(PDF_OWNER_PASS, attr->owner_passwd);
  memcpy(attr->user_passwd, PDF_PADDING_STRING, PDF_PASSWD_LEN);
  attr->permission = PDF_ENABLE_PRINT | PDF_PERMISSION_PAD;
  /* other available options: PDF_ENABLE_EDIT_ALL | PDF_ENABLE_COPY | PDF_ENABLE_EDIT */
}

/** \brief Create owner key (PDF spec. Algorithm 3.3)
 *
 * \param
 * \return
 *
 */
void PDF_Encrypt_CreateOwnerKey(TPDFEncryptRec *attr)
{
  ARC4_Ctx_Rec rc4_ctx;
  TMD5Context md5_ctx;
  uint8_t digest[MD5_KEY_LEN];
  uint8_t tmppwd[PDF_PASSWD_LEN];
  uint8_t tmppwd2[PDF_PASSWD_LEN];
	int i;

  // create md5-digest using the value of owner_passwd

  // Algorithm 3.3 step 2
  MD5_Init(&md5_ctx);
  MD5_Update(&md5_ctx, attr->owner_passwd, PDF_PASSWD_LEN);
  MD5_Final(digest, &md5_ctx);

  // Algorithm 3.3 step 3 (Revision 3 only)
  if (attr->mode==PDF_ENCRYPT_R3)
  {
    for (i = 0; i < 50; i++)
    {
      MD5_Init(&md5_ctx);
      MD5_Update(&md5_ctx, digest, attr->key_len);
      MD5_Final(digest, &md5_ctx);
    }
  }

  // Algorithm 3.3 step 4
  ARC4_Init(&rc4_ctx, digest, attr->key_len);

  // Algorithm 3.3 step 6
  ARC4_CryptBuf(&rc4_ctx, attr->user_passwd, tmppwd, PDF_PASSWD_LEN);

  // Algorithm 3.3 step 7
  if (attr->mode==PDF_ENCRYPT_R3)
  {
    for (i = 1; i <= 19; i++)
    {
      uint8_t j;
      uint8_t new_key[MD5_KEY_LEN];

      for (j = 0; j < attr->key_len; j++)
        new_key[j] = (uint8_t)(digest[j] ^ i);

      memcpy(tmppwd2, tmppwd, PDF_PASSWD_LEN);
      ARC4_Init(&rc4_ctx, new_key, attr->key_len);
      ARC4_CryptBuf(&rc4_ctx, tmppwd2, tmppwd, PDF_PASSWD_LEN);
    }
  }

  // Algorithm 3.3 step 8
  memcpy(attr->owner_key, tmppwd, PDF_PASSWD_LEN);
}

/** \brief Create encryption key
 *
 * \param
 * \param
 * \return
 *
 */
void PDF_Encrypt_CreateEncryptionKey(TPDFEncryptRec *attr)
{
  TMD5Context md5_ctx;
  uint8_t tmp_flg[4];
  uint8_t i;

  // Algorithm3.2 step2
  MD5_Init(&md5_ctx);
  MD5_Update(&md5_ctx, attr->user_passwd, PDF_PASSWD_LEN);

  // Algorithm3.2 step3
  MD5_Update(&md5_ctx, attr->owner_key, PDF_PASSWD_LEN);

  // Algorithm3.2 step4
  tmp_flg[0] = (uint8_t)(attr->permission);
  tmp_flg[1] = (uint8_t)(attr->permission >> 8);
  tmp_flg[2] = (uint8_t)(attr->permission >> 16);
  tmp_flg[3] = (uint8_t)(attr->permission >> 24);

  MD5_Update(&md5_ctx, tmp_flg, 4);

  // Algorithm3.2 step5
  MD5_Update(&md5_ctx, attr->encrypt_id, PDF_ID_LEN);
  MD5_Final(attr->encryption_key, &md5_ctx);

  // Algorithm 3.2 step6 (Revision 3 only)
  if (attr->mode==PDF_ENCRYPT_R3)
  {
    for (i = 0; i < 50; i++)
    {
      MD5_Init(&md5_ctx);
      MD5_Update (&md5_ctx, attr->encryption_key, attr->key_len);
      MD5_Final(attr->encryption_key, &md5_ctx);
    }
  }
}

/** \brief Create user key (PDF spec. Algorithm 3.5)
 *
 * \param
 * \param
 * \return
 *
 */
void PDF_Encrypt_CreateUserKey(TPDFEncryptRec *attr)
{
  ARC4_Ctx_Rec ctx;
  uint8_t digest[MD5_KEY_LEN];
  uint8_t digest2[MD5_KEY_LEN];
  TMD5Context md5_ctx;
	int i;

  // Algorithm 3.4/5 step1

  // Algorithm 3.4 step2
  ARC4_Init(&ctx, attr->encryption_key, attr->key_len);
  ARC4_CryptBuf(&ctx, attr->user_passwd, attr->user_key, PDF_PASSWD_LEN);

  if (attr->mode==PDF_ENCRYPT_R3)
  {
    // Algorithm 3.5 step2 (same as Algorithm3.2 step2)
    MD5_Init(&md5_ctx);
    MD5_Update(&md5_ctx, attr->user_passwd, PDF_PASSWD_LEN);

    // Algorithm 3.5 step3
    MD5_Update(&md5_ctx, attr->encrypt_id, PDF_ID_LEN);
    MD5_Final(digest, &md5_ctx);

    // Algorithm 3.5 step4
    ARC4_Init(&ctx, attr->encryption_key, attr->key_len);
    ARC4_CryptBuf(&ctx, digest, digest2, MD5_KEY_LEN);

    // Algorithm 3.5 step5
    for (i = 1; i <= 19; i++)
    {
      uint8_t j;
      uint8_t new_key[MD5_KEY_LEN];

      for (j = 0; j < attr->key_len; j++)
        new_key[j] = (uint8_t)(attr->encryption_key[j] ^ i);

      memcpy(digest, digest2, MD5_KEY_LEN);

      ARC4_Init(&ctx, new_key, attr->key_len);
      ARC4_CryptBuf(&ctx, digest, digest2, MD5_KEY_LEN);
    }

    // use the result of Algorithm 3.4 as 'arbitrary padding'
    memset(attr->user_key, 0, PDF_PASSWD_LEN);
    memcpy(attr->user_key, digest2, MD5_KEY_LEN);
  }
}

/** \brief Initialize encryption key
 *
 * \param
 * \param
 * \return
 *
 */
void PDF_Encrypt_InitKey(TPDFEncryptRec *attr, uint32_t object_id, uint16_t gen_no)
{
  TMD5Context md5_ctx;
  uint8_t key_len;

  attr->encryption_key[attr->key_len] = (uint8_t)object_id;
  attr->encryption_key[attr->key_len + 1] = (uint8_t)(object_id >> 8);
  attr->encryption_key[attr->key_len + 2] = (uint8_t)(object_id >> 16);
  attr->encryption_key[attr->key_len + 3] = (uint8_t)gen_no;
  attr->encryption_key[attr->key_len + 4] = (uint8_t)(gen_no >> 8);

  MD5_Init(&md5_ctx);
  MD5_Update(&md5_ctx, attr->encryption_key, attr->key_len + 5);
  MD5_Final(attr->md5_encryption_key, &md5_ctx);

  key_len = (attr->key_len + 5 > PDF_ENCRYPT_KEY_MAX) ? PDF_ENCRYPT_KEY_MAX : attr->key_len + 5;

  ARC4_Init(&attr->arc4ctx, attr->md5_encryption_key, key_len);
}

/** \brief Reset PDF ecnryption (ARC4)
 *
 * \param
 * \param
 * \return
 *
 */
void PDF_Encrypt_Reset(TPDFEncryptRec *attr)
{
  uint8_t key_len = (attr->key_len + 5 > PDF_ENCRYPT_KEY_MAX) ? PDF_ENCRYPT_KEY_MAX : attr->key_len + 5;

  ARC4_Init(&attr->arc4ctx, attr->md5_encryption_key, key_len);
}

/** \brief Encrypt buffer with ARC4
 *
 * \param
 * \param
 * \return
 *
 */
void PDF_Encrypt_CryptBuf(TPDFEncryptRec *attr, uint8_t *src, uint8_t *dst, uint32_t len)
{
  ARC4_CryptBuf(&attr->arc4ctx, src, dst, len);
}

//------------------------------------------------------------------------------
// PDF_WriteObject - write objects to PDF file
//------------------------------------------------------------------------------
/*int PDF_Write(FILE *fdst, uint8_t type, void *data)
{
  char str[512];
  char str2[512];
  uint32_t fr;
  uint32_t br, bw;         // File read/write count
  TPdfText *pdfText;
  TPdfHeader *pdfHeader;
  uint32_t pos;
  uint32_t len;

  pos = PDF_WR_ftell(fdst); //save current file position

  PDF_XrefTable[PDF_CurrObject].isPage = false;

  switch (type)
  {
    case PDF_OBJECT_RESOURCES:
      //resources object
      strcpy(str, PDF_RESOURCE_OBJECT);
      PDF_XrefTable[PDF_OBJNUM_RESOURCES].Position = pos;
      break;
    case PDF_OBJECT_FONT:
      //object for internal fonts, replaced with embedded font now
      len = *((int*)data);
      if (len==1)
      {
        strcpy(str, PDF_FONT1_OBJECT);
        PDF_XrefTable[PDF_OBJNUM_FONT1].Position = pos;
      } else
      {
        strcpy(str, PDF_FONT2_OBJECT);
        PDF_XrefTable[PDF_OBJNUM_FONT2].Position = pos;
      }
      break;
    case PDF_OBJECT_GRAPH:
      //graphical object, write without text pattern
      PDF_XrefTable[PDF_CurrObject].Position = pos; //save current file position
      pdfText = (TPdfText*)data;
      strcpy(str, pdfText->Text);
      len = strlen(str);
      sprintf(str2, PDF_STREAM_OBJ_START, PDF_CurrObject, len);
      br = strlen(str2);
      fr = f_write(fdst, str2, br, &bw);
      if (fr!=FR_OK)
        return fr;
      if (PDF_Encrypt)
      {
        //encrypt string
        PDF_Encrypt_InitKey(&PDF_EncryptRec, PDF_CurrObject, 0);
        PDF_Encrypt_CryptBuf(&PDF_EncryptRec, (uint8_t*)str, (uint8_t*)str2, len);
        fr = f_write(fdst, str2, len, &bw);
      } else
      {
        fr = f_write(fdst, str, len, &bw);
      }
      if (fr!=FR_OK)
        return fr;
      strcpy(str, PDF_STREAM_OBJ_END);
      PDF_CurrObject++;
      break;
    case PDF_OBJECT_EMBFONT:
      //embedded font object
      //write font definition to file
      PDF_XrefTable[PDF_OBJNUM_FONT1].Position = pos;
      len = strlen(PDF_FONT1_DEF);
      fr = f_write(fdst, PDF_FONT1_DEF, len, &bw);
      if (fr!=FR_OK)
        return fr;
      //write font description to file
      PDF_XrefTable[PDF_OBJNUM_FONTDESCR].Position = f_tell(fdst);
      len = strlen(PDF_FONT1_DESC);
      fr = f_write(fdst, PDF_FONT1_DESC, len, &bw);
      if (fr!=FR_OK)
        return fr;
      //write font widthes to file
      PDF_XrefTable[PDF_OBJNUM_FONTWIDTHS].Position = f_tell(fdst);
      len = strlen(PDF_FONT1_WIDTH_START);
      fr = f_write(fdst, PDF_FONT1_WIDTH_START, len, &bw);
      if (fr!=FR_OK)
        return fr;
      for (br=32; br<=255; br+=16)
      {
        len = strlen(PDF_FONT1_WIDTH_CONT);
        fr = f_write(fdst, PDF_FONT1_WIDTH_CONT, len, &bw);
        if (fr!=FR_OK)
          return fr;
      }
      len = strlen(PDF_FONT1_WIDTH_END);
      fr = f_write(fdst, PDF_FONT1_WIDTH_END, len, &bw);
      if (fr!=FR_OK)
        return fr;
      //write ttf font to file
      PDF_XrefTable[PDF_OBJNUM_EMBFONT].Position = f_tell(fdst);
      len = sizeof(acCourierFont);
      sprintf(str2, PDF_STREAM_FONTOBJ_START, PDF_OBJNUM_EMBFONT, len, len);
      br = strlen(str2);
      fr = f_write(fdst, str2, br, &bw);
      if (fr!=FR_OK)
        return fr;
      //write font file to document
      if (PDF_Encrypt)
        PDF_Encrypt_InitKey(&PDF_EncryptRec, PDF_OBJNUM_EMBFONT, 0);
      br = 0;
      //pos = 0;
      while (br<sizeof(acCourierFont))
      {
        if (sizeof(acCourierFont)-br>sizeof(str))
          len = sizeof(str);
        else
          len = sizeof(acCourierFont) - br;
        if (PDF_Encrypt)
        {
          //encrypt font stream
          PDF_Encrypt_CryptBuf(&PDF_EncryptRec, (uint8_t*)&acCourierFont[br], (uint8_t*)str, len);
        } else
        {
          for (pos=0; pos<len; pos++)
            str[pos] = acCourierFont[br + pos];
          //memcpy(str, &acCourierFont[br], len);
        }
        fr = f_write(fdst, str, len, &bw);
        if (fr!=FR_OK)
          return fr;
        br += len;
        //pos++;
        //if (pos>10)
        //{
        //  pos = 0;
        //  os_dly_wait(1);
        //}
      }
      //write end of stream object
      strcpy(str, PDF_STREAM_OBJ_END2);
      break;
  }
}*/

uint16_t PDF_PrepareString(char *src, char* dst, bool doEsc, uint16_t num, bool init)
{
  uint16_t len;
  char temp[PDF_BLOCK_SIZE+1];

  len = strlen(src);
  if (len > PDF_BLOCK_SIZE)
    len = PDF_BLOCK_SIZE;
  #ifdef PDF_USE_ENCRYPT
    if (init)
      PDF_Encrypt_InitKey(&PDF_EncryptRec, num, 0);
    PDF_Encrypt_CryptBuf(&PDF_EncryptRec, (uint8_t*)src, (uint8_t*)temp, len);
    if (doEsc)
      len = PDF_EscString((uint8_t*)dst, (uint8_t*)temp, len);
    else
      memcpy(dst, temp, len); /* not string already! */
  #else
    strncpy(temp, src, PDF_BLOCK_SIZE);
    temp[PDF_BLOCK_SIZE] = 0;
    if (doEsc)
      len = PDF_EscString((uint8_t*)dst, (uint8_t*)temp, len);
    else
      strcpy(dst, temp);
  #endif // PDF_USE_ENCRYPT

  return len;
}

/** \brief Initialize everything to start generating PDF file
 *
 * \param [in] name Name of file to write
 * \return FILE pointer
 *
 */
FILE* PDF_Start(char *name, char *title, char *author)
{
  uint8_t i;
  char str[PDF_BLOCK_SIZE*2];
  uint16_t len;
  TMD5Context md5_ctx;
  FILE* fd = NULL;

  PDF_CurrObject = PDF_OBJNUM_LAST;
  PDF_LastObject = PDF_CurrObject;
  PDF_PageNum = 0;
  PDF_XrefPos = 0;
  for (i=PDF_OBJNUM_ZERO; i<=PDF_OBJNUM_LAST; i++)
  {
    PDF_XrefTable[i] = 0;
  }
  //generate encryption keys
  PDF_Encrypt_Init(&PDF_EncryptRec);
  //generate file ID from serial number and test number/length of audit trail
  memcpy(str, PDF_DUMMY_ID, PDF_ID_LEN);
  //memcpy(&str[16], PDF_DUMMY_ID, PDF_ID_LEN);
  //REMARK: for some strings md5 hash is right but is not accepted, decoding doesn't work!
  PDF_WR_srand();
  sprintf(str, "%d%d", PDF_WR_rand(), PDF_WR_rand()); //ID string should be 16 bytes or longer!
  str[strlen(str)] = 0x20;
  MD5_Init(&md5_ctx);
  MD5_Update(&md5_ctx, (uint8_t*)str, PDF_ID_LEN);
  MD5_Final(PDF_EncryptRec.encrypt_id, &md5_ctx);
  #ifdef PDF_USE_ENCRYPT
    PDF_Encrypt_CreateOwnerKey(&PDF_EncryptRec);
    PDF_Encrypt_CreateEncryptionKey(&PDF_EncryptRec);
    PDF_Encrypt_CreateUserKey(&PDF_EncryptRec);
  #endif // PDF_USE_ENCRYPT

  fd = fopen(name, "wb");
  while (fd!=NULL)
  {
    /**< write header */
    if (!PDF_WR_fwrite(fd, PDF_HEADER, strlen(PDF_HEADER)))
      break;
    PDF_XrefTable[PDF_OBJNUM_ROOT] = PDF_WR_ftell(fd);
    /**< write root object */
    if (!PDF_WR_fwrite(fd, PDF_FIRST_OBJECT, strlen(PDF_FIRST_OBJECT)))
      break;
    PDF_XrefTable[PDF_OBJNUM_RESOURCES] = PDF_WR_ftell(fd);
    if (!PDF_WR_fwrite(fd, PDF_RESOURCE_OBJECT, strlen(PDF_RESOURCE_OBJECT)))
      break;
    #ifdef PDF_USE_EMBFONT
    #else
      PDF_XrefTable[PDF_OBJNUM_FONT1] = PDF_WR_ftell(fd);
      if (!PDF_WR_fwrite(fd, PDF_FONT1_OBJECT, strlen(PDF_FONT1_OBJECT)))
        break;
    #endif // PDF_USE_EMBFONT
    #ifdef PDF_USE_ENCRYPT
      PDF_XrefTable[PDF_OBJNUM_ENC] = PDF_WR_ftell(fd);
      if (!PDF_WR_fwrite(fd, PDF_ENC_OBJ_START, strlen(PDF_ENC_OBJ_START)))
        break;
      i = PDF_EscString((uint8_t*)str, PDF_EncryptRec.user_key, PDF_PASSWD_LEN); //encoded string could contains '()\', so escape them
      if (!PDF_WR_fwrite(fd, str, i))
        break;
      if (!PDF_WR_fwrite(fd, PDF_ENC_OBJ_MID, strlen(PDF_ENC_OBJ_MID)))
        break;
      i = PDF_EscString((uint8_t*)str, PDF_EncryptRec.owner_key, PDF_PASSWD_LEN); //encoded string could contains '()\', so escape them
      if (!PDF_WR_fwrite(fd, str, i))
        break;
      sprintf(str, PDF_ENC_OBJ_END, PDF_EncryptRec.permission);
      if (!PDF_WR_fwrite(fd, str, strlen(str)))
        break;
    #endif
    /**< write info module */
    PDF_XrefTable[PDF_OBJNUM_INFO] = PDF_WR_ftell(fd);
    if (!PDF_WR_fwrite(fd, PDF_INFO_OBJ_1, strlen(PDF_INFO_OBJ_1)))
      break;
    len = PDF_PrepareString(title, str, true, PDF_OBJNUM_INFO, true);
    if (!PDF_WR_fwrite(fd, str, len))
      break;
    if (!PDF_WR_fwrite(fd, PDF_INFO_OBJ_2, strlen(PDF_INFO_OBJ_2)))
      break;
    len = PDF_PrepareString(author, str, true, PDF_OBJNUM_INFO, true);
    if (!PDF_WR_fwrite(fd, str, len))
      break;
    if (!PDF_WR_fwrite(fd, PDF_INFO_OBJ_3, strlen(PDF_INFO_OBJ_3)))
      break;
    len = PDF_PrepareString(PDF_GEN_NAME, str, true, PDF_OBJNUM_INFO, true);
    if (!PDF_WR_fwrite(fd, str, len))
      break;
    if (!PDF_WR_fwrite(fd, PDF_INFO_OBJ_4, strlen(PDF_INFO_OBJ_4)))
      break;
    sprintf(str, PDF_DATE_FMT, 2017, 10, 22, 23, 7, 11);
    len = PDF_PrepareString(str, str, true, PDF_OBJNUM_INFO, true);
    if (!PDF_WR_fwrite(fd, str, len))
      break;
    if (!PDF_WR_fwrite(fd, PDF_INFO_OBJ_5, strlen(PDF_INFO_OBJ_5)))
      break;

    return fd;
  }
  /* close file handler, error occurred */
  if (fd!=NULL)
    PDF_WR_fclose(fd);

  return NULL;
}

/** \brief Write page object to document
 *
 * \param
 * \param
 * \return
 *
 */
uint8_t PDF_WritePage(FILE *fd)
{
  uint32_t len;
  char str[64];
  uint32_t pos = PDF_WR_ftell(fd);

  if ((pos - PDF_XrefPos) > PDF_MAX_BLOCKLEN)
    return PDF_ERR_LONGBLOCK;

  if (PDF_CurrObject>=PDF_MAX_NUM)
    return PDF_ERR_MAXNUM;

  PDF_XrefTable[PDF_CurrObject] = (uint16_t)(pos - PDF_XrefPos) | PDF_BIT_PAGE;
  PDF_XrefPos = pos;
  //PDF_XrefTable[PDF_CurrObject].isPage = true;
  sprintf(str, PDF_PAGE_OBJ_START, PDF_CurrObject);
  if (!PDF_WR_fwrite(fd, str, strlen(str)))
    return PDF_ERR_FILE;
  for (len=PDF_LastObject; len<PDF_CurrObject; len++)
  {
    sprintf(str, PDF_CONT_FMT, len);
    if (!PDF_WR_fwrite(fd, str, strlen(str)))
      return PDF_ERR_FILE;
  }
  if (!PDF_WR_fwrite(fd, PDF_PAGE_OBJ_END, strlen(PDF_PAGE_OBJ_END)))
    return PDF_ERR_FILE;
  PDF_CurrObject++;
  PDF_LastObject = PDF_CurrObject;

  return PDF_ERR_NONE;
}

/** \brief Add new page
 *
 * \param
 * \param
 * \return
 *
 */
uint8_t PDF_AddPage(FILE *fd)
{
  uint8_t ret = PDF_ERR_NONE;

  if (PDF_CurrObject>PDF_LastObject)
    ret = PDF_WritePage(fd);
  PDF_PageNum++;

  return ret;
}

/** \brief Add text to existing page
 *
 * \param [in] fd File descriptor
 * \param
 * \param
 * \return
 *
 */
uint8_t PDF_AddText(FILE *fd, uint16_t x, uint16_t y, char *text)
{
  char str[PDF_BLOCK_SIZE*2];
  char str2[PDF_BLOCK_SIZE*2];
  uint16_t len;
  uint32_t pos = PDF_WR_ftell(fd);

  if (PDF_CurrObject>=PDF_MAX_NUM)
    return PDF_ERR_MAXNUM;

  if ((pos - PDF_XrefPos) > PDF_MAX_BLOCKLEN)
    return PDF_ERR_LONGBLOCK;

  PDF_XrefTable[PDF_CurrObject] = (uint16_t)(pos - PDF_XrefPos);
  PDF_XrefPos = pos;
  sprintf(str, PDF_TEXT_START, 12, x, PDF_PAGE_HEIGHT - y);
  len = strlen(str);
  len += strlen(text);
  len += strlen(PDF_TEXT_END);
  sprintf(str2, PDF_STREAM_OBJ_START, PDF_CurrObject, len);
  if (!PDF_WR_fwrite(fd, str2, strlen(str2)))
    return PDF_ERR_FILE;
  len = PDF_PrepareString(str, str, false, PDF_CurrObject, true);
  if (!PDF_WR_fwrite(fd, str, len))
    return PDF_ERR_FILE;
  len = PDF_PrepareString(text, str, false, PDF_CurrObject, false);
  if (!PDF_WR_fwrite(fd, str, len))
    return PDF_ERR_FILE;
  len = PDF_PrepareString((char*)PDF_TEXT_END, str, false, PDF_CurrObject, false);
  if (!PDF_WR_fwrite(fd, str, len))
    return PDF_ERR_FILE;
  if (!PDF_WR_fwrite(fd, PDF_STREAM_OBJ_END, strlen(PDF_STREAM_OBJ_END)))
    return PDF_ERR_FILE;
  PDF_CurrObject++;

  return PDF_ERR_NONE;
}

/** \brief Finish PDF generation, close handlers, save tables and write end of the file
 *
 * \param [in] fd File descriptor
 * \return
 *
 */
uint8_t PDF_Finish(FILE *fd)
{
  char str[64];
  char str2[64];
  uint32_t len;
  uint32_t pos;

  /**< write last page if it's not stored yet */
  if (PDF_CurrObject>PDF_LastObject)
    PDF_WritePage(fd);
  /**< write pages list */
  PDF_XrefTable[PDF_OBJNUM_PAGES] = PDF_WR_ftell(fd);
  sprintf(str, PDF_PAGES_OBJ_START, PDF_PageNum);
  if (!PDF_WR_fwrite(fd, str, strlen(str)))
    return PDF_ERR_FILE;
  for (len=PDF_OBJNUM_LAST; len<=PDF_CurrObject; len++)
  {
    if (PDF_XrefTable[len] & PDF_BIT_PAGE)
    {
      PDF_XrefTable[len] &= ~PDF_BIT_PAGE;
      sprintf(str, PDF_CONT_FMT, len);
      if (!PDF_WR_fwrite(fd, str, strlen(str)))
        return PDF_ERR_FILE;
    }
  }
  if (!PDF_WR_fwrite(fd, PDF_PAGES_OBJ_END, strlen(PDF_PAGES_OBJ_END)))
    return PDF_ERR_FILE;
  /**< write xref table */
  //if (!PDF_WR_fwrite(fd, PDF_ENDLINE, 1, strlen(PDF_ENDLINE)))
  //  return PDF_ERR_FILE;
  //get XREF position
  pos = PDF_WR_ftell(fd);
  sprintf(str, PDF_XREF_START, PDF_CurrObject);
  if (!PDF_WR_fwrite(fd, str, strlen(str)))
    return PDF_ERR_FILE;
  //write first record
  if (!PDF_WR_fwrite(fd, PDF_XREF_FIRST, strlen(PDF_XREF_FIRST)))
    return PDF_ERR_FILE;
  //write all xref records from array
  PDF_XrefPos = 0;
  for (len=PDF_OBJNUM_ROOT; len<PDF_CurrObject; len++)
  {
    if (PDF_XrefTable[len]>0)
    {
      if (len < PDF_OBJNUM_LAST)
      {
        sprintf(str, PDF_XREF_RECORD, PDF_XrefTable[len], 'n');
      } else
      {
        PDF_XrefPos += PDF_XrefTable[len];
        sprintf(str, PDF_XREF_RECORD, PDF_XrefPos, 'n');
      }
    } else
    {
      sprintf(str, PDF_XREF_RECORD, 0, 'f');
    }
    if (!PDF_WR_fwrite(fd, str, strlen(str)))
      return PDF_ERR_FILE;
  }
  /**< write trailer */
  sprintf(str, PDF_TRAILER_START, PDF_CurrObject);
  if (!PDF_WR_fwrite(fd, str, strlen(str)))
    return PDF_ERR_FILE;
  #ifdef PDF_USE_ENCRYPT
    if (!PDF_WR_fwrite(fd, PDF_TRAILER_ENC, strlen(PDF_TRAILER_ENC)))
      return PDF_ERR_FILE;
  #endif // PDF_USE_ENCRYPT
  if (!PDF_WR_fwrite(fd, PDF_TRAILER_MID, strlen(PDF_TRAILER_MID)))
    return PDF_ERR_FILE;
  strcpy(str2, "<");
  for (len=0; len<PDF_ID_LEN; len++)
    strcat(str2, PDF_ByteToHex(PDF_EncryptRec.encrypt_id[len]));
  strcat(str2, ">");
  if (!PDF_WR_fwrite(fd, str2, strlen(str2))) /* print ID twice! */
    return PDF_ERR_FILE;
  if (!PDF_WR_fwrite(fd, str2, strlen(str2)))
    return PDF_ERR_FILE;
  if (!PDF_WR_fwrite(fd, PDF_TRAILER_END, strlen(PDF_TRAILER_END)))
    return PDF_ERR_FILE;
  /**< write xref start position */
  sprintf(str, PDF_XREF_END, pos);
  if (!PDF_WR_fwrite(fd, str, strlen(str)))
    return PDF_ERR_FILE;
  /**< write ending of the file */
  if (!PDF_WR_fwrite(fd, PDF_EOF, strlen(PDF_EOF)))
    return PDF_ERR_FILE;

  /**< close PDF file */
  PDF_WR_fclose(fd);

  return PDF_ERR_NONE;
}
