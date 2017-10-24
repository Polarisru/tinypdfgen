#include <stdio.h>
#include <stdlib.h>
#include "pdf.h"

int main()
{
  FILE *fd = NULL;
  printf("Hello world!\n");
  fd = PDF_Start("e:\\test.pdf", "Test PDF", "Erweka");
  PDF_AddPage(fd);
  PDF_AddText(fd, 200, 10, "Test PDF");
  PDF_Finish(fd);
  return 0;
}
