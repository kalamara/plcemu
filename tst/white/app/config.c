#include "config.h"

char * strdup_r(char * dest, const char * src) {
//strdup with realloc

    char * r = (!dest)?(char *)malloc(strlen(src)):realloc(
                                            (void*)dest, strlen(src));
        
    memset(r, 0, strlen(src));
    sprintf(r, "%s", src);
    
    return r;
}

entry_t new_entry_int(int i, char * name) {
	entry_t r = (entry_t)malloc(sizeof(struct entry));
	r->type_tag = ENTRY_INT;
	r->name = name;
	r->e.scalar_int = i;

	return r;
}

entry_t new_entry_str(char * str, char * name) {

	entry_t r = (entry_t)malloc(sizeof(struct entry));
	r->type_tag = ENTRY_STR;
	r->name = name;
	r->e.scalar_str = (char *)malloc(sizeof(str));
    sprintf(r->e.scalar_str, "%s", str);
	return r;
}

entry_t new_entry_map(config_t map, char * name) {
	
	entry_t r = (entry_t)malloc(sizeof(struct entry));
	r->type_tag = ENTRY_MAP;
	r->name = name;
	r->e.conf = map;

	return r;
}

entry_t new_entry_seq(sequence_t seq, char * name) {
	
	entry_t r = (entry_t)malloc(sizeof(struct entry));
	r->type_tag = ENTRY_SEQ;
	r->name = name;
	r->e.seq = seq;

	return r;
}

entry_t new_entry_null() {
	entry_t r = (entry_t)malloc(sizeof(struct entry));
	r->type_tag = ENTRY_NONE;
	r->name = "";
	r->e.scalar_int = 0;

	return r;
}

