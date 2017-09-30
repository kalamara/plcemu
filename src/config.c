#include <yaml.h>
#include "util.h"
#include "config.h"

char * strdup_r(const char * dest, const char * src) {
//strdup with realloc

    char * r = (!dest)?(char *)malloc(sizeof(src)):realloc(dest, sizeof(src));
        
    memset(r, 0, sizeof(src));
    sprintf(r, "%s", src);
    
    return r;
}

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

int get_numeric_entry(int key, const config_t conf){
    entry_t e = get_entry(key, conf);
    if(e && e->type_tag == ENTRY_INT){
    
        return e->e.scalar_int;
    } else {
    
        return CONF_ERR;    
    } 
}

char * get_string_entry(int key, const config_t conf){
    
    entry_t e = get_entry(key, conf);
    if(e && e->type_tag == ENTRY_STR){
    
        return e->e.scalar_str;
    } else {
    
        return NULL;    
    }    
}

sequence_t get_sequence_entry(int key, const config_t conf){
    entry_t e = get_entry(key, conf);
    if(e && e->type_tag == ENTRY_SEQ){
    
        return e->e.seq;
    } else {
    
        return NULL;    
    }    
}

config_t get_recursive_entry(int key, const config_t conf){
    entry_t e = get_entry(key, conf);
    if(e && e->type_tag == ENTRY_MAP){
    
        return e->e.conf;
    } else {
    
        return NULL;    
    }    
}

//TODO: in a c++ impl. this would be a hashmap
param_t new_param(const char * key, 
                     const char * val){
    
        param_t n = (param_t)malloc(sizeof(struct param));
        n->key = strdup(key);
        n->value = strdup(val);
        n->next = NULL;
        
        return n;        
}

param_t get_param(const char * key, const param_t params){
    param_t it = params;
    while(it && key){
        if(!strcmp(it->key, key)){
        
            return it;
        }
        it = it->next;            
    }
    
    return NULL;
}

char * get_param_val(const char * key, const param_t params){
    param_t it = get_param(key, params);
    
    return it?it->value:NULL;
}

param_t append_param(const param_t params, 
                     const char * key, 
                     const char * val){
        
    if(params == NULL){
        
        return new_param(key, val);
    } else {
        param_t ret = params;
        param_t it = params;
        while(it->next){
            it = it->next;
        }
        it->next = new_param(key, val);
                
        return ret;    
    }
}

param_t update_param(const param_t params, 
                     const char * key, 
                     const char * val){
        
    if(params == NULL){
        
        return new_param(key, val);         
    } else {
        param_t ret = params;
        param_t par = get_param(key, params);
        if(par){
            par->value = strdup_r(par->value, val);
        } else {
        //FIXME: ..and this is why we need a hashmap.
            ret = append_param(ret, key, val);  
        } 
        return ret;    
    }
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
    
    return CONF_ERR;  
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
	
	return r;
}

