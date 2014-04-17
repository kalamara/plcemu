#ifndef _HARDWARE_H_
#define _HARDWARE_H_

#ifdef USE_COMEDI
    #include <comedilib.h>
#else
	#include <sys/io.h>
#endif

/**
 * @brief Enable bus communication
 * @return error code
 */
int enable_bus();

/**
 * @brief isable bus communication
 * @return error code
 */
int disable_bus();

/**
 * @brief read digital input
 * @param index
 * @param value
 */
void dio_read(const int index ,unsigned char* value);

/**
 * @brief write bit to digital output
 * @param value
 * @param n index
 * @param bit
 */
void dio_write(const unsigned char *value, const int n, const int bit);

/**
 * @brief read / write binary mask to digital input / output
 * @param mask
 * @param bits
 */
void dio_bitfield(const unsigned char* mask, unsigned char *bits);

#endif //_HARDWARE_H_
