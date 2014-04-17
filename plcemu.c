#include "plcemu.h"
#include "plclib.h"
#include "project.h"

/*************GLOBALS************************************************/


BYTE Command, Response;
int Cur, PageLen;
int InWin, OutWin, TimWin, BlinkWin, MVarWin, EditWin, ConfWin, LdWin, FileWin, HelpWin;
int InWinBuf, OutWinBuf, TimWinBuf, BlinkWinBuf, MVarWinBuf, EditWinBuf, ConfWinBuf, LdWinBuf, FileWinBuf, HelpWinBuf;
int Update=FALSE;
int Enable=TRUE;
//Config variables

struct PLC_regs * pOld=NULL;
struct pollfd PlcCom[1];

int WinFlag = FALSE; //0->inputs, 1->outputs 2->mvars, 3->timers, 4->blinkers
struct timeval T;
int SaveFlag = 0; //0 loads, 1 saves
int Language = LANG_LD;

const char ErrMsg[BYTESIZE][MEDSTR] =
{ 		"Something went terribly wrong!",
		"Invalid Symbol!",
		"Wrong File!",
		"Invalid Operand!",
		"Invalid Numeric!",
		"Invalid Output!",
		"Invalid Command!",
		""
};

const char LangStr[3][TINYSTR] =
{ 	"Ladder Diagram",
	"Instruction List",
	"Structured Text"
};

int init_config(const char * filename)
{
	FILE * fcfg;
	char line[MEDSTR], name[SMALLSTR], path[MAXSTR], val[SMALLBUF];
//        filename = "plc.config";
	memset(path, 0, MAXSTR);
	sprintf(path, "%s", filename);
	if (fcfg = fopen(path, "r"))
	{
		memset(line, 0, SMALLSTR);
		memset(name, 0, SMALLSTR);
        memset(val, 0, NICKLEN);
		while (fgets(line, MEDSTR, fcfg))
		{
			sscanf(line, "%s\t%s", name, val);
			if (!strcmp(name, "STEP"))
				Step = atoi(val);
			if (!strcmp(name, "SIGENABLE"))
				Sigenable = atoi(val);
			if (!strcmp(name, "PAGELEN"))
				PageLen = atoi(val);
			if (!strcmp(name, "PAGEWIDTH"))
				Pagewidth = atoi(val);
			if (!strcmp(name, "NT"))
				Nt = atoi(val);
			if (!strcmp(name, "NS"))
				Ns = atoi(val);
			if (!strcmp(name, "NM"))
				Nm = atoi(val);
			if (!strcmp(name, "DI"))
				Di = atoi(val);
			if (!strcmp(name, "DQ"))
				Dq = atoi(val);
			if (!strcmp(name, "BASE"))
				Base = atoi(val);
			if (!strcmp(name, "WR_OFFS"))
				Wr_offs = atoi(val);
			if (!strcmp(name, "RD_OFFS"))
				Rd_offs = atoi(val);
			if (!strcmp(name, "COMEDI_FILE"))
				Comedi_file = atoi(val);
			if (!strcmp(name, "COMEDI_SUBDEV_�"))
				Comedi_subdev_i = atoi(val);
			if (!strcmp(name, "COMEDI_SUBDEV_Q"))
				Comedi_subdev_q = atoi(val);
			if (!strcmp(name, "HW"))
				sprintf(Hw, "%s", val);
			if (!strcmp(name, "PIPE"))
				sprintf(Pipe, "%s", val);
			if (!strcmp(name, "RESPONSE"))
				sprintf(Responsefile, "%s", val);
			memset(line, 0, MEDSTR);
			memset(name, 0, SMALLSTR);
		}
		fclose(fcfg);
		if (Step > 0
		&& Sigenable > 29
		&& PageLen > 23
		&& Pagewidth > 79
		&& Nt >= 0
		&& Ns >= 0
		&& Nm >= 0
		&& Di >= 0
		&& Dq >= 0
		&& Wr_offs >= 0
		&& Rd_offs >= 0
		&& Base > 0)
			return OK;
	}
	return ERR;
}

