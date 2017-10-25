#ifndef _PDF_WRAPPER_H
#define _PDF_WRAPPER_H

#include <stdint.h>
#include <stdbool.h>

extern uint32_t PDF_WR_rand(void);
extern void PDF_WR_srand(void);
extern bool PDF_WR_fwrite(FILE* fd, const void* buf, uint16_t len);
extern uint32_t PDF_WR_ftell(FILE* fd);
extern void PDF_WR_fclose(FILE* fd);

#endif // _PDF_WRAPPER_H
