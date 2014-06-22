#include <ctype.h>
#include "plcemu.h"
#include "plclib.h"
#include "parser.h"

extern BYTE Command, Response;

/******************parse ladder files!**********************/
/*
 1.read a text file and store it in array Lines.
 2.parse each unresolved line i in Lines[i] up to '+','(',0, blank, '|'.
 a. parse grammar.
 i.read next character
 ii. increase line position counter from Pos[i]
 iii.if unaccepted character, return error. else return uccepted character number.

 b. if blank or 0,'|', empty value for the line. if 0 set it resolved.
 c. if '(',
 i. see if it is a coil. (expect Q,S,R,T,D,M,W followed by number)
 ii. resolve coils value from the correspondant value
 iii. mark line as resolved by setting Pos[i] = -1.
 d. if '-' do nothing and go to next character
 e. if '+' stop and continue with next line.
 f. if ! negate next operand by setting normally closed mode
 g. otherwise operand is expected(i,q,f,r,m,t,c,b)
 if operand
 read number starting at Pos[i]+1
 if there is a number
 get the number
 get its number of digits
 compare number with max no for operand
 get value from operand[number]
 if in normally closed mode negate value
 AND it with current value
 3.reset normally closed mode
 (At this point, all lines have been resolved,or paused at a '+', with one value in their stack.
 Pos[i] is holding the position of each lines cursor.)

 4.  parse vertically
 cursor=0
 while there are unresolved lines(minmin returns non negative)
 resolve lines
 j = get min i for min Pos[i] , Pos[i] > cursor
 cursor = Pos[j]
 curline = j
 start at line j

 parse downwards:
 get character at Lines[++j][cursor]
 a. if '|' do nothing
 b. if '+'
 if new_node
 curline=j;
 new_node=false
 else store Val[i], Val[j] = Val[i] OR Val[j]
 c. otherwise do nothing and set new_node
 */
extern int Di, Dq, Nm, Nt, Ns;

int extract_number(const char *line, const unsigned int start)
{ //read characters from string line
	int i, n;
//starting @ position start
    if (start >= strlen(line))
		return ERROR;
	n = 0;
    for (i = start; isdigit(line[i]); i++)
		n = 10 * n + (line[i] - '0');
    if (i == start)
		//no digits read
		return ERROR;
	return n;
//return number read or error 
}

int read_char(const char * line, const unsigned int c)
{ //read ONE character from line[idx]
//parse grammatically:
	int r = 0;
    if (c > strlen(line))
		return ERROR;
    if (line[c] == 0 || line[c] == ';' || line[c] == '\n'
            || line[c] == '\r')
        return LD_END;
    if (line[c] == ' ' || line[c] == '.' || line[c] == '#'
            || line[c] == '\t')
        return LD_BLANK;
    if (isdigit(line[c]))
        return line[c] - '0';
    switch (line[c])
	{
    case '(': //COIL
        r = LD_COIL;
		break;
	case '-': //horizontal line
        r = LD_AND;
		break;
	case '|': //vertical line
        r = LD_OR;
		break;
	case '!': //normally clozed
        r = LD_NOT;
		break;
	case '+': //
        r = LD_NODE;
		break;
	case 'Q': //dry contact output
        r = LD_CONTACT;
		break;
    case '[': //set output
        r = LD_SET;
		break;
    case ']': //reset output
        r = LD_RESET;
		break;
	case 'T': //start timer
        r = LD_START;
		break;
    case ')'://down timer
        r = LD_DOWN;
		break;
	case 'M': //pulse to counter
        r = LD_PULSEIN;
		break;
	case 'W': //write response
        r = LD_WRITE;
		break;
	case 'i': //input
        r = LD_INPUT;
		break;
	case 'q': //output value
        r = LD_OUTPUT;
		break;
	case 'f': //falling edge
        r = LD_FALLING;
		break;
	case 'r': //rising Edge
        r = LD_RISING;
		break;
	case 'm': //pulse of counter
        r = LD_MEMORY;
		break;
	case 't': //timer.q
        r = LD_TIMEOUT;
		break;
	case 'c': //read command
        r = LD_COMMAND;
		break;
	case 'b': //blinker
        r = LD_BLINKOUT;
		break;
	default:
		r = ERR_BADCHAR; //error
	}
//return value or error
	return r;
}

int resolve_operand(struct PLC_regs * p, const int type, const int idx)
{
	int r = ERR_BADINDEX;
	switch (type)
	{
    case LD_INPUT: //input
		if (idx >= 0 && idx < Di * BYTESIZE)
			r = resolve(p, BOOL_DI, idx);
		break;
    case LD_OUTPUT: //output value
		if (idx >= 0 && idx < Dq * BYTESIZE)
            r = resolve(p, BOOL_DQ, idx);
		break;
    case LD_FALLING: //falling edge
		if (idx >= 0 && idx < Di * BYTESIZE)
			r = fe(p, BOOL_DI, idx);
		break;
    case LD_RISING: //rising Edge
		if (idx >= 0 && idx < Di * BYTESIZE)
			r = re(p, BOOL_DI, idx);
		break;
    case LD_MEMORY: //memory variable
		if (idx >= 0 && idx < Nm)
            r = resolve(p, BOOL_COUNTER, idx);
		break;
    case LD_TIMEOUT: //timer.q
		if (idx >= 0 && idx < Nt)
            r = resolve(p, BOOL_TIMER, idx);
		break;
    case LD_COMMAND: //read command serially: maximum 1 instance of this can be true
		r = (Command == idx) ? 1 : 0;
		break;
    case LD_BLINKOUT: //blinker
		if (idx >= 0 && idx < Ns)
            r = resolve(p, BOOL_BLINKER, idx);
		break;
	default:
		r = ERROR; //this should never happen
	}
	return r;
}

