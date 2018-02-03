#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <ctype.h>
#include <math.h>
#include <unistd.h>

#include "config.h"
#include "util.h"
#include "data.h"
#include "instruction.h"
#include "rung.h"
#include "plclib.h"
#include "parser-tree.h"
#include "parser-il.h"
#include "parser-ld.h"
#include "ui.h"
#include "hardware.h"
#include "app.h"
#include "plcemu.h"
/*************GLOBALS************************************************/
plc_t Plc;
config_t Conf;
//int UiReady=FALSE;

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

plc_t init_emu(const config_t conf) {
   
    hw_config(conf);
        
    int di = get_sequence_entry(CONFIG_DI, conf)->size / BYTESIZE + 1; 
    int dq = get_sequence_entry(CONFIG_DQ, conf)->size / BYTESIZE + 1;
    int ai = get_sequence_entry(CONFIG_AI, conf)->size;
    int aq = get_sequence_entry(CONFIG_AQ, conf)->size;
    int nt = get_sequence_entry(CONFIG_TIMER, conf)->size;
    int ns = get_sequence_entry(CONFIG_PULSE, conf)->size;
    int nm = get_sequence_entry(CONFIG_MREG, conf)->size;
    int nr = get_sequence_entry(CONFIG_MVAR, conf)->size;
    int step = get_numeric_entry(CONFIG_STEP, conf);
    char * hw = get_string_entry(CONFIG_HW, conf);
    
    plc_t p = new_plc(di, dq, ai, aq, nt, ns, nm, nr, step, hw);
    p = configure(conf, p);
    p->status = 0;
    p->update = TRUE;

    //signal(conf->sigenable, sigenable);
    signal(SIGINT, sigkill);
    signal(SIGTERM, sigkill);
    
    project_init();
    
    open_pipe(get_string_entry(CONFIG_HW, conf), p);
    return p;
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
    Conf = init_config();
    
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
       
    if ((load_config_yml(cvalue, Conf))->err < PLC_OK) {
        plc_log("Invalid configuration file %s\n", cvalue);
        return PLC_ERR;
    }
//initialize PLC
    Plc = init_emu(Conf);
    sequence_t programs = get_sequence_entry(CONFIG_PROGRAM, Conf);
    for(; programs && prog < programs->size; prog++){
        if(programs->vars[prog].name){
            Plc = plc_load_program_file(programs->vars[prog].name, Plc);
        }
    }
//start hardware
    enable_bus();
//start UI
    
    ui_init(Conf);
    //UiReady=more;
    config_t command = cli_init_command(Conf);
    config_t state = cli_init_state(Conf);
    Plc = plc_start(Plc);    
    while (get_numeric_entry(0, command)!=COM_QUIT) {
       if(Plc->update != 0){
           state = get_state(Plc, state);
           ui_draw(state);
           Plc->update = 0;
        }   
        command = ui_update(command);
        Plc = apply_command(command, &Conf, Plc);
        Plc = plc_func(Plc);
    }
    sigkill();
    disable_bus();
    clear_config(Conf);
    close_log();
    ui_end();
    return 0;
}

