#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <time.h>
#include <zmq.h>
#include <pthread.h>

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
char * Update_buf = NULL;

pthread_t Reader;

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

void * read_cli(void * sock) {
    
    size_t l = 0;
    char * b = NULL;
    
    config_t command = copy_sequences(init_config(), ui_init_command());
    while(getline((char **)&b, &l, stdin)>=0){
        command = parse_cli(b, command);
        char * serialized = serialize_config(command);
        printf("Sending... \n%s\n", serialized);
        zmq_send (sock, 
                  serialized, 
                  strlen(serialized),
                  0);
        zmq_recv (sock, 
                    Response_buf, 
                    CONF_STR, 
                    0); 
        printf("Response:\n %s\n", Response_buf);                     
        
        free(b);
        free(serialized);
    }
    return NULL;
}

//  UI client

int main (void)
{    
    Cli_buf = (char*)malloc(CONF_STR);
    Response_buf = (char*)malloc(CONF_STR);
    Update_buf = (char*)malloc(CONF_STR);
    memset(Cli_buf, 0, CONF_STR);

    void *context = zmq_ctx_new ();
    void *requester = zmq_socket (context, ZMQ_REQ);
    void *subscriber = zmq_socket (context, ZMQ_SUB);
    
    printf ("Connecting to PLC EMU...\n");
    zmq_connect (requester, "tcp://localhost:5555");
    zmq_connect (subscriber, "tcp://localhost:5556");
    zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, "---", 3);
    pthread_create(&Reader, NULL, read_cli, requester);
    
    for(;;){
        if(zmq_recv (subscriber, 
                    Update_buf, 
                    CONF_STR, 
                    ZMQ_DONTWAIT)>=0){ 
            printf("Update:\n %s\n", Update_buf);
        }
        /*
        Cli_buf = read_cli(Cli_buf);
        command = parse_cli(Cli_buf, command);
        char * serialized = serialize_config(command);
        printf("Sending %s...\n", serialized);
        zmq_send (requester, 
                  serialized, 
                  strlen(serialized),
                  0);
        zmq_recv (requester, 
                    Response_buf, 
                    CONF_STR, 
                    0); 
                             
        free(serialized);*/
    }
    zmq_close (subscriber);
    zmq_close (requester);
    zmq_ctx_destroy (context);
    
    return 0;
}
