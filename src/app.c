#include "config.h"
#include "hardware.h"
#include "util.h"
#include "data.h"
#include "instruction.h"
#include "rung.h"
#include "plclib.h"
#include "ui.h"
#include "plcemu.h"
#include "app.h"

config_t init_config(){
 // in a c++ implementation this all can be done automatically 
 //using a hashmap
    config_t conf = new_config(N_CONFIG_VARIABLES);
   
   

    config_t hw = new_config(N_HW_VARS);

    hw = update_entry(
        HW_LABEL,
        new_entry_str("DRY", "LABEL"), 
        hw);
    
   /* hw = update_entry(
        HW_IFACE,
        new_entry_map(NULL, "IFACE"), 
        hw);
 */
    conf = update_entry(
        CONFIG_HW,
        new_entry_map(hw, "HW"),
        conf);
        

    conf = update_entry(
        CONFIG_STEP,
        new_entry_int(100, "STEP"),
        conf);
 
   /*******************************************/
  
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

    return conf;
}

static plc_t declare_names(int operand,
                    const variable_t var,                      
                    plc_t plc){

    char * name = var->name;
    if(name){
        unsigned char index = var->index;
        char * byte = get_param_val("BYTE", var->params);
        if(byte){
            int idx = atoi(byte) * BYTESIZE;
            char * bit = get_param_val("BIT", var->params);
            if(bit){
                idx += atoi(bit);
            } 
       
            if(idx>=0 && idx < 256){
                index = (unsigned char)idx;
            }
        }
        return declare_variable(plc, operand, index, name); 
    } else return plc;
} 

static plc_t configure_limits(int operand,                        
                       const variable_t var, 
                       plc_t plc){
    plc_t p = plc;
    char * max = NULL;
    char * min = NULL;
    
    if((max = get_param_val("MAX", var->params))){
        p = configure_io_limit(p, operand, var->index, max, TRUE);
    }
    if((min = get_param_val("MIN", var->params))){
        p = configure_io_limit(p, operand, var->index, min, FALSE);
    }   
    return p;                         
}

static plc_t init_values(int operand,                        
                  const variable_t var, 
                  plc_t plc){
    char * val = NULL;
    
    if((val = get_param_val("VALUE", var->params))){
        return init_variable(plc, operand, var->index, val);
    }
    return plc;                         
}

static plc_t configure_readonly(int operand,                        
                        const variable_t var, 
                        plc_t plc){
    char * val = NULL;
    
    if((val = get_param_val("READONLY", var->params))){
      return configure_variable_readonly(plc, operand, var->index, val);
    } 
    return plc;               
}

static plc_t configure_di(const config_t conf, plc_t plc){
 
    sequence_t seq = get_sequence_entry(CONFIG_DI, conf);
   
    if(seq) {
        plc_t p = plc;
        int i = 0;
        for(; i < seq->size; i++){
        //names
            p = declare_names(OP_INPUT, &(seq->vars[i]), p);    
        }
        return p;
    } 
    return plc;
}

static plc_t configure_dq(const config_t conf, plc_t plc){

    sequence_t seq = get_sequence_entry(CONFIG_DQ, conf); 
    
    if(seq) {
        plc_t p = plc;
        int i = 0;
        for(; i < seq->size; i++){
        //names
            p = declare_names(OP_OUTPUT, &(seq->vars[i]), p);    
        }
        return p;
    } 
    return plc;
}

static plc_t configure_ai(const config_t conf, plc_t plc){

    sequence_t seq = get_sequence_entry(CONFIG_AI, conf);
    
    if(seq) {
        
        plc_t p = plc;
        int i = 0;
        for(; i < seq->size; i++){
            //names
            p = declare_names(OP_REAL_INPUT, &(seq->vars[i]), p);
            //limits    
            p = configure_limits(OP_REAL_INPUT, &(seq->vars[i]), p);
        }
        return p;
    } 
    return plc;
}

