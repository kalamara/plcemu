#include "plcemu.h"
#include "plclib.h"
#include "project.h"
int PLC_task(struct PLC_regs * p)
{ //
/**************start editing here***************************/
	BYTE one, two, three;
	one = resolve(p, DI, 1);
	two = fe(p, DI, 2);
	three = re(p, DI, 3);
	/*  contact(p,DQ,1,one);
	 contact(p,DQ,2,two);
	 contact(p,DQ,3,three);      */
	if (one)
		set(p, TIMER, 0);
	if (three)
		reset(p, TIMER, 0);
	if (two)
		down_timer(p, 0);
    return 0;
    /***************end of editable portion***********************/

}
int PLC_init()
{
	/*********************same here******************************/
    return 0;
}
