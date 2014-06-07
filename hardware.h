#ifndef _HARDWARE_H_
#define _HARDWARE_H_
/**
 *@file hardware.h
 *@brief hardware access functions
*/

/**
 * @brief Enable bus communication
 * @return error code
 */
int enable_bus();

/**
 * @brief Disable bus communication
 * @return error code
 */
int disable_bus();

/**
 * @brief fetch all input bytes if necessary
 * @return error code
 */
int dio_fetch(long timeout);

/**
 * @brief flush all output bytes if necessary
 * @return error code
 */
int dio_flush();


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
