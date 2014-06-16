#include <fcntl.h>
#include <sys/stat.h>
#include <curses.h>
#include <time.h>
#include "wedit.h"
#include "greek.h"
#include "plcemu.h"
#include "ui.h"

/*************GLOBALS************************************************/

int Cur=0;
int PageLen=TERMLEN;
int InWin, OutWin, TimWin, BlinkWin, MVarWin, EditWin, ConfWin, LdWin, FileWin, HelpWin;
int InWinBuf, OutWinBuf, TimWinBuf, BlinkWinBuf, MVarWinBuf, EditWinBuf, ConfWinBuf, LdWinBuf, FileWinBuf, HelpWinBuf;
//int Update=FALSE;
int Enable=TRUE;

int WinFlag = FALSE; //0->inputs, 1->outputs 2->mvars, 3->timers, 4->blinkers
int SaveFlag = 0; //0 loads, 1 saves

void ui_display_message(char * msgstr)
{
    draw_info_line(PageLen + 1, msgstr);
}

void init_help()
{
	FILE * f;
	char line[MAXSTR], helpline[MAXSTR];
	buf_clear(HelpWinBuf);
    if ((f = fopen("./help", "r")))
	{
		while (fgets(line, MEDSTR, f))
		{ //read help file
			sprintf(helpline, " %s", line);
			app_line(HelpWinBuf, helpline);
		}
		fclose(f);
	}
}

void time_header()
{
	char t[TINYSTR], *p;
	char str[SMALLSTR];
	time_t now;

	time(&now);
	strcpy(t, ctime(&now));
	t[19] = '\0';
	p = t + 10;
    sprintf(str, " PLC-EMUlator v%4.2f %14s ", PRINTABLE_VERSION, p);
	draw_header(str);
}

void load_timers()
{
	int i;
	char s[32], color;
	buf_clear(TimWinBuf);
	for (i = 0; i < Nt; i++)
	{
		if (plc.t[i].Q)
			color = '.'; //red
		else
			color = '#'; //green
        sprintf(s, "%cT%dx%ld\t %ld %ld %s", color, i, plc.t[i].S, plc.t[i].V,
				plc.t[i].P, plc.t[i].nick);
		//printf("%s\n",s);
		app_line(TimWinBuf, s);
	}
}

void load_blinkers()
{
	int i;
	char s[32], color;
	buf_clear(BlinkWinBuf);
	for (i = 0; i < Ns; i++)
	{
		if (plc.s[i].Q)
			color = '.'; //red
		else
			color = '#'; //green
        sprintf(s, "%cS%dx%ld\t %s", color, i, plc.s[i].S, plc.s[i].nick);
		//printf("%s\n",s);
		app_line(BlinkWinBuf, s);
	}
}

void load_mvars()
{
	int i;
    char s[TINYSTR], color;
	buf_clear(MVarWinBuf);
	for (i = 0; i < Nm; i++)
	{
		if (plc.m[i].PULSE)
			color = '#'; //green
		else if (plc.m[i].RO) //locked
			color = '.'; //red
		else
			color = ' ';
        sprintf(s, "%cM%d.\t %ld %s", color, i, plc.m[i].V, plc.m[i].nick);
		//printf("%s\n",s);
		app_line(MVarWinBuf, s);
	}
}

void load_inputs()
{
	int i;
    char s[TINYSTR], color, bit;
	buf_clear(InWinBuf);
	for (i = 0; i < BYTESIZE * Di; i++)
	{
		if (!(((plc.maskin[i / BYTESIZE]) >> (i % BYTESIZE)) % 2)
				&& !(((plc.maskin_N[i / BYTESIZE]) >> (i % BYTESIZE)) % 2))
		{

			if (((plc.inputs[i / BYTESIZE]) >> (i % BYTESIZE)) % 2)
			{
				color = '#'; //green
				bit = '1';
			}
			else
			{
				color = ' ';
				bit = '0';
			}
		}
		else
		{
			color = '.'; //red
			if (((plc.maskin[i / BYTESIZE]) >> (i % BYTESIZE)) % 2)
				bit = '1';
			else if (((plc.maskin_N[i / BYTESIZE]) >> (i % BYTESIZE)) % 2)
				bit = '0';
		}
		sprintf(s, "%cI%d.\t %c %s", color, i, bit, plc.di[i].nick);
		//printf("%s\n",s);
		app_line(InWinBuf, s);
	}
}

