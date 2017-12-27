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

int Enable = TRUE;
int More = TRUE;
char * Ui_buf = NULL; 
void * Zmq_context = NULL;
void * Zmq_responder = NULL;
void * Zmq_publisher = NULL;

void ui_display_message(char * msgstr)
{
    printf("%s\n", msgstr);
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
	if(strcmp(t, lasttime)){
        //sprintf(buf, "%s","\033[2J"); // Clear screen
        //strcat(str,buf);
                sprintf(buf,
                " PLC-EMUlator v%4.2f %14s\n ", 
                PRINTABLE_VERSION, p);
                strcat(str,buf);
                sprintf(lasttime, "%s", t);
                ui_display_message(str);
        }
}

void ui_draw(config_t state)
{
//send state for printing
    char * state_buf = serialize_config(state);
    zmq_send (Zmq_publisher, 
                state_buf, 
                strlen(state_buf),
                ZMQ_DONTWAIT);
    free(state_buf);
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
    Ui_buf = (char*)malloc(CONF_STR);
    memset(Ui_buf, 0, CONF_STR);
    Zmq_context = zmq_ctx_new ();
    
    Zmq_responder = zmq_socket (Zmq_context, ZMQ_REP);
    int rc = zmq_bind (Zmq_responder, "tcp://*:5555");
    
    Zmq_publisher = zmq_socket (Zmq_context, ZMQ_PUB);
    if(rc >= 0){  
        rc = zmq_bind (Zmq_publisher, "tcp://*:5556");
    }
    return rc;
}

config_t ui_update(config_t command)
{
    //time_header();
 //peek socket for messages
    int rc = zmq_recv (Zmq_responder, 
                        Ui_buf, 
                        CONF_STR, 
                        ZMQ_DONTWAIT);
    if(rc > 0){
        printf ("Received %s\n", Ui_buf); 
        command = deserialize_config(Ui_buf, command);
    }         
 //deserialize command 
    return command;
}

void ui_end()
{
    More = FALSE;
    zmq_close (Zmq_publisher);
    zmq_close (Zmq_responder);
    zmq_ctx_destroy (Zmq_context);
    return;
}

void ui_toggle_enabled()
{
    Enable = Enable ? 0 : 1;
}
