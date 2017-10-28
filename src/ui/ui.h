#ifndef _UI_H_
#define _UI_H_

/**
 *@file ui.h
 *@brief main header
*/
#define CLI_COM 0 //commands always go to first payload entry

enum
{
    PAGE_MAIN = 1,
    PAGE_EXIT,
    PAGE_EDIT,
    PAGE_FILE,
    PAGE_EDITMODE,
    PAGE_HELP,
    N_PAGES
}PAGES;

typedef enum
{///winows
    WIN_DI,        ///digital input
    WIN_AI,        ///analog input
    WIN_DQ,        ///digital output
    WIN_AQ,        ///analog output
    WIN_MEMORY,   ///pulse of counter
    WIN_REAL,     ///floating point variable
    WIN_TIMER,     ///output of timer
    WIN_BLINKER,   ///output of blinker
    WIN_PROGRAM,
    N_WIN
}WINDOWS;


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
    COM_QUIT,
    N_COM
} COMMANDS;

extern plc_t Plc;
extern char Lines[MAXBUF][MAXSTR];///ladder lines
extern int Lineno;	///actual no of active lines

config_t ui_init_command();
config_t ui_init_state();

void ui_display_message(char *msgstr);
void ui_draw(config_t state);
config_t ui_update(config_t command);
int ui_init();
void ui_end();
void ui_toggle_enabled();

#endif //UI_H
