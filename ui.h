#ifndef _UI_H_
#define _UI_H_
#include "plclib.h"

/**
 *@file ui.h
 *@brief main header
*/

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

enum
{///winows
    WIN_DI,        ///digital input
    WIN_DQ,        ///digital output
    WIN_COUNTER,   ///pulse of counter
    WIN_TIMER,     ///output of timer
    WIN_BLINKER,   ///output of blinker
    N_WIN
}WINDOWS;

void ui_display_message(char *msgstr);
void ui_draw();
int ui_update(int page);
int ui_init();
void ui_end();
void ui_toggle_enabled();

#endif //UI_H
