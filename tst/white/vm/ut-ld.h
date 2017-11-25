#ifndef _UT_LD_H_
#define _UT_LD_H_

void ut_minmin()
{
    int arr[5] = {2,4,1,8,10};
    int result = minmin(arr, 3, 5);
    CU_ASSERT(result == 1); //arr[1] is the first item larger than 3
} 

void ut_parse_ld_line()
{
    int result = parse_ld_line(NULL);
    CU_ASSERT(result == PLC_ERR);
    
    struct ld_line line;
    memset(&line, 0, sizeof(line));
    
    result = parse_ld_line(&line);
    CU_ASSERT(result == PLC_ERR);
    CU_ASSERT(line.status == STATUS_ERROR);
    
    memset(&line, 0, sizeof(line));
    line.buf = " -----";
    result = parse_ld_line(&line);
    CU_ASSERT(result == PLC_OK);
    CU_ASSERT(line.cursor == strlen(line.buf));
    CU_ASSERT(line.status == STATUS_RESOLVED);
    
    memset(&line, 0, sizeof(line));
    line.buf = " ---+  ";
    result = parse_ld_line(&line);
    CU_ASSERT(result == PLC_OK);
    CU_ASSERT(line.cursor == strlen(line.buf)-3);
    CU_ASSERT(line.status == STATUS_UNRESOLVED);
    
    memset(&line, 0, sizeof(line));
    line.buf = " ---Y--";
    result = parse_ld_line(&line);
    CU_ASSERT(result == ERR_BADOPERAND);
    CU_ASSERT(line.cursor == strlen(line.buf)-2);
    CU_ASSERT(line.status == STATUS_ERROR);
    
    memset(&line, 0, sizeof(line));
    line.buf = " ---i--";
    result = parse_ld_line(&line);
    CU_ASSERT(result == ERR_BADINDEX);
    CU_ASSERT(line.cursor == strlen(line.buf)-1);
    CU_ASSERT(line.status == STATUS_ERROR);
    
    memset(&line, 0, sizeof(line));
    line.buf = " ---i0/5--";
    result = parse_ld_line(&line);
    CU_ASSERT(result == PLC_OK);
    CU_ASSERT(line.cursor == strlen(line.buf));
    CU_ASSERT(line.status == STATUS_RESOLVED); 
    CU_ASSERT(line.stmt == NULL);
    
    memset(&line, 0, sizeof(line));
    line.buf = " ---i0/5--( ";
    result = parse_ld_line(&line);
    CU_ASSERT(result == ERR_BADCOIL);
    CU_ASSERT(line.cursor == strlen(line.buf)-1); 
    CU_ASSERT(line.status == STATUS_ERROR);
    CU_ASSERT(line.stmt == NULL);
    
    memset(&line, 0, sizeof(line));
    line.buf = " ---i0/5--(QQ ";
    result = parse_ld_line(&line);
    CU_ASSERT(result == ERR_BADINDEX);
    CU_ASSERT(line.cursor == strlen(line.buf)-2); 
    CU_ASSERT(line.status == STATUS_ERROR);
    CU_ASSERT(line.stmt == NULL);
    
    memset(&line, 0, sizeof(line));
    line.buf = " ---!i0/5--(Q0/3 ";
    result = parse_ld_line(&line);
    CU_ASSERT(result == PLC_OK);
    CU_ASSERT(line.cursor == strlen(line.buf)-4); 
    CU_ASSERT(line.status == STATUS_RESOLVED);
    CU_ASSERT(line.stmt->tag == TAG_ASSIGNMENT);
    CU_ASSERT(line.stmt->v.ass.type == LD_COIL);
    
    CU_ASSERT(line.stmt->v.ass.right
                       ->v.exp.op == IL_AND);
    CU_ASSERT(line.stmt->v.ass.right
                       ->v.exp.mod == IL_NEG);
    CU_ASSERT(line.stmt->v.ass.right
                       ->v.exp.a
                       ->tag == TAG_IDENTIFIER);
    CU_ASSERT(line.stmt->v.ass.right
                       ->v.exp.a
                       ->v.id.operand == OP_INPUT);
    CU_ASSERT(line.stmt->v.ass.right
                       ->v.exp.a
                       ->v.id.byte == 0);
    CU_ASSERT(line.stmt->v.ass.right
                       ->v.exp.a
                       ->v.id.bit == 5);
    CU_ASSERT(line.stmt->v.ass.right
                       ->v.exp.b == NULL);
    
    clear_tree(line.stmt);
    memset(&line, 0, sizeof(line));
    
    line.buf = " ---q1/1--+";
    result = parse_ld_line(&line);
    CU_ASSERT(result == PLC_OK);
    CU_ASSERT(line.cursor == strlen(line.buf)-1);
    CU_ASSERT(line.status == STATUS_UNRESOLVED);
    CU_ASSERT(line.stmt->tag == TAG_EXPRESSION);
    CU_ASSERT(line.stmt->v.exp.op == IL_AND);
    CU_ASSERT(line.stmt->v.exp.mod == IL_NORM);
    CU_ASSERT(line.stmt->v.exp.a
                       ->tag == TAG_IDENTIFIER);
    CU_ASSERT(line.stmt->v.exp.a
                       ->v.id.operand == OP_OUTPUT);
    CU_ASSERT(line.stmt->v.exp.a
                       ->v.id.byte == 1);
    CU_ASSERT(line.stmt->v.exp.a
                       ->v.id.bit == 1);
    CU_ASSERT(line.stmt->v.exp.b == NULL);
    
    clear_tree(line.stmt);
    memset(&line, 0, sizeof(line));
    line.buf = " ---f1/1--|--+";
    result = parse_ld_line(&line);
    CU_ASSERT(result == PLC_OK);
    CU_ASSERT(line.cursor == strlen(line.buf)-1);
    CU_ASSERT(line.status == STATUS_UNRESOLVED);
    CU_ASSERT(line.stmt == NULL);
    
    memset(&line, 0, sizeof(line));
    line.buf = " ---q1/1-!i0/3--+";
    result = parse_ld_line(&line);
    CU_ASSERT(result == PLC_OK);
    CU_ASSERT(line.cursor == strlen(line.buf)-1);
    CU_ASSERT(line.status == STATUS_UNRESOLVED);
    CU_ASSERT(line.stmt->tag == TAG_EXPRESSION);
    CU_ASSERT(line.stmt->v.exp.op == IL_AND);
    CU_ASSERT(line.stmt->v.exp.mod == IL_NEG);
    CU_ASSERT(line.stmt->v.exp.a
                       ->tag == TAG_IDENTIFIER);
    CU_ASSERT(line.stmt->v.exp.a
                       ->v.id.operand == OP_INPUT);
    CU_ASSERT(line.stmt->v.exp.a
                       ->v.id.byte == 0);
    CU_ASSERT(line.stmt->v.exp.a
                        ->v.id.bit == 3);
    CU_ASSERT(line.stmt->v.exp.b
                       ->tag == TAG_EXPRESSION);
    CU_ASSERT(line.stmt->v.exp.b
                       ->v.exp.op == IL_AND);
    CU_ASSERT(line.stmt->v.exp.b
                       ->v.exp.mod == IL_NORM);
    CU_ASSERT(line.stmt->v.exp.b
                       ->v.exp.a
                       ->tag == TAG_IDENTIFIER);
    CU_ASSERT(line.stmt->v.exp.b
                       ->v.exp.a
                       ->v.id.operand == OP_OUTPUT);
    CU_ASSERT(line.stmt->v.exp.b
                       ->v.exp.a
                       ->v.id.byte == 1);
    CU_ASSERT(line.stmt->v.exp.b
                       ->v.exp.a
                       ->v.id.bit == 1);
    CU_ASSERT(line.stmt->v.exp.b
                       ->v.exp.b == NULL);
    clear_tree(line.stmt);
    memset(&line, 0, sizeof(line));
}