static plc_t configure_aq(const config_t conf, plc_t plc){

    sequence_t seq = get_sequence_entry(CONFIG_AQ, conf);
    
    if(seq) {
        plc_t p = plc;
        int i = 0;
        for(; i < seq->size; i++){
            //names
            p = declare_names(OP_REAL_OUTPUT, &(seq->vars[i]), p);
            //limits    
            p = configure_limits(OP_REAL_OUTPUT, &(seq->vars[i]), p);
        }
        return p;
    } 
    return plc;
}

static plc_t configure_counters(const config_t conf, plc_t plc){

    sequence_t seq = get_sequence_entry(CONFIG_MREG, conf);
    
    if(seq) {
        plc_t p = plc;
        int i = 0;
        char * val = NULL;
        for(; i < seq->size; i++){
            //names
            p = declare_names(OP_MEMORY, &(seq->vars[i]), p);
            //defaults
            p = init_values(OP_MEMORY, &(seq->vars[i]), p);
            //readonlies
            p = configure_readonly(OP_MEMORY, &(seq->vars[i]), p);
            //directions    
            if((val = get_param_val("COUNT", seq->vars[i].params))){
                p = configure_counter_direction(p, i, val);
                val = NULL;
            }
        }
        return p;   
    } 
    return plc;
}

static plc_t configure_reals(const config_t conf, plc_t plc){

    sequence_t seq = get_sequence_entry(CONFIG_MVAR, conf);
    
    if(seq) {
        plc_t p = plc;
        int i = 0;
        for(; i < seq->size; i++){
            //names
            p = declare_names(OP_REAL_MEMORY, &(seq->vars[i]), plc);
            //defaults
            p = init_values(OP_REAL_MEMORY, &(seq->vars[i]), p);
            //readonlies
            p = configure_readonly(OP_REAL_MEMORY, &(seq->vars[i]), p); 
        }
        return p; 
    } 
    return plc;
}

static plc_t configure_timers(const config_t conf, plc_t plc){

    sequence_t seq = get_sequence_entry(CONFIG_TIMER, conf);
    
    if(seq) {
        int i = 0;
        char * scale = NULL;
        char * preset = NULL;
        char * ondelay = NULL;
        plc_t p = plc;
        for(; i < seq->size; i++){
            //names
            p = declare_names(OP_TIMEOUT, &(seq->vars[i]), plc);
            //scales
            if((scale = get_param_val("RESOLUTION", 
                                        seq->vars[i].params))){
                p = configure_timer_scale(p, i, scale);
                scale = NULL;
            }
            //presets            
            if((preset = get_param_val("PRESET", 
                                        seq->vars[i].params))){
                p = configure_timer_preset(p, i, preset);
                preset = NULL;
            }
            //modes
            if((ondelay = get_param_val("ONDELAY", 
                                        seq->vars[i].params))){
                p = configure_timer_delay_mode(p, i, ondelay);
                ondelay = NULL;
            }
        }   
        return p; 
    } 
    return plc;    
}

static plc_t configure_pulses(const config_t conf, plc_t plc){

    sequence_t seq = get_sequence_entry(CONFIG_PULSE, conf);
    
    if(seq) {
        int i = 0;
        char * scale = NULL;
        plc_t p = plc;
        for(; i < seq->size; i++){
            //names
            p = declare_names(OP_BLINKOUT, &(seq->vars[i]), plc);
            //scales
            if((scale = get_param_val("RESOLUTION",
                                     seq->vars[i].params))){
                p = configure_pulse_scale(p, i, scale);
                scale = NULL;
            }
        }   
        return p; 
    } 
    return plc;    
}

static config_t get_dio_values(const plc_t plc, 
                        const config_t state, 
                        BYTE type){
    config_t ret = state;
    sequence_t dios = get_sequence_entry(type, ret);
    if(dios == NULL || (
        type != CONFIG_DI &&
        type != CONFIG_DQ)) {
        
        return state;
    } 
    variable_t viter = dios->vars;
    int i = 0;
    BYTE val = 0;
    while(i < dios->size){
        if(viter != NULL) {
           
            if(type == CONFIG_DI){
                val = plc->di[i].I;
            } else if(type == CONFIG_DQ){
                val = plc->dq[i].Q;    
            }
            viter->params = update_param(
                                viter->params,
                                "STATE",
                                 val?"TRUE":"FALSE");	        
	        
	    }
	    viter = &(dios->vars)[++i];
    }
    return ret;
}

