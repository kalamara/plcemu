#include <yaml.h>
#include "util.h"
#include "plclib.h"
#include "config.h"

const char * Config_vars[N_CONFIG_VARIABLES] = {
    "STEP",
    "PIPE",
    "SIGENABLE",
    "PAGELEN",
    "PAGEWIDTH",
    "HW",
    "NT",
    "NS",
    "NR",
    "NM",
    "NDI",
    "NDQ",
    "NAI",
    "NAQ",
    "RESPONSE",
    "USPACE",
    "USPACE_BASE",
    "USPACE_WR",
    "USPACE_RD",
    "COMEDI",
    "COMEDI_FILE",
    "COMEDI_SUBDEV",
    "SUBDEV_IN",
    "SUBDEV_OUT",
    "SUBDEV_ADC",
    "SUBDEV_DAC",
    "SIM",
    "SIM_INPUT",
    "SIM_OUTPUT",
    "IL",
    "LD",
    //sequences
    "AI",
    "AQ",
    "DI",
    "DQ",
    "MVAR",
    "MREG",
    "TIMER",
    "PULSE"
};

const char * Variable_params[N_VARIABLE_PARAMS] = {

    "INDEX",
    "ID",
    "VALUE",
    "MIN",
    "MAX"
};

void yaml_config_error(yaml_parser_t parser){

    //print line
}

void yaml_parser_error(yaml_parser_t parser){

     switch (parser.error)
     {
        case YAML_MEMORY_ERROR:
            plc_log( 
            "Memory error: Not enough memory for parsing");
            break;

        case YAML_READER_ERROR:
            if (parser.problem_value != -1) {
                plc_log( 
                "Reader error: %s: #%X at %d", 
                parser.problem,
                parser.problem_value, 
                parser.problem_offset);
            }
            else {
                plc_log( 
                "Reader error: %s at %d", 
                parser.problem,
                parser.problem_offset);
            }
            break;

        case YAML_SCANNER_ERROR:
            if (parser.context) {
                plc_log( 
                "Scanner error: %s at line %d, column %d"
                        "%s at line %d, column %d", 
                        parser.context,
                        parser.context_mark.line+1,
                        parser.context_mark.column+1,
                        parser.problem, 
                        parser.problem_mark.line+1,
                        parser.problem_mark.column+1);
            }
            else {
                plc_log( 
                "Scanner error: %s at line %d, column %d",
                        parser.problem, parser.problem_mark.line+1,
                        parser.problem_mark.column+1);
            }
            break;

        case YAML_PARSER_ERROR:
            if (parser.context) {
                plc_log( 
                "Parser error: %s at line %d, column %d"
                        "%s at line %d, column %d", 
                        parser.context,
                        parser.context_mark.line+1,
                        parser.context_mark.column+1,
                        parser.problem, 
                        parser.problem_mark.line+1,
                        parser.problem_mark.column+1);
            }
            else {
                plc_log( 
                "Parser error: %s at line %d, column %d",
                        parser.problem, 
                        parser.problem_mark.line+1,
                        parser.problem_mark.column+1);
            }
            break;

        default:
            /* Couldn't happen. */
            plc_log( 
            "Internal error\n");
            break;
    }
}

void config_timer_vars(BYTE n, config_t * conf){

    (*conf)->nt = n;
    if((*conf)->timers != NULL){
        free((*conf)->timers);
        (*conf)->timers = NULL;
    }    
    (*conf)->timers = (variable_t )malloc(n * sizeof(struct variable));
}

void config_pulse_vars(BYTE n, config_t * conf){

    (*conf)->ns = n;
    if((*conf)->pulses != NULL){
        free((*conf)->pulses);
        (*conf)->pulses = NULL;
    }    
    (*conf)->pulses = (variable_t )malloc(n * sizeof(struct variable));
}

void config_mem_vars(BYTE n, config_t * conf){

    (*conf)->nr = n;
    if((*conf)->mvars != NULL){
        free((*conf)->mvars);
        (*conf)->mvars = NULL;
    }    
    (*conf)->mvars = (variable_t )malloc(n * sizeof(struct variable));
}

