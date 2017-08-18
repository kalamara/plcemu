#include <yaml.h>
#include "util.h"
#include "config.h"
#include "plclib.h"

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
    "COMEDI",
    "SIM",
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

static void yaml_config_error(yaml_parser_t parser){

    //print line
}

static void yaml_parser_error(yaml_parser_t parser){

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

static entry_t new_entry_int(int i, char * name) {

	entry_t r = (entry_t)malloc(sizeof(struct entry));
	r->type_tag = ENTRY_INT;
	r->name = name;
	r->e.scalar_int = i;

	return r;
}

static entry_t new_entry_str(char * str, char * name) {

	entry_t r = (entry_t)malloc(sizeof(struct entry));
	r->type_tag = ENTRY_STR;
	r->name = name;
	r->e.scalar_str = (char *)malloc(sizeof(str));
    sprintf(r->e.scalar_str, "%s", str);
	return r;
}

static entry_t new_entry_map(config_t map, char * name) {
	
	entry_t r = (entry_t)malloc(sizeof(struct entry));
	r->type_tag = ENTRY_MAP;
	r->name = name;
	r->e.conf = map;

	return r;
}

static entry_t new_entry_seq(sequence_t seq, char * name) {
	
	entry_t r = (entry_t)malloc(sizeof(struct entry));
	r->type_tag = ENTRY_SEQ;
	r->name = name;
	r->e.seq = seq;

	return r;
}

static entry_t new_entry_null() {
	entry_t r = (entry_t)malloc(sizeof(struct entry));
	r->type_tag = ENTRY_NONE;
	r->name = "";
	r->e.scalar_int = 0;

	return r;
}

static config_t update_entry(
    unsigned int key, 
    const entry_t item,
    const config_t conf) {
    
    if( conf == NULL ||
        key >= conf->size) {
    
        return conf;
    } else {
    
        config_t r = conf;
        r->map[key] = item;
        
        return r;
    }
}

entry_t get_entry(int key, const config_t conf){

    if(conf == NULL || 
        key < 0 || 
        key > conf->size) {
        
        return NULL;    
    }
    
    return conf->map[key];
}

int get_var_key(const char * name){
    for(int i = 0; i < N_VARIABLE_PARAMS; i++) {
        if(!strcmp(name, Variable_params[i])) {
            
            return i;
        }
    }
    
    return PLC_ERR;
}

int get_key(const char * name, const config_t where) {
     if(where != NULL){
    
        for(int i = 0; i < where->size; i++) {
            if( where->map[i] != NULL &&
                !strcmp(name, where->map[i]->name)) {
                
                return i;
            }
        }
    }
    
    return PLC_ERR;  
}

config_t new_config(int size) {
    config_t r = (config_t)malloc(sizeof(struct config));
	memset(r, 0, sizeof(struct config));
	r->size = size;
	r->map = (entry_map_t)malloc(size*sizeof(struct entry));
    memset(r->map, 0, size*sizeof(struct entry));
    
	return r;
}

sequence_t new_sequence(int size) {
    sequence_t r = (sequence_t)malloc(size*sizeof(struct sequence));
	memset(r, 0, sizeof(struct sequence));
	r->size = size;
	r->vars = (variable_t)malloc(size*sizeof(struct variable));
	memset(r->vars, 0, size*sizeof(struct variable));
	/*
	int i = 0;
	for(; i < size; i++){
	     
	}
    */
	return r;
}

config_t init_config(){
 //note: in a c++ implementation this all can be done automatically 
 //using a hashmap
    config_t conf = new_config(N_CONFIG_VARIABLES);
   
    config_t uspace = new_config(N_USPACE_VARS);
        
    uspace = update_entry(
        USPACE_BASE,
	    new_entry_int(50176, "USPACE_BASE"),
	    uspace);
	
	uspace = update_entry(
	    USPACE_WR, 
	    new_entry_int(0, "USPACE_WR"),
	    uspace);
	    
	uspace = update_entry(
	    USPACE_RD, 
	    new_entry_int(8, "USPACE_RD"),
	    uspace);
	
	config_t subdev = new_config(N_SUBDEV_VARS);
	
    subdev = update_entry(
        SUBDEV_IN,
	    new_entry_int(0, "SUBDEV_IN"),
	    subdev);
	    
	subdev = update_entry(
	    SUBDEV_OUT,
	    new_entry_int(1, "SUBDEV_OUT"),
	    subdev);
	    
    subdev = update_entry(
        SUBDEV_ADC, 
	    new_entry_int(2, "SUBDEV_ADC"),
	    subdev);
	    
	subdev = update_entry(
	    SUBDEV_DAC, 
	    new_entry_int(3, "SUBDEV_DAC"),
	    subdev);
	
	config_t comedi = new_config(N_COMEDI_VARS);
	
	comedi = update_entry(
	    COMEDI_FILE,
	    new_entry_int(0, "COMEDI_FILE"),
	    comedi);
	    
	comedi = update_entry(
	    COMEDI_SUBDEV, 
	    new_entry_map(subdev, "COMEDI_SUBDEV"),
	    comedi);
    
    config_t sim = new_config(N_SIM_VARS);
    
    sim = update_entry(
        SIM_INPUT,
        new_entry_str("sim.in", "SIM_INPUT"), 
        sim);
        
    sim = update_entry(
        SIM_OUTPUT,
        new_entry_str("sim.out", "SIM_OUTPUT"),
        sim);    

    conf = update_entry(
        CONFIG_STEP,
        new_entry_int(1, "STEP"),
        conf);
    
    conf = update_entry(
        CONFIG_PIPE,
        new_entry_str("plcpipe", "PIPE"),
        conf);
    
    conf = update_entry(
        CONFIG_HW,
        new_entry_str("STDI/O", "HW"),
        conf);
        
    conf = update_entry(
        CONFIG_USPACE,
        new_entry_map(uspace, "USPACE"),
        conf);
    
    conf = update_entry(
        CONFIG_COMEDI,
        new_entry_map(comedi, "COMEDI"),
        conf);
    
    conf = update_entry(
        CONFIG_SIM,
        new_entry_map(sim, "SIM"),
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

    return conf;
}

void clear_config(config_t *c){

}

static int log_yml_event(yaml_event_t event){

    switch(event.type){ 
    
        case YAML_NO_EVENT: 
        	plc_log("No event!"); 
            break;
    /* Stream start/end */
        case YAML_STREAM_START_EVENT: 
        	plc_log("STREAM START"); 
            break;
        case YAML_STREAM_END_EVENT: 
        	plc_log("STREAM END");   
            break;
    /* Block delimeters */
        case YAML_DOCUMENT_START_EVENT: 
        	plc_log("Start Document"); 
            break;
        case YAML_DOCUMENT_END_EVENT: 
        	plc_log("End Document");   
            break;
        case YAML_SEQUENCE_START_EVENT: 
        	plc_log("Start Sequence"); 
            break;
        case YAML_SEQUENCE_END_EVENT: 
        	plc_log("End Sequence");
            break;
        case YAML_MAPPING_START_EVENT: 
        	plc_log("Start Mapping");         
            break;
        case YAML_MAPPING_END_EVENT: 
        	plc_log("End Mapping");      
            break;
    /* Data */
        case YAML_ALIAS_EVENT:  
        	plc_log("Got alias (anchor %s)", event.data.alias.anchor); 
            break;
        case YAML_SCALAR_EVENT: 
        	plc_log("Got scalar (value %s)", event.data.scalar.value); 
        	break;
        default:
            plc_log("default?");
    }
    return PLC_OK;
}

char * strdup_r(const char * dest, const char * src) {
//strdup with realloc
    char * r = (dest == NULL)?(char *)malloc(sizeof(src)):dest;
        
    realloc(r, sizeof(src));
    memset(r, 0, sizeof(src));
    sprintf(r, "%s", src);
    
    return r;
}

int store_value(BYTE key, const char * value, config_t * c){

    entry_t e; 
    if( c == NULL) 
        return PLC_ERR;
    
    e = get_entry(key, *c);
    
    if(e == NULL)
        return PLC_ERR;
        
    switch(e->type_tag){
         case ENTRY_INT:
            e->e.scalar_int = atoi(value);
            break;
         
         case ENTRY_STR:
         
            e->e.scalar_str = strdup_r(e->e.scalar_str, value);
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
                    
    entry_t s = (*c)->map[seq];
    
    if( s == NULL ||
        s->type_tag != ENTRY_SEQ ||
        idx >= s->e.seq->size) {
        
        return PLC_ERR;
    }            
    
    variable_t vars = s->e.seq->vars;
    
    switch(key) {
        case VARIABLE_INDEX: 
            vars[idx].index = atoi(value);
            break;
            
        case VARIABLE_ID: 
            vars[idx].name = strdup_r(vars[idx].name, value);
            break;
            
        case VARIABLE_VALUE: 
            vars[idx].value = strdup_r(vars[idx].value, value);
            break;
            
        case VARIABLE_MAX: 
            vars[idx].max = strdup_r(vars[idx].max, value);
            break; 
            
        case VARIABLE_MIN: 
            vars[idx].min = strdup_r(vars[idx].min, value);
            break;         
               
        default: 
            return PLC_ERR;    
    }
        
    return PLC_OK;                       
}

int process(int sequence, 
            yaml_parser_t *parser, 
            config_t conf){
             
     int ret = PLC_OK;
     BYTE storage = STORE_KEY;   
     int done = FALSE;
     int key = PLC_ERR;
     int found_seq = sequence > PLC_ERR;
     int idx = PLC_ERR;
     yaml_event_t event;
     memset(&event, 0, sizeof(event));
     
     if(parser == NULL
//     || parser->context == NULL 
     || conf == NULL)
     
        return PLC_ERR;
           
     while(done == FALSE){
     
        if (!yaml_parser_parse(parser, &event)){   
                yaml_parser_error(*parser);
                ret = PLC_ERR;
        } else {
   
            switch(event.type){
            
                case YAML_SCALAR_EVENT: {
                    char * val = (char *)event.data.scalar.value;
                    
                    if(storage == STORE_KEY) {
                        if(found_seq) {
                            
                            key = get_var_key(val);
                        } else {
                        
                            key = get_key(val,conf);    
                        }
                        storage = STORE_VAL;
                    } else {
   
                        if(found_seq) {
                        
                            if(key == VARIABLE_INDEX){
                             
                                 idx = atoi(val);
                            } else {  
                             
                                ret = store_seq_value(
                                         sequence, 
                                         idx, 
                                         key, 
                                         val, 
                                         &conf);       
                            }   
                        } else {
                        
                            ret = store_value(key, val, &conf);
                        }
                        storage = STORE_KEY;    
                    }
                }   break;
                
                case YAML_SEQUENCE_START_EVENT:
                
                    found_seq = TRUE;
                    break;
                
                case YAML_SEQUENCE_END_EVENT:
                
                    found_seq = FALSE;
                    break;
                
                case YAML_MAPPING_START_EVENT: {
                    
                    entry_t c = get_entry(key, conf);
                    if( c != NULL &&
                        c->type_tag == ENTRY_MAP) {
                    
                        ret = process(
                            found_seq?key:-1, 
                            parser, 
                            c->e.conf);
                        
                    } else {
                    
                        ret = process(
                            found_seq?key:-1, 
                            parser, 
                            conf);
                    }
                    storage = STORE_KEY;
                    
                }   break;
                    
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
             //log_yml_event(event);
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

static void emit_variable(variable_t var, yaml_emitter_t *emitter) {
    yaml_event_t evt;
    if( var->name != NULL &&
        var->name[0]) {
       
       char idx[4];
       memset(idx, 0, 4);
    
       yaml_mapping_start_event_initialize(
    			        &evt,
    			        NULL,
    			        NULL,
    			        FALSE,
    			        YAML_BLOCK_MAPPING_STYLE);
    	 	    
       yaml_emitter_emit(emitter, &evt);
    		//log_yml_event(evt);
    		            
       yaml_scalar_event_initialize(
                        	&evt,
                    	    NULL,
                    		NULL,
                    		"INDEX",
                    		5,
                    		TRUE,
                    		TRUE, 
                    		YAML_PLAIN_SCALAR_STYLE); 
       yaml_emitter_emit(emitter, &evt);
                    		
       sprintf(idx, "%d", var->index);		
       yaml_scalar_event_initialize(
                        	&evt,
                    	    NULL,
                    		NULL,
                    		(unsigned char *)idx,
                    		strlen(idx),
                    		TRUE,
                    		TRUE, 
                    		YAML_PLAIN_SCALAR_STYLE); 	
       yaml_emitter_emit(emitter, &evt);
    		            
   
       yaml_scalar_event_initialize(
                        	&evt,
                    	    NULL,
                    		NULL,
                    		"ID",
                    		2,
                    		TRUE,
                    		TRUE, 
                    		YAML_PLAIN_SCALAR_STYLE); 
       yaml_emitter_emit(emitter, &evt);
                    			
       yaml_scalar_event_initialize(
                        	&evt,
                    	    NULL,
                    		NULL,
                    		(unsigned char *)var->name,
                    		strlen(var->name),
                    		TRUE,
                    		TRUE, 
                    		YAML_PLAIN_SCALAR_STYLE); 	
       yaml_emitter_emit(emitter, &evt);

       yaml_scalar_event_initialize(
                        	&evt,
                    	    NULL,
                    		NULL,
                    		"VALUE",
                    		5,
                    		TRUE,
                    		TRUE, 
                    		YAML_PLAIN_SCALAR_STYLE); 
       yaml_emitter_emit(emitter, &evt);
                    			
       yaml_scalar_event_initialize(
                        	&evt,
                    	    NULL,
                    		NULL,
                    		(unsigned char *)var->value,
                    		strlen(var->value),
                    		TRUE,
                    		TRUE, 
                    		YAML_PLAIN_SCALAR_STYLE); 	
        yaml_emitter_emit(emitter, &evt);
        if(strcmp(var->min, var->max)) {
            yaml_scalar_event_initialize(
                        	&evt,
                    	    NULL,
                    		NULL,
                    		"MIN",
                    		3,
                    		TRUE,
                    		TRUE, 
                    		YAML_PLAIN_SCALAR_STYLE); 
            yaml_emitter_emit(emitter, &evt);
                    			
            yaml_scalar_event_initialize(
                        	&evt,
                    	    NULL,
                    		NULL,
                    		(unsigned char *)var->min,
                    		strlen(var->min),
                    		TRUE,
                    		TRUE, 
                    		YAML_PLAIN_SCALAR_STYLE); 	
            yaml_emitter_emit(emitter, &evt);

            yaml_scalar_event_initialize(
                        	&evt,
                    	    NULL,
                    		NULL,
                    		"MAX",
                    		3,
                    		TRUE,
                    		TRUE, 
                    		YAML_PLAIN_SCALAR_STYLE); 
            yaml_emitter_emit(emitter, &evt);
                    			
            yaml_scalar_event_initialize(
                        	&evt,
                    	    NULL,
                    		NULL,
                    		(unsigned char *)var->max,
                    		strlen(var->max),
                    		TRUE,
                    		TRUE, 
                    		YAML_PLAIN_SCALAR_STYLE); 	
            yaml_emitter_emit(emitter, &evt);
        }       
        yaml_mapping_end_event_initialize(&evt); 	
        yaml_emitter_emit(emitter, &evt); 
    }
} 

static void emit_entry(entry_t entry, yaml_emitter_t *emitter) {
    int i = 0;
    yaml_event_t evt;
     
    yaml_scalar_event_initialize(
    	&evt,
	    NULL,
		NULL,
		(unsigned char *)entry->name,
		strlen(entry->name),
		TRUE,
		TRUE, 
		YAML_PLAIN_SCALAR_STYLE); 	
		
	yaml_emitter_emit(emitter, &evt);
	//log_yml_event(evt);
	char buf[TINYBUF];
	memset(buf, 0, TINYBUF);
	entry_t iter = NULL;
	variable_t viter = NULL;
	
	switch (entry->type_tag){
	
		case ENTRY_STR:
		
			yaml_scalar_event_initialize(
    		&evt,
	    	NULL,
			NULL,
			(unsigned char *)entry->e.scalar_str,
			strlen(entry->e.scalar_str),
			TRUE,
			TRUE, 
			YAML_PLAIN_SCALAR_STYLE); 	
		
			yaml_emitter_emit(emitter, &evt); 
			//log_yml_event(evt);		
			break;
		
		case ENTRY_INT:
			
			sprintf(buf, "%d", entry->e.scalar_int);
			yaml_scalar_event_initialize(
    		&evt,
	    	NULL,
			NULL,
			(unsigned char *)buf,
			strlen(buf),
			TRUE,
			TRUE, 
			YAML_PLAIN_SCALAR_STYLE); 	
		
			yaml_emitter_emit(emitter, &evt);
			//log_yml_event(evt); 		
			break;
				 
		case ENTRY_MAP:
		
		    yaml_mapping_start_event_initialize(
    			&evt,
    			NULL,
    			NULL,
    			FALSE,
    			YAML_BLOCK_MAPPING_STYLE);
    	 	    
    		yaml_emitter_emit(emitter, &evt);
    		//log_yml_event(evt);
    		iter = *(entry->e.conf->map);
    		
			while(i < entry->e.conf->size){
			    if(iter != NULL) {
				    emit_entry(iter, emitter);  
				}
				iter = (entry->e.conf->map)[++i];
			}	
			yaml_mapping_end_event_initialize(&evt); 	
    		yaml_emitter_emit(emitter, &evt); 
    		//log_yml_event(evt);
			break;
		
		case ENTRY_SEQ:
		  yaml_sequence_start_event_initialize(
    			&evt,
    			NULL,
    			NULL,
    			TRUE,
    			YAML_BLOCK_SEQUENCE_STYLE);
    		//log_yml_event(evt);
    	    yaml_emitter_emit(emitter, &evt); 	
		//emit size as int
		  
		    sprintf(buf, "%d", entry->e.seq->size);
			yaml_scalar_event_initialize(
    		&evt,
	    	NULL,
			NULL,
			(unsigned char *)buf,
			strlen(buf),
			TRUE,
			TRUE, 
			YAML_PLAIN_SCALAR_STYLE); 	
		
			yaml_emitter_emit(emitter, &evt);
			//log_yml_event(evt); 		
			//emit values as map
			viter = entry->e.seq->vars;
    		
    		i = 0;
			while(i < entry->e.seq->size){
			    if(viter != NULL) {
				   emit_variable(viter, emitter);
				}
				*viter = (entry->e.seq->vars)[++i];
			}	
			
			yaml_sequence_end_event_initialize(&evt); 	
    		yaml_emitter_emit(emitter, &evt); 
    		//log_yml_event(evt);
			break;
			
		default:break;
	}
	
}

int emit(yaml_emitter_t *emitter, const config_t conf) {
    int r = PLC_OK;     
   
    yaml_event_t evt;
    char val[MAXSTR];
    memset(val,0,MAXSTR);
    //doc start
    yaml_document_start_event_initialize(&evt, NULL, NULL, NULL, FALSE); 
	yaml_emitter_emit(emitter, &evt); 		
   // log_yml_event(evt);
    
    yaml_mapping_start_event_initialize(
    	&evt,
    	NULL,
    	NULL,
    	FALSE,
    	YAML_BLOCK_MAPPING_STYLE);
    	 	    
    yaml_emitter_emit(emitter, &evt);
   // log_yml_event(evt);
    
     
    entry_map_t config_map = conf->map;
    entry_t iter = *config_map;
    int i = 0;
    while(i < N_CONFIG_VARIABLES) {
        if(iter != NULL){
    	    emit_entry(iter, emitter);
    	}
    	iter = config_map[++i];
    }
   
    //mapping end
    yaml_mapping_end_event_initialize(&evt); 	
    yaml_emitter_emit(emitter, &evt); 		
    //log_yml_event(evt);
    
    //doc end
    yaml_document_end_event_initialize(&evt,FALSE);
    yaml_emitter_emit(emitter, &evt);
    //log_yml_event(evt); 		
    yaml_event_delete(&evt); 	
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