int timeval_subtract(struct timeval *result, struct timeval *x,
		struct timeval *y)
{ /* Subtract the `struct timeval' values X and Y,
 storing the result in RESULT.
 Return 1 if the difference is negative, otherwise 0.  */
	/* Perform the carry for the later subtraction by updating y. */
	if (x->tv_usec < y->tv_usec)
	{
		int nsec = (y->tv_usec - x->tv_usec) / MILLION + 1;
		y->tv_usec -= MILLION * nsec;
		y->tv_sec += nsec;
	}
	if (x->tv_usec - y->tv_usec > MILLION)
	{
		int nsec = (x->tv_usec - y->tv_usec) / MILLION;
		y->tv_usec += MILLION * nsec;
		y->tv_sec -= nsec;
	}
	/* Compute the time remaining to wait.
	 tv_usec is certainly positive. */
	result->tv_sec = x->tv_sec - y->tv_sec;
	result->tv_usec = x->tv_usec - y->tv_usec;
	/* Return 1 if result is negative. */
	return x->tv_sec < y->tv_sec;
}
void sigenable()
{
	Enable = Enable ? 0 : 1;
}

void init_help()
{
	FILE * f;
	char line[MAXSTR], helpline[MAXSTR];
	buf_clear(HelpWinBuf);
	if (f = fopen("./help", "r"))
	{
		while (fgets(line, MEDSTR, f))
		{ //read help file
			sprintf(helpline, " %s", line);
			app_line(HelpWinBuf, helpline);
		}
		fclose(f);
	}
}