void config_reg_vars(BYTE n, config_t * conf){

    (*conf)->nm = n;
    if((*conf)->mregs != NULL){
        free((*conf)->mregs);
        (*conf)->mregs = NULL;
    }    
    (*conf)->mregs = (variable_t )malloc(n * sizeof(struct variable));
}

void config_di_vars(BYTE n, config_t * conf){

    (*conf)->di = n;
    if((*conf)->dinps != NULL){
        free((*conf)->dinps);
        (*conf)->dinps = NULL;
    }    
    (*conf)->dinps = (variable_t )malloc(n * sizeof(struct variable));
}

void config_dq_vars(BYTE n, config_t * conf){

    (*conf)->dq = n;
    if((*conf)->douts != NULL){
        free((*conf)->douts);
        (*conf)->douts = NULL;
    }    
    (*conf)->douts = (variable_t )malloc(n * sizeof(struct variable));
}

void config_ai_vars(BYTE n, config_t * conf){

    (*conf)->ai = n;
    if((*conf)->ainps != NULL){
        free((*conf)->ainps);
        (*conf)->ainps = NULL;
    }    
    (*conf)->ainps = (variable_t )malloc(n * sizeof(struct variable));
}

void config_aq_vars(BYTE n, config_t * conf){

    (*conf)->aq = n;
    if((*conf)->aouts != NULL){
        free((*conf)->aouts);
        (*conf)->aouts = NULL;
    }    
    (*conf)->aouts = (variable_t )malloc(n * sizeof(struct variable));

}

config_t init_config(){
 
    config_t conf = (config_t)malloc(sizeof(struct config));
    memset(conf, 0, sizeof(struct config));
//registers    
    config_timer_vars(4, &conf);
    config_pulse_vars(4, &conf);
    
    config_reg_vars(4, &conf);
    
    config_mem_vars(4, &conf);
    
    config_di_vars(8, &conf);
    
    config_dq_vars(8, &conf);
    
    config_ai_vars(4, &conf);
    
    config_aq_vars(4, &conf);
    
//ui    
    conf->sigenable = 36;
    conf->page_width = 80;
    conf->page_len = 24;
//hardware    
    conf->base = ADVANTECH_HISTORICAL_BASE;
    conf->wr_offs = 0;
    conf->rd_offs = 8;
    conf->comedi_file = 0;
    conf->comedi_subdev_i = 0;
    conf->comedi_subdev_q = 1;
    conf->comedi_subdev_ai = 2;
    conf->comedi_subdev_aq = 3;
//polling    
    conf->step = 1;
    
    return conf;
}

void clear_config(config_t *c){

    if (*c != NULL){
        if((*c)->aouts != NULL){
            free((*c)->aouts);
            (*c)->aouts = NULL;
        }
        if((*c)->ainps != NULL){
            free((*c)->ainps);
            (*c)->ainps = NULL; 
        }
        if((*c)->douts != NULL){
            free((*c)->douts);
            (*c)->douts = NULL;
        }    
        if((*c)->dinps != NULL){
            free((*c)->dinps);
            (*c)->dinps = NULL;
        }
        if((*c)->mvars != NULL){
            free((*c)->mvars);
            (*c)->mvars = NULL;
        }    
        if((*c)->mregs != NULL){
            free((*c)->mregs);
            (*c)->mregs = NULL;
        }
        if((*c)->timers != NULL){
            free((*c)->timers);
            (*c)->timers = NULL;
        }
        if((*c)->pulses != NULL){
            free((*c)->pulses);
            (*c)->pulses = NULL;
        }
        free(*c);
    }
    *c = NULL;    
}

int log_yml_event(yaml_event_t event){

    switch(event.type){ 
    
        case YAML_NO_EVENT: plc_log("No event!"); 
            break;
    /* Stream start/end */
        case YAML_STREAM_START_EVENT: plc_log("STREAM START"); 
            break;
        case YAML_STREAM_END_EVENT: plc_log("STREAM END");   
            break;
    /* Block delimeters */
        case YAML_DOCUMENT_START_EVENT: plc_log("Start Document"); 
            break;
        case YAML_DOCUMENT_END_EVENT: plc_log("End Document");   
            break;
        case YAML_SEQUENCE_START_EVENT: plc_log("Start Sequence"); 
            break;
        case YAML_SEQUENCE_END_EVENT: plc_log("End Sequence");
            break;
        case YAML_MAPPING_START_EVENT: plc_log("Start Mapping");         
            break;
        case YAML_MAPPING_END_EVENT: plc_log("End Mapping");      
            break;
    /* Data */
        case YAML_ALIAS_EVENT:  plc_log("Got alias (anchor %s)", event.data.alias.anchor); 
            break;
        case YAML_SCALAR_EVENT: plc_log("Got scalar (value %s)", event.data.scalar.value); 
        
        break;
        default:
            plc_log("default?");
    }
    return PLC_OK;
}

