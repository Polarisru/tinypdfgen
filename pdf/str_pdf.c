#include "str_pdf.h"

const char PDF_HEADER[] = "%PDF-1.4\n%\xE2\xE3\xD2\xD3\n\n";
const char PDF_FIRST_OBJECT[] = "1 0 obj\n<< /Type /Catalog /Pages 2 0 R>>\nendobj\n";
const char PDF_PAGES_OBJ_START[] = "2 0 obj\n<< /Type /Pages /Count %d /Kids [";
const char PDF_PAGES_OBJ_END[] = "] /MediaBox [0 0 595 842]>>\nendobj\n";
//const char PDF_RESOURCE_OBJECT[] = "3 0 obj\n<</Font <</F1 4 0 R /F2 5 0 R>>>>\nendobj\n";
const char PDF_RESOURCE_OBJECT[] = "3 0 obj\n<</Font <</F1 4 0 R>>>>\nendobj\n";
const char PDF_FONT1_OBJECT[] = "4 0 obj\n<</Type /Font /Subtype /Type1 /BaseFont /Courier>>\nendobj\n";
const char PDF_FONT2_OBJECT[] = "5 0 obj\n<</Type /Font /Subtype /Type1 /BaseFont /Courier-Bold>>\nendobj\n";

//const char PDF_FONT1_DEF[] = "4 0 obj\n<</Type/Font/Subtype/TrueType/Name/F1/BaseFont/ABCDEE+Courier#20New#20Erweka/Encoding/WinAnsiEncoding/FontDescriptor 9 0 R/FirstChar 32/LastChar 255/Widths 10 0 R>>\nendobj\n";
const char PDF_FONT1_DEF[] = "4 0 obj\n<</Type /Font /Subtype /TrueType /BaseFont /BAAAAA+CourierErweka /Encoding /WinAnsiEncoding /FontDescriptor 9 0 R /FirstChar 32 /LastChar 255 /MissingWidth 600 /Widths 10 0 R>>\nendobj\n";
//const char PDF_FONT1_DESC[] = "9 0 obj\n<</Type/FontDescriptor/FontName/ABCDEE+Courier#20New#20Erweka/Flags 32/ItalicAngle 0/Ascent 833/Descent -188/CapHeight 613/AvgWidth 600/MaxWidth 626/FontWeight 400/XHeight 250/StemV 60/FontBBox[ -13 -188 613 613]/FontFile2 11 0 R>>\nendobj\n";
const char PDF_FONT1_DESC[] = "9 0 obj\n<</Type /FontDescriptor /Ascent 833 /Descent -188 /Flags 32 /FontBBox[ -13 -188 613 613] /FontName /BAAAAA+CourierErweka /ItalicAngle 0 /StemV 60 /XHeight 250 /FontFile2 11 0 R>>\nendobj\n";
//const char PDF_FONT1_DESC[] = "9 0 obj\n<</Type/FontDescriptor/FontName/BAAAAA+CourierNewErweka/Flags 5/FontBBox[-13 -273 613 783]/ItalicAngle 0/Ascent 832/Descent -300/CapHeight 782/StemV 80/FontFile2 11 0 R>>\nendobj\n";
const char PDF_FONT1_WIDTH_START[] = "10 0 obj\n[";
const char PDF_FONT1_WIDTH_CONT[] = " 600 600 600 600 600 600 600 600 600 600 600 600 600 600 600 600";
const char PDF_FONT1_WIDTH_END[] = "]\nendobj\n";;

