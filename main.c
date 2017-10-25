#include <stdio.h>
#include <stdlib.h>
#include "pdf.h"

int main()
{
  FILE *fd = NULL;
  printf("Hello world!\n");
  fd = PDF_Start("e:\\test2.pdf", "Test (PDF", "Erweka");
  if (fd!=NULL)
  {
    PDF_AddPage(fd);
    PDF_AddText(fd, 300, 20, "Test PDF");
    PDF_AddText(fd, 100, 50, "Test 1");
    PDF_AddText(fd, 100, 80, "Test 2");
    PDF_AddText(fd, 100, 110, "Test 3");
    PDF_AddText(fd, 100, 140, "Test 4");
    PDF_AddText(fd, 100, 170, "Test 5");
    PDF_AddText(fd, 100, 200, "Test 6");
    PDF_AddText(fd, 100, 230, "Test 7");
    PDF_AddText(fd, 100, 260, "Test 8");
    PDF_AddText(fd, 100, 290, "Test 9");
    PDF_AddText(fd, 100, 320, "Test 10");
    PDF_AddPage(fd);
    PDF_AddText(fd, 300, 20, "Test PDF 2");
    PDF_Finish(fd);
  } else
  {
    printf("Error opening file!\n");
  }
  return 0;
}