int store_value(BYTE key, const char * value, config_t * c){
    switch(key){
        case CONFIG_HW:
            strncpy((*c)->hw, value, MAXSTR);
            break;
        case CONFIG_PIPE:
            strncpy((*c)->pipe, value, MAXSTR);
            break;    
        case CONFIG_STEP:
            (*c)->step = atol(value);
            break;
        case CONFIG_USPACE_BASE:
            (*c)->base = atol(value);
            break;
        case CONFIG_USPACE_WR:
            (*c)->wr_offs = atoi(value);
            break;
        case CONFIG_USPACE_RD:
            (*c)->rd_offs = atoi(value);
            break;                
        case CONFIG_COMEDI_FILE:
            (*c)->comedi_file = atoi(value);
            break;
        case CONFIG_SUBDEV_IN:
            (*c)->comedi_subdev_i = atol(value);
            break;
        case CONFIG_SUBDEV_OUT:
            (*c)->comedi_subdev_q = atol(value);
            break;
        case CONFIG_SUBDEV_ADC:
            (*c)->comedi_subdev_ai = atoi(value);
            break;
        case CONFIG_SUBDEV_DAC:
            (*c)->comedi_subdev_aq = atoi(value);
            break;                    
        case CONFIG_SIM_INPUT:
            strncpy((*c)->sim_in_file, value, MAXSTR);
            break;
        case CONFIG_SIM_OUTPUT:
            strncpy((*c)->sim_out_file, value, MAXSTR);
            break;
            
         case CONFIG_PROGRAM_IL:
         case CONFIG_PROGRAM_LD:
            strncpy((*c)->program_file, value, MAXSTR);
            break;  
            
         case CONFIG_NT:
            config_timer_vars(atoi(value), c);
            break;                
        case CONFIG_NS:
            config_pulse_vars(atoi(value), c);
            break;
        case CONFIG_NM:
            config_reg_vars(atoi(value), c);
            break;
        case CONFIG_NR:
            config_mem_vars(atoi(value), c);
            break;
        case CONFIG_NDI:
            config_di_vars(atoi(value), c);
            break;
        case CONFIG_NDQ:
            config_dq_vars(atoi(value), c);
            break;                    
        case CONFIG_NAI:
            config_ai_vars(atoi(value), c);
            break;
        case CONFIG_NAQ:
            config_aq_vars(atoi(value), c);
            break;                    
        default: return PLC_ERR;
    }
    return PLC_OK;
}

int store_seq_value(BYTE seq,
                    BYTE idx, 
                    BYTE key, 
                    const char * value, 
                    config_t * c){
                    
    variable_t vars;
    switch(seq) {
        case SEQ_AI:
            vars = (*c)->ainps;
            if(idx >= (*c)->ai)
                return PLC_ERR;
            break;
        case SEQ_AQ:
            vars = (*c)->aouts;
            if(idx >= (*c)->aq)
                return PLC_ERR;
            break;    
        case SEQ_DI:
            vars = (*c)->dinps;
            if(idx >= (*c)->di)
                return PLC_ERR;
            break;
        case SEQ_DQ:
            vars = (*c)->douts;
            if(idx >= (*c)->dq)
                return PLC_ERR;
            break;   
        case SEQ_MVAR:
            vars = (*c)->mvars;
            if(idx >= (*c)->nr)
                return PLC_ERR;
            break;
        case SEQ_MREG:
            vars = (*c)->mregs;
            if(idx >= (*c)->nr)
                return PLC_ERR;
            break;    
        case SEQ_TIMER:
            vars = (*c)->timers;
            if(idx >= (*c)->nt)
                return PLC_ERR;
            break;
        case SEQ_PULSE:
            vars = (*c)->pulses;
            if(idx >= (*c)->ns)
                return PLC_ERR;
            break;         
        
        default:
            return PLC_ERR;
    }
    switch(key) {
        case VARIABLE_INDEX: 
            vars[idx].index = atoi(value);
            break;
        case VARIABLE_ID: 
            strncpy(vars[idx].name, value, MAXSTR);
            break;
        case VARIABLE_VALUE: 
            strncpy(vars[idx].value, value, MAXSTR);
            break;
        case VARIABLE_MAX: 
            strncpy(vars[idx].max, value, MAXSTR);
            break; 
        case VARIABLE_MIN: 
            strncpy(vars[idx].min, value, MAXSTR);
            break;            
        default: 
            return PLC_ERR;    
    }    
    return PLC_OK;                       
}

