#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <time.h>
#include <zmq.h>

#include "data.h"
#include "instruction.h"
#include "rung.h"
#include "plclib.h"
#include "plcemu.h"
#include "config.h"
#include "ui.h"

/*************GLOBALS************************************************/

char * Cli_buf = NULL; 
char * Response_buf = NULL;
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
    
    size_t l = 0;
    char * b = NULL;
    int n = getline((char **)&b, &l, stdin);
    if(n >=0){
        sprintf(buf, "%s", b);
        free(b);
    }
    return buf;
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
    //plc_log("CLI: %s", input);
    
    return command;
}

//  UI client

int main (void)
{
    
    Cli_buf = (char*)malloc(CONF_STR);
    Response_buf = (char*)malloc(CONF_STR);
    memset(Cli_buf, 0, CONF_STR);
    config_t def = init_config();
    config_t command = copy_sequences(def, ui_init_command());
    void *context = zmq_ctx_new ();
    void *requester = zmq_socket (context, ZMQ_REQ);
    printf ("Connecting to PLC EMU...\n");
    zmq_connect (requester, "tcp://localhost:5555");
    
    for(;;){
        zmq_recv (requester, 
                    Response_buf, 
                    CONF_STR, 
                    ZMQ_DONTWAIT); 
        printf("Got %s...\n", Response_buf);
        Cli_buf = read_cli(Cli_buf);
        command = parse_cli(Cli_buf, command);
        char * serialized = serialize_config(command);
        printf("Sending %s...\n", serialized);
        zmq_send (requester, 
                  serialized, 
                  strlen(serialized),
                  ZMQ_DONTWAIT);
                             
        free(serialized);
    }
    zmq_close (requester);
    zmq_ctx_destroy (context);
    
    return 0;
}