void load_outputs()
{
	int i;
    char s[TINYSTR], color, bit;
	buf_clear(OutWinBuf);
	for (i = 0; i < BYTESIZE * Dq; i++)
	{
		if (!(((plc.maskout[i / BYTESIZE]) >> (i % BYTESIZE)) % 2)
				&& !(((plc.maskout_N[i / BYTESIZE]) >> (i % BYTESIZE)) % 2))
		{

			if (((plc.outputs[i / BYTESIZE]) >> (i % BYTESIZE)) % 2)
			{
				bit = '1';
				color = '#'; //green
			}
			else
			{
				color = ' ';
				bit = '0';
			}
		}
		else
		{
			color = '.'; //red
			if (((plc.maskout[i / BYTESIZE]) >> (i % BYTESIZE)) % 2)
				bit = '1';
			else if (((plc.maskout_N[i / BYTESIZE]) >> (i % BYTESIZE)) % 2)
				bit = '0';
		}
		sprintf(s, "%cQ%d.\t %c %s", color, i, bit, plc.dq[i].nick);
		//printf("%s\n",s);
		app_line(OutWinBuf, s);
	}
}

void load_ld()
{
	int i;
	buf_clear(LdWinBuf);
	for (i = 0; i < Lineno; i++)
		app_line(LdWinBuf, Lines[i]);

}