int find_config_var(const char *name, 
                    const char **where, 
                    unsigned int size){

    for(int i = 0; i < size; i++)
        if(!strcmp(name, where[i]))
            return i;
    return PLC_ERR;        
}

int process_scalar(const char *value,
                   BYTE is_seq,  
                   config_t * conf, 
                   BYTE *storage, 
                   int *key,
                   int *seq,
                   int *idx){
                   
    int ret = PLC_OK;
    
    if(*storage == STORE_KEY){
        if(is_seq)
            *key = find_config_var(
                value, 
                Variable_params,
                N_VARIABLE_PARAMS);
        else{         
            *key = find_config_var(
                value, 
                Config_vars,
                N_CONFIG_VARIABLES);
            if(IS_SEQUENCE(*key))
                *seq = SEQUENCE(*key);
        }      
        if(*key < PLC_OK)
            ret = *key;                          
        *storage = STORE_VAL;
    }
    else{ //val
        if(is_seq){
            if(*key == VARIABLE_INDEX)
                    *idx = atoi(value);
            else  
                ret = store_seq_value(*seq, 
                                          *idx, 
                                          *key, 
                                          value, 
                                          conf);        
        } else 
             ret = store_value(*key, value, conf);
        *storage = STORE_KEY; 
    } 
    
    return ret;                
}

int process(int sequence, 
            yaml_parser_t *parser, 
            config_t conf){
             
     int ret = PLC_OK;
     BYTE storage = STORE_KEY;   
     int done = FALSE;
     int key = PLC_ERR;
     int seq = sequence;
     int found_seq = sequence > PLC_ERR;
     int idx = PLC_ERR;
     yaml_event_t event;
     memset(&event, 0, sizeof(event));
     
     if(parser == NULL
     || conf == NULL)
     
        return PLC_ERR;
           
     while(done == FALSE){
     
        if (!yaml_parser_parse(parser, &event)){   
                yaml_parser_error(*parser);
                ret = PLC_ERR;
        }
        else {
            
            switch(event.type)
            {
                case YAML_SCALAR_EVENT:
                    ret = process_scalar(
                        event.data
                             .scalar
                             .value,
                        found_seq,
                        &conf,
                        &storage,
                        &key,
                        &seq,
                        &idx);
                    break;
                
                case YAML_SEQUENCE_START_EVENT:
                    found_seq = TRUE;
                    break;
                
                case YAML_SEQUENCE_END_EVENT:
                    found_seq = FALSE;
                    break;
                
                case YAML_MAPPING_START_EVENT:
                    ret = process(seq, parser, conf);
                    storage = STORE_KEY;   
                    break;
                    
                case YAML_MAPPING_END_EVENT:
                case YAML_STREAM_END_EVENT:     
                    done = TRUE;
                    break;
                    
                case YAML_NO_EVENT:
                    ret = PLC_ERR;
                    break;    
                    
                default: break;    
            }
         }
         if(ret < PLC_OK) {
             done = TRUE;
             log_yml_event(event);
         }                                              
         yaml_event_delete(&event);   
     }
     
     return ret;
}
             