static config_t get_aio_values(const plc_t plc, 
                        const config_t state, 
                        BYTE type){
    config_t ret = state;
    sequence_t aios = get_sequence_entry(type, ret);
    if(aios == NULL || (
        type != CONFIG_AI &&
        type != CONFIG_AQ &&
        type != CONFIG_MVAR)) {
        
        return state;
    } 
    variable_t viter = aios->vars;
    int i = 0;
    double val = 0;
    char valbuf[TINYBUF] = "";
    memset(valbuf, 0, TINYBUF);
    while(i < aios->size){
        if(viter != NULL) {
           
            if(type == CONFIG_AI){
                val = plc->ai[i].V;
            } else if(type == CONFIG_AQ){
                val = plc->aq[i].V;    
            } else if(type == CONFIG_MVAR){
                val = plc->mr[i].V;    
            }
            sprintf(valbuf, "%f", val);
            viter->params = update_param(
                                viter->params,
                                "VALUE",
                                 valbuf);	        
	        
	    }
	    viter = &(aios->vars)[++i];
    }
    return ret;
}

static config_t get_reg_values(const plc_t plc, 
                        const config_t state){
    config_t ret = state;
    sequence_t regs = get_sequence_entry(CONFIG_MREG, ret);
    if(regs == NULL){
       
        return state;
    } 
    variable_t viter = regs->vars;
    int i = 0;
    uint64_t val = 0;
    while(i < regs->size){
        if(viter != NULL) {
           
            val = plc->m[i].V;    
            char vs[TINYBUF];
            memset(vs,0, TINYBUF);
            sprintf(vs,"%ld", val); 
            viter->params = update_param(
                                viter->params,
                                "VALUE",
                                 vs);	        
	    }
	    viter = &(regs->vars)[++i];
    }
    return ret;
}

static config_t get_timer_values(const plc_t plc, 
                          const config_t state){
    config_t ret = state;
    sequence_t timers = get_sequence_entry(CONFIG_TIMER, ret);
    if(timers == NULL){
       
        return state;
    } 
    variable_t viter = timers->vars;
    int i = 0;
    long val = 0;
    BYTE out = 0;
    while(i < timers->size){
        if(viter != NULL) {
           
            val = plc->t[i].V;
            out = plc->t[i].Q;    
            char vs[TINYBUF];
            memset(vs,0, TINYBUF);
            sprintf(vs,"%ld",val); 
            viter->params = update_param(
                                viter->params,
                                "VALUE",
                                 vs);
            viter->params = update_param(
                                viter->params,
                                "OUT",
                                out?"TRUE":"FALSE");
                                             	        
	    }
	    viter = &(timers->vars)[++i];
    }
    return ret;
}

static config_t get_pulse_values(const plc_t plc, 
                          const config_t state){
    config_t ret = state;
    sequence_t pulses = get_sequence_entry(CONFIG_PULSE, ret);
    if(pulses == NULL){
       
        return state;
    } 
    variable_t viter = pulses->vars;
    int i = 0;
    BYTE out = 0;
    while(i < pulses->size){
        if(viter != NULL) {
        
            out = plc->s[i].Q;    
            viter->params = update_param(
                                viter->params,
                                "OUT",
                                out?"TRUE":"FALSE");
                                             	        
	    }
	    viter = &(pulses->vars)[++i];
    }
    return ret;
}



//FIXME: support for multiple interfaces
#ifdef SIM
#define HW_TYPE HW_SIM
#else //SIM

#ifdef COMEDI
#define HW_TYPE HW_COMEDI
#else //COMEDI

#ifdef USPACE
#define HW_TYPE HW_USPACE
#else //USPACE

#define HW_TYPE HW_DRY