void ut_find_next_node()
{
    char lines[MAXBUF][MAXSTR];
    memset(lines, 0, MAXBUF * MAXSTR);
    
    sprintf(lines[0], "%s\n", " i0/1--)Q0/0");
    sprintf(lines[1], "%s\n", " i0/2--+");
    sprintf(lines[2], "%s\n", " i0/3--+--(Q0/1");
    
    ld_line_t * program = construct_program(lines, 3);
    int result = horizontal_parse(3, program);
    CU_ASSERT(result == PLC_OK);
    result = find_next_node(program, 0, 3);
    CU_ASSERT(result == 1);
    //printf("%d\n", result);

    destroy_program(3, program);
    
    sprintf(lines[0], "%s\n", " i0/1--(Q0/0");
    sprintf(lines[1], "%s\n", " i0/2--");
    sprintf(lines[2], "%s\n", " i0/3----(Q0/1");
    
    program = construct_program(lines, 3);
    result = horizontal_parse(3, program);
    CU_ASSERT(result == PLC_OK);
    result = find_next_node(program, 0, 3);
    CU_ASSERT(result == PLC_ERR);
    //printf("%d\n", result);

    destroy_program(3, program);
    
    
    sprintf(lines[0], "%s\n", " i0/1--+-[Q0/0");
    sprintf(lines[1], "%s\n", " i0/2--+ ");
    sprintf(lines[2], "%s\n", " i0/3--+           ");
    
    sprintf(lines[3], "%s\n", " ");
    sprintf(lines[4], "%s\n", " i0/4--+ ");
    sprintf(lines[5], "%s\n", " i0/5--+--(Q0/1           ");
    
    program = construct_program(lines, 6);
    result = horizontal_parse(6, program);
    CU_ASSERT(result == PLC_OK);
    result = find_next_node(program, 0, 3);
    CU_ASSERT(result == 0);
    //printf("%d\n", result);

    destroy_program(6, program);
    
}

