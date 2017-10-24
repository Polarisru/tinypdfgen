#ifndef _PDF_WRAPPER_H
#define _PDF_WRAPPER_H

#include <stdlib.h>
#include <time.h>

#define PDF_RAND                  rand()
#define PDF_SRAND                 srand(time(0))
#define PDF_WRITE(fd, buf, size)  fwrite(buf, 1, size, fd)

#endif // _PDF_WRAPPER_H
