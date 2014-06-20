#include <fcntl.h>
#include <sys/stat.h>
#include <poll.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>
#include "plcemu.h"
#include "plclib.h"
#include "parser.h"
#include "ui.h"
#include "hardware.h"
#include "project.h"
/*************GLOBALS************************************************/


BYTE Command, Response;

//Config variables

int ScreenWidth = TERMLEN;
int UiReady=FALSE;
int Update=FALSE;
struct PLC_regs * pOld=NULL;
struct pollfd PlcCom[1];
struct timeval T;

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

void plc_log(const char * msg, ...)
{
   va_list arg;
   time_t now;
   time(&now);
   char msgstr[MAXSTR];
   memset(msgstr,0,MAXSTR);
   va_start(arg, msg);
   vsprintf(msgstr,msg,arg);
   va_end(arg);
   if(ErrLog)
   {
      fprintf(ErrLog, msgstr);
      fprintf(ErrLog, ":%s", ctime(&now));
      fflush(ErrLog);
   }
   if(UiReady)
       ui_display_message(msgstr);
   else
       printf("%s\n",msgstr);
}

int init_config(const char * filename)
{
	FILE * fcfg;
	char line[MEDSTR], name[SMALLSTR], path[MAXSTR], val[SMALLBUF];

    memset(path, 0, MAXSTR);
	sprintf(path, "%s", filename);

    Step =1;
    Sigenable=36;
    Pagewidth=48;
    Nt=4;
    Ns=4;
    Nm=4;
    Di=8;
    Dq=8;
    Base=50176;
    Wr_offs=0;
    Rd_offs=8;
    Comedi_file=0;
    Comedi_subdev_i=0;
    Comedi_subdev_q=1;
    memset((void*)Pipe, 0, MAXSTR);
    memset((void*)Responsefile, 0, MAXSTR);
    memset((void*)SimInFile, 0, MAXSTR);
    memset((void*)SimOutFile, 0, MAXSTR);
    memset((void*)Hw, 0, MAXSTR);

    memset((void*)line, 0, MEDSTR);
    memset((void*)name, 0, SMALLSTR);
    memset((void*)val, 0, NICKLEN);

    if ((fcfg = fopen(path, "r")))
	{
		while (fgets(line, MEDSTR, fcfg))
		{
			sscanf(line, "%s\t%s", name, val);
			if (!strcmp(name, "STEP"))
				Step = atoi(val);
			if (!strcmp(name, "SIGENABLE"))
				Sigenable = atoi(val);
			if (!strcmp(name, "PAGELEN"))
                ScreenWidth = atoi(val);
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
            if (!strcmp(name, "COMEDI_SUBDEV_I"))
				Comedi_subdev_i = atoi(val);
			if (!strcmp(name, "COMEDI_SUBDEV_Q"))
				Comedi_subdev_q = atoi(val);
			if (!strcmp(name, "HW"))
				sprintf(Hw, "%s", val);
			if (!strcmp(name, "PIPE"))
				sprintf(Pipe, "%s", val);
			if (!strcmp(name, "RESPONSE"))
				sprintf(Responsefile, "%s", val);
            if (!strcmp(name, "SIM_INPUT"))
                sprintf(SimInFile, "%s", val);
            if (!strcmp(name, "SIM_OUTPUT"))
                sprintf(SimOutFile, "%s", val);
			memset(line, 0, MEDSTR);
			memset(name, 0, SMALLSTR);
		}
		fclose(fcfg);
        ErrLog = fopen(LOG,"w+");
		if (Step > 0
		&& Sigenable > 29
        && ScreenWidth > 23
		&& Pagewidth > 79
		&& Nt >= 0
		&& Ns >= 0
		&& Nm >= 0
		&& Di >= 0
		&& Dq >= 0
		&& Wr_offs >= 0
		&& Rd_offs >= 0
		&& Base > 0)
            return PLC_OK;
	}
    return PLC_ERR;
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
    ui_toggle_enabled();
}

