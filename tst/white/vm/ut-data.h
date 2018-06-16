#ifndef _UT_DATA_H_
#define _UT_DATA_H_

/*boolean ops*/
void ut_operate_b()
{
    //invalid operation
    BYTE op = -1;
    data_t a;
    a.u = 1;
    data_t b;
    b.u = 2;
    //boolean, no modifier
    op = IL_SET;         ///S
    data_t r = operate(op, T_BOOL, a, b);
    CU_ASSERT(r.u==0);
    
    op = IL_RESET;       ///R
    r = operate(op, T_BOOL, a, b);
    CU_ASSERT(r.u==0);
    
    //boolean, all modifiers
    a.u = 13;
    b.u = 7;
    
    op = IL_AND;         ///AND
         
    r = operate(op, T_BOOL, a, b);
    CU_ASSERT(r.u==TRUE); //logical and(13,7)
    
    op = IL_AND + NEGATE;         
    
    r = operate(op, T_BOOL, a, b);
    CU_ASSERT(r.u == TRUE); //logical and(13,248)
   
    op = IL_OR;         ///OR
    r = operate(op, T_BOOL, a, b);
    CU_ASSERT(r.u == TRUE); //logical or(13,7)
    
    op = IL_OR + NEGATE;                     
    r = operate(op, T_BOOL, a, b);
    CU_ASSERT(r.u == TRUE); //logical or(13,248)
    
    op = IL_XOR;         ///XOR          
    r = operate(op, T_BOOL, a, b);
    CU_ASSERT(r.u == FALSE); //logical xor(13,7)
    
    op = IL_XOR + NEGATE;      
    r = operate(op, T_BOOL, a, b);
    CU_ASSERT(r.u == FALSE); //logical xor(13,248)
    
    //any operand, only push
    a.u = 2;
    b.u = 0;
    op = IL_ADD;          ///ADD 
    r = operate(op, T_BOOL, a, b);
    CU_ASSERT(r.u == TRUE); // TRUE + FALSE = true 
    
    a.u = 5;
    b.u = 2;
    op = IL_ADD + NEGATE;          ///ADD
   
    r = operate(op, T_BOOL, a, b);
    //printf("%d\n", r);
    CU_ASSERT(r.u==TRUE); // TRUE + TRUE = TRUE 
    
    op = IL_SUB;          ///SUBTRACT
    
    a.u = 0;
    b.u = 2;
    r = operate(op, T_BOOL, a, b);
    CU_ASSERT(r.u == TRUE); // FALSE - ( - TRUE) = TRUE 
    
    a.u = 5;
    b.u = 2;
    op = IL_SUB + NEGATE;          ///SUBTRACT

    r = operate(op, T_BOOL, a, b);
    CU_ASSERT(r.u == FALSE); // TRUE - ( - TRUE) = overflow 
    
    op = IL_MUL;          ///MULTIPLY
    
    a.u = 5;
    b.u = 5;
    r = operate(op, T_BOOL, a, b);        //TRUE * TRUE =  TRUE
    CU_ASSERT(r.u == TRUE);
    
    a.u = 1;
    b.u = 0;
    op = IL_DIV;          ///DIVIDE
    
    r = operate(op, T_BOOL, a, b);
    CU_ASSERT(r.u == TRUE);  //TRUE DIV FALSE = TRUE 
    
    a.u = 5;
    b.u = 2;
    r = operate(op, T_BOOL, a, b);
    CU_ASSERT(r.u == TRUE);  // TRUE / TRUE = TRUE
    
    op = IL_GT;          ///GREATER THAN
    r = operate(op, T_BYTE,  a, b);
    CU_ASSERT(r.u == TRUE);  // 
       
    op = IL_LE;
    r = operate(op, T_BOOL, a, b);
    CU_ASSERT(r.u == TRUE);     //TRUE == TRUE  
}

