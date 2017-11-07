#ifndef _MD5_H
#define _MD5_H

#include <stdint.h>

#define MD5_KEY_LEN         (16)

typedef struct
{
  uint32_t buf[4];
  uint32_t bits[2];
  uint8_t in[64];
} TMD5Context;

// The four core functions - F1 is optimized somewhat
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))
// This is the central step in the HPDF_MD5 algorithm
#define MD5STEP(f, w, x, y, z, data, s) \
 ( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )

void MD5_Init(TMD5Context *ctx);
void MD5_Update(TMD5Context *ctx, uint8_t *buf, uint32_t len);
void MD5_Final(uint8_t digest[16], TMD5Context *ctx);

#endif