void init_emu()
{
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
    memset(plc.m, 0, Nm * sizeof(struct mvar));

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
    memcpy(pOld->t, plc.dq, Nt * sizeof(struct timer));
    memcpy(pOld->s, plc.dq, Ns * sizeof(struct blink));
	plc.command = 0;
	plc.status = TRUE;
    Update = TRUE;
	signal(Sigenable, sigenable);
    PlcCom[0].fd = open(Pipe, O_NONBLOCK | O_RDONLY);
    PlcCom[0].events = POLLIN | POLLPRI;
    gettimeofday(&T, NULL);
	PLC_init();
}

/**
 * @brief extract alphanumeric variable name from file line starting
 *at index start
 * @in line
 * @out name
 * @param start
 * @return index position
 */
int extract_name(const char* line, char* name, int start )
{
    int j = start;
    int k = 0;
    memset(name, 0, SMALLSTR);
    while (isspace(line[j]))    //get name
        j++;
    while (isalpha(line[j])
           && k < SMALLSTR
           && j < MAXSTR)
        name[k++] = line[j++];
    return j;
}

/**
 * @brief extract numeric index from file line starting
 *at index start
 * @in line
 * @out name
 * @param start
 * @return index position
 */
int extract_index(const char* line, int idx, int start)
{
    char idx_str[SMALLSTR];
    int j = start;
    int k = 0;
    memset(idx_str, 0, SMALLSTR);

    while (isspace(line[j]))    //get index
        j++;
    while (isdigit(line[j])
           && k < SMALLSTR
           && j < MAXSTR)
        idx_str[k++] = line[j++];
    return atoi(idx_str);
}


/**
 * @brief extract configuration value from file line starting
 *at index start, drop comments
 * @in line
 * @out val
 * @param start
 * @return index position
 */
int extract_value( const char* line, char * val, int start)
{
    int k = 0;
    int j = start;
    memset(val, 0, SMALLSTR);
    while (isspace(line[j]))    //get value
        j++;

    while(line[j] && k < 16
       && line[j] != 10
       && line[j] != 13
       && line[j] != ';'
       && line[j] != '\t'
       && k < SMALLSTR
       && j < MAXSTR)
        val[k++] = line[j++];
    return j;
}

int configure_input(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "I"))
    {
        if (idx >= BYTESIZE * Di)
            return ERR_BADINDEX;
        sprintf(plc.di[idx].nick, "%s", val);
    }
    return PLC_OK;
}

int configure_output(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "Q"))
    {
        if (idx >= BYTESIZE * Dq)
            return ERR_BADINDEX;
        sprintf(plc.dq[idx].nick, "%s", val);
    }
    return PLC_OK;
}

int configure_register(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "M"))
    {
        if (idx >= Nm)
            return ERR_BADINDEX;
        sprintf(plc.m[idx].nick, "%s", val);
    }
    return PLC_OK;
}

int configure_timer(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "T"))
    {
        if (idx >= Nt)
            return ERR_BADINDEX;
        sprintf(plc.t[idx].nick, "%s", val);
    }
    return PLC_OK;
}

int configure_blinker(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "B"))
    {
        if (idx >= Ns)
            return ERR_BADINDEX;
        sprintf(plc.s[idx].nick, "%s", val);
    }
    return PLC_OK;
}

int configure_serial(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "COM"))
    {
        if (idx >= MEDSTR)
            return ERR_BADINDEX;
        sprintf(com_nick[idx], "%s", val);
    }
    return PLC_OK;
}

int init_register(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "MEMORY"))
    {
        if (idx >= Nm)
            return ERR_BADINDEX;
        plc.m[idx].V = atol(val);
    }
    return PLC_OK;
}

int define_reg_direction(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "COUNT"))
    {
        if (idx >= Nm)
            return ERR_BADINDEX;
        if (!strcmp(val, "DOWN"))
            plc.m[idx].DOWN = TRUE;
        else
            return ERR_BADOPERATOR;
    }
    return PLC_OK;
}

