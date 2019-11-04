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
#include "hardware.h"
#include "plclib.h"
#include "plcemu.h"
#include "config.h"
#include "ui.h"

/*************GLOBALS************************************************/

char * Cli_buf = NULL; 
char * Response_buf = NULL;
char * Update_buf = NULL;

pthread_t Reader;
config_t init_config(){
 //TODO: in a c++ implementation this all can be done automatically 
 //using a hashmap
    config_t conf = new_config(N_CONFIG_VARIABLES);
    conf = update_entry(
        CONFIG_TIMER,
        new_entry_seq(new_sequence(4), "TIMERS"),
        conf);
    
    conf = update_entry(
        CONFIG_PULSE,
        new_entry_seq(new_sequence(4), "PULSES"),
        conf);
        
    conf = update_entry(
        CONFIG_MREG,
        new_entry_seq(new_sequence(4), "MREG"),
        conf);
        
    conf = update_entry(
        CONFIG_MVAR,
        new_entry_seq(new_sequence(4), "MVAR"),
        conf);
    
    conf = update_entry(
        CONFIG_DI,
        new_entry_seq(new_sequence(8), "DI"),
        conf);
 
    conf = update_entry(
        CONFIG_DQ,
        new_entry_seq(new_sequence(8), "DQ"),
        conf);
    
    conf = update_entry(
        CONFIG_AI,
        new_entry_seq(new_sequence(8), "AI"),
        conf);
    
    conf = update_entry(
        CONFIG_AQ,
        new_entry_seq(new_sequence(8), "AQ"),
        conf);

    conf = update_entry(
        CONFIG_PROGRAM,
        new_entry_seq(new_sequence(2), "PROGRAM"),
        conf);
}

void * read_cli(void * sock) {
    
    size_t l = 0;
    char * b = NULL;
    
    config_t command = cli_init_command(init_config());
    while(getline((char **)&b, &l, stdin)>=0){
        command = cli_parse(b, command);
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
    }
    zmq_close (subscriber);
    zmq_close (requester);
    zmq_ctx_destroy (context);
    
    return 0;
}
