#ifndef _INIT_H_
#define _INIT_H_


/**
 * @brief extract alphanumeric variable name from file line starting
 * at index start
 * @in line
 * @out name
 * @param start
 * @return index position, or error
 */
int extract_name(const char *line, char *name, int start );

/**
 * @brief extract numeric index from file line starting
 * at index start
 * @in line
 * @out idx
 * @param start
 * @return index position, or error
 */
int extract_index(const char *line, int *idx, int start);

/**
 * @brief extract configuration value from file line starting
 * at index start, drop comments
 * @in line
 * @out val
 * @param start
 * @return index position, or error
 */
int extract_value( const char *line, char *val, int start);
//tech debt


#endif //_INIT_H_

