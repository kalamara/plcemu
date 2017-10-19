#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <pthread.h>


#include "data.h"
#include "instruction.h"
#include "rung.h"
#include "plclib.h"
#include "plcemu.h"
#include "config.h"
#include "ui.h"

/*************GLOBALS************************************************/

int Enable = TRUE;
int More = TRUE;
char * Cli_buf = NULL; //only reader thread writes here
pthread_t Reader;

void ui_display_message(char * msgstr)
{
    printf("%s\n", msgstr);
}

void init_help()
{
	FILE * f;
    char line[MAXSTR], helpline[MAXSTR];
    if ((f = fopen("./help", "r"))){
		while (fgets(line, MEDSTR, f))
         //read help file
            sprintf(helpline, " %s\n", line);
		fclose(f);
	}
}

void * read_cli(void *buf) {
    
    int len = sizeof(buf);
    memset(buf, 0, len);
    
    int n = getline((char **)&buf, &len, stdin);
    plc_log("CLI: %s!\n", buf);

    return NULL;
}


char lasttime[TINYSTR] = "";
void time_header()
{
	char t[TINYSTR], *p;
	char str[MEDSTR] = "";
	char buf[SMALLSTR] = "";
	time_t now;

	time(&now);
	strcpy(t, ctime(&now));
	t[19] = '\0';
	p = t + 10;
	if(strcmp(t, lasttime))
    {
        //sprintf(buf, "%s","\033[2J"); // Clear screen
        //strcat(str,buf);
        sprintf(buf," PLC-EMUlator v%4.2f %14s\n ", PRINTABLE_VERSION, p);
        strcat(str,buf);
        sprintf(lasttime, "%s", t);
        ui_display_message(str);
     }
}

void ui_draw(config_t state)
{
    char str[MAXSTR];
    char buf[MEDSTR];
    time_t now;
    char t[TINYSTR], *timestr;    
    int i = 0;
    char instr[SMALLSTR];
    char outstr[SMALLSTR];
    char mstr[SMALLSTR];
    
    memset(str,0,MAXSTR);
    memset(buf,0,MEDSTR);
    memset(instr,0,SMALLSTR);
    memset(outstr,0,SMALLSTR);
    memset(mstr,0,SMALLSTR);
    
    for(i = 0;i < BYTESIZE * Plc->ni; i++)
        instr[i] = (((Plc->inputs[i/BYTESIZE])>>(i%BYTESIZE)) % 2)?'1':'0';
    
    for(i = 0;i < BYTESIZE * Plc->nq; i++)
        outstr[i] = (((Plc->outputs[i/BYTESIZE])>>(i%BYTESIZE)) % 2)?'1':'0';
    
    sprintf(buf, "Inputs: %s \n", instr);
    ui_display_message(buf);

    sprintf(buf, "Outputs: %s \n", outstr);
    ui_display_message(buf);
    
    for(i = 0; i < Plc->nai; i++){
        sprintf(instr, "%lx", Plc->real_in[i]);  
        sprintf(buf, "Analog Input %d (raw): 0x%s \n", i, instr);
        ui_display_message(buf);
        sprintf(instr, "%lf", Plc->ai[i].V);
        sprintf(buf, "Analog Input %d : %s \n", i, instr);
        ui_display_message(buf);
    }    
    
    for(i = 0; i < Plc->naq; i++){
        sprintf(outstr, "%lx", Plc->real_out[i]);  
        sprintf(buf, "Analog Output %d (raw): %s \n", i, outstr);
        ui_display_message(buf);
        sprintf(outstr, "%lf", Plc->aq[i].V);
        sprintf(buf, "Analog Output %d : %s \n", i, outstr);
        ui_display_message(buf);
    }
    for(i = 0; i < Plc->nm; i++){
        sprintf(mstr, "%lx", Plc->m[i].V);  
        sprintf(buf, "Memory Register %d (u): %s \n", i, mstr);
        ui_display_message(buf);
    }
    for(i = 0; i < Plc->nmr; i++){
        sprintf(mstr, "%lf", Plc->mr[i].V);
        sprintf(buf, "Memory Register %d (real): %s \n", i, mstr);
        ui_display_message(buf);
    }    
        
    for(i = 0; i < Lineno; i++)
        ui_display_message(Lines[i]);
    
    if (Plc->status % 2 == ST_RUNNING) //running
        sprintf(buf, "Hardware:%s  RUNNING\n", Plc->hw);
    else
        sprintf(buf, "Hardware:%s  STOPPED\n", Plc->hw);
    
    strcat(str,buf);
    time(&now);
	strcpy(t, ctime(&now));
	t[19] = '\0';
	timestr = t + 10;
	sprintf(buf," PLC-EMUlator v%4.2f %14s\n ", PRINTABLE_VERSION, timestr);

    strcat(str,buf);
    ui_display_message(str);
}

int ui_init()
{
    
    init_help();
    Cli_buf = (char*)malloc(MAXBUF);
    int rc = pthread_create(&Reader, NULL, read_cli, (void *) Cli_buf);
    //rc = pthread_join(reader, NULL);
    return 1;
}

config_t parse_cli(char * input){
    return NULL;
}
config_t ui_update()
{
    //time_header();
    if(Cli_buf[0]){
        config_t c = parse_cli(Cli_buf);
        pthread_join(Reader, NULL);
       // memset(Cli_buf, 0, MAX_BUF);
        pthread_create(&Reader, NULL, read_cli, (void *) Cli_buf);
        return c;
    }
    return NULL;
}

void ui_end()
{
    More = FALSE;
    
    return;
}

void ui_toggle_enabled()
{
    Enable = Enable ? 0 : 1;
}