void init_emu()
{
    //int i;
	enable_bus();
	plc.inputs = (BYTE *) malloc(Di);
	plc.outputs = (BYTE *) malloc(Dq);
	plc.edgein = (BYTE *) malloc(Di);
	plc.maskin = (BYTE *) malloc(Di);
	plc.maskout = (BYTE *) malloc(Dq);
	plc.maskin_N = (BYTE *) malloc(Di);
	plc.maskout_N = (BYTE *) malloc(Dq);
	plc.di = (struct digital_input*) malloc(
			BYTESIZE * Di * sizeof(struct digital_input));
	plc.dq = (struct digital_output*) malloc(
			BYTESIZE * Dq * sizeof(struct digital_output));
	plc.t = (struct timer *) malloc(Nt * sizeof(struct timer));
	plc.s = (struct blink *) malloc(Ns * sizeof(struct blink));
	plc.m = (struct mvar *) malloc(Nm * sizeof(struct mvar));
	memset(plc.inputs, 0, Di);
	memset(plc.outputs, 0, Dq);
	memset(plc.maskin, 0, Di);
	memset(plc.maskout, 0, Dq);
	memset(plc.maskin_N, 0, Di);
	memset(plc.maskout_N, 0, Dq);
	memset(plc.di, 0, BYTESIZE * Di * sizeof(struct digital_input));
	memset(plc.dq, 0, BYTESIZE * Dq * sizeof(struct digital_output));
	memset(plc.t, 0, Nt * sizeof(struct timer));
	memset(plc.s, 0, Ns * sizeof(struct blink));
	memset(plc.m, 0, Nm * sizeof(struct timer));

	pOld = (struct PLC_regs *) malloc(sizeof(struct PLC_regs));

	pOld->inputs = (BYTE *) malloc(Di);
	pOld->outputs = (BYTE *) malloc(Dq);
	pOld->maskin = (BYTE *) malloc(Di);
	pOld->edgein = (BYTE *) malloc(Di);
	pOld->maskout = (BYTE *) malloc(Dq);
	pOld->maskin_N = (BYTE *) malloc(Di);
	pOld->maskout_N = (BYTE *) malloc(Dq);
	pOld->di = (struct digital_input*) malloc(
			BYTESIZE * Di * sizeof(struct digital_input));
	pOld->dq = (struct digital_output*) malloc(
			BYTESIZE * Dq * sizeof(struct digital_output));
	pOld->t = (struct timer *) malloc(Nt * sizeof(struct timer));
	pOld->s = (struct blink *) malloc(Ns * sizeof(struct blink));
	pOld->m = (struct mvar *) malloc(Nm * sizeof(struct mvar));

	memcpy(pOld->inputs, plc.inputs, Di);
	memcpy(pOld->outputs, plc.outputs, Dq);
	memcpy(pOld->m, plc.di, Nm * sizeof(struct mvar));
	memcpy(pOld->t, plc.dq, Nt * sizeof(struct blink));
	memcpy(pOld->s, plc.dq, Ns * sizeof(struct timer));
	plc.command = 0;
	plc.status = TRUE;
	WinFlag = TRUE;
	Update = TRUE;
	Enable = 1;
	signal(Sigenable, sigenable);
	PLC_init();
	Cur = 0;
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
	sprintf(str, " PLC-EMUlator v%4.2f %14s ", VERSION, p);
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
	char s[32], color;
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
	char s[32], color, bit;
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
	char s[32], color, bit;
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

int io_page()
{
	static int redraw = TRUE;
    int c, i, ret;
    //, ch;
	i = 0;
	static int win_sticky;
	char str[SMALLSTR];
	if (Update)
	{
		redraw = TRUE;
		Update = FALSE;
	}
	if (redraw)
	{
		if (plc.status % 2) //running
			sprintf(str, "Hardware:%s Language:%s RUNNING", Hw,
					LangStr[Language]);
		else
			sprintf(str, "Hardware:%s Language:%s STOPPED", Hw,
					LangStr[Language]);
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
		redraw = FALSE;
	}
    ret = PAGE_IO;
	if (Enable)
	{
		if (WinFlag == DI)
		{
			draw_info_line(1,
					"F1/2:Force 1/0|F3:Unforce|F4:Run/Stop|F5:Edit|F7:Load|F8:Save|F9:Help|F10:Quit");
			i = wselect(InWin, InWinBuf);
			win_set(InWin, i);
		}
		else if (WinFlag == DQ)
		{
			draw_info_line(1,
					"F1/2:Force 1/0|F3:Unforce|F4:Run/Stop|F5:Edit|F7:Load|F8:Save|F9:Help|F10:Quit");
			i = wselect(OutWin, OutWinBuf);
			win_set(OutWin, i);
		}
		else if (WinFlag == COUNTER)
		{
			draw_info_line(1,
					"F1/2:Toggle Pulse|F4:Run/Stop|F5:Edit|F6:Lock|F7:Load|F8:Save|F9:Help|F10:Quit");
			i = wselect(MVarWin, MVarWinBuf);
			win_set(MVarWin, i);
		}
		else if (WinFlag == TIMER)
		{
			draw_info_line(1,
					"F1:Start|F2:Pause|F3:Reset|F4:Run/Stop|F5:Edit|F7:Load|F8:Save|F9:Help|F10:Quit");
			i = wselect(TimWin, TimWinBuf);
			win_set(TimWin, i);
		}
		else if (WinFlag == BLINKER)
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
		Update = TRUE;
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
			if (WinFlag == DI)
			{
				plc.maskin[i / BYTESIZE] |= (1 << i % BYTESIZE);
				plc.maskin_N[i / BYTESIZE] &= ~(1 << i % BYTESIZE);
			}
			else if (WinFlag == DQ)
			{
				plc.maskout[i / BYTESIZE] |= (1 << i % BYTESIZE);
				plc.maskout_N[i / BYTESIZE] &= ~(1 << i % BYTESIZE);
			}
			else if (WinFlag == COUNTER)
				set(&plc, COUNTER, i);
			else if (WinFlag == TIMER)
				set(&plc, TIMER, i);
			redraw = TRUE;

			break;
		case KEY_F(2):    //F2 forces 0
			if (WinFlag == DI)
			{
				plc.maskin[i / BYTESIZE] &= ~(1 << i % BYTESIZE);
				plc.maskin_N[i / BYTESIZE] |= (1 << i % BYTESIZE);
			}
			else if (WinFlag == DQ)
			{
				plc.maskout[i / BYTESIZE] &= ~(1 << i % BYTESIZE);
				plc.maskout_N[i / BYTESIZE] |= (1 << i % BYTESIZE);
			}
			else if (WinFlag == COUNTER)
				reset(&plc, COUNTER, i);
			else if (WinFlag == TIMER)
				reset(&plc, TIMER, i);

			redraw = TRUE;
			break;
		case KEY_F(3):    //F3 unforces
			if (WinFlag == DI)
			{
				plc.maskin[i / BYTESIZE] &= ~(1 << i % BYTESIZE);
				plc.maskin_N[i / BYTESIZE] &= ~(1 << i % BYTESIZE);
			}
			else if (WinFlag == DQ)
			{
				plc.maskout[i / BYTESIZE] &= ~(1 << i % BYTESIZE);
				plc.maskout_N[i / BYTESIZE] &= ~(1 << i % BYTESIZE);
			}
			else if (WinFlag == TIMER)
				down_timer(&plc, i);
			redraw = TRUE;
			break;
		case KEY_F(4):    //F4 runs/stops
			if (!plc.status % 2)    //stopped
				plc.status |= TRUE;
			else
				--plc.status;    //running
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
        return (PAGE_IO);
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
        return (PAGE_IO);
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
    //x = 20;
	col = 0;
	y = row + 1;
    //n = 10;

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
			if (Language == LANG_LD)
				Language = LANG_IL;
			else
				Language = LANG_LD;
			memset(Lines[i], 0, MAXSTR);
			memset(Labels[i], 0, MAXSTR);
			plc.status |= 0;    //RUN mode
		}
        return (PAGE_IO);

	case KEY_ESC:
		redraw = TRUE;
		win_clear(EditWin);
		plc.status |= 1;    //RUN mode
        return (PAGE_IO);

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

int load_file(char * path, int ini)
{ //ini =1 if file is loaded initially, i.e. messages should be printf'd not draw_info_line'd
	FILE * f;
	char * tab;
	char line[MAXSTR], name[SMALLSTR], val[NICKLEN], msg[SMALLBUF], idx_str[LABELLEN];
	int idx, r, lineno, i, j, k, found_start = FALSE;
	i = 0;
	//TODO for instruction list:
	if (f = fopen(path, "r"))
	{
		memset(line, 0, MAXSTR);
		memset(name, 0, SMALLSTR);
		memset(val, 0, NICKLEN);
		disable_bus();
		init_emu();

		lineno = 0;
		while (fgets(line, MEDSTR, f))
		{    //read initialization values
			if ((line[0] == 'L' && line[1] == 'D')
					|| (line[0] == 'I' && line[1] == 'L'))
			{    //or 'IL' for IL
				found_start = TRUE;
			}
			else if (!found_start)
			{
				j = 0;
				k = 0;
				memset(name, 0, SMALLSTR);
				memset(val, 0, NICKLEN);
				memset(idx_str, 0, LABELLEN);
				//read alpha characters
				while (isspace(line[j]))    //ignore blanks
					j++;
				while (isalpha(line[j]))
					name[k++] = line[j++];
				k = 0;
				while (isspace(line[j]))    //ignore blanks
					j++;
				while (isdigit(line[j]))
					idx_str[k++] = line[j++];
				while (isspace(line[j]))    //ignore blanks
					j++;
				k = 0;
				while (line[j] && k < 16 && line[j] != 10 && line[j] != 13
						&& line[j] != ';' && line[j] != '\t')
					val[k++] = line[j++];
				/*    	        	sscanf(line, "%s\t%s\t%s", name,idx_str, val);*/
				idx = atoi(idx_str);
				lineno++;
				if (idx < 0)
				{
					r = ERR_BADINDEX;
					break;
				}
				else if (!strcmp(name, "I"))
				{
					if (idx >= BYTESIZE * Di)
					{
						r = ERR_BADINDEX;
						break;
					}
					sprintf(plc.di[idx].nick, "%s", val);
				}
				else if (!strcmp(name, "Q"))
				{
					if (idx >= BYTESIZE * Dq)
					{
						r = ERR_BADINDEX;
						break;
					}
					sprintf(plc.dq[idx].nick, "%s", val);
				}
				else if (!strcmp(name, "M"))
				{
					if (idx >= Nm)
					{
						r = ERR_BADINDEX;
						break;
					}
					sprintf(plc.m[idx].nick, "%s", val);
				}
				else if (!strcmp(name, "T"))
				{
					if (idx >= Nt)
					{
						r = ERR_BADINDEX;
						break;
					}
					sprintf(plc.t[idx].nick, "%s", val);
				}
				else if (!strcmp(name, "B"))
				{
					if (idx >= Ns)
					{
						r = ERR_BADINDEX;
						break;
					}
					sprintf(plc.s[idx].nick, "%s", val);
				}
				else if (!strcmp(name, "COM"))
				{
					if (idx >= MEDSTR)
					{
						r = ERR_BADINDEX;
						break;
					}
					sprintf(com_nick[idx], "%s", val);
				}
				else if (!strcmp(name, "MEMORY"))
				{
					if (idx >= Nm)
					{
						r = ERR_BADINDEX;
						break;
					}
					plc.m[idx].V = atol(val);
				}
				else if (!strcmp(name, "COUNT"))
				{
					if (idx >= Nm)
					{
						r = ERR_BADINDEX;
						break;
					}
					if (!strcmp(val, "DOWN"))
						plc.m[idx].DOWN = TRUE;
					else
					{
						r = ERR_BADOPERATOR;
						break;
					}
				}
				else if (!strcmp(name, "COUNTER"))
				{
					if (idx >= Nm)
					{
						r = ERR_BADINDEX;
						break;
					}
					if (!strcmp(val, "OFF"))
						plc.m[idx].RO = TRUE;
					else
					{
						r = ERR_BADOPERATOR;
						break;
					}
				}
				else if (!strcmp(name, "TIME"))
				{
					if (idx >= Nt)
					{
						r = ERR_BADINDEX;
						break;
					}
					plc.t[idx].S = atoi(val);
				}
				else if (!strcmp(name, "PRESET"))
				{
					if (idx >= Nt)
					{
						r = ERR_BADINDEX;
						break;
					}
					plc.t[idx].P = atoi(val);
				}
				else if (!strcmp(name, "DELAY"))
				{
					if (idx >= Nt)
					{
						r = ERR_BADINDEX;
						break;
					}
					if (!strcmp(val, "ON"))
						plc.t[idx].ONDELAY = TRUE;
					else
					{
						r = ERR_BADOPERAND;
						break;
					}
				}
				else if (!strcmp(name, "BLINK"))
				{
					if (idx >= Ns)
					{
						r = ERR_BADINDEX;
						break;
					}
					plc.s[idx].S = atoi(val);
				}
				else if (name[0] != ';' && isalnum(name[0]) != 0
						&& strcmp(name, "LD"))
				{
					r = ERR_BADOPERAND;
					break;
				}
			}
			else
			{
				while (strchr(line, '\t') != NULL )    //tabs are not supported
				{
					tab = strchr(line, '\t');
					*tab = '.';
				}
				memset(Lines[i], 0, MAXSTR);
				memset(Labels[i], 0, MAXSTR);
				sprintf(Lines[i++], "%s", line);
				//i++;
			}
			r = OK;
			memset(line, 0, MAXSTR);
			memset(name, 0, SMALLSTR);
		}
		fclose(f);
		Lineno = i;
	}
	else
		r = ERR_BADFILE;
//	printf(msg,"");
	if (r < 0)
	{
		switch (r)
		{
		case ERR_BADFILE:
			sprintf(msg, "Invalid filename:!");
			break;
		case ERR_BADINDEX:
			sprintf(msg, "%s:%d:Index out of bounds!", path, lineno);
			break;
		case ERR_BADOPERAND:
			sprintf(msg, "%s:%d:Invalid word %s!", path, lineno, name);
			break;
		case ERR_BADOPERATOR:
			sprintf(msg, "%s:%d:Invalid word %s!", path, lineno, val);
			break;

		default:
			break;
		}
		if (ini == 1)
			printf("%s\n", msg);
		else
			draw_info_line(PageLen + 1, msg);
		return ERROR;
	}
	else
	{
		return OK;
	}
}

int file_page()
{
	static int redraw = TRUE;
	static char path[MEDSTR];
	int c;
	static char buf[MEDSTR] = "";
//	int i;
    //FILE * f;
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
			if (load_file(path, 0) < 0)
                return PAGE_FILE;
		}
		plc.status |= 1;    //RUN mode
        return (PAGE_IO);
	}
	if (c == KEY_ESC)
	{
		redraw = TRUE;
		win_clear(FileWin);
		plc.status |= 1;    //RUN mode
        return (PAGE_IO);
	}
    return (PAGE_FILE);
}