int load_config_yml(const char * filename, config_t conf) {
    yaml_parser_t parser;
    
    FILE * fcfg;
    char path[MAXSTR];

    memset(path, 0, MAXSTR);
    sprintf(path, "%s", filename);
    
    memset(&parser, 0, sizeof(parser));
    
    int r = PLC_OK;
    
    if (!yaml_parser_initialize(&parser)) {
        yaml_parser_error(parser);
        return PLC_ERR;    
    }
    if ((fcfg = fopen(path, "r"))) {
        plc_log("Looking for configuration from %s ...", path);
        yaml_parser_set_input_file(&parser, fcfg);
        r = process(PLC_ERR, &parser, conf);
        if(r < PLC_OK)
            plc_log( "Configuration error ");
        fclose(fcfg);
    } else {
        r = PLC_ERR;
        plc_log("Could not open file %s", filename);
    }
    yaml_parser_delete(&parser);
    return r;
}

yaml_event_t generate_events();

int emit(yaml_emitter_t *emitter, const config_t conf) {
    int r = PLC_OK;         
    
    
    
    return r;
}

int save_config_yml(const char * filename, const config_t conf) {
    
    yaml_emitter_t emitter;
    yaml_event_t event;
    
    FILE * fcfg;
    char path[MAXSTR];
    int r = PLC_OK;
    
    memset(path, 0, MAXSTR);
    sprintf(path, "%s", filename);

    if(!yaml_emitter_initialize(&emitter)){
        return PLC_ERR;    
    }
    if ((fcfg = fopen(path, "wb"))) {
         plc_log("Save configuration to %s ...", path);
         
         yaml_emitter_set_output_file(&emitter, fcfg);
         yaml_stream_start_event_initialize(&event, YAML_UTF8_ENCODING);
         
         r = yaml_emitter_emit(&emitter, &event);
         
         if(r)
            r = emit(&emitter, conf);
         if(r){
            yaml_stream_end_event_initialize(&event);
            r = yaml_emitter_emit(&emitter, &event);   
         }
            
         if(r < PLC_OK)
            plc_log( "Configuration error ");
            
         fclose(fcfg);
    } else {
        r = PLC_ERR;
        plc_log("Could not open file %s for write", filename);
    }
    yaml_emitter_delete(&emitter);
    return r;
}



