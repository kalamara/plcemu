#ifndef _APP_H_
#define _APP_H_
/**
 *@file app.h
 *@brief app function definitions header
*/

/**
 *@brief apply configuration to plc
 *@param plc
 *@param configuration
 *@return configured plc 
 */
plc_t configure(const config_t conf, plc_t plc);

/**
 *@brief get plc state in serializable form
 *@param plc
 *@param current state
 *@return updated state 
 */
config_t get_state(const plc_t plc, 
                   const config_t state);

/**
 *@brief apply a command
 *@param ui command in serializable form
 *@param PLC configuration
 *@param the PLC
 *@return updated PLC
 */                   
plc_t apply_command(const config_t com, 
                    config_t * conf,
                    plc_t plc);
#endif //_APP_H_