#endif //USPACE
#endif //COMEDI
#endif //SIM
int Lookup[N_CONFIG_VARIABLES] = {
    PLC_ERR, //CONFIG_STEP,
    PLC_ERR, //CONFIG_PIPE,
    PLC_ERR, //CONFIG_HW,
    PLC_ERR, //CONFIG_USPACE,
    PLC_ERR, //CONFIG_COMEDI,
    PLC_ERR, //CONFIG_SIM,
    PLC_ERR, //CONFIG_PROGRAM,
        OP_REAL_INPUT,  //CONFIG_AI
        OP_REAL_OUTPUT, //CONFIG_AQ
        OP_INPUT,       //CONFIG_DI,
        OP_OUTPUT,      //CONFIG_DQ,
        OP_MEMORY,  	//CONFIG_MVAR,
        OP_REAL_MEMORY, //CONFIG_MREG,
        OP_BLINKOUT,    //CONFIG_TIMER,
        OP_TIMEOUT,     //CONFIG_PULSE, 
};

app_t configure(const config_t conf, app_t app){

    app_t a = app;
    a->conf = conf;
    hardware_t hw = get_hardware(HW_TYPE);
    hw->status = hw->configure(conf);
  //TODO: handle NULL errors here 
    sequence_t s = get_sequence_entry(CONFIG_DI, conf);
    int di = s ? (s->size / BYTESIZE + 1) : 0;
    s = get_sequence_entry(CONFIG_DQ, conf);
    int dq = s ? (s->size / BYTESIZE + 1) : 0;
    s = get_sequence_entry(CONFIG_AI, conf);
    int ai = s ? s->size : 0;
    s = get_sequence_entry(CONFIG_AQ, conf);
    int aq = s ? s->size : 0;
    s = get_sequence_entry(CONFIG_TIMER, conf);
    int nt = s ? s->size : 0;
    s = get_sequence_entry(CONFIG_PULSE, conf);
    int ns = s ? s->size : 0;
    s = get_sequence_entry(CONFIG_MREG, conf);
    int nm = s ? s->size : 0;
    s = get_sequence_entry(CONFIG_MVAR, conf);
    int nr = s ? s->size : 0;
    
    int step = get_numeric_entry(CONFIG_STEP, conf);
    
    plc_t p = new_plc(di, dq, ai, aq, nt, ns, nm, nr, step, hw);

    p->status = 0;
    p->update = TRUE;
  //these errors should be already handled
    p = configure_di(conf, p);
    p = configure_dq(conf, p);
    p = configure_ai(conf, p);
    p = configure_aq(conf, p);
    p = configure_counters(conf, p);
    p = configure_reals(conf, p);
    p = configure_timers(conf, p);
    p = configure_pulses(conf, p);
    
    if(a->plc != NULL){
        clear_plc(a->plc);
    }
    a->plc = p;
        
    return a;
}

config_t get_state(const plc_t plc, 
                   const config_t state){
    config_t r = state;
    int i = 0;
    //set status
    r = set_numeric_entry(0, plc->status, r);
    //assign values    
    if(plc->update & CHANGED_I){
        r = get_dio_values(plc, r, CONFIG_DI);
        //analog
        r = get_aio_values(plc, r, CONFIG_AI);
    }
    if(plc->update & CHANGED_O){
        r = get_dio_values(plc, r, CONFIG_DQ);
        //analog
        r = get_aio_values(plc, r, CONFIG_AQ);
    }
    if(plc->update & CHANGED_M){
    //registers
        r = get_reg_values(plc, r);
    //reals
        r = get_aio_values(plc, r, CONFIG_MVAR);
    }
    if(plc->update & CHANGED_T){
    //timers
        r = get_timer_values(plc, r);
    }
    if(plc->update & CHANGED_S){
    //pulses
        r = get_pulse_values(plc, r);
    }
    
    //show forced
    //add program
    if( plc->update & CHANGED_STATUS) {
        sequence_t programs = get_sequence_entry(CONFIG_PROGRAM, r);
    
        for(i = 0;i < plc->rungno; i++){
            codeline_t liter = plc->rungs[i]->code;
            int lineno = 0;
            char label[SMALLBUF] = "";
            while(liter){
                sprintf(label, "LINE %d", ++lineno);
            
                param_t code = get_param(label,programs->vars[i].params);
                if(code == NULL){ 
                    programs->vars[i].params = append_param(
                                        programs->vars[i].params,
                                        label,
                                        liter->line);
                }
                liter = liter->next;    
            }      
        }
    }
    return r;
}