int define_reg_readonly(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "COUNTER"))
    {
        if (idx >= Nm)
            return ERR_BADINDEX;
        if (!strcmp(val, "OFF"))
            plc.m[idx].RO = TRUE;
        else
            return ERR_BADOPERATOR;
    }
    return PLC_OK;
}

int init_timer_set(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "TIME"))
    {
        if (idx >= Nt)
            return ERR_BADINDEX;
        plc.t[idx].S = atoi(val);
    }
    return PLC_OK;
}

int init_timer_preset(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "PRESET"))
    {
        if (idx >= Nt)
            return ERR_BADINDEX;
        plc.t[idx].P = atoi(val);
    }
    return PLC_OK;
}

int init_timer_delay(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "DELAY"))
    {
        if (idx >= Nt)
            return ERR_BADINDEX;
        if (!strcmp(val, "ON"))
            plc.t[idx].ONDELAY = TRUE;
        else
            return ERR_BADOPERAND;
    }
    return PLC_OK;
}

int init_blinker_set(const char* name,const int idx,const char* val)
{
    if (!strcmp(name, "BLINK"))
    {
        if (idx >= Ns)
            return ERR_BADINDEX;
        plc.s[idx].S = atoi(val);
    }
    return PLC_OK;
}

int plc_load_file(char * path)
{ //ini =1 if file is loaded initially, i.e. messages should be printf'd not draw_info_line'd
	FILE * f;
    char * tab = 0;
    int idx = 0;
    int r = 0;
    int lineno=0;
    int i=0;
    int j=0;
    int found_start = FALSE;
    char line[MAXSTR], name[SMALLSTR], val[SMALLSTR];
    if ((f = fopen(path, "r")))
	{
        memset(line, 0, MAXSTR);
		disable_bus();
        while (fgets(line, MAXSTR, f))
		{    //read initialization values
            j = extract_name(line, name, j);
            if(!strcmp(name, "LD")
            || !strcmp(name, "IL"))
            {
                found_start = TRUE;
                break;
            }
            /*if ((line[0] == 'L' && line[1] == 'D')
                    || (line[0] == 'I' && line[1] == 'L'))

			{    //or 'IL' for IL
				found_start = TRUE;
            }
            else*/ if (!found_start)
			{
                memset(name, 0, SMALLSTR);
                memset(val, 0, NICKLEN);
                idx = extract_index(line, idx, j);
                if (idx < 0)
                {
                    r = ERR_BADINDEX;
                    break;
                }
                extract_value( line, val, j);
                lineno++;
                if((r = configure_input(name, idx, val))<0)
                    break;
                if((r = configure_output(name, idx, val))<0)
                    break;
                if((r = configure_register(name, idx, val))<0)
                    break;
                if((r = configure_timer(name, idx, val))<0)
                    break;
                if((r = configure_blinker(name, idx, val))<0)
                    break;
                if((r = configure_serial(name, idx, val))<0)
                    break;
                if((r = init_register(name, idx, val))<0)
                    break;
                if((r = define_reg_direction(name, idx, val))<0)
                    break;
                if((r = define_reg_readonly(name, idx, val))<0)
                    break;
                if((r = init_timer_set(name, idx, val))<0)
                    break;
                if((r = init_timer_preset(name, idx, val))<0)
                    break;
                if((r = init_timer_delay(name, idx, val))<0)
                    break;
                if((r = init_blinker_set(name, idx, val))<0)
                    break;
                if (name[0] != ';'
                        && isalnum(name[0]) != 0
                        && strcmp(name, "LD")
                        && strcmp(name, "IL"))
				{
					r = ERR_BADOPERAND;
					break;
				}
			}
			else
            {//copy line
				while (strchr(line, '\t') != NULL )    //tabs are not supported
				{
					tab = strchr(line, '\t');
					*tab = '.';
				}
				memset(Lines[i], 0, MAXSTR);
				memset(Labels[i], 0, MAXSTR);
				sprintf(Lines[i++], "%s", line);
			}
            r = PLC_OK;
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
            plc_log( "Invalid filename:!");
			break;
		case ERR_BADINDEX:
            plc_log("%s:%d:Index out of bounds!", path, lineno);
			break;
		case ERR_BADOPERAND:
            plc_log("%s:%d:Invalid word %s!", path, lineno, name);
			break;
		case ERR_BADOPERATOR:
            plc_log("%s:%d:Invalid word %s!", path, lineno, val);
			break;
		default:
			break;
		}
		return ERROR;
	}
	else
        return PLC_OK;
}

