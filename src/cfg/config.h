#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ADVANTECH_HISTORICAL_BASE 50176
#define CONF_OK 0
#define CONF_ERR -1
#define CONF_STR 2048 //string length
#define CONF_MAX_SEQ 256 //max sequence elements
#define CONF_NUM 24 //number digits 
#define CONF_F 0
#define CONF_T 1

typedef enum{
    MAP_ROOT,
    MAP_USPACE,
    MAP_COMEDI,
    MAP_COMEDI_SUBDEV,
    MAP_SIM,
    MAP_VARIABLE,
    N_MAPPINGS    
}CONFIG_MAPPINGS;

typedef enum{
    USPACE_BASE,
    USPACE_WR,
    USPACE_RD,
    N_USPACE_VARS
}USPACE_VARS;

typedef enum{
    SUBDEV_IN,
    SUBDEV_OUT,
    SUBDEV_ADC,
    SUBDEV_DAC,
    N_SUBDEV_VARS
}SUBDEV_VARS;

typedef enum{
    COMEDI_FILE,
    COMEDI_SUBDEV,
    N_COMEDI_VARS
}COMEDI_VARS; 

typedef enum {
    SIM_INPUT,
    SIM_OUTPUT,
    N_SIM_VARS
}SIM_VARS;

typedef enum{
    CONFIG_STEP,
    CONFIG_PIPE,
    CONFIG_HW,
    CONFIG_USPACE,
    CONFIG_COMEDI,
    CONFIG_SIM,
    
     //(runtime updatable) sequences,
    CONFIG_PROGRAM,
    CONFIG_AI,
    CONFIG_AQ,
    CONFIG_DI,
    CONFIG_DQ,
    CONFIG_MVAR,
    CONFIG_MREG,
    CONFIG_TIMER,
    CONFIG_PULSE, 
    N_CONFIG_VARIABLES
} CONFIG_VARIABLES;

typedef enum {
    STORE_KEY,
    STORE_VAL,
    N_STORAGE_MODES
} STORAGE_MODE;

typedef enum {
    ENTRY_NONE,
	ENTRY_INT,
	ENTRY_STR,
	ENTRY_MAP,
	ENTRY_SEQ,
	N_ENTRY_TYPES
} ENTRY_TYPE;

typedef struct param {
    char * key; //min, max, default, preset, readonly, countdown, etc.
    char * value;
    struct param * next;
} * param_t;

/*FIXME: make a hashmap out of this */
#define MAX_PARAM 256
typedef param_t * param_table_t;

typedef struct variable {
    unsigned char index;
    char * name ;
/*    unsigned char updated;
    char * value;
  */
  param_t params;    
} * variable_t;

typedef struct sequence {
    int size;
    variable_t vars;
} * sequence_t;

typedef struct entry {
	int type_tag;
	char * name;
	
	union {
	    int scalar_int;
		char * scalar_str;
		sequence_t seq;
		struct config * conf;
	} e;
} * entry_t;

typedef entry_t * entry_map_t;

typedef struct config {
    unsigned int size;
    int err;
    entry_map_t map;
} * config_t;

/**
 * @brief construct a sequence
 * @param the size of the sequence
 * @return a newly alloced sequence
 */
sequence_t new_sequence(int size);

/**
 * @brief copy a sequence
 * @param another sequence
 * @return a newly alloced sequence
 */
sequence_t copy_sequence(sequence_t other);

/**
 * @brief construct  configuration
 * @param the size of the entry map
 * @return a newly alloced config
 */
config_t new_config(int size);

/**
 * @brief copy a configuration
 * @param another configuration
 * @return a newly alloced config
 */
config_t copy_config(config_t other);

/**
 * @brief cleanup and free configuration
 * @param the configuration
 * @return NULL
 */
config_t clear_config(config_t c);

/**
 * @brief new numeric entry
 * @param numeric value
 * @param entry map key
 * @return newly allocated entry
 */
entry_t new_entry_int(int i, char * name);

/**
 * @brief new string entry
 * @param str value
 * @param entry map key
 * @return newly allocated entry
 */
entry_t new_entry_str(char * str, char * name);

/**
 * @brief new recursive map entry
 * @param the map
 * @param entry map key
 * @return newly allocated entry
 */
entry_t new_entry_map(config_t map, char * name);

/**
 * @brief new fixed length sequence entry
 * @param the sequence
 * @param entry map key
 * @return newly allocated entry
 */
entry_t new_entry_seq(sequence_t seq, char * name);

/**
 * @brief new empty entry (for debugging only)
 * @return newly allocated entry
 */
