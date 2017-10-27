#include <stdio.h>
#include <stdlib.h>
#include "pdf.h"

int main()
{
  printf("Hello world!\n");
  if (PDF_Start("e:\\test2.pdf", "Test (PDF", "Erweka") == PDF_ERR_NONE)
  {
    PDF_AddHeader(300, 20, "Test PDF");
    PDF_AddPage(true);
    PDF_AddText(100, 50, "Test Test Test Test Test Test Test Test 1");
    PDF_AddText(100, 80, "Test 2");
    PDF_AddText(100, 110, "Test 3");
    PDF_AddText(100, 140, "Test 4");
    PDF_AddText(100, 170, "Test 5");
    PDF_AddText(100, 200, "Test 6");
    PDF_AddText(100, 230, "Test 7");
    PDF_AddText(100, 260, "Test 8");
    PDF_AddText(100, 290, "Test 9");
    PDF_AddText(100, 320, "Test 10");
    PDF_AddStream(PDF_IMAGE_INSERT);
    PDF_AddPage(true);
    PDF_AddText(100, 80, "Test 2");
    PDF_Finish();
  } else
  {
    printf("Error opening file!\n");
  }
  return 0;
}
