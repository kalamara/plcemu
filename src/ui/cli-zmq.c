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
char * Cli_buf = NULL; //only reader thread writes here

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
    
    size_t l = 0;
    char * b = NULL;
    int n = getline((char **)&b, &l, stdin);
    if(n >=0){
        sprintf(buf, "%s", b);
        free(b);
    }
    return buf;
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
        sprintf(buf,
                " PLC-EMUlator v%4.2f %14s\n ", 
                PRINTABLE_VERSION, p);
        strcat(str,buf);
        sprintf(lasttime, "%s", t);
        ui_display_message(str);
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

//  UI client

int main (void)
{
    printf ("Connecting to PLC EMU...\n");
    void *context = zmq_ctx_new ();
    void *requester = zmq_socket (context, ZMQ_REQ);
    zmq_connect (requester, "tcp://localhost:5555");

    int request_nbr;
    /*for (request_nbr = 0; request_nbr != 10; request_nbr++) {
        char buffer [10];
        printf ("Sending Hello %d...\n", request_nbr);
        zmq_send (requester, "Hello", 5, 0);
        zmq_recv (requester, buffer, 10, 0);
        printf ("Received World %d\n", request_nbr);
    }*/
    zmq_close (requester);
    zmq_ctx_destroy (context);
    return 0;
}
