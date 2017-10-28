#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
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

void print_help()
{
	FILE * f;
    char line[MAXSTR];
    if ((f = fopen("./help", "r"))){
		while (fgets(line, MEDSTR, f)){
         //read help file
            printf("%s", line);
        }
		fclose(f);
	}
}

void * read_cli(void *buf) {
    
    size_t len = sizeof(buf);
    memset(buf, 0, len);
    
    int n = getline((char **)&buf, &len, stdin);
    
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
    print_config_yml(stdout, state);
    time_header();
}

config_t ui_init_command(){
    config_t com = new_config(N_CONFIG_VARIABLES);
    return update_entry(CLI_COM, new_entry_int(0, "COMMAND"), com);
}

config_t ui_init_state(){
    config_t stat = new_config(N_CONFIG_VARIABLES);
    return update_entry(CLI_COM, new_entry_int(0, "STATUS"), stat);
}

int ui_init(const config_t conf)
{
 //   init_help();

    Cli_buf = (char*)malloc(MAXBUF);
    memset(Cli_buf, 0, MAXBUF);
    int rc = pthread_create(&Reader, NULL, read_cli, (void *) Cli_buf);
    
    return rc;
}

config_t parse_cli(const char * input, config_t command){
    if(!strncasecmp(input, "HELP" , 4)){
        print_help();
        command = set_numeric_entry(CLI_COM, COM_HELP, command);
    } else if(!strncasecmp(input, "QUIT" , 4)){
    
        command = set_numeric_entry(CLI_COM, COM_QUIT, command);
    } else if(!strncasecmp(input, "START" , 5)){
    
        command = set_numeric_entry(CLI_COM, COM_START, command);
    } else if(!strncasecmp(input, "STOP" , 4)){

        command = set_numeric_entry(CLI_COM, COM_STOP, command);
    }
    plc_log("CLI: %s", input);
    
    return command;
}

config_t ui_update(config_t command)
{
    //time_header();
    if(Cli_buf[0]){
        config_t c = parse_cli(Cli_buf, command);
        pthread_join(Reader, NULL);
        pthread_create(&Reader, NULL, read_cli, (void *) Cli_buf);
        return c;
    }
    return command;
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
