#include <string.h>
#include <time.h>

#include "data.h"
#include "instruction.h"
#include "rung.h"
#include "plclib.h"
#include "plcemu.h"
#include "config.h"
#include "ui.h"

/*************GLOBALS************************************************/

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


char lasttime[TINYSTR] = "";
void cli_header()
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

config_t cli_parse( char * input, config_t command){
    strtok(input, " \n");
    char * filename = NULL;
    if(!strncasecmp(input, "HELP" , 4)){
        print_help();
        command = set_numeric_entry(CLI_COM, COM_HELP, command);
    } else if(!strncasecmp(input, "QUIT" , 4)){
    
        command = set_numeric_entry(CLI_COM, COM_QUIT, command);
    } else if(!strncasecmp(input, "START" , 5)){
    
        command = set_numeric_entry(CLI_COM, COM_START, command);
    } else if(!strncasecmp(input, "STOP" , 4)){

        command = set_numeric_entry(CLI_COM, COM_STOP, command);
    } else if(!strncasecmp(input, "LOAD" , 4)){
    
        command = set_numeric_entry(CLI_COM, COM_LOAD, command);
        //parse rest
        filename = strtok(NULL, " \n");
        if(filename != NULL){
            command = update_entry(CLI_ARG, 
                new_entry_str(filename, "FILE"), 
                command);
        }
        
    } else if(!strncasecmp(input, "SAVE" , 4)){
        //parse rest
        command = set_numeric_entry(CLI_COM, COM_SAVE, command);
        filename = strtok(NULL, " ");
        if(filename != NULL){
            command = update_entry(CLI_COM + 1, 
                new_entry_str(filename, "FILE"),
                command);
        }
    }
    
    //plc_log("CLI: %s", input);
    
    return command;
}

