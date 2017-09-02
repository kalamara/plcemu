#ifndef _CONFIG_H_
#define _CONFIG_H_
#include <yaml.h>

#define ADVANTECH_HISTORICAL_BASE 50176

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
    CONFIG_SIGENABLE,//DEPRECATED
    CONFIG_PAGELEN,//DEPRECATED
    CONFIG_PAGEWIDTH,//DEPRECATED
    CONFIG_HW,
    CONFIG_NT,
    CONFIG_NS,
    CONFIG_NR,
    CONFIG_NM,
    CONFIG_NDI,
    CONFIG_NDQ,
    CONFIG_NAI,
    CONFIG_NAQ,
    CONFIG_RESPONSE,//DEPRECATED
    CONFIG_USPACE,
    CONFIG_COMEDI,
    CONFIG_SIM,
    CONFIG_PROGRAM_IL,
    CONFIG_PROGRAM_LD,
     //(runtime updatable) sequences,
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
    VARIABLE_INDEX,
    VARIABLE_ID,
    VARIABLE_VALUE,
    VARIABLE_MIN,
    VARIABLE_MAX,
    N_VARIABLE_PARAMS
} VARIABLE_PARAMS;

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

typedef struct variable {
    unsigned char index;
    char * name ;
    char * value;
    char * min;
    char * max;
      
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
 * @brief initialize configuration
 * @return a newly alloced config
 */
config_t init_config();

/**
 * @brief cleanup and free configuration
 * @param the configuration
 * @return NULL
 */
config_t clear_config(config_t c);

/**
 * @brief entry point: load text file into configuration
 * @param filename (full path)
 * @param the configuration
 * @return new config
 */
config_t load_config_yml(const char * filename, config_t conf);

/**
 * @brief entry point: save configuration to text file 
 * @param filename (full path)
 * @param the configuration
 * @return OK or ERR
 */
int save_config_yml(const char * filename, const config_t conf);

/**
 * @brief get config entry by key
 */
entry_t get_entry(int key, const config_t conf);

/**
 * @brief get config key by viteral alpha value
 */
int get_key(const char * value, const config_t conf);

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
config_t store_seq_value(unsigned char seq,
                    unsigned char idx, 
                    unsigned char key, 
                    const char * value, 
                    config_t c);

/**
 * @brief process an initialized parser, recursively for each mapping
 * @param sequence no. if the mapping is inside of a sequence, 
 * PLC_ERR otherwise
 * @param the parser
 * @param the configuration where the parsed values are stored
 * @return config with applied values or changed errorcode
 */
config_t process(int sequence, 
            yaml_parser_t *parser, 
            config_t conf);

/**
 * @brief emit all configuration values to a yaml emitter
 * @param the emitter
 * @param the configuration where the parsed values are stored
 * @return OK or ERR
 */
 
int emit(yaml_emitter_t *emitter,
         const config_t conf);
 
#endif //_CONFIG_H_




































