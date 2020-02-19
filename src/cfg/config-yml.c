#include <yaml.h>
#include "util.h"
#include "config.h"

/*
static void yaml_config_error(yaml_parser_t parser){

    //print line
}
*/
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

static config_t process_seq_element(
                   yaml_event_t event,
                   int sequence, 
                   const char * key,
                   int * idx,
                   config_t config){
    
    config_t conf = config;
    char * val = (char *)event.data.scalar.value;
    long size = 0;
    if(key[0] == 0) { //we are not in a map yet
        size = strtol(val, NULL, 10);
    }
    if(size > 0 &&
        size < CONF_MAX_SEQ){
        conf = resize_sequence(conf, sequence, (int)size);
    } else if(!strcmp(key, "INDEX")){
                             
            *idx = atoi(val);
    } else {  
        sequence_t s = get_sequence_entry(sequence, conf); 
        if( s != NULL){                
            conf = store_seq_value(s,*idx,key,val,conf);       
        }    
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

/**
 * @brief forward declaration for mutual recursion: process an initialized parser, recursively for each mapping
 * @param sequence no. if the mapping is inside of a sequence, 
 * PLC_ERR otherwise
 * @param the parser
 * @param the configuration where the parsed values are stored
 * @return config with applied values or changed errorcode
 */
config_t process(int sequence, 
            yaml_parser_t *parser, 
            config_t configuration);

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
                    
        c->e.conf = process(seq,parser,c->e.conf);        
        conf->map[k] = c;
    } else {
                    
        conf = process(seq,parser,conf);
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
                    memset(key, 0, CONF_STR);
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
             plc_log("Could not parse event:");
             log_yml_event(event);
         }            
         //log_yml_event(event);                                  
         yaml_event_delete(&event);   
     }
     return config;
}
            

static void emit_variable(const variable_t var, yaml_emitter_t *emitter) {
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
    		
    		            
        yaml_scalar_event_initialize(
                        	&evt,
                    	    NULL,
                    		NULL,
                    		(yaml_char_t *)"INDEX",
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
                    		(yaml_char_t *)idx,
                    		strlen(idx),
                    		CONF_T,
                    		CONF_T, 
                    		YAML_PLAIN_SCALAR_STYLE); 	
        yaml_emitter_emit(emitter, &evt);
    		            
   
        yaml_scalar_event_initialize(
                        	&evt,
                    	    NULL,
                    		NULL,
                    		(yaml_char_t *)"ID",
                    		2,
                    		CONF_T,
                    		CONF_T, 
                    		YAML_PLAIN_SCALAR_STYLE); 
        yaml_emitter_emit(emitter, &evt);
                    			
        yaml_scalar_event_initialize(
                        	&evt,
                    	    NULL,
                    		NULL,
                    		(yaml_char_t *)var->name,
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
                    		(yaml_char_t *)it->key,
                    		strlen(it->key),
                    		CONF_T,
                    		CONF_T, 
                    		YAML_PLAIN_SCALAR_STYLE); 
            yaml_emitter_emit(emitter, &evt);
 //fixme: mulitiline                   			
            yaml_scalar_event_initialize(
                        	&evt,
                    	    NULL,
                    		NULL,
                    		(yaml_char_t *)it->value,
                    		strlen(it->value),
                    		CONF_T,
                    		CONF_T, 
                    		YAML_PLAIN_SCALAR_STYLE); 
            //log_yml_event(evt);        			
            yaml_emitter_emit(emitter, &evt);
            it = it->next; 
        }    
        yaml_mapping_end_event_initialize(&evt); 
        	
        yaml_emitter_emit(emitter, &evt); 
    }
} 

