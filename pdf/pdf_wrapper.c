#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "pdf_wrapper.h"

/** \brief
 *
 * \param
 * \param
 * \return
 *
 */
uint32_t PDF_WR_rand(void)
{
  return rand();
}

/** \brief
 *
 * \param
 * \param
 * \return
 *
 */
void PDF_WR_srand(void)
{
  srand(time(0));
}

/** \brief
 *
 * \return void
 *
 */
void PDF_WR_fopen()
{

}

/** \brief
 *
 * \param
 * \param
 * \return
 *
 */
bool PDF_WR_fwrite(FILE* fd, const void* buf, uint16_t len)
{
  uint32_t bw;

  bw = fwrite(buf, 1, len, fd);

  return (bw==len);
}

/** \brief
 *
 * \param
 * \param
 * \return
 *
 */
uint32_t PDF_WR_ftell(FILE* fd)
{
  return ftell(fd);
}

/** \brief Close file
 *
 * \param [in] fd File descriptor
 * \return
 *
 */
void PDF_WR_fclose(FILE* fd)
{
  fclose(fd);
}

/** \brief
 *
 * \param
 * \param
 * \return
 *
 */
void PDF_WR_gettime()
{
  time_t now;
  struct tm *ptr;

  time(&now);
  ptr = localtime(&now);
}
