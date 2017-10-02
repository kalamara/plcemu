#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "plcemu.h"
#include "config.h"

void plc_log(const char * msg, ...)
{
    va_list arg;
    char msgstr[256];
    memset(msgstr,0, 256);
    va_start(arg, msg);
    vsprintf(msgstr,msg,arg);
    va_end(arg);
    printf("%s\n",msgstr);
}

config_t init_config(){
 //TODO: in a c++ implementation this all can be done automatically 
 //using a hashmap
    config_t conf = new_config(N_CONFIG_VARIABLES);
   
    config_t uspace = new_config(N_USPACE_VARS);
        
    uspace = update_entry(
        USPACE_BASE,
	    new_entry_int(50176, "BASE"),
	    uspace);
	
	uspace = update_entry(
	    USPACE_WR, 
	    new_entry_int(0, "WR"),
	    uspace);
	    
	uspace = update_entry(
	    USPACE_RD, 
	    new_entry_int(8, "RD"),
	    uspace);
	
	config_t subdev = new_config(N_SUBDEV_VARS);
	
    subdev = update_entry(
        SUBDEV_IN,
	    new_entry_int(0, "IN"),
	    subdev);
	    
	subdev = update_entry(
	    SUBDEV_OUT,
	    new_entry_int(1, "OUT"),
	    subdev);
	    
    subdev = update_entry(
        SUBDEV_ADC, 
	    new_entry_int(2, "ADC"),
	    subdev);
	    
	subdev = update_entry(
	    SUBDEV_DAC, 
	    new_entry_int(3, "DAC"),
	    subdev);
	
	config_t comedi = new_config(N_COMEDI_VARS);
	
	comedi = update_entry(
	    COMEDI_FILE,
	    new_entry_int(0, "FILE"),
	    comedi);
	    
	comedi = update_entry(
	    COMEDI_SUBDEV, 
	    new_entry_map(subdev, "SUBDEV"),
	    comedi);
    
    config_t sim = new_config(N_SIM_VARS);
    
    sim = update_entry(
        SIM_INPUT,
        new_entry_str("sim.in", "INPUT"), 
        sim);
        
    sim = update_entry(
        SIM_OUTPUT,
        new_entry_str("sim.out", "OUTPUT"),
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
