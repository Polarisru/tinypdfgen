#ifndef _ARC4_H
#define _ARC4_H

#include <stdint.h>

#define ARC4_BUF_SIZE       (256)

typedef struct
{
  uint8_t idx1;
  uint8_t idx2;
  uint8_t state[ARC4_BUF_SIZE];
} ARC4_Ctx_Rec;

void ARC4_Init(ARC4_Ctx_Rec *ctx, uint8_t *key, uint8_t key_len);
void ARC4_CryptBuf(ARC4_Ctx_Rec *ctx, uint8_t *in, uint8_t *out, uint16_t len);

#endif
