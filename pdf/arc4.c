#include "arc4.h"

//------------------------------------------------------------------------------
// ARC4_Init - initialize ARC4 encryption
//------------------------------------------------------------------------------
void ARC4_Init(ARC4_Ctx_Rec *ctx, uint8_t *key, uint8_t key_len)
{
  uint8_t tmp_array[ARC4_BUF_SIZE];
  uint32_t i;
  uint32_t j = 0;
  uint8_t tmp;

  for (i = 0; i < ARC4_BUF_SIZE; i++)
    ctx->state[i] = (uint8_t)i;

  for (i = 0; i < ARC4_BUF_SIZE; i++)
    tmp_array[i] = key[i % key_len];

  for (i = 0; i < ARC4_BUF_SIZE; i++) 
  {
    j = (j + ctx->state[i] + tmp_array[i]) % ARC4_BUF_SIZE;

    tmp = ctx->state[i];
    ctx->state[i] = ctx->state[j];
    ctx->state[j] = tmp;
  }

  ctx->idx1 = 0;
  ctx->idx2 = 0;
}

//------------------------------------------------------------------------------
// ARC4_CryptBuf - encrypt data buffer
//------------------------------------------------------------------------------
void ARC4_CryptBuf(ARC4_Ctx_Rec *ctx, uint8_t *in, uint8_t *out, uint16_t len)
{
  uint32_t i;
  uint32_t t;
  uint8_t K;
  uint8_t tmp;

  for (i = 0; i < len; i++) 
	{
    ctx->idx1 = (uint8_t)((ctx->idx1 + 1) % 256);
    ctx->idx2 = (uint8_t)((ctx->idx2 +  ctx->state[ctx->idx1]) % 256);

    tmp = ctx->state[ctx->idx1];
    ctx->state[ctx->idx1] = ctx->state[ctx->idx2];
    ctx->state[ctx->idx2] = tmp;

    t = (ctx->state[ctx->idx1] + ctx->state[ctx->idx2]) % 256;
    K = ctx->state[t];

    out[i] = (uint8_t)(in[i] ^ K);
  }
}