int main_page()
{
	static int redraw = TRUE;
    int c, i, ret;
    //, ch;
	i = 0;
	static int win_sticky;
    if(redraw)
        ui_draw();
    ret = PAGE_MAIN;
	if (Enable)
	{
        if (WinFlag == WIN_DI)
		{
			draw_info_line(1,
					"F1/2:Force 1/0|F3:Unforce|F4:Run/Stop|F5:Edit|F7:Load|F8:Save|F9:Help|F10:Quit");
			i = wselect(InWin, InWinBuf);
			win_set(InWin, i);
		}
        else if (WinFlag == WIN_DQ)
		{
			draw_info_line(1,
					"F1/2:Force 1/0|F3:Unforce|F4:Run/Stop|F5:Edit|F7:Load|F8:Save|F9:Help|F10:Quit");
			i = wselect(OutWin, OutWinBuf);
			win_set(OutWin, i);
		}
        else if (WinFlag == WIN_COUNTER)
		{
			draw_info_line(1,
					"F1/2:Toggle Pulse|F4:Run/Stop|F5:Edit|F6:Lock|F7:Load|F8:Save|F9:Help|F10:Quit");
			i = wselect(MVarWin, MVarWinBuf);
			win_set(MVarWin, i);
		}
        else if (WinFlag == WIN_TIMER)
		{
			draw_info_line(1,
					"F1:Start|F2:Pause|F3:Reset|F4:Run/Stop|F5:Edit|F7:Load|F8:Save|F9:Help|F10:Quit");
			i = wselect(TimWin, TimWinBuf);
			win_set(TimWin, i);
		}
        else if (WinFlag == WIN_BLINKER)
		{
			draw_info_line(1,
					"F4:Run/Stop|F5:Edit|F7:Load|F8:Save|F9:Help|F10:Quit");
			i = wselect(BlinkWin, BlinkWinBuf);
			win_set(BlinkWin, i);
		}
		else if (WinFlag == 5)
		{
			if (!plc.status % 2) //stopped
			{	//wedit
				draw_info_line(1,
						"F4:Run and execute |F7:Load|F8:Save|F9:Help|F10:Quit");
                return PAGE_EDITMODE;
			}
			else
			{
				draw_info_line(1,
						"F4:Stop and edit |F7:Load|F8:Save|F9:Help|F10:Quit");
				i = wselect(LdWin, LdWinBuf);
				win_set(LdWin, i);
			}
		}
		c = lastchar();
        redraw = TRUE;
		switch (c)
		{
		//arrows change window
		case KEY_RIGHT:
			if (WinFlag < 5)
				WinFlag++;
			break;
		case KEY_LEFT:
			if (WinFlag > 0)
				WinFlag--;
			break;
		case KEY_TAB:	//hotkey for edit window
			if (WinFlag < 5)
			{
				win_sticky = WinFlag;
				WinFlag = 5;
			}
			else
				WinFlag = win_sticky;
			break;

		case KEY_F(1):    //F1 forces 1
            if (WinFlag == WIN_DI)
			{
				plc.maskin[i / BYTESIZE] |= (1 << i % BYTESIZE);
				plc.maskin_N[i / BYTESIZE] &= ~(1 << i % BYTESIZE);
			}
            else if (WinFlag == WIN_DQ)
			{
				plc.maskout[i / BYTESIZE] |= (1 << i % BYTESIZE);
				plc.maskout_N[i / BYTESIZE] &= ~(1 << i % BYTESIZE);
			}
            else if (WinFlag == WIN_COUNTER)
                set(&plc, WIN_COUNTER, i);
            else if (WinFlag == WIN_TIMER)
                set(&plc, WIN_TIMER, i);
			redraw = TRUE;

			break;
		case KEY_F(2):    //F2 forces 0
            if (WinFlag == WIN_DI)
			{
				plc.maskin[i / BYTESIZE] &= ~(1 << i % BYTESIZE);
				plc.maskin_N[i / BYTESIZE] |= (1 << i % BYTESIZE);
			}
            else if (WinFlag == WIN_DQ)
			{
				plc.maskout[i / BYTESIZE] &= ~(1 << i % BYTESIZE);
				plc.maskout_N[i / BYTESIZE] |= (1 << i % BYTESIZE);
			}
            else if (WinFlag == WIN_COUNTER)
                reset(&plc, WIN_COUNTER, i);
            else if (WinFlag == WIN_TIMER)
                reset(&plc, WIN_TIMER, i);

			redraw = TRUE;
			break;
		case KEY_F(3):    //F3 unforces
            if (WinFlag == WIN_DI)
			{
				plc.maskin[i / BYTESIZE] &= ~(1 << i % BYTESIZE);
				plc.maskin_N[i / BYTESIZE] &= ~(1 << i % BYTESIZE);
			}
            else if (WinFlag == WIN_DQ)
			{
				plc.maskout[i / BYTESIZE] &= ~(1 << i % BYTESIZE);
				plc.maskout_N[i / BYTESIZE] &= ~(1 << i % BYTESIZE);
			}
            else if (WinFlag == WIN_TIMER)
				down_timer(&plc, i);
			redraw = TRUE;
			break;
		case KEY_F(4):    //F4 runs/stops
			if (!plc.status % 2)    //stopped
            {
                plc_log("Start!");
                plc.status = 1;
            }
			else
            {
                plc_log("Stop!");
				--plc.status;    //running
            }
			redraw = TRUE;
			break;
		case KEY_F(5):    //edit
			Cur = i;
            ret = PAGE_EDIT;
			break;
		case KEY_F(6):    //toggle lock
			if (WinFlag == 2)
				plc.m[i].RO = (plc.m[i].RO) ? 0 : 1;
			redraw = TRUE;
			break;

		case KEY_F(7):
            ret = PAGE_FILE;
			SaveFlag = 0;
			break;
		case KEY_F(8):
            ret = PAGE_FILE;
			SaveFlag = TRUE;
			break;

		case KEY_F(9):
            ret = PAGE_HELP;
			break;

		case KEY_F(10):
            ret = PAGE_EXIT;
			break;
		default:
			break;
		}
	}
	return ret;
}

int exit_page()
{
	static int redraw = TRUE;
	int c;
	if (redraw)
	{
		wdraw(ConfWin);
		wshowall(ConfWin, ConfWinBuf);
		redraw = FALSE;
	}
	grgetch();
	c = lastchar();
	if (c == 'n' || c == 'N')
		return (0);
	if (c == 'y' || c == 'Y')
	{
		redraw = TRUE;
		win_clear(ConfWin);
        return (PAGE_MAIN);
	}
    return (PAGE_EXIT);
}

