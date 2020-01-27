#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <time.h>
#include <pthread.h>

#include "data.h"
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

void ui_draw(config_t state)
{
    print_config(stdout, state);
    cli_header();
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

int ui_init(const config_t conf)
{
    Cli_buf = (char*)malloc(MAXBUF);
    memset(Cli_buf, 0, MAXBUF);
    int rc = pthread_create(&Reader, NULL, read_cli, (void *) Cli_buf);
    
    return rc;
}

config_t ui_update(config_t command)
{
    command = set_numeric_entry(CLI_COM, COM_NONE, command);
    
    if(Cli_buf != NULL && Cli_buf[0]){
        config_t c = cli_parse(Cli_buf, command);
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