/*floating point ops*/
void ut_operate_r()
{
    //invalid operation
    BYTE op = -1;
    data_t a;
    data_t b;

   
    //any operand, only push
    a.r = 2.0l;
    b.r = 1.0l;
    op = IL_ADD;          ///ADD
    data_t r = operate(op, T_REAL, a, b);

   CU_ASSERT_DOUBLE_EQUAL(r.r, 3.0l, FLOAT_PRECISION);
        
    a.r = 5.999l;
    b.r = 2.000l;
    op = IL_ADD + NEGATE;          ///ADD
    r = operate(op, T_REAL, a, b); // 

    CU_ASSERT_DOUBLE_EQUAL(r.r, 3.999l, FLOAT_PRECISION);
    
      
    a.r = 1234.5678l;
    b.r = 0.0001;
    op = IL_SUB;          ///SUBTRACT
    r = operate(op, T_REAL, a, b);
    CU_ASSERT_DOUBLE_EQUAL(r.r, 1234.5677l, FLOAT_PRECISION);

    a.r = 0.0l;
    b.r = 0.0l;
    op = IL_MUL;          ///MULTIPLY
    r = operate(op, T_REAL, a, b);
    CU_ASSERT_DOUBLE_EQUAL(r.r, 0.0l, FLOAT_PRECISION);
    
    a.r = 5.5l;
    b.r = 5.5l;
    
    op = IL_MUL;
    r = operate(op, T_REAL, a, b);
    CU_ASSERT_DOUBLE_EQUAL(r.r, 30.25l, FLOAT_PRECISION);
   
    a.r = 1.0l;
    b.r = 0.0l;
    op = IL_DIV;          ///DIVIDE
    r = operate(op, T_REAL, a, b);
    CU_ASSERT_DOUBLE_EQUAL(r.r, -1.0l, FLOAT_PRECISION);
    //printf("%f\n", r.r);   //??
    
    a.r = 111111.111111l;
    b.r = -111111.111111l;
    op = IL_DIV;          ///DIVIDE
    r = operate(op, T_REAL, a, b);
    CU_ASSERT_DOUBLE_EQUAL(r.r, -1.0l, FLOAT_PRECISION);
    
    a.r = 999999.999999;
    b.r = 333333.333333;
    op = IL_DIV;          ///DIVIDE
    r = operate(op, T_REAL, a, b);
    CU_ASSERT_DOUBLE_EQUAL(r.r, 3.0l, FLOAT_PRECISION);
    
        
    a.r = 555555555555.5l;
    b.r = 0.0000000002;
    
    op = IL_GT;          ///GREATER THAN
    r = operate(op, T_REAL,  a, b);
    CU_ASSERT_DOUBLE_EQUAL(r.r, 1.0l, FLOAT_PRECISION);
    
    op = IL_GE;          ///GREATER OR EQUAL
    r = operate(op, T_REAL, a, b);
    CU_ASSERT_DOUBLE_EQUAL(r.r, 1.0l, FLOAT_PRECISION);
    
    op = IL_EQ;          ///EQUAL
    r = operate(op, T_REAL, a, b);
    CU_ASSERT_DOUBLE_EQUAL(r.r, 0.0l, FLOAT_PRECISION);  

    op = IL_NE;          ///NOT EQUAL
    r = operate(op, T_REAL, a, b);
    CU_ASSERT_DOUBLE_EQUAL(r.r, 1.0l, FLOAT_PRECISION);  

    op = IL_LT;          ///LESS THAN
    r = operate(op, T_REAL, a, b); 
    CU_ASSERT_DOUBLE_EQUAL(r.r, 0.0l, FLOAT_PRECISION);
}

/*scalar ops*/
void ut_operate()
{
    //invalid operation
    BYTE op = -1;
    data_t a;
    a.u = 1;
    data_t b;
    b.u = 2;
    data_t r = operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u==0);
    
    op = N_IL_INSN;
    r = operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u==0);
    
    //no operand
    op = IL_POP;
    r = operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u==0);
    
    op = IL_RET;         
    r = operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u==0);
    
    //arithmetic LABEL
    op = IL_JMP;         
    r = operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u==0);
    
    //subroutine call (unimplemented)
    op = IL_CAL;         ///CAL
    r = operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u==0);
    
    
    //bitwise, all modifiers
    a.u = 0x1122334455667788;
    b.u = 0xabcdef1234567890;
    
    op = IL_AND;         ///AND
    r = operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u==0x80); //bitwise and(x88,x90)
         
    op = IL_AND + NEGATE;         
    r = operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u == 0x8); //bitwise and(x88, xff - x90)
    //printf("%d\n", r.u);
    
    op = IL_AND;         ///AND
    r = operate(op, T_WORD, a, b);
    CU_ASSERT(r.u==0x7080); //bitwise and(x7788,x7890)
         
    op = IL_AND + NEGATE;         
    r = operate(op, T_WORD, a, b);
    CU_ASSERT(r.u == 0x708); //bitwise and(x7788,xffff-x7890)
    
    op = IL_AND;         ///AND
    r = operate(op, T_DWORD, a, b);
    CU_ASSERT(r.u==0x14467080); //bitwise and(x55667788,x34567890)
         
    op = IL_AND + NEGATE;         
    r = operate(op, T_DWORD, a, b);
    CU_ASSERT(r.u == 0x41200708); //bitwise and(x55667788,
                                  //xffffffff - x34567890)
                                  
     op = IL_AND;         ///AND
    r = operate(op, T_LWORD, a, b);
    CU_ASSERT(r.u==0x100230014467080); //bitwise and
    //(x1122334455667788,xabcdef1234567890)
         
    op = IL_AND + NEGATE;         
    r = operate(op, T_LWORD, a, b);
    CU_ASSERT(r.u == 0x1022104441200708); 
    
    //bitwise and(
    //x1122334455667788,
    //xffffffffffffffff - xabcdef1234567890)    
    
    op = IL_OR;         ///OR
    r = operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u == 0x98); //bitwise OR(x88,x90)
   
    
    op = IL_OR + NEGATE;         
    r = operate(op, T_WORD, a, b);
    CU_ASSERT(r.u == 0xf7ef); //bitwise or(x7788,xffff-x7890)
            
    
    op = IL_XOR;         ///XOR
    r = operate(op, T_DWORD, a, b);
    CU_ASSERT(r.u == 0x61300F18); //bitwise XOR(x55667788,x34567890)
          
    op = IL_XOR + NEGATE;         
    r = operate(op, T_LWORD, a, b);
    CU_ASSERT(r.u == 0x451023a99ECFF0E7); 