int resolve_coil(struct PLC_regs * p, const int type, const int idx,const int val)
{
    int r = 0;
	switch (type)
	{
    case LD_CONTACT:
		if (idx >= 0 && idx < Dq * BYTESIZE)
            r = contact(p, BOOL_DQ, idx, val);
		else
			r = ERR_BADINDEX;
		break;
    case LD_START:
		if (idx >= 0 && idx < Nt)
		{
			if (val)
                r = set(p, BOOL_TIMER, idx);
		}
		else
			r = ERR_BADINDEX;
		break;
    case LD_DOWN:
		if (idx >= 0 && idx < Nt)
		{
			if (val)
				r = down_timer(p, idx);
		}
		else
			r = ERR_BADINDEX;
		break;
    case LD_PULSEIN:
		if (idx >= 0 && idx < Nm)
            r = contact(p, BOOL_COUNTER, idx, val);
		else
			r = ERR_BADINDEX;
		break;
    case LD_WRITE:
		Response = idx; //unimplemented
		break;
	default:
		r = ERR_BADCOIL; //this should never happen
	}
	return r;
}

int resolve_set(struct PLC_regs * p, const int type, const int idx, const int val)
{
    int r = 0;
	switch (type)
	{
    case LD_CONTACT:
		if (idx >= 0 && idx < Dq * BYTESIZE)
		{
			if (val)
                r = set(p, BOOL_DQ, idx);
		}
		else
			r = ERR_BADINDEX;
		break;
    case LD_START:
		if (idx >= 0 && idx < Nt)
		{
			if (val)
                r = set(p, BOOL_TIMER, idx);
		}
		else
			r = ERR_BADINDEX;
		break;
    case LD_PULSEIN:
		if (idx >= 0 && idx < Nm)
		{
			if (val)
                r = set(p, BOOL_COUNTER, idx);
		}
		else
			r = ERR_BADINDEX;
		break;
		/*	case WRITE:
		 Response=idx;//unimplemented
		 break;*/
	default:
		r = ERR_BADCOIL; //this should never happen
	}
	return r;
}

int resolve_reset(struct PLC_regs * p, const int type, const int idx, const int val)
{
    int r = 0;
	switch (type)
	{
    case LD_CONTACT:
		if (idx >= 0 && idx < Dq * BYTESIZE)
		{
			if (val)
                r = reset(p, BOOL_DQ, idx);
		}
		else
			r = ERR_BADINDEX;
		break;
    case LD_START:
		if (idx >= 0 && idx < Nt)
		{
			if (val)
				r = down_timer(p, idx);
		}
		else
			r = ERR_BADINDEX;
		break;
    case LD_PULSEIN:
		if (idx >= 0 && idx < Nm)
		{
			if (val)
                r = reset(p, BOOL_COUNTER, idx);
		}
		else
			r = ERR_BADINDEX;
		break;
		/*	case WRITE:
		 Response=idx;//unimplemented
		 break;*/
	default:
		r = ERR_BADCOIL; //this should never happen
	}
	return r;
}