int help_page()
{
	int c, i = 0;
	static int redraw_help = TRUE;
	if (redraw_help)
	{
		wdraw(HelpWin);
		wshowall_c(HelpWin, HelpWinBuf);
		redraw_help = FALSE;
//	win_set(help,i);
	}
	i = wselect(HelpWin, HelpWinBuf);
	win_set(HelpWin, i);

	c = lastchar();

	if (c == KEY_ESC)
	{
		redraw_help = TRUE;
        return (PAGE_MAIN);
	}
    return (PAGE_HELP);
}

int edit_page(int i)
{ //edit comments, value if memory/timer/blinker, scale if timer/blinker, preset & up/down if timer
	static int redraw = TRUE;
	int c;
	static int maxrow;
    static char buf[NICKLEN] = "";
	static int row = 0;
	static int col = 0;
    int y = 0;
            //x, y;
    //n;

	if (plc.status % 2)    //if running
		--plc.status;    //running

	if (redraw)
	{    //init window
		redraw = FALSE;
		wdraw(EditWin);
		draw_footer("Esc:Cancel  Enter:Enter");
		wshowall(EditWin, EditWinBuf);
		redraw = FALSE;

		switch (WinFlag)
		{
		case 0:    //inputs
			maxrow = TRUE;
			win_puts(EditWin, 1, 1, "Comment        :");
			sprintf(buf, "%s", plc.di[i].nick);
			win_puts(EditWin, 1, 20, buf);
			break;
		case 1:    //outputs
			maxrow = 1;
			win_puts(EditWin, 1, 1, "Comment        :");
			sprintf(buf, "%s", plc.dq[i].nick);
			win_puts(EditWin, 1, 20, buf);
			break;
		case 2:    //memory
			maxrow = 3;
			win_puts(EditWin, 1, 1, "Comment        :");
			sprintf(buf, "%s", plc.m[i].nick);
			win_puts(EditWin, 1, 20, buf);
			win_puts(EditWin, 2, 1, "Value          :");
            sprintf(buf, "%ld", plc.m[i].V);
			win_puts(EditWin, 2, 20, buf);
			win_puts(EditWin, 3, 1, "Downcounting     :");
			sprintf(buf, "%d", plc.m[i].DOWN);
			win_puts(EditWin, 3, 20, buf);
			break;
		case 3:    //timers
			maxrow = 5;
			win_puts(EditWin, 1, 1, "Comment        :");
			sprintf(buf, "%s", plc.t[i].nick);
			win_puts(EditWin, 1, 20, buf);
			win_puts(EditWin, 2, 1, "Value          :");
            sprintf(buf, "%ld", plc.t[i].V);
			win_puts(EditWin, 2, 20, buf);
			win_puts(EditWin, 3, 1, "Preset         :");
            sprintf(buf, "%ld", plc.t[i].P);
			win_puts(EditWin, 3, 20, buf);
			win_puts(EditWin, 4, 1, "Cycles/count   :");
            sprintf(buf, "%ld", plc.t[i].S);
			win_puts(EditWin, 4, 20, buf);
			win_puts(EditWin, 5, 1, "ON/OFF delay   :");
			sprintf(buf, "%d", plc.t[i].ONDELAY);
			win_puts(EditWin, 5, 20, buf);
			break;

		case 4:    //blinkers
			maxrow = 2;
			win_puts(EditWin, 1, 1, "Comment        :");
			sprintf(buf, "%s", plc.s[i].nick);
			win_puts(EditWin, 1, 20, buf);
			win_puts(EditWin, 2, 1, "Cycles/count   :");
            sprintf(buf, "%ld", plc.s[i].S);
			win_puts(EditWin, 2, 20, buf);
			break;
		case 5:
			maxrow = 2;
			win_puts(EditWin, 1, 1, "Press Enter to switch");
			win_puts(EditWin, 2, 1, "languages (PLC will");
			win_puts(EditWin, 3, 1, "stop and the current");
			win_puts(EditWin, 4, 1, "program will be lost!");
		default:
			break;

		}

	}

	col = 0;
	y = row + 1;

	switch (WinFlag)
	{    //update correct row every time
	case 0:    //inputs
		if (row == 0)
			sprintf(buf, "%s", plc.di[i].nick);
		break;
	case 1:    //outputs
		if (row == 0)
			sprintf(buf, "%s", plc.dq[i].nick);
		break;
	case 2:    //memory
		switch (row)
		{
		case 0:
			sprintf(buf, "%s", plc.m[i].nick);
			break;
		case 1:
            sprintf(buf, "%ld", plc.m[i].V);
			break;
		case 2:
			sprintf(buf, "%d", plc.m[i].DOWN);
			break;
		default:
			break;
		}
		break;
	case 3:    //timers
		switch (row)
		{
		case 0:
			sprintf(buf, "%s", plc.t[i].nick);
			break;
		case 1:
            sprintf(buf, "%ld", plc.t[i].V);
			break;
		case 2:
            sprintf(buf, "%ld", plc.t[i].P);
			break;
		case 3:
            sprintf(buf, "%ld", plc.t[i].S);
			break;
		case 4:
			sprintf(buf, "%d", plc.t[i].ONDELAY);
			break;
		default:

			break;
		}
		break;
	case 4:    //blinkers
		if (row == 0)
			sprintf(buf, "%s", plc.s[i].nick);
		if (row == 1)
            sprintf(buf, "%ld", plc.s[i].S);
		break;
	default:
		break;
	}
	win_gets(EditWinBuf, y, 20, 16, buf);
	switch (WinFlag)
	{    //update correct value with input
	case 0:    //inputs
		if (row == 0)
			sprintf(plc.di[i].nick, "%s", buf);
		break;
	case 1:    //outputs
		if (row == 0)
			sprintf(plc.dq[i].nick, "%s", buf);
		break;
	case 2:    //memory
		switch (row)
		{
		case 0:
			sprintf(plc.m[i].nick, "%s", buf);
			break;
		case 1:
			plc.m[i].V = atoi(buf);
			break;
		case 2:
			plc.m[i].DOWN = atoi(buf) ? 1 : 0;
		default:
			break;
		}
		break;
	case 3:    //timers
		switch (row)
		{
		case 0:
			sprintf(plc.t[i].nick, "%s", buf);
			break;
		case 1:
			plc.t[i].V = atoi(buf);
			break;
		case 2:
			plc.t[i].P = atoi(buf);
			break;
		case 3:
			plc.t[i].S = atoi(buf);
			break;
		case 4:
			plc.t[i].ONDELAY = atoi(buf) ? 1 : 0;
		default:
			break;
		}
		break;
	case 4:    //blinkers
		if (row == 0)
			sprintf(plc.s[i].nick, "%s", buf);
		if (row == 1)
			plc.s[i].S = atoi(buf);
		break;

	default:
		break;
	}
	c = lastchar();
	switch (c)
	{
	case KEY_UP:
		if (row > 0)
			--row;
		break;
	case KEY_DOWN:
		if (row < maxrow - 1)
			++row;
		break;
	case KEY_LEFT:
		if (col > 0)
			--col;
		break;
	case KEY_RIGHT:
        if (col < NICKLEN)
			++col;
		break;
	case 10:
	case 13:
	case KEY_END:
		redraw = TRUE;
		win_clear(EditWin);
		plc.status |= 1;    //RUN mode
		if (WinFlag == 5)
		{
/*			if (Language == LANG_LD)
				Language = LANG_IL;
			else
                Language = LANG_LD;*/
			memset(Lines[i], 0, MAXSTR);
			memset(Labels[i], 0, MAXSTR);
			plc.status |= 0;    //RUN mode
		}
        return (PAGE_MAIN);

	case KEY_ESC:
		redraw = TRUE;
		win_clear(EditWin);
		plc.status |= 1;    //RUN mode
        return (PAGE_MAIN);

	}
    return (PAGE_EDIT);
}