int plc_func(int daemon)
{
	struct timeval tp, tn, dt;
	long timeout;
	BYTE i_bit, com[2];
	int q_bit, n, i, j;
	int written=FALSE;
	int rfd = 0; //response file descriptor
	int r = OK;

	int i_changed = FALSE;
	int o_changed = FALSE;
	int m_changed = FALSE;
	int t_changed = FALSE;
	int s_changed = FALSE;
    char test[NICKLEN];
	if ((plc.status) % 2)    //run
	{    //read inputs
		for (i = 0; i < Di; i++)
		{	//for each input byte
			plc.inputs[i] = 0;
			for (j = 0; j < BYTESIZE; j++)
			{	//read n bit into in
				n = i * BYTESIZE + j;
				i_bit = 0;
				dio_read(n, &i_bit);
				plc.inputs[i] |= i_bit << j;
			}	//mask them
			plc.inputs[i] = (plc.inputs[i] | plc.maskin[i]) & ~plc.maskin_N[i];
			if (plc.inputs[i] != pOld->inputs[i])
				i_changed = TRUE;
			plc.edgein[i] = (plc.inputs[i]) ^ (pOld->inputs[i]);
		}
		//manage timers
		for (i = 0; i < Nt; i++)
		{
			if (plc.t[i].V < plc.t[i].P && plc.t[i].START)
			{
				if (plc.t[i].sn < plc.t[i].S)
					plc.t[i].sn++;
				else
				{
					t_changed = TRUE;
					plc.t[i].V++;
					plc.t[i].sn = 0;

				}
				plc.t[i].Q = (plc.t[i].ONDELAY) ? 0 : 1;	//on delay
			}
			else if (plc.t[i].START)
			{
				plc.t[i].Q = (plc.t[i].ONDELAY) ? 1 : 0;	//on delay
			}
		}
		for (i = 0; i < Ns; i++)
		{
			if (plc.s[i].S > 0)
			{	//if set up
				if (plc.s[i].sn > plc.s[i].S)
				{
					s_changed = TRUE;
					plc.s[i].Q = (plc.s[i].Q) ? 0 : 1;	//toggle
					plc.s[i].sn = 0;
				}
				else
					plc.s[i].sn++;
			}
		}
		read_mvars(&plc);

//poll on plcpipe for command, for max STEP msecs
		PlcCom[0].fd = open(Pipe, O_NONBLOCK | O_RDONLY);
		PlcCom[0].events = POLLIN | POLLPRI;
		gettimeofday(&tn);	//how much time passed for previous cycle?
		timeval_subtract(&dt, &tn, &T);
//	sprintf(test,"Refresh time approx:%d microseconds",dt.tv_usec);
//	draw_info_line(PageLen+ 2,test);
		while (dt.tv_usec > THOUSAND * Step)//if timeout, add another circle;
			dt.tv_usec -= THOUSAND * Step;
		timeout = Step - dt.tv_usec / THOUSAND;
		written = poll(PlcCom, 1, timeout);
		gettimeofday(&tp);	//how much time did poll wait?
		timeval_subtract(&dt, &tp, &tn);
		if (written)
		{
			if (read(PlcCom[0].fd, com, 2))
			{
				if (com[0] == 0)
					com[0] = 0;	//NOP
				else
					plc.command = com[0] - ASCIISTART;
				sprintf(test, "LAST command:%d, %s", plc.command,
						com_nick[com[0] - ASCIISTART]);
				if (daemon == 1)
				{
					printf("%s\n", test);
					sprintf(test, "");
				}
				else
					draw_info_line(PageLen + 1, test);
			}
		}
		else if (written == 0)
			plc.command = 0;
		else
		{
			if (daemon == 1)
				printf("PIPE ERROR\n");
			else
				draw_info_line(PageLen + 1, "PIPE ERROR");
			plc.command = 0;
		}

//	sprintf(test,"Poll time approx:%d milliseconds",dt.tv_usec/1000);
		close(PlcCom[0]);
		gettimeofday(&T);	//start timing next cycle
		Command = plc.command;
		dec_inp(&plc); //read inputs
		PLC_task(&plc);

		if (Language == LANG_LD)
			r = LD_task(&plc);
		if (Language == LANG_IL)
			r = IL_task(&plc);

		enc_out(&plc);
		plc.response = Response;

		plc.command = 0;
		for (i = 0; i < Dq; i++)
		{	//write masked outputs
			plc.outputs[i] = (plc.outputs[i] | plc.maskout[i])
					& ~plc.maskout_N[i];
			for (j = 0; j < BYTESIZE; j++)
			{	//write n bit out
				n = BYTESIZE * i + j;
				q_bit = (plc.outputs[i] >> j) % 2;
				dio_write(plc.outputs, n, q_bit);
			}
			if (plc.outputs[i] != pOld->outputs[i])
				o_changed = TRUE;
		}
		for (i = 0; i < Nm; i++)	//check counter pulses
		{
			if (plc.m[i].PULSE != pOld->m[i].PULSE)
			{
				plc.m[i].EDGE = TRUE;
				m_changed = TRUE;
			}
		}

		write_mvars(&plc);

		if (i_changed)
		{
//draw_info_line(4+PageLen/2," Input changed!");
			memcpy(pOld->inputs, plc.inputs, Di);
			Update = TRUE;
			i_changed = FALSE;
		}
		if (o_changed)
		{
//draw_info_line(4+PageLen/2," Output changed!");
			memcpy(pOld->outputs, plc.outputs, Dq);
			Update = TRUE;
			o_changed = FALSE;
		}
		if (m_changed)
		{
			memcpy(pOld->m, plc.m, Nm * sizeof(struct mvar));
			Update = TRUE;
			m_changed = FALSE;
		}
		if (t_changed)
		{
			memcpy(pOld->t, plc.t, Nt * sizeof(struct timer));
			Update = TRUE;
			t_changed = FALSE;
		}
		if (s_changed)
		{
			memcpy(pOld->s, plc.s, Ns * sizeof(struct blink));
			Update = TRUE;
			s_changed = FALSE;
		}
		//write out response
		if (plc.response)
		{
			rfd = open(Responsefile, O_NONBLOCK | O_WRONLY);
			write(rfd, plc.response, 1);
			close(rfd);
			plc.response = 0;
		}
	}
	else
		usleep(Step * THOUSAND);
	return r;
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
    return PAGE_IO;
}