int resolve_lines(struct PLC_regs * p)
{
    int i;//, j,
    int c, type, r, n_mode, idx, digits, operand;
	char * l;
    //char tst[MEDSTR * 10];//, tst2[TERMLEN];
    //tst[0] = 0;

	r = 0;
	for (i = 0; i < Lineno; i++)
	{
		n_mode = 0; //normally open mode
		l = Lines[i];
        c = LD_AND; //default character = '-'
		//sprintf(tst,"LINE %d of %d:",i,Lineno);
        while (Pos[i] > RESOLVED && c != LD_NODE)
		{	//loop
			c = read_char(l, Pos[i]);
			switch (c)
			{
			case ERR_BADCHAR:
				r = c;
				Pos[i] = RESOLVED;
				break;
            case LD_END:	//if 0 , this sould be a coil
				Pos[i] = RESOLVED;
				break;
            case LD_BLANK:
            case LD_OR:	//if blank or '|', empty value for the line.
				Val[i] = FALSE | FINAL;
				Pos[i]++;
				break;
            case LD_NOT:
				n_mode = 1;	//normally closed mode
            case LD_AND:
				Pos[i]++;
				break;
            case LD_NODE:
				if (Val[i] >= FINAL)
					Val[i] -= FINAL;
				break;
            case LD_COIL://see if it is a coil.(NEW: ()[] (expect Q,T,M,W followed by number)
            case LD_SET:
            case LD_RESET:
            case LD_DOWN:
				Pos[i]++;
				type = c;
				c = read_char(l, Pos[i]);
                if (c >= LD_CONTACT && c < LD_END)
				{
					operand = c;
					c = read_char(l, Pos[i]);
					Pos[i]++;
					idx = extract_number(l, Pos[i]);
					if (idx >= 0)
					{
//				sprintf(tst2,"LINE %d VALUE %d",i,Val[i]);
//				strcat(tst,tst2);
//				draw_info_line(4+Pagelen/2,tst);
						if (Val[i] >= FINAL)
						{
							switch (type)
							{
                            case LD_COIL:
								r = resolve_coil(p, operand, idx,
										Val[i] & TRUE);
								break;
                            case LD_DOWN:
								r = resolve_coil(p, operand, idx,
										Val[i] & TRUE ? FALSE : TRUE);
								break;
                            case LD_SET:
								r = resolve_set(p, operand, idx, Val[i] & TRUE);
								break;
                            case LD_RESET:
								r = resolve_reset(p, operand, idx,
										Val[i] & TRUE);
								break;
							}
							Pos[i] = -FINAL;
						}
						Pos[i] = RESOLVED;
					}
					else
						r = ERR_BADINDEX;
				}
				else
					r = ERR_BADCOIL;
				break;
			default:	//otherwise operand is expected(i,q,f,r,m,t,c,b)
                if (c >= LD_INPUT && c < LD_CONTACT)
				{	//valid input symbol
					operand = c;
					c = read_char(l, Pos[i]);
					Pos[i]++;
					idx = extract_number(l, Pos[i]);
					if (idx >= 0)
					{
						if (idx > 100)
							digits = 2;
						else if (idx > 10)
							digits = 1;
						else
							digits = 0;
						Pos[i] += digits;
						r = resolve_operand(p, operand, idx);
						if (n_mode)
						{
							r = r ? FALSE : TRUE;
							n_mode = FALSE;
						}
						Val[i] &= FINAL | r;
					}
					else
						r = ERR_BADINDEX;
				}
				else
					r = ERR_BADOPERAND;
				Pos[i]++;
			}		//end switch
			if (r < 0)
			{		//error
				Pos[i] = RESOLVED;
				Lines[i][0] = '.';
				Val[i] = FALSE;
				return r;
			}
		}		//end while
		if (Val[i] &= TRUE)
			Lines[i][0] = '#';
		else
			Lines[i][0] = ' ';
	}		//end for
	/*(At this point, all lines have been resolved,or paused at a '+'*/
    return r;
}

int minmin(const int * arr, const int min, const int max)
{//for an array arr of integers ,return the smallest of indices i so that arr[i] =  min(arr) >= min 
	int i;
	int v = MAXSTR;		//cant be more than length  of line
	int r = RESOLVED;
	for (i = max - 1; i >= 0; i--)
	{
		if (arr[i] <= v && arr[i] >= min)
		{
			v = arr[i];
			r = i;
		}
	}
	return r;
}

int LD_task(struct PLC_regs * p)
{		//main loop
    int r, i, j, k, tempval;
	int cursor = 0;		//care only for unresolved lines
    int curline = 0;
    //char tst[MEDSTR * 10];
    //char tst2[SMALLBUF];
    //int tester = 0;
	for (j = 0; j < Lineno; j++)
	{
		Pos[j] = 1;
		Val[j] = TRUE | FINAL;
	}
	r = resolve_lines(p);
	if (r < 0)
		return r;		//error
	curline = minmin(Pos, cursor, Lineno);//find first unfinalized line that has stopped at smallest cursor larger than current cursor
	if (curline < 0)
        return 0;		//finished
	cursor = Pos[curline];
	/* parse vertically
	 start at current line
	 */
	for (j = 0; j < Lineno; j++)
	{
		if (Pos[j] > RESOLVED)
		{
			if (Val[j] < FINAL)
			{
				curline = j;
				cursor = Pos[j];
				for (i = curline; i < Lineno; i++)
				{
					if ((Lines[i][cursor] != '|' && Lines[i][cursor] != '+')
							|| i >= Lineno - 1)		//vertical line interrupted
					{
						//	    new_node = TRUE;
						tempval = Val[curline] & TRUE;
						while (Stack != NULL)
						{
//				sprintf(tst2,"OR (%s)  ",Stack->operand?"TRUE":"FALSE");
//				strcat(tst,tst2);
							tempval = pop(tempval);
						}
						for (k = curline;
								(k <= i)
										&& (Lines[k][cursor] == '|'
												|| Lines[k][cursor] == '+');
								k++)
						{
							if (Lines[k][cursor] == '+')
							{
								Val[k] = tempval | FINAL;
								Pos[k]++;
							}
						}
						r = resolve_lines(p);
						if (r < 0)
							return r;		//error
						break;
					}
					else if (Lines[i][cursor] == '+')
					{
						push(BOOL + IL_OR, Val[i] & TRUE);
					}
				}
			}
		}
	}		//end for
//	tester++;
    return 0;
}