entry_t new_entry_null();

/**
 * @brief update and replace an entry in a configuration
 * @param the entry key
 * @param the new entry
 * @param the configuration
 * @return the updated configuration
 */
config_t update_entry(unsigned int key, 
                      const entry_t item,
                      const config_t conf);

/**
 * @brief get config entry by key
 */
entry_t get_entry(int key, const config_t conf);

/**
 * @brief copy an entry
 * @param another entry
 * @return newly allocated entry
 */
entry_t copy_entry(entry_t other);

/**
 * @brief get numeric config entry by key
 */
int get_numeric_entry(int key, const config_t conf);

/**
 * @brief set numeric config entry by key
 */
config_t set_numeric_entry(int key, int val, const config_t conf);

/**
 * @brief get string config entry by key
 */
char * get_string_entry(int key, const config_t conf);

/**
 * @brief get sequence config entry by key
 */
sequence_t get_sequence_entry(int key, const config_t conf);

/**
 * @brief get variable by name
 * @param the name
 * @param sequence of variables
 * @return found variable or NULL
 */
variable_t get_variable(const char * name, const sequence_t seq);

/**
 * @brief get recursive map config entry by key
 */
config_t get_recursive_entry(int key, const config_t conf);

/**
 * @brief get config key by literal value
 */
int get_key(const char * value, const config_t conf);

/**
 * @brief get param val by literal key value
 */
char * get_param_val(const char * key, const param_t params);

/**
 * @brief construct a new param
 * @param the key
 * @param the value
 * @return newly alloced param
 */
param_t new_param(const char * key, 
                     const char * val);

/**
 * @brief deep copy parameter list
 * @param another list
 * @return new param list
 */
param_t copy_params(param_t other);

/**
 * @brief get param by literal value
 */
param_t get_param(const char * key, const param_t params);

/**
 * @brief append param
 */
param_t append_param(const param_t params, 
                     const char * key, 
                     const char * val);
/**
 * @brief append or update param value
 * @param param param   
 * @param key
 * @param value
 * @return updated param
 */
param_t update_param(const param_t params, 
                     const char * key, 
                     const char * val);

/**
 * @brief high level convenience function.
 * append or update a sequence param value
 * @param the configuration
 * @param block sequence name
 * @param variable index
 * @param key
 * @param value
 * @return updated sequence or NULL
 */
config_t edit_seq_param(config_t conf,                     
                                const char * seq_name, 
                                unsigned char idx,
                                const char * key, 
                                const char * val);

/**
 * @brief store a value to a map
 * @param the key
 * @param the value
 * @param where to store
 * @return config with applied value or changed errorcode 
 */
config_t store_value(
    unsigned char key, 
    const char * value, 
    config_t c);

/**
 * @brief store a value to a map that is nested in a sequence
 * @param the sequence 
 * @param the sequence index
 * @param the key
 * @param the value
 * @param where to store
 * @return config with applied value or changed errorcode
 */
config_t store_seq_value(const sequence_t seq,
                    unsigned char idx, 
                    const char * key, 
                    const char * value, 
                    config_t c);

/**
 * @brief copy all the block sequences
 * @param source
 * @param destination
 * @return destination
 */ 
config_t copy_sequences(const config_t from, config_t to);

/**
 * @brief resize sequence in memory
 * @param the configuration
 * @param seq number
 * @param new size
 * @return updated configuration
 */
config_t resize_sequence(config_t config, int sequence, int size);

/********these are abstract, implementation is required per serialization format (yml/json/cbor)*****/

/**
 * @brief entry point: load text file into configuration
 * @param filename (full path)
 * @param the configuration
 * @return new config
 */
config_t load_config(const char * filename, config_t conf);


/**
 * @brief entry point: save configuration to text file 
 * @param filename (full path)
 * @param the configuration
 * @return OK or ERR
 */
int save_config(const char * filename, const config_t conf);

/**
 * @brief print configuration to a file
 * @param the open file
 * @param the configuration where the parsed values are stored
 * @return OK or ERR
 */        
int print_config(FILE * f, const config_t conf);  

/**
 * @brief serialize configuration to a string
 * @param the configuration where the parsed values are stored
 * @return string allocated with CONF_STR size  
 * must be consequently free'd.
 */          
char * serialize_config(const config_t conf);

/**
 * @brief deserialize string to a config
 * @param a string
 * @param a config that defines the structure
 * @return the configuration where the parsed values are stored, or NULL
 */          
config_t deserialize_config(const char * buf, 
                            const config_t conf);         
 

 
#endif //_CONFIG_H_




































