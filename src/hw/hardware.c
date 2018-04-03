#include "config.h"
#include "hardware.h"

extern struct hardware Comedi;
extern struct hardware Uspace;
extern struct hardware Sim;

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

        default: return &Sim;
    }
}