int save_file(char * path)
{
	FILE * f;
	int i;
	//open file for writing
	if ((f = fopen(path, "w")) == NULL )
	{
		return ERROR;
	}
	else
	{
		for (i = 0; i < Di * BYTESIZE; i++)
		{
			if (plc.di[i].nick[0] != 0)
				fprintf(f, "I\t%d\t%s\t\n", i, plc.di[i].nick);
		}
		for (i = 0; i < Dq * BYTESIZE; i++)
		{
			if (plc.dq[i].nick[0] != 0)
				fprintf(f, "Q\t%d\t%s\t\n", i, plc.dq[i].nick);
		}

		for (i = 0; i < Nm; i++)
		{
			if (plc.m[i].nick[0] != 0)
				fprintf(f, "M\t%d\t%s\t\n", i, plc.m[i].nick);
			if (plc.m[i].V > 0)
                fprintf(f, "MEMORY\t%d\t%ld\t\n", i, plc.m[i].V);
			if (plc.m[i].DOWN > 0)
				fprintf(f, "COUNT\t%d\tDOWN\t\n", i);
			if (plc.m[i].RO > 0)
				fprintf(f, "COUNTER\t%d\tOFF\t\n", i);
		}
		for (i = 0; i < Nt; i++)
		{
			if (plc.t[i].nick[0] != 0)
				fprintf(f, "T\t%d\t%s\t\n", i, plc.t[i].nick);
			if (plc.t[i].S > 0)
                fprintf(f, "TIME\t%d\t%ld\t\n", i, plc.t[i].S);
			if (plc.t[i].P > 0)
                fprintf(f, "PRESET\t%d\t%ld\t\n", i, plc.t[i].P);
			if (plc.t[i].ONDELAY > 0)
				fprintf(f, "DELAY\t%d\tON\t\n", i);
		}
		for (i = 0; i < Ns; i++)
		{
			if (plc.s[i].nick[0] != 0)
				fprintf(f, "B\t%d\t%s\t\n", i, plc.s[i].nick);
			if (plc.s[i].S > 0)
                fprintf(f, "BLINK\t%d\t%ld\t\n", i, plc.s[i].S);
		}
		for (i = 0; i < MEDSTR; i++)
		{
			if (com_nick[i][0] != 0)
				fprintf(f, "COM\t%d\t%s\t\n", i, com_nick[i]);
		}
		fprintf(f, "\n%s\n", "LD");
		for (i = 0; i < Lineno; i++)
			fprintf(f, "%s\n", Lines[i]);
		fclose(f);
	}
    return 0;
}

