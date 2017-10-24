#ifndef _STR_PDF_H
#define _STR_PDF_H

#include <stdint.h>

extern const char PDF_HEADER[];
extern const char PDF_FIRST_OBJECT[];
extern const char PDF_PAGES_OBJ_START[];
extern const char PDF_PAGES_OBJ_END[];
extern const char PDF_RESOURCE_OBJECT[];
extern const char PDF_FONT1_OBJECT[];
extern const char PDF_FONT2_OBJECT[];

extern const char PDF_FONT1_DEF[];
extern const char PDF_FONT1_DESC[];
extern const char PDF_FONT1_WIDTH_START[];
extern const char PDF_FONT1_WIDTH_CONT[];
extern const char PDF_FONT1_WIDTH_END[];

//const char PDF_STREAM_OBJECT[];
extern const char PDF_PAGE_OBJ_START[];
extern const char PDF_PAGE_OBJ_END[];
extern const char PDF_ENC_OBJ_START[];
extern const char PDF_ENC_OBJ_MID[];
extern const char PDF_ENC_OBJ_END[];
extern const char PDF_INFO_OBJ_1[];
extern const char PDF_INFO_OBJ_2[];
extern const char PDF_INFO_OBJ_3[];
extern const char PDF_INFO_OBJ_4[];
extern const char PDF_INFO_OBJ_5[];
extern const char PDF_TRAILER_START[];
extern const char PDF_TRAILER_MID[];
extern const char PDF_TRAILER_END[];
extern const char PDF_TRAILER_ENC[];
extern const char PDF_EOF[];
extern const char PDF_TEXT_START[];
extern const char PDF_TEXT_END[];
extern const char PDF_STREAM_OBJ_START[];
extern const char PDF_STREAM_FONTOBJ_START[];
extern const char PDF_STREAM_OBJ_END[];
extern const char PDF_STREAM_OBJ_END2[];
//const char PDF_BLOCK_HEADER1[];
extern const char PDF_BLOCK_HEADER1[];
extern const char PDF_BLOCK_HEADER2[];
extern const char PDF_BLOCK_HEADER3[];
extern const char PDF_BLOCK_HEADER4[];
extern const char PDF_BLOCK_HEADER5[];
extern const char PDF_NUMPAGE_TEXT[];

extern const char PDF_XREF_START[];
extern const char PDF_XREF_FIRST[];
extern const char PDF_XREF_RECORD[];
extern const char PDF_XREF_END[];
extern const char PDF_CONT_FMT[];
extern const char PDF_DATE_FMT[];

extern const char PDF_ENDLINE[];

extern const uint8_t PDF_DUMMY_ID[];

extern const uint8_t PDF_PADDING_STRING[];

#endif
