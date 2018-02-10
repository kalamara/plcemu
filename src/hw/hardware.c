#include "config.h"
#include "hardware.h"

extern struct hardware Comedi;
extern struct hardware Uspace;
extern struct hardware Sim;

hardware_t get_hardware( int type){
    switch(type){
        case HW_COMEDI:
            return &Comedi;
 
        case HW_USPACE:
            return &Uspace;

        default: return &Sim;
    }
}