//const char PDF_STREAM_OBJECT[] = "%d 0 obj\n<< /Length %d >>\nstream\n%s\nendstream\nendobj\n";
const char PDF_PAGE_OBJ_START[] = "%d 0 obj\n<< /Type /Page /Parent 2 0 R /Resources 3 0 R /Contents [";
const char PDF_PAGE_OBJ_END[] = "]>>\nendobj\n";
const char PDF_ENC_OBJ_START[] = "6 0 obj\n<</Filter /Standard /V 1 /R 2 /Length 40 /U(";
const char PDF_ENC_OBJ_MID[] = ") /O(";
const char PDF_ENC_OBJ_END[] = ") /P %d>>\nendobj\n";
const char PDF_INFO_OBJ_1[] = "7 0 obj\n<</Title (";
const char PDF_INFO_OBJ_2[] = ") /Author (";
const char PDF_INFO_OBJ_3[] = ") /Creator (";
const char PDF_INFO_OBJ_4[] = ") /CreationDate (";
const char PDF_INFO_OBJ_5[] = ")>>\nendobj\n";
const char PDF_TRAILER[] = "trailer <</Size %d /Root 1 0 R /Info 7 0 R /ID[<%s><%s>]>>\n";
const char PDF_TRAILER_ENC[] = "trailer <</Size %d /Root 1 0 R /Info 7 0 R /Encrypt 6 0 R /ID [<%s><%s>] >>\n";
const char PDF_EOF[] = "%%EOF\n";
const char PDF_TEXT_START[] = "BT /F1 %d Tf %d %d Td (";
const char PDF_TEXT_END[] = ") Tj ET\n";
const char PDF_STREAM_OBJ_START[] = "%d 0 obj\n<< /Length %d >>\nstream\n";
const char PDF_STREAM_FONTOBJ_START[] = "%d 0 obj\n<< /Length %d /Length1 %d /Length2 0 /Length3 0 >>\nstream\n";
const char PDF_STREAM_OBJ_END[] = "endstream\nendobj\n";
const char PDF_STREAM_OBJ_END2[] = "\nendstream\nendobj\n";
//const char PDF_BLOCK_HEADER1[] = "BT /F2 14 Tf 250 790 Td (Audit protocol) Tj ET\n";
//const char PDF_BLOCK_HEADER1[] = "BT /F2 13 Tf 250 790 Td (%s) Tj ET\n";
const char PDF_BLOCK_HEADER1[] = "BT 2 Tr /F1 13 Tf 250 790 Td (%s) Tj 0 Tr ET\n";
const char PDF_BLOCK_HEADER2[] = "BT /F1 13 Tf 450 790 Td (Page    / %d) Tj ET\n";
const char PDF_BLOCK_HEADER3[] = "BT /F1 13 Tf 70 765 Td (Printed by: %s) Tj ET BT 330 765 Td (Printed at: %s) Tj ET\n";
const char PDF_BLOCK_HEADER4[] = "BT /F1 13 Tf 70 740 Td (Device S/N: %s) Tj ET BT 330 740 Td (Firmware version: %s) Tj ET\n";
const char PDF_BLOCK_HEADER5[] = "50 725 515 90 re 50 35 m 565 35 l h S\n";
const char PDF_NUMPAGE_TEXT[] = "BT /F1 14 Tf 475 790 Td (%4d) Tj ET\n";

const char PDF_XREF_START[] = "xref\n0 %d\n";
const char PDF_XREF_FIRST[] = "0000000000 65535 f \n";
const char PDF_XREF_RECORD[] = "%010d 00000 %c \n";
const char PDF_XREF_END[] = "startxref\n%d\n";
const char PDF_CONT_FMT[] = "%d 0 R ";

const char PDF_AUDIT_TITLE[] = "Audit protocol";
const char PDF_TEST_TITLE[] = "Test protocol";
const char PDF_TEXT_CREATOR[] = "Erweka PDF";
const char PDF_DATE_FMT[] = "%4d%02d%02d%02d%02d%02dZ";
const char PDF_OWNER_PASS[] = "ERWEKA911";

const char PDF_ENDLINE[] = "\n";

const uint8_t PDF_DUMMY_ID[] = {0xc5, 0x27, 0x49, 0x0a, 0x70, 0x6a, 0x90, 0x91, 0x07, 0xa9, 0xce, 0xee, 0xdf, 0x94, 0x97, 0xb3};

const uint8_t PDF_PADDING_STRING[] = {
    0x28, 0xBF, 0x4E, 0x5E, 0x4E, 0x75, 0x8A, 0x41,
    0x64, 0x00, 0x4E, 0x56, 0xFF, 0xFA, 0x01, 0x08,
    0x2E, 0x2E, 0x00, 0xB6, 0xD0, 0x68, 0x3E, 0x80,
    0x2F, 0x0C, 0xA9, 0xFE, 0x64, 0x53, 0x69, 0x7A
};
