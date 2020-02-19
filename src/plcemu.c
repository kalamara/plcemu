#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <ctype.h>
#include <math.h>
#include <unistd.h>

#include "config.h"
#include "hardware.h"
#include "util.h"
#include "data.h"
#include "instruction.h"
#include "rung.h"
#include "plclib.h"
#include "parser-tree.h"
#include "parser-il.h"
#include "parser-ld.h"
#include "ui.h"

#include "app.h"
#include "plcemu.h"
/*************GLOBALS************************************************/

app_t App;

typedef enum {
    MSG_PLCERR,
    MSG_BADCHAR,
    MSG_BADFILE,
    MSG_BADOPERAND,
    MSG_BADINDEX,
    MSG_BADCOIL,
    MSG_BADOPERATOR,
    MSG_TIMEOUT,
    MSG_OVFLOW,
    N_ERRMSG
}ERRORMESSAGES;

const char ErrMsg[N_ERRMSG][MEDSTR] = {
        "Something went terribly wrong!",
        "Invalid Symbol!",
        "Wrong File!",
        "Invalid Operand!",
        "Invalid Numeric!",
        "Invalid Output!",
        "Invalid Command!",
        "Timeout!",
        "Stack overflow!"
};

const char LangStr[3][TINYSTR] ={
     "Ladder Diagram",
    "Instruction List",
    "Structured Text"
};


void sigenable() {
    ui_toggle_enabled();
}

/**
* @brief graceful shutdown
*/
void sigkill() {
    plc_log("program interrupted");
    double mean, var = 0;
    unsigned long loop = get_loop();
    get_variance(&mean, &var);
    plc_log("Total loops: %d", loop);
    plc_log("Average loop time: %f us", mean);
    plc_log("Standard deviation: +-%f us", sqrt(var));
    ui_end();
    exit(0);
}

app_t init_emu(const config_t conf) {
    
    app_t a = (app_t)malloc(sizeof(struct app));
    memset(a,0,sizeof(struct app));
    
    a = configure(conf, a);

    //signal(conf->sigenable, sigenable);
    signal(SIGINT, sigkill);
    signal(SIGTERM, sigkill);
    
    project_init();
    
    open_pipe("plcpipe", a->plc);
    
    return a;
}

const char * Usage = "Usage: plcemu [-c config file] \n \
        Options:\n \
        -h displays this help message\n \
        -c uses a configuration file other than config.yml";

void print_error(int errcode)
{
    const char * errmsg;
    switch(errcode) {
        case ERR_BADCHAR:
            errmsg = ErrMsg[MSG_BADCHAR];
            break;
        case ERR_BADFILE:
            errmsg = ErrMsg[MSG_BADFILE];
            break;
        case ERR_BADOPERAND:
            errmsg = ErrMsg[MSG_BADOPERAND];
            break;
        case ERR_BADINDEX:
            errmsg = ErrMsg[MSG_BADINDEX];
            break; 
        case ERR_BADCOIL:
            errmsg = ErrMsg[MSG_BADCOIL];
            break;
        case ERR_BADOPERATOR:
            errmsg = ErrMsg[MSG_BADOPERATOR];
            break;
        case ERR_TIMEOUT:
            errmsg = ErrMsg[MSG_TIMEOUT];
            break;   
        case ERR_OVFLOW:
            errmsg = ErrMsg[MSG_OVFLOW];
            break;                 
        default://PLC_ERR
            errmsg = ErrMsg[MSG_PLCERR];
    }
    plc_log("error %d: %s", -errcode, errmsg);
}

int main(int argc, char **argv)
{
    //int errcode = PLC_OK;
    int prog = 0;
    char * confstr = "config.yml";
  
    config_t conf = init_config();
        
    char * cvalue = NULL;
    opterr = 0;
    int c;
    while ((c = getopt (argc, argv, "hc:")) != PLC_ERR){
        switch (c) {
            case 'h':
                 plc_log(Usage);
                 return 1;
                break;
            case 'c':
                cvalue = optarg;
                break;
            case '?':
                plc_log(Usage);
                if (optopt == 'c'){
                    plc_log( 
                    "Option -%c requires an argument\n", optopt);
                } else if (isprint (optopt)){
                    plc_log(
                    "Unknown option `-%c'\n", optopt);
                } else{
                    plc_log(
                    "Unknown option character `\\x%x'\n",
                    optopt);
                }
                return 1;
            default:
               
                abort ();
        }
    }
    if(cvalue == NULL)
       cvalue = confstr;
       
    if ((load_config(cvalue, conf))->err < PLC_OK) {
        plc_log("Invalid configuration file %s\n", cvalue);
        //return PLC_ERR;
    }
//initialize PLC
    App = init_emu(conf);
    sequence_t programs = get_sequence_entry(CONFIG_PROGRAM, conf);
    for(; programs && prog < programs->size; prog++){
        if(programs->vars[prog].name){
            App->plc = plc_load_program_file(programs->vars[prog].name, App->plc);
        }
    }

//start UI    
    ui_init(App->conf);
    //UiReady=more;
    config_t command = cli_init_command(conf);
    config_t state = cli_init_state(conf);
    if(conf->err == PLC_OK){
        App->plc = plc_start(App->plc);    
    }
    while (get_numeric_entry(CLI_COM, command)!=COM_QUIT) {
        
        if(App->plc->update != 0){
           state = get_state(App->plc, state);
           ui_draw(state);
           App->plc->update = 0;
        }   
        command = ui_update(command);
        App = apply_command(command, App);
        state = App->conf;
        App->plc = plc_func(App->plc);
    }
    sigkill();
    App->plc = plc_stop(App->plc);
    clear_config(App->conf);
    close_log();
    ui_end();
    return 0;
}