int main(int argc, char **argv)
{
    int i, errcode, daemon_flag = FALSE;
        //    p, j, n,
    //int changed = FALSE;
	int page = 0;
//	BYTE * in_p, *out_p;
	char str[SMALLSTR], confstr[SMALLSTR], inistr[SMALLSTR];

	strcpy(confstr, "plc.config");
	sprintf(inistr, "");
	for (i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "-d"))
			daemon_flag = TRUE;
		else
		{	//check if previous arg was -i or -c
			if (!strcmp(argv[i - 1], "-i"))
				strcpy(inistr, argv[i]);
			else if (!strcmp(argv[i - 1], "-c"))
				strcpy(confstr, argv[i]);
			else
			{
				if ((strcmp(argv[i], "-i") && strcmp(argv[i], "-c"))
						|| argc == i + 1)
				{
					printf(
							"Usage: plcemu [-i program file] [-c config file] [-d] \n\
Options:\n\
-i loads initially a text file with initialization values and LD/IL program, \n\
-c uses a configuration file other than plc.config \n\
-d runs PLC-EMU as daemon \n");
					return ERR;
				}
			}
		}
	}
	if (init_config(confstr) < 0)
	{
		printf("Invalid configuration file\n");
		return ERR;
	}

	init_emu();

	if (inistr[0] && load_file(inistr, 1) < 0)
		printf("Invalid program file\n");

	if (daemon_flag == FALSE)
	{
		win_start();
		PageLen = PageLen - 3;
		InWin = win_open(3, 1, PageLen / 2, Pagewidth / 4 - 1, "DIGITAL INPUTS");
		OutWin = win_open(3, 1 + Pagewidth / 4, PageLen / 2, Pagewidth / 4 - 1,
				"DIGITAL OUTPUTS");
		MVarWin = win_open(3, 1 + Pagewidth / 2, PageLen / 2, Pagewidth / 4 - 1,
				"MEMORY COUNTERS");
		TimWin = win_open(3, 1 + 3 * Pagewidth / 4, PageLen / 4 - 1,
				Pagewidth / 4 - 2, "TIMERS");
		BlinkWin = win_open(4 + PageLen / 4, 1 + 3 * Pagewidth / 4,
				PageLen / 4 - 1, Pagewidth / 4 - 2, "BLINKERS");
		LdWin = win_open(6 + PageLen / 2, 1, PageLen / 2 - 9, Pagewidth - 2,
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
		//    del_buf = buf_open();
		HelpWinBuf = buf_open();
		page = 1;
		gettimeofday(&T);
		init_help();
		while (page)
		{
			time_header();
            if (page == PAGE_IO)
				page = io_page();
            else if (page == PAGE_EXIT)
				page = exit_page();
            else if (page == PAGE_EDIT)
				page = edit_page(Cur);
            else if (page == PAGE_FILE)
				page = file_page();
            else if (page == PAGE_EDITMODE)
				page = edit_mode();
            else if (page == PAGE_HELP)
				page = help_page();

			errcode = plc_func(FALSE);
			if (errcode < 0)
			{
				sprintf(str, "error code %d", -errcode);
				draw_info_line(4 + PageLen / 2, ErrMsg[-1 - errcode]);
			}
		}
	}
	else
	{
		for (;;)
		{
			if (plc.status > 0)
			{
				errcode = plc_func(TRUE);
				if (errcode < 0)
				{
					printf("%s\n", ErrMsg[1 - errcode]);
					plc.status = 0;
				}
			}
		}
	}
	disable_bus();
	win_end();
    return 0;
}
