#include <string.h>
#include <time.h>
#include "config.h"
#include "hardware.h"
#include "data.h"
#include "instruction.h"
#include "rung.h"
#include "plclib.h"

#include "ui.h"
#include "plcemu.h"

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

config_t cli_init_command(config_t conf){
    config_t com = new_config(N_CONFIG_VARIABLES);
    com = update_entry(CLI_COM, new_entry_int(COM_NONE, "COMMAND"), com);
    return copy_sequences(conf, com);
}

config_t cli_init_state(config_t conf){
    config_t stat = new_config(N_CONFIG_VARIABLES);
    stat = update_entry(CLI_COM, new_entry_int(0, "STATUS"), stat);
    return copy_sequences(conf, stat);
}

config_t cli_parse( char * input, config_t command){
    
    char * filename = NULL;
    char * block = NULL;
    char * index = NULL;
    char * value = NULL;
    char * key = NULL;
    if(command == NULL
    || input == NULL){
        return command;
    }
    command = set_numeric_entry(CLI_COM, COM_NONE, command);
    strtok(input, " \n");
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
        filename = strtok(NULL, " \n");
        if(filename != NULL){
            command = update_entry(CLI_ARG, 
                new_entry_str(filename, "FILE"),
                command);
        }
    } else if(!strncasecmp(input, "FORCE" , 5)){
        //parse rest
        block = strtok(NULL, " \n");
        index = strtok(NULL, " \n");
        value = strtok(NULL, " \n");
        if(block != NULL 
        && index != NULL
        && value != NULL){
            sequence_t s = edit_seq_param(command, 
                                        block, 
                                        atoi(index), 
                                        "FORCE",
                                        value);              
            if(s!=NULL){
                command = set_numeric_entry(CLI_COM, COM_FORCE, command);
            }
        }
    } else if(!strncasecmp(input, "UNFORCE" , 7)){
        //parse rest
        block = strtok(NULL, " \n");
        index = strtok(NULL, " \n");
        if(block != NULL 
        && index != NULL){
            sequence_t s = edit_seq_param(command, 
                                        block, 
                                        atoi(index), 
                                        "FORCE",
                                        "UNFORCE");              
            if(s!=NULL){
                command = set_numeric_entry(CLI_COM, COM_UNFORCE, command);
            }
        }    
    } else if(!strncasecmp(input, "EDIT" , 4)){
        //parse rest
        block = strtok(NULL, " \n");
        index = strtok(NULL, " \n");
        key   = strtok(NULL, " \n");
        value = strtok(NULL, " \n");
        if(block != NULL 
        && index != NULL
        && key   != NULL
        && value != NULL){
            sequence_t s = edit_seq_param(command, 
                                        block, 
                                        atoi(index), 
                                        key,
                                        value);              
            if(s!=NULL){
                command = set_numeric_entry(CLI_COM, COM_EDIT, command);
            }
        }
    }
    //plc_log("CLI: %s", input);
    return command;
}

