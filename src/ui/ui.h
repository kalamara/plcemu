#ifndef _UI_H_
#define _UI_H_

/**
 *@file ui.h
 *@brief main user interface header
*/

typedef enum 
{/// CLI schema
    CLI_COM,//commands always go to first payload entry
    CLI_ARG, //optional arguments start at second payload entry
    N_PAYLOADS
} CLI_PAYLOADS;

typedef enum
{///CLI commands
    COM_NONE,
    COM_START,
    COM_STOP,
    COM_HELP,
    COM_FORCE,
    COM_UNFORCE,
    COM_EDIT,
    COM_LOAD,
    COM_SAVE,
    COM_CONFIG, //TODO
    COM_QUIT,
    N_COM
} COMMANDS;

/*****************generic user interface*****************************************/

/**
 * @brief display a message to user
 * @param the message
 */
void ui_display_message(char *msgstr);

/**
 * @brief draw the plc state
 * @param the state
 */
void ui_draw(config_t state);

/**
 * @brief update ui and get the next command
 * @param the command configuration
 * @return next command
 */
config_t ui_update(config_t command);

/**
 * @brief initialize ui
 * @return OK or ERROR
 */
int ui_init();

/**
 * @brief cleanup
 */
void ui_end();

/**
 * @brief enable/disable ui 
 * @deprecated
 */
void ui_toggle_enabled();

/***************command line helpers******************************************/

/**
 * @brief initialize a command configuration
 * @param system configuration, carries logical block sequences
 * @return new command configuration
 */
config_t cli_init_command(config_t conf);

/**
 * @brief initialize plc state configuration
 * @param system configuration, carries logical block sequences
 * @return new plc state
 */
config_t cli_init_state(config_t conf);

/**
 * @brief print a header with time and version
 */
void cli_header();

/**
 * @brief parse CLI input and retrieve command
 * @param console input string
 * @param the command configuration
 * @return next command
 */
config_t cli_parse( char * input, config_t command);

#endif //UI_H