void configure(const config_t conf, plc_t plc){

    plc->ni = conf->di;
    plc->nq = conf->dq;
    plc->nai = conf->ai;
    plc->naq = conf->aq;
    plc->nt = conf->nt;
    plc->ns = conf->ns;
    plc->nm = conf->nm;
    plc->nmr = conf->nr;
    
    sprintf(plc->hw, "%s", conf->hw);
    
    plc->inputs = (BYTE *) malloc(plc->ni);
    plc->outputs = (BYTE *) malloc(plc->nq);
    plc->edgein = (BYTE *) malloc(plc->ni);
    plc->maskin = (BYTE *) malloc(plc->ni);
    plc->maskout = (BYTE *) malloc(plc->nq);
    plc->maskin_N = (BYTE *) malloc(plc->ni);
    plc->maskout_N = (BYTE *) malloc(plc->nq);
    plc->real_in = (uint64_t *) malloc(conf->ai * sizeof(uint64_t));
    plc->real_out = (uint64_t *) malloc(conf->aq * sizeof(uint64_t));
    plc->mask_ai = (double *) malloc(conf->ai * sizeof(double));
    plc->mask_aq = (double *) malloc(conf->aq * sizeof(double));
    plc->di = (di_t) malloc(
            BYTESIZE * plc->ni * sizeof(struct digital_input));
    plc->dq = (do_t) malloc(
            BYTESIZE * plc->nq * sizeof(struct digital_output));
    
    plc->t = (dt_t) malloc(plc->nt * sizeof(struct timer));
    plc->s = (blink_t) malloc(plc->ns * sizeof(struct blink));
    plc->m = (mvar_t) malloc(plc->nm * sizeof(struct mvar));
    plc->mr = (mreal_t) malloc(plc->nmr * sizeof(struct mreal));
   
    plc->ai = (aio_t) malloc(
             conf->ai * sizeof(struct analog_io));
    plc->aq = (aio_t) malloc(
             conf->aq * sizeof(struct analog_io));
   
    memset(plc->real_in, 0, plc->nai*sizeof(uint64_t));
    memset(plc->real_out, 0, plc->naq*sizeof(uint64_t));
    memset(plc->inputs, 0, plc->ni);
    memset(plc->outputs, 0, plc->nq);
    memset(plc->maskin, 0, plc->ni);
    memset(plc->maskout, 0, plc->nq);
    memset(plc->maskin_N, 0, plc->ni);
    memset(plc->maskout_N, 0, plc->nq);
    
    memset(plc->mask_ai, 0, plc->nai * sizeof(double));
    memset(plc->mask_aq, 0, plc->naq * sizeof(double));
    
    memset(plc->di, 0, BYTESIZE * plc->ni * sizeof(struct digital_input));
    memset(plc->dq, 0, BYTESIZE * plc->nq * sizeof(struct digital_output));
    memset(plc->t, 0, plc->nt * sizeof(struct timer));
    memset(plc->s, 0, plc->ns * sizeof(struct blink));
    memset(plc->m, 0, plc->nm * sizeof(struct mvar));
    memset(plc->mr, 0, plc->nmr * sizeof(struct mreal));

    plc_t p_old=NULL;
    p_old = (plc_t) malloc(sizeof(struct PLC_regs));

    p_old->ni = conf->di;
    p_old->nq = conf->dq;
    p_old->nai = conf->ai;
    p_old->naq = conf->aq;
    p_old->nt = conf->nt;
    p_old->ns = conf->ns;
    p_old->nm = conf->nm;
    p_old->nmr = conf->nr;
    
    p_old->inputs = (BYTE *) malloc(conf->di);
    p_old->outputs = (BYTE *) malloc(conf->dq);
    p_old->maskin = (BYTE *) malloc(conf->di);
    p_old->edgein = (BYTE *) malloc(conf->di);
    p_old->maskout = (BYTE *) malloc(conf->dq);
    p_old->maskin_N = (BYTE *) malloc(conf->di);
    p_old->maskout_N = (BYTE *) malloc(conf->dq);
    p_old->di = (di_t) malloc(
            BYTESIZE * conf->di * sizeof(struct digital_input));
    p_old->dq = (do_t) malloc(
            BYTESIZE * conf->dq * sizeof(struct digital_output));
    p_old->t = (dt_t) malloc(conf->nt * sizeof(struct timer));
    p_old->s = (blink_t) malloc(conf->ns * sizeof(struct blink));
    p_old->m = (mvar_t) malloc(conf->nm * sizeof(struct mvar));
    p_old->mr = (mreal_t) malloc(conf->nr * sizeof(struct mreal));
    
    
    p_old->real_in = (uint64_t *) malloc(conf->ai * sizeof(uint64_t));
    p_old->real_out = (uint64_t *) malloc(conf->aq * sizeof(uint64_t));
    p_old->mask_ai = (double *) malloc(conf->ai * sizeof(double));
    p_old->mask_aq = (double *) malloc(conf->aq * sizeof(double));
    p_old->ai = (aio_t) malloc(
             conf->ai * sizeof(struct analog_io));
    p_old->aq = (aio_t) malloc(
             conf->aq * sizeof(struct analog_io));
    
    p_old->di = (di_t) malloc(
            BYTESIZE * plc->ni * sizeof(struct digital_input));
    p_old->dq = (do_t) malloc(
            BYTESIZE * plc->nq * sizeof(struct digital_output));
    
    memcpy(p_old->inputs, plc->inputs, conf->di);
    memcpy(p_old->outputs, plc->outputs, conf->dq);
    memset(p_old->real_in, 0, plc->nai*sizeof(uint64_t));
    memset(p_old->real_out, 0, plc->naq*sizeof(uint64_t));
    
    memcpy(p_old->m, plc->m, conf->nm * sizeof(struct mvar));
    memcpy(p_old->mr, plc->mr, conf->nr * sizeof(struct mreal));
    memcpy(p_old->t, plc->t, conf->nt * sizeof(struct timer));
    memcpy(p_old->s, plc->s, conf->ns * sizeof(struct blink));
    
    plc->old = p_old;
    plc->command = 0;
    plc->status = ST_RUNNING;
    plc->step = conf->step;
    plc->response_file = conf->response_file;
}