app_t apply_command(const config_t com, 
                    app_t a){
    char * confstr = "config.yml";
    char * cvalue = NULL;
    char * val = NULL;
    sequence_t seq = NULL;
    int s = CONFIG_PROGRAM;
    int v = -1;
    plc_t p = NULL;
    if(a != NULL){
        switch(get_numeric_entry(CLI_COM, com)){
            case COM_START:
                if(a->conf->err == PLC_OK){
                    a->plc = plc_start(a->plc);
                } else {
                    plc_log("Configuration error\n");
                }
                break;//TODO: handle also PLC error
            
            case COM_STOP:
        
                a->plc = plc_stop(a->plc);
                break;
        
            case COM_LOAD://TODO: file parsing should be done in the UI and 
            //copied over here (with multiple edit commands, so this one 
            //will be deprecated)
            
                a->plc = plc_stop(a->plc);
                a->conf = init_config();
                cvalue = get_string_entry(CLI_ARG, com);
                if( cvalue == NULL ||
                    cvalue[0] == 0){
                    cvalue = confstr;
                }
                if ((load_config(cvalue, a->conf))->err < PLC_OK) {
                    plc_log("Invalid configuration file %s\n", cvalue);
                } else {
                    a = configure(a->conf, a);
                }    
                break;
            
            case COM_SAVE://TODO: file saving whould be done in the UI, 
            //PLC configuration is already dumped into the UI 
            //but configuration should be also backed up in the PLC
        
                a->plc = plc_stop(a->plc);
                cvalue = get_string_entry(CLI_ARG, com);
                if( cvalue == NULL ||
                    cvalue[0] == 0){
                    cvalue = confstr;
                }
                if ((save_config(cvalue, a->conf)) < PLC_OK) {
                    plc_log("Invalid configuration file %s\n", cvalue);
                }
                break;    
        
            case COM_FORCE:         
            
                for(s = CONFIG_PROGRAM; s < N_CONFIG_VARIABLES; s++){
                    seq = get_sequence_entry(s, com); 
                    for(v = 0; seq && v < seq->size; v++){
                    //filter sequences who have a param "FORCE"
                        val = get_param_val("FORCE", seq->vars[v].params);
                        if(val){  //apply force   
                            p = force(a->plc, Lookup[s], v, val);
                            if(p){
                                a->plc = p;
                            }
                        }
                    } 
                }
                if(p == NULL){
                    plc_log("Invalid force command\n");
                }
                break;
            
            case COM_UNFORCE:
            
                for(s = CONFIG_PROGRAM; s < N_CONFIG_VARIABLES; s++){
                    seq = get_sequence_entry(s, com); 
                    for(v = 0; seq && v < seq->size; v++){
                //filter sequences who have a param "FORCE"
                        val = get_param_val("FORCE", seq->vars[v].params);
                        if(val){  //apply force   
                            p = unforce(a->plc, Lookup[s], v);
                            if(p){
                                a->plc = p;
                            }
                        }
                    } 
                }
                if(p == NULL){
                    plc_log("Invalid force command\n");
                }
                break;
             case COM_EDIT:
                //TODO: filter sequences who have an updated variable                    
                a->plc = configure_di(com, a->plc);
                a->plc = configure_dq(com, a->plc);
                a->plc = configure_ai(com, a->plc);
                a->plc = configure_aq(com, a->plc);
                a->plc = configure_counters(com, a->plc);
                a->plc = configure_reals(com, a->plc);
                a->plc = configure_timers(com, a->plc);
                a->plc = configure_pulses(com, a->plc);
                a->conf = copy_sequences(com, a->conf);
                break;
            //TODO: new command: CONFIGURE to set up the hardware and register sizes
            default: break;
        }
    }
    return a;
}
 
