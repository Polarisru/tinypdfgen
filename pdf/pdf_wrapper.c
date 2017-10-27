#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "pdf_wrapper.h"

/** \brief Ger random number
 *
 * \return Random number
 *
 */
uint32_t PDF_WR_rand(void)
{
  return rand();
}

/** \brief Initialize random number generator
 *
 * \return
 *
 */
void PDF_WR_srand(void)
{
  srand(time(0));
}

/** \brief Open file by name
 *
 * \param [in] name Name of the file
 * \return void
 *
 */
hFile PDF_WR_fopen(char *name)
{
  return fopen(name, "wb");
}

/** \brief
 *
 * \param
 * \param
 * \return
 *
 */
bool PDF_WR_fwrite(hFile fd, const void* buf, uint16_t len)
{
  uint32_t bw;

  bw = fwrite(buf, 1, len, fd);

  return (bw==len);
}

/** \brief Read current position of the file
 *
 * \param [in] fd File handler
 * \return Current position in the file
 *
 */
uint32_t PDF_WR_ftell(hFile fd)
{
  return ftell(fd);
}

/** \brief Close file
 *
 * \param [in] fd File descriptor
 * \return
 *
 */
void PDF_WR_fclose(hFile fd)
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
void PDF_WR_gettime(PdfTime *dt)
{
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  dt->year = tm.tm_year + 1900;
  dt->month = tm.tm_mon + 1;
  dt->day = tm.tm_mday;
  dt->hour = tm.tm_hour;
  dt->min = tm.tm_min;
  dt->sec = tm.tm_sec;
}
