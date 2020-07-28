#include "config.h"
#include "hardware.h"

extern struct hardware Comedi;
extern struct hardware Uspace;
extern struct hardware Sim;
extern struct hardware Dry;

hardware_t get_hardware( int type){
    switch(type){
        case HW_COMEDI:
#ifdef COMEDI        
            return &Comedi;
#else
            return NULL;
#endif             
        case HW_USPACE:
            return &Uspace;

        case HW_SIM: 
        	return &Sim;
        default: 
            return &Dry;
    }
}