config_t init_config(){
 //TODO: in a c++ implementation this all can be done automatically 
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

config_t clear_config(config_t c){

    return (config_t)NULL;
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
    return CONF_OK;
}

config_t store_value(
            unsigned char key, 
            const char * value, 
            config_t config){

    entry_t e; 
    if( config == NULL) {
    
        return NULL;
    }
    
    config_t conf = config;
    e = get_entry(key, conf);
    
    if(e == NULL) {
        conf->err = CONF_ERR;
        
        return conf;
    }
    
    switch(e->type_tag){
         case ENTRY_INT:
            e->e.scalar_int = atoi(value);
            break;
         
         case ENTRY_STR:
         
            e->e.scalar_str = strdup_r(e->e.scalar_str, value);
            break;
            
         default: return conf;
    }        
    conf->map[key] = e;
       
    return conf;
}

config_t store_seq_value(
                    unsigned char seq,
                    unsigned char idx,  
                    const char * key,
                    const char * value, 
                    config_t config){
                    
    config_t conf = config;                
    entry_t s = conf->map[seq];
    
    if( s == NULL ||
        key == NULL || 
        value == NULL ||
        s->type_tag != ENTRY_SEQ ||
        idx >= s->e.seq->size) {
        
        conf->err = CONF_ERR;
        
        return conf;
    }            
    
    variable_t var = &(conf->map[seq]
                            ->e.seq
                            ->vars[idx]);
    
    conf->map[seq]
        ->e.seq
        ->vars[idx].index = idx;
    
    if(!strcmp(key, "ID")){
         conf->map[seq]
             ->e.seq
             ->vars[idx].name = strdup_r(var->name, value);
    } else {
        
        conf->map[seq]
            ->e.seq
            ->vars[idx].params = update_param(
                conf->map[seq]
                    ->e.seq
                    ->vars[idx].params,
                key,
                value);    
    }   
        
    return conf;                       
}

static config_t process_seq_element(
                   yaml_event_t event,
                   int sequence, 
                   const char * key,
                   int * idx,
                   config_t config){
    
    config_t conf = config;
    char * val = (char *)event.data.scalar.value;
                    
    if(!strcmp(key, "INDEX")){
                             
            *idx = atoi(val);
    } else {  
                             
            conf = store_seq_value(sequence, 
                                      *idx, 
                                      key, 
                                      val, 
                                      conf);       
    }                      
      
    return conf;                       
}

static config_t process_scalar(
                   yaml_event_t event,
                   const char * key,
                   config_t config){
    
    config_t conf = config;
    char * val = (char *)event.data.scalar.value;
               
    conf = store_value(
                    get_key(key, conf), 
                    val, 
                    conf);
                 
    return conf;                       
}

static config_t process_mapping(
                    const char * key,
                    int seq,
                    yaml_parser_t *parser,
                    config_t config){
    
    config_t conf = config;                
    int k = get_key(key, conf);
    entry_t c = get_entry(k, conf);
    
    if( c != NULL &&
        c->type_tag == ENTRY_MAP) {
                    
        c->e.conf = process(
                        seq, 
                        parser, 
                        c->e.conf);        
        conf->map[k] = c;
    } else {
                    
        conf = process(
            seq, 
            parser, 
            conf);
    }
    
    return conf;    
} 

config_t process(int sequence, 
            yaml_parser_t *parser, 
            config_t configuration){
             
    config_t config = configuration;
    unsigned char storage = STORE_KEY;   
    int done = CONF_F;
    char key[CONF_STR];
    int idx = CONF_ERR;
    yaml_event_t event;
    memset(&event, 0, sizeof(event));
    memset(key, 0, CONF_STR);
    if(config == NULL) {
     
        return NULL;
     }
     
     if(parser == NULL) {
        config->err = CONF_ERR;
     
        return config;
     }
//     || parser->context == NULL 
           
     while(done == CONF_F){
     
        if (!yaml_parser_parse(parser, &event)){   
                yaml_parser_error(*parser);
                config->err = CONF_ERR;
        } else {
   
            switch(event.type){
                case YAML_SCALAR_EVENT: 
//swap storage to process val after key and vice versa 
                    if(storage == STORE_KEY) {
                        memset(key, 0, CONF_STR);
                        sprintf(key, "%s", 
                            (char *)event.data.scalar.value);
                        
                        storage = STORE_VAL;
                    } else {
                        if(sequence > CONF_ERR) {
                    
                            config = process_seq_element(
                                    event,
                                    sequence,
                                    key,
                                    &idx,
                                    config);
                        } else {
                            
                            config = process_scalar(
                                    event,
                                    key,
                                    config);
                        }
                        storage = STORE_KEY;            
                    }
                    break;
                
                case YAML_SEQUENCE_START_EVENT:

                    sequence = get_key(key, config);
                    break;
                
                case YAML_SEQUENCE_END_EVENT:

                    sequence = CONF_ERR;
                    break;
                
                case YAML_MAPPING_START_EVENT:
                
                    config = process_mapping(
                                key,
                                sequence,
                                parser,
                                config); 
                    storage = STORE_KEY;            
                    break;
                    
                case YAML_MAPPING_END_EVENT:
                case YAML_STREAM_END_EVENT:     
                    
                    done = CONF_T;
                    break;
                    
                case YAML_NO_EVENT:
                    config->err = CONF_ERR;
                    break;    
                    
                default: break;    
            }
         }
         if(config->err < CONF_OK) {
             done = CONF_T;
             //log_yml_event(event);
         }                                              
         yaml_event_delete(&event);   
     }
     
     return config;
}
             
config_t load_config_yml(const char * filename, config_t conf) {
    yaml_parser_t parser;
    
    FILE * fcfg;
    char path[CONF_STR];

    memset(path, 0, CONF_STR);
    sprintf(path, "%s", filename);
    
    memset(&parser, 0, sizeof(parser));
    
    config_t r = conf;
    
    if (!yaml_parser_initialize(&parser)) {
    
        yaml_parser_error(parser);    
    }
    if ((fcfg = fopen(path, "r"))) {
        plc_log("Looking for configuration from %s ...", path);
        yaml_parser_set_input_file(&parser, fcfg);
        r = process(CONF_ERR, &parser, conf);
        if(r->err < CONF_OK)
            plc_log( "Configuration error ");
        fclose(fcfg);
    } else {
        r->err = CONF_ERR;
        plc_log("Could not open file %s", filename);
    }
    yaml_parser_delete(&parser);
    return r;
}

static void emit_variable(variable_t var, yaml_emitter_t *emitter) {
    yaml_event_t evt;
    if(var->name != NULL &&
        var->name[0]) {
       
        char idx[4];
        memset(idx, 0, 4);
    
        yaml_mapping_start_event_initialize(
    			        &evt,
    			        NULL,
    			        NULL,
    			        CONF_F,
    			        YAML_BLOCK_MAPPING_STYLE);
    	 	    
        yaml_emitter_emit(emitter, &evt);
    		//log_yml_event(evt);
    		            
        yaml_scalar_event_initialize(
                        	&evt,
                    	    NULL,
                    		NULL,
                    		"INDEX",
                    		5,
                    		CONF_T,
                    		CONF_T, 
                    		YAML_PLAIN_SCALAR_STYLE); 
        yaml_emitter_emit(emitter, &evt);
                    		
        sprintf(idx, "%d", var->index);		
        yaml_scalar_event_initialize(
                        	&evt,
                    	    NULL,
                    		NULL,
                    		(unsigned char *)idx,
                    		strlen(idx),
                    		CONF_T,
                    		CONF_T, 
                    		YAML_PLAIN_SCALAR_STYLE); 	
        yaml_emitter_emit(emitter, &evt);
    		            
   
        yaml_scalar_event_initialize(
                        	&evt,
                    	    NULL,
                    		NULL,
                    		"ID",
                    		2,
                    		CONF_T,
                    		CONF_T, 
                    		YAML_PLAIN_SCALAR_STYLE); 
        yaml_emitter_emit(emitter, &evt);
                    			
        yaml_scalar_event_initialize(
                        	&evt,
                    	    NULL,
                    		NULL,
                    		(unsigned char *)var->name,
                    		strlen(var->name),
                    		CONF_T,
                    		CONF_T, 
                    		YAML_PLAIN_SCALAR_STYLE); 	
        yaml_emitter_emit(emitter, &evt);
       
        param_t it = var->params;
        while(it){
            yaml_scalar_event_initialize(
                        	&evt,
                    	    NULL,
                    		NULL,
                    		(unsigned char *)it->key,
                    		strlen(it->key),
                    		CONF_T,
                    		CONF_T, 
                    		YAML_PLAIN_SCALAR_STYLE); 
            yaml_emitter_emit(emitter, &evt);
                    			
            yaml_scalar_event_initialize(
                        	&evt,
                    	    NULL,
                    		NULL,
                    		(unsigned char *)it->value,
                    		strlen(it->value),
                    		CONF_T,
                    		CONF_T, 
                    		YAML_PLAIN_SCALAR_STYLE); 	
            yaml_emitter_emit(emitter, &evt);
            it = it->next; 
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
		CONF_T,
		CONF_T, 
		YAML_PLAIN_SCALAR_STYLE); 	
		
	yaml_emitter_emit(emitter, &evt);
	//log_yml_event(evt);
	char buf[CONF_NUM];
	memset(buf, 0, CONF_NUM);
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
			CONF_T,
			CONF_T, 
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
			CONF_T,
			CONF_T, 
			YAML_PLAIN_SCALAR_STYLE); 	
		
			yaml_emitter_emit(emitter, &evt);
			//log_yml_event(evt); 		
			break;
				 
		case ENTRY_MAP:
		
		    yaml_mapping_start_event_initialize(
    			&evt,
    			NULL,
    			NULL,
    			CONF_F,
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
    			CONF_T,
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
			CONF_T,
			CONF_T, 
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
    int r = CONF_OK;     
   
    yaml_event_t evt;
    char val[CONF_STR];
    memset(val,0,CONF_STR);
    //doc start
    yaml_document_start_event_initialize(&evt, NULL, NULL, NULL, CONF_F); 
	yaml_emitter_emit(emitter, &evt); 		
   // log_yml_event(evt);
    
    yaml_mapping_start_event_initialize(
    	&evt,
    	NULL,
    	NULL,
    	CONF_F,
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
    yaml_document_end_event_initialize(&evt,CONF_F);
    yaml_emitter_emit(emitter, &evt);
    //log_yml_event(evt); 		
    yaml_event_delete(&evt); 	
    return r;
}

int save_config_yml(const char * filename, const config_t conf) {
    
    yaml_emitter_t emitter;
    yaml_event_t event;
    
    FILE * fcfg;
    char path[CONF_STR];
    int r = CONF_OK;
    
    memset(path, 0, CONF_STR);
    sprintf(path, "%s", filename);

    if(!yaml_emitter_initialize(&emitter)){
        return CONF_ERR;    
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
            
         if(r < CONF_OK)
            plc_log( "Configuration error ");
            
         fclose(fcfg);
    } else {
        r = CONF_ERR;
        plc_log("Could not open file %s for write", filename);
    }
    yaml_emitter_delete(&emitter);
    return r;
}

