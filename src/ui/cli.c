#include <string.h>
#include <time.h>
#include "config.h"
#include "schema.h"
//#include "hardware.h"
#include "data.h"
#include "instruction.h"
#include "rung.h"
#include "plclib.h"

#include "ui.h"
#include "util.h"
#include "app.h"
#include "plcemu.h"

/*************GLOBALS************************************************/
extern struct entry ConfigSchema[];

const char * Command[N_COM] = {
        "",
        "START",
        "STOP",
        "HELP",
        "FORCE",
        "UNFORCE",
        "EDIT",
        "LOAD",
        "SAVE",
        "CONFIG",
        "QUIT"
};

struct entry CommandSchema[N_PAYLOADS] = {
    {//CLI_COM,
         .type_tag = ENTRY_INT,
         .name = "COMMAND",
         .e = {
                .scalar_int = 0
         }
    },
    {//CLI_ARG,
         .type_tag = ENTRY_MAP,
         .name = "ARG",
         .e = {
              .conf = NULL
         }
    },
};

struct entry StatusSchema[N_PAYLOADS] = {
    {//CLI_COM,
         .type_tag = ENTRY_INT,
         .name = "STATUS",
         .e = {
                .scalar_int = 0
         }
    },
    {//CLI_ARG,
         .type_tag = ENTRY_MAP,
         .name = "ARG",
         .e = {
              .conf = NULL
         }
    },
};

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
    
    config_t com = init_config(CommandSchema, N_PAYLOADS);
    com = set_recursive_entry(CLI_ARG, copy_config(conf), com);
    return com;
}

config_t cli_init_state(config_t conf){

    config_t stat = init_config(StatusSchema, N_PAYLOADS);
    stat = set_recursive_entry(CLI_ARG, copy_config(conf), stat);
    
    return stat;
}

config_t cli_parse( char * input, config_t command){
    
    char * filename = NULL;
    char * block = NULL;
    char * index = NULL;
    char * value = NULL;
    char * key = NULL;
    config_t res = command;
    if(command == NULL
    || input == NULL){
        return command;
    }
    command = set_numeric_entry(CLI_COM, COM_NONE, command);
    config_t arg = get_recursive_entry(CLI_ARG, res);
    
    strtok(input, " \n");
    if(!strncasecmp(input, Command[COM_HELP] , 4)){
        print_help();
        command = set_numeric_entry(CLI_COM, COM_HELP, command);
    } else if(!strncasecmp(input, Command[COM_QUIT] , 4)){
    
        command = set_numeric_entry(CLI_COM, COM_QUIT, command);
    } else if(!strncasecmp(input, Command[COM_START] , 4)){
    
        command = set_numeric_entry(CLI_COM, COM_START, command);
    } else if(!strncasecmp(input, Command[COM_STOP] , 4)){

        command = set_numeric_entry(CLI_COM, COM_STOP, command);
    } else if(!strncasecmp(input, Command[COM_LOAD] , 4)){
    
        command = set_numeric_entry(CLI_COM, COM_LOAD, command);
        //parse rest
        filename = strtok(NULL, " \n");
        if(filename != NULL){
            config_t conf = init_config(ConfigSchema, N_CONFIG_VARIABLES);    
            if ((load_config(filename, conf))->err < PLC_OK) {
                    plc_log("Invalid configuration file %s\n", filename);
            } else {
                    //a = configure(a->conf, a);
            }    
            /*command = update_entry(CLI_ARG, 
                new_entry_str(filename, "FILE"), 
                command);*/
        }
    } else if(!strncasecmp(input, Command[COM_SAVE] , 4)){
        //parse rest
        command = set_numeric_entry(CLI_COM, COM_SAVE, command);
        filename = strtok(NULL, " \n");
        if(filename != NULL){
        /*    command = update_entry(CLI_ARG, 
                new_entry_str(filename, "FILE"),
                command);*/
        }
    } else if(!strncasecmp(input, Command[COM_FORCE] , 4)){
        //parse rest
        block = strtok(NULL, " \n");
        index = strtok(NULL, " \n");
        value = strtok(NULL, " \n");
        if(block != NULL 
        && index != NULL
        && value != NULL){
            
            res = edit_seq_param(arg,
                                 block,
                                 atoi(index),
                                 Command[COM_FORCE],
                                 value);
            if(res){                                          
                command = set_numeric_entry(CLI_COM, COM_FORCE, command);
//                update recursive entry
                command = set_recursive_entry(CLI_ARG, res, command);
            }
        }
    } else if(!strncasecmp(input, Command[COM_UNFORCE] , 4)){
        //parse rest
        block = strtok(NULL, " \n");
        index = strtok(NULL, " \n");
        if(block != NULL 
        && index != NULL){
            res = edit_seq_param(arg,
                                 block,
                                 atoi(index),
                                 "FORCE",
                                 "UNFORCE");
            if(res){                                          
                command=set_numeric_entry(CLI_COM, COM_UNFORCE, command);
            }
        }    
    } else if(!strncasecmp(input, Command[COM_EDIT] , 4)){
        //parse rest
        block = strtok(NULL, " \n");
        index = strtok(NULL, " \n");
        key   = strtok(NULL, " \n");
        value = strtok(NULL, " \n");
        if(block != NULL 
        && index != NULL
        && key   != NULL
        && value != NULL){
            res = edit_seq_param(arg,
                                 block,
                                 atoi(index),
                                 key,
                                 value);
            if(res){                                          
                command = set_numeric_entry(CLI_COM, COM_EDIT, command);
            }
        }
    }//TODO: CONF command
    /*if(get_numeric_entry(CLI_COM, command)!=COM_NONE){
        char * serialized = serialize_config(command);
        plc_log("CLI: %s", serialized);
        if(serialized){
                free(serialized);
                serialized = NULL;
        }
    }*/
    return command;
}