int file_page()
{
	static int redraw = TRUE;
	static char path[MEDSTR];
	int c;
	static char buf[MEDSTR] = "";

	if (redraw)
	{
		if (plc.status % 2)    //if running
			--plc.status;    //running
		wdraw(FileWin);
		draw_footer("Esc:Cancel  Enter:Enter");
		wshowall(FileWin, FileWinBuf);
		redraw = FALSE;
	}
	win_gets(FileWinBuf, 1, 1, MEDSTR, buf);
	c = lastchar();
	if (c == 10 || c == 13)
	{    //enter
		redraw = TRUE;
		win_clear(ConfWin);
		sprintf(path, "%s", buf);
		if (SaveFlag)
		{    //save to file
			if (save_file(path) < 0)
			{
				draw_info_line(PageLen + 1, "Invalid filename!");
                return PAGE_FILE;
			}
		}
		else
		{    //init from file
            if (plc_load_file(path) < 0)
                return PAGE_FILE;
		}
		plc.status |= 1;    //RUN mode
        return (PAGE_MAIN);
	}
	if (c == KEY_ESC)
	{
		redraw = TRUE;
		win_clear(FileWin);
		plc.status |= 1;    //RUN mode
        return (PAGE_MAIN);
	}
    return (PAGE_FILE);
}

int edit_mode()
{
	char line[MAXSTR];
	int i = 0;
	wedit(LdWinBuf, LdWin, NULL );	//del_buf);
	WinFlag = 0;
	memset(line, 0, MAXSTR);
	for (i = 0; i <= Lineno; i++)
	{
		memset(Lines[i], 0, MAXSTR);
		memset(Labels[i], 0, MAXSTR);
	}
	i = 0;
	while (buf_cpline(LdWinBuf, i, line) >= 0)
	{
		line[0] = '-';
		sprintf(Lines[i], "%s", line);
		i++;
		memset(line, 0, MAXSTR);
	}
	Lineno = i;
    return PAGE_MAIN;
}

