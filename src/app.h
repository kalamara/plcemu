#ifndef _APP_H_
#define _APP_H_
/**
 *@file app.h
 *@brief app function definitions header
*/

typedef struct app {
    plc_t plc;
    config_t conf;
} * app_t;

config_t init_config();

/**
 *@brief apply configuration to application
 *@param configuration
 *@param the application
 *@return configured app
 */
app_t configure(const config_t conf, app_t app);

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
 *@param app configuration
 *@param the emulator app
 *@return updated app
 */                   
app_t apply_command(const config_t com, 
                    app_t a);
#endif //_APP_H_
