#include <stdio.h>
#include <stdlib.h>
#include "pdf.h"

int main()
{
  printf("Hello world!\n");
  if (PDF_Start("e:\\test.pdf", "Test PDF", "Erweka") == PDF_ERR_NONE)
  {
    PDF_SetFont(PDF_FONT_COURIER_BOLD);
    PDF_AddFrameToHeader(50, 10, 515, 90, 0xC0C0C0, true);
    PDF_AddFrameToHeader(50, 10, 515, 90, 0x0000FF, false);
    PDF_AddTextToHeader(270, 25, 16, "Test PDF");
    PDF_AddPage(true);
    PDF_SetFont(PDF_FONT_TIMES);
    PDF_AddText(100, 120, 14, "Test Test Test Test Test Test Test Test 1");
    PDF_SetFont(PDF_FONT_COURIER_ITALIC);
    PDF_AddText(100, 150, 14, "Test (2");
    PDF_AddText(100, 180, 14, "Test 3");
    PDF_AddText(100, 210, 14, "Test (4");
    PDF_AddText(100, 240, 14, "Test 5");
    PDF_AddText(100, 270, 14, "Test \\6");
    PDF_AddText(100, 300, 14, "Test 7");
    PDF_AddText(100, 330, 14, "Test 8");
    PDF_AddText(100, 360, 14, "Test 9");
    PDF_AddText(100, 390, 14, "Test 10");
    //PDF_AddStream((char*)PDF_IMAGE_INSERT);
    PDF_AddPage(true);
    PDF_SetColor(0xFF8000);
    PDF_AddText(100, 120, 18, "Test 2");
    PDF_Finish();
  } else
  {
    printf("Error opening file!\n");
  }
  return 0;
}
