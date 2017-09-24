#include "data.h"

/*************************data_t**************************************/
data_t negate(unsigned char *op, data_t b) {
      data_t r = b;
      if (*op & NEGATE){	    //negate b
		*op -= NEGATE;
		r.u = -1 - (b.u);
		//magic?
	}
	return r;
}

uint64_t operate_u(unsigned char op, uint64_t a, uint64_t b) {
    uint64_t r=0;
	
    switch (op){
	//boolean or bitwise, all modifiers,
	case IL_AND:	//AND
		r = a & b;
		break;

	case IL_OR:	//OR
		r = a | b;
		break;

	case IL_XOR:	//XOR
		r = a ^ b;
		break;
	//arithmetic	
	case IL_ADD:
			r = a + b;
		break;
		
	case IL_SUB:
			r = a - b;
	    break;
	    
	case IL_MUL:
		r = (a * b) ;
		break;
		
	case IL_DIV:
		r = b != 0 ? a / b : - 1;
		break;
		
	//comparison
	case IL_GT:
		r = (a > b);
		break;
		
	case IL_GE:
		r = (a >= b);
		break;
		
	case IL_EQ:
		r = (a == b);
		break;
		
	case IL_NE:
		r = (a != b);
		break;
		
	case IL_LT:
		r = (a < b);
		break;
		
	case IL_LE:
		r = (a <= b);
		break;
		
	default:
		break;
	}
    return r;
}

double operate_d(unsigned char op, double a, double b) {
    double r=0;
    switch (op) {
    //arithmetic	
	case IL_ADD:
			r = a + b;
		break;
		
	case IL_SUB:
			r = a - b;
	    break;
	    
	case IL_MUL:
		r = (a * b) ;
		break;
		
	case IL_DIV:
		r = b != 0 ? a / b : - 1;
		break;
		
	//comparison
	case IL_GT:
		r = (a > b);
		break;
		
	case IL_GE:
		r = (a >= b);
		break;
		
	case IL_EQ:
		r = (a == b);
		break;
		
	case IL_NE:
		r = (a != b);
		break;
		
	case IL_LT:
		r = (a < b);
		break;
		
	case IL_LE:
		r = (a <= b);
		break;
		
	default:
		break;
	}
    return r;
}

data_t operate( unsigned char op, 
                unsigned char type, 
                const data_t a, 
                const data_t b ) {
    data_t r;	    //return value
    data_t n = negate(&op, b);
    uint64_t modulo = 1;
	switch(type)
	{
	    case T_REAL:
	        r.r = operate_d(op, a.r, n.r ) ;
	        break;
	    
	    case T_BOOL: 
	        r.u = BOOL(operate_u(op, BOOL(a.u), BOOL(n.u)));
	        break;
	    
	    case T_BYTE:
	        modulo = 1 << BYTESIZE;
	        //r.u = operate_u(op, a.u % 0x100, bu % 0x100) % 0x100;
	        r.u = operate_u(op, a.u % modulo, n.u % modulo) % modulo;
	        break;
	    
	    case T_WORD:
	        modulo = 1 << WORDSIZE;
	        r.u = operate_u(op, a.u % modulo, n.u % modulo) % modulo;
	        break;
	    
	    case T_DWORD:
	        modulo = 0x100000000;
	        r.u = operate_u(op, a.u % modulo, n.u % modulo) % modulo;
	        break;
	   
	    default://64bit uint
	        r.u = operate_u(op, a.u , n.u ) ;
	    break;
	}
	return r;
}

