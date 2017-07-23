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
    SEQ_AI,
    SEQ_AQ,
    SEQ_DI,
    SEQ_DQ,
    SEQ_MVAR,
    SEQ_MREG,
    SEQ_TIMER,
    SEQ_PULSE,
    N_SEQUENCES
}CONFIG_SEQUENCES; 

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
    CONFIG_USPACE_BASE,
    CONFIG_USPACE_WR,
    CONFIG_USPACE_RD,
    CONFIG_COMEDI,
    CONFIG_COMEDI_FILE,
    CONFIG_COMEDI_SUBDEV,
    CONFIG_SUBDEV_IN,
    CONFIG_SUBDEV_OUT,
    CONFIG_SUBDEV_ADC,
    CONFIG_SUBDEV_DAC,
    CONFIG_SIM,
    CONFIG_SIM_INPUT,
    CONFIG_SIM_OUTPUT,
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

#define IS_SEQUENCE(x) x >= CONFIG_AI
#define SEQUENCE(x) x - CONFIG_AI

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

typedef struct entry {
	int type_tag;
	char * name;
	struct entry * next;
	union {
	    int scalar_int;
		char * scalar_str;
		struct entry ** map;
	} e;
} * entry_t;

typedef entry_t * entry_map_t;

typedef struct variable {
    BYTE index;
    char  name[MAXSTR];
    char  value[MAXSTR];
    char  min[MAXSTR];
    char  max[MAXSTR];
      
} * variable_t;

typedef struct config {
    //FILE * ErrLog;
    
    //vm configuration    
    BYTE nt;
    BYTE ns;
    BYTE nm;
    BYTE nr;
    BYTE di;
    BYTE dq;
    BYTE ai;
    BYTE aq; 

    //ui
    int sigenable;
    BYTE page_width;
    BYTE page_len;
    
    //hw
    char hw[MAXSTR];
    //userland device
    unsigned int base;
    BYTE wr_offs;
    BYTE rd_offs;
    //comedi
    unsigned int comedi_file;    
    unsigned int comedi_subdev_i;
    unsigned int comedi_subdev_q;
    unsigned int comedi_subdev_ai;
    unsigned int comedi_subdev_aq;
    //simulation
    char sim_in_file[MAXSTR];
    char sim_out_file[MAXSTR];
    
    //sampling
    unsigned long step;
    char pipe[MAXSTR];
    //obsolete
    char response_file[MAXSTR];
    
    //initialization
    variable_t  mvars;
    variable_t  mregs;
    variable_t  dinps;
    variable_t  douts;
    variable_t  timers;
    variable_t  pulses;
    variable_t  ainps;
    variable_t  aouts;
    
    char program_file[MAXSTR];
} * config_t;

/**
 * @brief initialize configuration
 * @return a newly alloced config
 */
config_t init_config();

/**
 * @brief cleanup and free configuration
 * @param the configuration
 */
void clear_config(config_t *c);

/**
 * @brief entry point: load text file into configuration
 * @param filename (full path)
 * @param the configuration
 * @return OK or ERR
 */
int load_config_yml(const char * filename, config_t conf);

/**
 * @brief entry point: save configuration to text file 
 * @param filename (full path)
 * @param the configuration
 * @return OK or ERR
 */
int save_config_yml(const char * filename, const config_t conf);

/**
 * @brief configure a plc with a configuration
 * @param the config
 * @param the plc
 */
void configure(const config_t conf, plc_t plc);

/**
 * @brief store a value to a map
 * @param the key
 * @param the value
 * @param where to store
 * @return OK or ERR 
 */
int store_value(BYTE key, const char * value, config_t * c);

/**
 * @brief store a value to a map that is nested in a sequence
 * @param the sequence 
 * @param the sequence index
 * @param the key
 * @param the value
 * @param where to store
 * @return OK or ERR 
 */
int store_seq_value(BYTE seq,
                    BYTE idx, 
                    BYTE key, 
                    const char * value, 
                    config_t * c);

/**
 * @brief process an initialized parser, recursively for each mapping
 * @param sequence no. if the mapping is inside of a sequence, 
 * PLC_ERR otherwise
 * @param the parser
 * @param the configuration where the parsed values are stored
 * @return OK or ERR
 */
int process(int sequence, 
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




