//bitwise xor(
//x1122334455667788,
//xffffffffffffffff - xabcdef1234567890)    
            
    //any operand, only negation
    op = IL_LD;          ///LD
    r = operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u == 0);
    
    op = IL_ST;         ///ST
    r = operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u == 0);
    
    //any operand, only push
    a.u = 2;
    b.u = 0;
    op = IL_ADD;          ///ADD
    r = operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u == 2);
        
    a.u = 5;
    b.u = 2;
    op = IL_ADD + NEGATE;          ///ADD
    r = operate(op, T_BYTE, a, b); // 255 - 2 + 5 = 258 = 256 + 2
    //printf("%d\n", r);
    CU_ASSERT(r.u==2);
       
    a.u = 0x12345678;
    b.u = 0x1;
    op = IL_SUB;          ///SUBTRACT
    r = operate(op, T_DWORD, a, b);
    CU_ASSERT(r.u == 0x12345677);
    //printf("%d\n", r.u);
        
    a.u = 5;
    b.u = 2;
    op = IL_SUB + NEGATE;          ///SUBTRACT
    r = operate(op, T_BYTE, a, b); //256 - (255 -2) + 5
    CU_ASSERT(r.u == 8);
    
    a.u = 0xffffffffffffffff;
    b.u = 0;
    op = IL_MUL;          ///MULTIPLY
    r = operate(op, T_LWORD, a, b);
    CU_ASSERT(r.u == 0);
    
    a.u = 0xffffffff;
    b.u = 2;
    op = IL_MUL;          ///overflow
    r = operate(op, T_WORD, a, b);
    CU_ASSERT(r.u == 0xfffe);
    
    //printf("%x\n", r.u);
    
    a.u = 5;
    b.u = 5;
    
    op = IL_MUL + NEGATE;   
    r = operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u == 226);
    
    a.u = 1;
    b.u = 0;
    op = IL_DIV;          ///DIVIDE
    r = operate(op, T_BYTE, a, b);
    CU_ASSERT(r.u == 255);  //NaN is max int
    
    a.u = 100000;
    b.u = 100000;
    op = IL_DIV;          ///DIVIDE
    r = operate(op, T_WORD, a, b);
    CU_ASSERT(r.u == 1);  //NaN is max int
    
    a.u = 1000000000;
    b.u = 100000000;
    op = IL_DIV;          ///DIVIDE
    r = operate(op, T_DWORD, a, b);
    CU_ASSERT(r.u == 10);  //NaN is max int
    
        
    a.u = 0x555555555555555;
    b.u = 0x2;
    
    op = IL_GT;          ///GREATER THAN
    r = operate(op, T_BYTE,  a, b);
    CU_ASSERT(r.u == TRUE);  // 
    
    op = IL_GE;          ///GREATER OR EQUAL
    r = operate(op, T_WORD, a, b);
    CU_ASSERT(r.u == TRUE);  
    
    op = IL_EQ;          ///EQUAL
    r = operate(op, T_DWORD, a, b);
    CU_ASSERT(r.u == FALSE);  

    op = IL_NE;          ///NOT EQUAL
    r = operate(op, T_LWORD, a, b);
    CU_ASSERT(r.u == TRUE);  

    op = IL_LT + NEGATE;          ///LESS THAN
    r = operate(op, T_BYTE, a, b); //5 < 253
    CU_ASSERT(r.u == TRUE);  
   
}

#endif //_UT_DATA_H_