void ut_parse_ld_program()
{
    struct PLC_regs p;
    init_mock_plc(&p);
    
    char lines[MAXBUF][MAXSTR];
    memset(lines, 0, MAXBUF * MAXSTR); 
    plc_t r = parse_ld_program("", lines, NULL);
    
    CU_ASSERT_PTR_NULL(r);
    
    int result = vertical_parse(0, 0, NULL);
    CU_ASSERT(result == PLC_ERR);
    
    sprintf(lines[0], "%s\n", " i0/1--+---[Q0/0       ");
    sprintf(lines[1], "%s\n", "       |     ");
    sprintf(lines[2], "%s\n", " i0/3--+       ");
    
    ld_line_t * program = construct_program(lines, 3);
    
    result = horizontal_parse(3, program);
    CU_ASSERT(program[0]->cursor == 7);

    result = find_next_node(program, 0, 3);
    CU_ASSERT(result == 0);
    
    result = vertical_parse(0, 3, program);
    CU_ASSERT(result == PLC_OK);
    
    CU_ASSERT(program[0]->stmt->tag == TAG_EXPRESSION);
    CU_ASSERT(program[0]->stmt->v.exp.op == IL_OR);
    CU_ASSERT(program[0]->stmt->v.exp.mod == IL_PUSH);
    CU_ASSERT(program[0]->stmt->v.exp.a->tag == TAG_EXPRESSION);
    CU_ASSERT(program[0]->stmt->v.exp.a->v.exp.a->tag == TAG_IDENTIFIER);
    
    CU_ASSERT(program[2]->stmt->tag == TAG_EXPRESSION);
    CU_ASSERT(program[2]->stmt->v.exp.op == IL_OR);
    CU_ASSERT(program[2]->stmt->v.exp.mod == IL_PUSH);
    
    destroy_program(3, program);

    r = parse_ld_program("1or3.ld", lines, &p);
    CU_ASSERT(p.status == PLC_OK);
    CU_ASSERT_STRING_EQUAL(p.rungs[0]->id, "1or3.ld");
    char code[3*MAXBUF];
    memset(code, 0, 3*MAXBUF);
    sprintf(code, "%s%s%s", lines[0], lines[1], lines[2]);  
    //printf("%s\n", p.rungs[0]->code);
    
    CU_ASSERT_STRING_EQUAL(p.rungs[0]->code->line, lines[0]);  
    char dump[MAXSTR * MAXBUF];
    memset(dump, 0, MAXBUF * MAXSTR);
    dump_rung(p.rungs[0], dump);
    
    //printf("%s\n", dump);
    
    clear_rung(p.rungs[0]);
    init_mock_plc(&p);
   
    memset(dump, 0, MAXBUF * MAXSTR);
    
    sprintf(lines[0], "%s\n", " i0/1--+ ");
    sprintf(lines[1], "%s\n", " i0/2--+-+-[Q0/0  ");
    sprintf(lines[2], "%s\n", " i0/3----+ ");
    sprintf(lines[3], "%s\n", "  ");
    sprintf(lines[4], "%s\n", " i0/4--+   ");
    sprintf(lines[5], "%s\n", " i0/5--+-(Q0/1            ");
    result = parse_ld_program("many_ors.ld", lines, &p)->status;
    
    CU_ASSERT(result == PLC_OK);
    
    dump_rung(p.rungs[0], dump);
    
    //printf("%s\n", dump);
    const char * expected = "\
0.LD i0/3\n\
1.OR(i0/2\n\
2.OR(i0/1\n\
3.)\n\
4.)\n\
5.S?Q0/0\n\
6.LD i0/5\n\
7.OR(i0/4\n\
8.)\n\
9.ST Q0/1\n\
";

   CU_ASSERT_STRING_EQUAL(dump, expected);
    
}

#endif //_UT_LD_H_