void ui_draw()
{
    char str[SMALLSTR];
    if (plc.status % 2) //running
        sprintf(str, "Hardware:%s  RUNNING", Hw);
    else
        sprintf(str, "Hardware:%s  STOPPED", Hw);
    draw_footer(str);
//	draw_info_line(1,"F1/2:Force 1/0|F3:Unforce|F4:Run|F5:Edit|F6:Lock|F7:Load|FBYTESIZE:Save|F9:Help|F10:Quit");

    wdraw(InWin);
    wdraw(OutWin);
    wdraw(TimWin);
    wdraw(MVarWin);
    wdraw(BlinkWin);
    wdraw(LdWin);

    load_inputs();
    load_outputs();
    load_mvars();
    load_timers();
    load_blinkers();
    load_ld();

    wshowall_c(InWin, InWinBuf);
    wshowall_c(OutWin, OutWinBuf);
    wshowall_c(MVarWin, MVarWinBuf);
    wshowall_c(TimWin, TimWinBuf);
    wshowall_c(BlinkWin, BlinkWinBuf);
    wshowall_c(LdWin, LdWinBuf);
}

int ui_init(int screenW)
{
    char str[SMALLSTR];
    WinFlag = TRUE;
    Enable = 1;
    Cur = 0;
    win_start();
    PageLen = screenW - 3;
    InWin = win_open(3, 1, PageLen / 2 - 3, Pagewidth / 4 - 1, "DIGITAL INPUTS");
    OutWin = win_open(3, 1 + Pagewidth / 4, PageLen / 2 - 3, Pagewidth / 4 - 1,
            "DIGITAL OUTPUTS");
    MVarWin = win_open(3, 1 + Pagewidth / 2, PageLen / 2 - 3, Pagewidth / 4 - 1,
            "MEMORY COUNTERS");
    TimWin = win_open(3, 1 + 3 * Pagewidth / 4, PageLen / 4 - 2,
            Pagewidth / 4 - 2, "TIMERS");
    BlinkWin = win_open(4 + PageLen / 4 - 2, 1 + 3 * Pagewidth / 4,
            PageLen / 4 - 2, Pagewidth / 4 - 2, "BLINKERS");
    LdWin = win_open(2 + PageLen / 2, 1, PageLen / 2 - 4, Pagewidth - 2,
            "PLC TASK");
    EditWin = win_open(10, 15, 8, 40, " Configuration ");
    ConfWin = win_open(10, 15, 4, 50, " Exit PLC-emu ? ");
    FileWin = win_open(10, 15, 8, 40, " Write a UNIX path");
    HelpWin = win_open(6, 1, PageLen - 10, 80, "HELP");

    InWinBuf = buf_open();
    OutWinBuf = buf_open();
    MVarWinBuf = buf_open();
    TimWinBuf = buf_open();
    BlinkWinBuf = buf_open();
    LdWinBuf = buf_open();
    EditWinBuf = buf_open();
    ConfWinBuf = buf_open();
    strcpy(str, " Quit?  wh(Y)? Why (N)ot? ");
    app_line(ConfWinBuf, str);

    FileWinBuf = buf_open();
    HelpWinBuf = buf_open();
    init_help();
    return 1;
}

int ui_update(int page)
{
    int nextpage=0;
    time_header();
    switch(page)
    {
        case PAGE_MAIN:
            nextpage = main_page();
            break;
        case PAGE_EXIT:
            nextpage = exit_page();
            break;
        case PAGE_EDIT:
            nextpage = edit_page(Cur);
            break;
        case PAGE_FILE:
            nextpage = file_page();
            break;
        case PAGE_EDITMODE:
            nextpage = edit_mode();
            break;
        case PAGE_HELP:
            nextpage = help_page();
            break;
        default:
            break;
    }
    return nextpage;
}

void ui_end()
{
    win_end();
}

void ui_toggle_enabled()
{
    Enable = Enable ? 0 : 1;
}