static void emit_entry(const entry_t entry, yaml_emitter_t *emitter) {
    int i = 0;
    yaml_event_t evt;
    if(!entry){
    
        return;
    }
    //plc_log("string %s len %d", entry->name, strlen(entry->name));
    
    yaml_scalar_event_initialize(
    	&evt,
	    NULL,
		(yaml_char_t *)YAML_STR_TAG,
		(yaml_char_t *)entry->name,
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
         	(yaml_char_t *)YAML_STR_TAG,
			(yaml_char_t *)entry->e.scalar_str,
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
			(yaml_char_t *)YAML_INT_TAG,
			(yaml_char_t *)buf,
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
    			(yaml_char_t *)YAML_MAP_TAG,
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
    			(yaml_char_t *)YAML_SEQ_TAG,
    			CONF_T,
    			YAML_BLOCK_SEQUENCE_STYLE);
    		//log_yml_event(evt);
    	    yaml_emitter_emit(emitter, &evt); 	
		//emit size as int
		  
		    sprintf(buf, "%d", entry->e.seq->size);
			yaml_scalar_event_initialize(
    		        &evt,
	    	        NULL,
			(yaml_char_t *)YAML_STR_TAG,
			(yaml_char_t *)buf,
			strlen(buf),
			CONF_T,
			CONF_T, 
			YAML_PLAIN_SCALAR_STYLE); 	
		
			yaml_emitter_emit(emitter, &evt);		
			//emit values as map
			viter = entry->e.seq->vars;
    		
    		        i = 0;
			while(i < entry->e.seq->size){
			    if(viter) {
				   emit_variable(viter, emitter);
				}
				viter = &(entry->e.seq->vars)[++i];
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

    //doc start
    yaml_document_start_event_initialize(&evt, 
                                        NULL, 
                                        NULL, 
                                        NULL, 
                                        CONF_F); 
	yaml_emitter_emit(emitter, &evt); 		
   // log_yml_event(evt);
    
    yaml_mapping_start_event_initialize(&evt,
    	                                NULL,
    	                                NULL,
    	                                CONF_F,
    	                                YAML_BLOCK_MAPPING_STYLE);
    yaml_emitter_emit(emitter, &evt);
   // log_yml_event(evt);

    entry_t iter = conf->map[0];
    int i = 0;
    while(i < conf->size) {
        if(iter){
    	    emit_entry(iter, emitter);
    	}
    	iter = conf->map[++i];
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

int print_config_to_emitter(yaml_emitter_t emitter, 
                            const config_t conf){
    yaml_event_t event;
    
    int r = CONF_OK;
    yaml_stream_start_event_initialize(&event, YAML_UTF8_ENCODING);
    r = yaml_emitter_emit(&emitter, &event);
    if(r){
            r = emit(&emitter, conf);
    }
    if(r){
            yaml_stream_end_event_initialize(&event);    
            r = yaml_emitter_emit(&emitter, &event);   
    }
    return r;            
}

int print_config(FILE * fcfg, const config_t conf) {
    
    yaml_emitter_t emitter;
    //yaml_event_t event;
    
    int r = CONF_OK;
    
    if(!yaml_emitter_initialize(&emitter)){
        return CONF_ERR;    
    }
    if (fcfg) {
         
         yaml_emitter_set_output_file(&emitter, fcfg);
    }
    r = print_config_to_emitter(emitter, conf); 
    yaml_emitter_delete(&emitter);
    
    return r;
}

int save_config(const char * filename, const config_t conf) {
     
    FILE * fcfg;
    char path[CONF_STR];
    int r = CONF_OK;
    
    memset(path, 0, CONF_STR);
    sprintf(path, "%s", filename);

    if ((fcfg = fopen(path, "wb"))) {
         plc_log("Save configuration to %s ...", path);
 
         print_config(fcfg, conf);        
         if(r < CONF_OK)
            plc_log( "Configuration error ");
            
         fclose(fcfg);
    } else {
        r = CONF_ERR;
        plc_log("Could not open file %s for write", filename);
    }
    return r;
}

config_t load_config(const char * filename, config_t conf) {
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

char * serialize_config(const config_t conf) {
    
    yaml_emitter_t emitter;
    size_t written;
    
    if(!yaml_emitter_initialize(&emitter)){
        
        return NULL;    
    }
    char * buf = (char *)malloc(CONF_STR);       
    yaml_emitter_set_output_string(
        &emitter,
  	    ( yaml_char_t *) buf,
        CONF_STR,
        &written);
    print_config_to_emitter(emitter, conf); 
    yaml_emitter_delete(&emitter);    
    
    return buf;
}

config_t deserialize_config(const char * buf, const config_t conf) {
    yaml_parser_t parser;
    memset(&parser, 0, sizeof(parser));    
    config_t r = conf;
    
    if (!yaml_parser_initialize(&parser)) {
        yaml_parser_error(parser); 
        
        return r;   
    }
    if(buf != NULL){
        yaml_parser_set_input_string(
            &parser, 
            ( yaml_char_t *)buf, 
            strlen(buf));
        r = process(CONF_ERR, &parser, conf);
    } else {
        r->err = CONF_ERR;
    }    
    if(r->err < CONF_OK){
            plc_log( "Configuration error ");
    } 
    yaml_parser_delete(&parser);
    
    return r;
}




