#ifndef _PDF_WRAPPER_H
#define _PDF_WRAPPER_H

#include <stdint.h>
#include <stdbool.h>

#define hFile     FILE*

extern uint32_t PDF_WR_rand(void);
extern void PDF_WR_srand(void);
extern hFile PDF_WR_fopen(char *name);
extern bool PDF_WR_fwrite(hFile fd, const void* buf, uint16_t len);
extern uint32_t PDF_WR_ftell(hFile fd);
extern void PDF_WR_fclose(hFile fd);

#endif // _PDF_WRAPPER_H