config_t update_entry(
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

entry_t copy_entry(entry_t other){

    entry_t r = NULL;
    if(other == NULL){
    
        return NULL;
    }
    switch(other->type_tag){
        case ENTRY_INT:
            r = new_entry_int(
                    other->e.scalar_int, 
                    other->name);
            break;
	    case ENTRY_STR:
	        r = new_entry_str(
                    other->e.scalar_str, 
                    other->name);
	        break;
	    case ENTRY_MAP:
	        r = new_entry_map(copy_config(other->e.conf), other->name);
	        
	        break;
	    case ENTRY_SEQ:
	        r = new_entry_seq(copy_sequence(other->e.seq), other->name);
	        break;
        default: //NULL
            r = new_entry_null();
    }
    return r;
}

int get_numeric_entry(int key, const config_t conf){
    entry_t e = get_entry(key, conf);
    if(e && e->type_tag == ENTRY_INT){
    
        return e->e.scalar_int;
    } else {
    
        return CONF_ERR;    
    } 
}

config_t set_numeric_entry(int key, int val, config_t conf){
    config_t c = conf;
    
    entry_t e = get_entry(key, c);
    if(!e){
        return NULL;
    }
    e->e.scalar_int = val;
    conf->map[key] = e;
    
    return c;
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

variable_t get_variable(const char * name, const sequence_t seq){
    if(seq != NULL){
        int i = 0;
        for(; i < seq->size; i++){
            if(seq->vars[i].name != NULL &&
                !strcmp(seq->vars[i].name, name)){
                    
                return &(seq->vars[i]);            
            }
        }
    }
    return NULL;
}

config_t get_recursive_entry(int key, const config_t conf){
    entry_t e = get_entry(key, conf);
    if(e && e->type_tag == ENTRY_MAP){
    
        return e->e.conf;
    } else {
    
        return NULL;    
    }    
}

config_t set_recursive_entry(int key, const config_t val, config_t conf){
    config_t c = conf;
   
    entry_t e = get_entry(key, c);
    if(!e){
        return NULL;
    }
    e->e.conf = val;
    conf->map[key] = e;
    
    return c;
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

param_t copy_params(param_t other){
    param_t iter = other;
    param_t r = NULL;
    while(iter){
        r = append_param(r, iter->key, iter->value);
        iter = iter->next;
    }
    return r;
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

config_t edit_seq_param(config_t conf,                     
                                const char * seq_name, 
                                unsigned char idx,
                                const char * key, 
                                const char * val){
    int k = get_key(seq_name, conf);
    sequence_t seq = get_sequence_entry(k, conf);
    if(seq == NULL || k == CONF_ERR){
    
        return NULL;
    }
    return store_seq_value(seq, idx, key, val, conf);
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

config_t copy_config(config_t other){
    if(other == NULL){
        
        return NULL;
    }
    config_t r = new_config(other->size);
    int i = 0;
    for(;i< other->size; i++){
        r->map[i] = copy_entry(other->map[i]);
    }
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

sequence_t copy_sequence(sequence_t other){
    if(other == NULL){
        
        return NULL;
    }
    sequence_t r = new_sequence(other->size);
    int i = 0;
    for(;i < other->size; i++){
        r->vars[i].index = other->vars[i].index;
        if(other->vars[i].name){
            r->vars[i].name = strdup(other->vars[i].name);
        }
        r->vars[i].params = copy_params(other->vars[i].params);
    }
    return r;
}

config_t clear_config(config_t c){

    return (config_t)NULL;
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
                    const sequence_t s,
                    unsigned char idx,  
                    const char * key,
                    const char * value, 
                    config_t config){
    config_t conf = config;                
    if( s == NULL ||
        key == NULL || 
        value == NULL ||
        idx >= s->size) {     
        conf->err = CONF_ERR;
        
        return conf;
    }            
    variable_t var = &(s->vars[idx]);
    s->vars[idx].index = idx;
    if(!strcmp(key, "ID")){
        s->vars[idx].name = strdup_r(var->name, value);
    } else {
        s->vars[idx].params = update_param(s->vars[idx].params,key,value);    
    }   
    return conf;                       
}

config_t resize_sequence(config_t config, int sequence, int size){
    config_t conf = config;
    sequence_t seq = get_sequence_entry(sequence, conf);
    if(size <= 0 ||
        size > CONF_MAX_SEQ || 
        seq == NULL){
        conf->err = CONF_ERR;
        
        return conf;
    }  
    seq->size = size;
	seq->vars = (variable_t)realloc(seq->vars, size*sizeof(struct variable));
	memset(seq->vars, 0, size*sizeof(struct variable));    
        
    return conf;
}

config_t copy_sequences(const config_t conf, config_t com){
    
    if(!conf || !com){
    
        return NULL;
    }
    int i = 0;    
    for(; i < conf->size; i++){
        entry_t en = get_entry(i, conf);
        if(en && en->type_tag == ENTRY_SEQ){
            com = update_entry(i,
                copy_entry(en),
                com);
        }
    }
    return com;
}

config_t init_config(const struct entry schema[], unsigned int size){
 
    config_t conf = new_config(size);
    int i = 0;
    for(; i < size; i++){
        const entry_t iter = &(schema[i]);
        int sz = 0;
        if(iter){
            switch(iter->type_tag){
            case ENTRY_INT:
                conf = update_entry(i,
                            new_entry_int(iter->e.scalar_int, 
                                          iter->name),
                            conf);
                break;
            case ENTRY_STR:
                conf = update_entry(i,
                            new_entry_str(iter->e.scalar_str, 
                                          iter->name), 
                            conf);
                break;             
            case ENTRY_MAP:
                conf = update_entry(i,
                            new_entry_map(copy_config(iter->e.conf), 
                                          iter->name),
                            conf);
                break;
            case ENTRY_SEQ:
                if(iter->e.seq){
                    sz = iter->e.seq->size;
                }
                conf = update_entry(i,
                            new_entry_seq(new_sequence(sz),
                                          iter->name),
                            conf); 
                break;                                  
            default: break;
            }
        }
    }
    return conf;
}

