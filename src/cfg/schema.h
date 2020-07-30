#ifndef _SCHEMA_H_
#define _SCHEMA_H_
/**
 *@file schema.h
 *@brief configuration schema definitions header
*/

typedef enum{
    MAP_ROOT,
    MAP_HW,
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

typedef enum {
    HW_LABEL,
    HW_IFACE,
    N_HW_VARS
}HW_VARS;

typedef enum{
    CONFIG_STATUS,
    CONFIG_STEP,
    CONFIG_HW,
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



#endif //_SCHEMA_H_
