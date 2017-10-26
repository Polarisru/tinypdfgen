# TinyPDFGen

## Minimal example

```C
#include <stdio.h>
#include <stdlib.h>
#include "pdf.h"

int main()
{
  FILE *fd = NULL;

  fd = PDF_Start("e:\\test.pdf", "PDF Title", "PDF Author");
  if (fd!=NULL)
  {
    PDF_AddPage(fd);
    PDF_AddText(fd, 300, 20, "Test PDF");
    PDF_AddText(fd, 100, 50, "Test 1");
    PDF_AddText(fd, 100, 80, "Test 2");
    PDF_AddPage(fd);
    PDF_AddText(fd, 300, 20, "Test PDF 2");
    PDF_Finish(fd);
  } else
  {
    printf("Error opening file!\n");
  }
  return 0;
}
```

## TinyPDFGen Roadmap
- [x] generate simple multipage document with/without encryption
- [x] add PDF_ENCRYPT_R3 support
- [ ] add possibility to use different not embedded fonts
- [ ] add embedded font
- [ ] add headers/footers
- [ ] write documentation
- [ ] write FatFS wrapper
