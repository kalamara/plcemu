#include "plcemu.h"
#include "plclib.h"

/***********INTERNAL FUNCTIONS***************************************/
void dec_inp(struct PLC_regs * p)
{ //decode input bytes
	int i;
	for (i = 0; i < Di * BYTESIZE; i++)
	{
		p->di[i].I = (p->inputs[i / BYTESIZE] >> i % BYTESIZE) % 2;
		p->di[i].RE = (p->di[i].I)
				&& ((p->edgein[i / BYTESIZE] >> i % BYTESIZE) % 2);
		p->di[i].FE = (!p->di[i].I)
				&& ((p->edgein[i / BYTESIZE] >> i % BYTESIZE) % 2);
	}
}

void enc_out(struct PLC_regs * p)
{ //encode digital outputs to output bytes
	int i;
	BYTE out[Dq];
	memset(out, 0, Dq);
	for (i = 0; i < Dq * BYTESIZE; i++)
	{
        out[i / BYTESIZE] |= (p->dq[i].Q || (p->dq[i].SET && !p->dq[i].RESET))
				<< i % BYTESIZE;
	}
	memcpy(p->outputs, out, Dq);
}

void read_mvars(struct PLC_regs *p)
{
	int i;
	for (i = 0; i < Nm; i++)
	{
		if (p->m[i].SET || p->m[i].RESET)
			p->m[i].PULSE = p->m[i].SET && !p->m[i].RESET;
	}
}

void write_mvars(struct PLC_regs * p)
{
	int i;
	for (i = 0; i < Nm; i++)
	{
		if (!p->m[i].RO)
		{
			if (p->m[i].PULSE && p->m[i].EDGE) //up/down counting
			{
				p->m[i].V += (p->m[i].DOWN) ? -1 : 1;
				p->m[i].EDGE = FALSE;
			}
		}

	}
}
/**************USER FUNCTIONS*****************************************/

int re(const struct PLC_regs * p, const int type, const int idx)
{ //return rising edge of operand

	switch (type)
	{
    case BOOL_DI:
		return (p->di[idx].RE);
		break;
    case BOOL_COUNTER:
		return (p->m[idx].PULSE) && (p->m[idx].EDGE);
		break;
	default:
        return ERROR;
	}
}

int fe(const struct PLC_regs * p, const int type, const int idx)
{ //return falling edge of operand

	switch (type)
	{
    case BOOL_DI:
		return (p->di[idx].FE);
		break;
    case BOOL_COUNTER:
		return (!p->m[idx].PULSE) && (p->m[idx].EDGE);
		break;
	default:
        return ERROR;
	}
}

int set(struct PLC_regs * p, const int type, const int idx)
{ //set operand
	switch (type)
	{
    case BOOL_DQ:
		p->dq[idx].SET = TRUE;
		p->dq[idx].RESET = FALSE;
		break;
    case BOOL_COUNTER:
		p->m[idx].SET = TRUE;
		p->m[idx].RESET = FALSE;
		if (!p->m[idx].PULSE)
			p->m[idx].EDGE = TRUE;
		break;
    case BOOL_TIMER:
		p->t[idx].START = TRUE;
		break;
	default:
        return ERROR;
	}
    return 0;
}

int reset(struct PLC_regs * p, const int type,const int idx)
{ //reset operand
	switch (type)
	{
    case BOOL_DQ:
		p->dq[idx].RESET = TRUE;
		p->dq[idx].SET = FALSE;
		break;
    case BOOL_COUNTER:
		p->m[idx].RESET = TRUE;
		p->m[idx].SET = FALSE;
		if (p->m[idx].PULSE)
			p->m[idx].EDGE = TRUE;
		break;
    case BOOL_TIMER:
		p->t[idx].START = FALSE;
		break;
	default:
        return ERROR;
	}
    return 0;
}

int contact(struct PLC_regs * p, const int type, const int idx, const unsigned char val)
{ //contacts an output with a value
	switch (type)
	{
    case BOOL_DQ:
		p->dq[idx].Q = val;
		break;
    case BOOL_COUNTER:
		if (p->m[idx].PULSE != val)
			p->m[idx].EDGE = TRUE;
		else
			p->m[idx].EDGE = FALSE;
		p->m[idx].PULSE = val;
		break;
    case BOOL_TIMER:
		p->t[idx].START = TRUE;
		break;
	default:
        return ERROR;
	}
    return 0;
}

int resolve(struct PLC_regs * p, const int type, const int idx)
{ //return an operand value
	switch (type)
	{
    case BOOL_DQ:
        return p->dq[idx].Q || (p->dq[idx].SET && !p->dq[idx].RESET);
    case BOOL_COUNTER:
		return p->m[idx].PULSE;
    case BOOL_DI:
		return p->di[idx].I;
    case BOOL_BLINKER:
		return p->s[idx].Q;
    case BOOL_TIMER:
		return p->t[idx].Q;
	default:
        return ERROR;
	}
}

int down_timer(struct PLC_regs * p, const int idx)
{ //RESET timer
	p->t[idx].START = FALSE;
	p->t[idx].V = 0;
	p->t[idx].Q = 0;
    return 0;
}