int plc_func(int daemon)
{
	struct timeval tp, tn, dt;
    static long timeout = 0;
    BYTE i_bit = 0;
    BYTE com[2];
    int q_bit = 0;
    int n=0;
    int i=0;
    int j=0;
	int written=FALSE;
	int rfd = 0; //response file descriptor
    int r = PLC_OK;
	int i_changed = FALSE;
	int o_changed = FALSE;
	int m_changed = FALSE;
	int t_changed = FALSE;
	int s_changed = FALSE;
    //char test[NICKLEN];
    dt.tv_sec = 0;
    dt.tv_usec = 0;
	if ((plc.status) % 2)    //run
	{    //read inputs
        dio_fetch(timeout);
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
        gettimeofday(&tn,NULL);	//how much time passed for previous cycle?
        timeval_subtract(&dt, &tn, &T);
        dt.tv_usec = dt.tv_usec % (THOUSAND * Step);
        timeout = Step - dt.tv_usec / THOUSAND;
//plc_log("Refresh time approx:%d microseconds",dt.tv_usec);
        written = poll(PlcCom, 0, timeout);
//plc_log("Poll time approx:%d milliseconds",dt.tv_usec/1000);
        gettimeofday(&tp, NULL);	//how much time did poll wait?
        timeval_subtract(&dt, &tp, &tn);
		if (written)
        {//manage serial comm
            if (read(PlcCom[0].fd, com, 2))
			{
				if (com[0] == 0)
					com[0] = 0;	//NOP
				else
					plc.command = com[0] - ASCIISTART;
                plc_log("LAST command:%d, %s", plc.command,
                       com_nick[com[0] - ASCIISTART]);
            }
		}
		else if (written == 0)
			plc.command = 0;
		else
		{
            plc_log("PIPE ERROR\n");
			plc.command = 0;
		}
//	sprintf(test,"Poll time approx:%d milliseconds",dt.tv_usec/1000);

        gettimeofday(&T, NULL);	//start timing next cycle
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
        dio_flush();
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
            write(rfd, &(plc.response), 1);
            close(rfd);
            plc.response = 0;
		}
	}
    else
    {
        usleep(Step * THOUSAND);
        timeout = 0;
    }
    return r;
}

int main(int argc, char **argv)
{
    int i, errcode, daemon_flag = FALSE;
    int more = 0;
    char confstr[SMALLSTR], inistr[SMALLSTR];

	strcpy(confstr, "plc.config");
    sprintf(inistr, " ");
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
                    return PLC_ERR;
				}
			}
		}
	}
	if (init_config(confstr) < 0)
	{
		printf("Invalid configuration file\n");
        return PLC_ERR;
	}

    init_emu();

    if (inistr[0] && plc_load_file(inistr) < 0)
		printf("Invalid program file\n");

	if (daemon_flag == FALSE)
    {
        more = ui_init(ScreenWidth);
        UiReady=more;
    }
    while (more)
    {
        if(daemon_flag == FALSE)
        {
            if(Update)
                ui_draw();
            more = ui_update(more);
        }
        else
            more = plc.status;
        errcode = plc_func(FALSE);
        if (errcode < 0)
        {
            plc_log("error %d: %s", -errcode, ErrMsg[1 - errcode]);
            plc.status = 0;
        }
	}
    fclose(ErrLog);
	disable_bus();
    if (daemon_flag)
        ui_end();
    return 0;
}
