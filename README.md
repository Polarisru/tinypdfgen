# TinyPDFGen

## Minimal example

```C
#include <stdio.h>
#include <stdlib.h>
#include "pdf.h"

int main()
{
  fd = ;
  if (PDF_Start("e:\\test.pdf", "PDF Title", "PDF Author") == PDF_ERR_NONE)
  {
    PDF_AddPage(false);
    PDF_AddText(300, 20, "Test PDF");
    PDF_AddText(100, 50, "Test 1");
    PDF_AddText(100, 80, "Test 2");
    PDF_AddPage(false);
    PDF_AddText(300, 20, "Test PDF 2");
    PDF_Finish();
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
- [x] add headers/footers
- [x] different text colors
- [x] add possibility to use different not embedded fonts
- [x] add graphical objects: frames (streams with no text formatting)
- [ ] add possibility to use images
- [ ] write documentation
- [ ] write FatFS wrapper
### Not really usefull
- [ ] add embedded font (because of big size - whole font should be stored)