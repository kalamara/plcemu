#ifndef _UT_APP_H_ 
#define _UT_APP_H_
extern struct entry ConfigSchema[];

void ut_apply_command()
{
    extern char MsgStr[];
//degenerates
    app_t r = apply_command(NULL, NULL);
    
    config_t conf = init_config(ConfigSchema, N_CONFIG_VARIABLES);
    config_t com = cli_init_command(conf);
    config_t arg = get_recursive_entry(CLI_ARG, com);
    
    com = set_numeric_entry(CLI_COM, COM_START, com);
    r = apply_command(com, NULL);
    
    app_t a = (app_t)malloc(sizeof(struct app));
    memset(a,0,sizeof(struct app));
    
    a = configure(conf, a);
//unsupported command    
    
//start should start
    r = apply_command(com, a);
    
    CU_ASSERT(r->plc->status == ST_RUNNING);

//stop shpould stop    
    com = set_numeric_entry(CLI_COM, COM_STOP, com);
    r = apply_command(com, a);
    
    CU_ASSERT(r->plc->status == ST_STOPPED);

    extern char * Mock_val;
    extern unsigned char Mock_op;
    extern int Mock_idx;
    
    com = set_numeric_entry(CLI_COM, COM_FORCE, com);
//force invalid block
    r = apply_command(com, a);
    CU_ASSERT_STRING_EQUAL(MsgStr, "Invalid force command\n");
    CU_ASSERT_PTR_NULL(Mock_val);
//force block out of bounds tested in library already     
//"FORCE DI 1 1"    
    config_t c = edit_seq_param(arg, "DI", 1, "FORCE","1");
    com = set_recursive_entry(CLI_ARG, c, com);
    
    r = apply_command(com, a);
    CU_ASSERT_STRING_EQUAL(Mock_val, "1");
    CU_ASSERT(Mock_op == OP_INPUT);
    CU_ASSERT(Mock_idx == 1);
    
    com = set_numeric_entry(CLI_COM, COM_UNFORCE, com);

//"UNFORCE DI 1"    
    c = edit_seq_param(arg, "DI", 1, "FORCE","UNFORCE");
    com = set_recursive_entry(CLI_ARG, c, com);
    
    r = apply_command(com, a);
    CU_ASSERT_PTR_NULL(Mock_val);
    CU_ASSERT(Mock_op == 0xff);
    CU_ASSERT(Mock_idx == -1);
    
    com = set_numeric_entry(CLI_COM, COM_EDIT, com);

//"EDIT DI 1 NAME X"    
    c = edit_seq_param(arg, "DI", 1, "ID", "X");
    com = set_recursive_entry(CLI_ARG, c, com);
    
    r = apply_command(com, a);
    CU_ASSERT_STRING_EQUAL(Mock_val, "X");
    CU_ASSERT(Mock_op == OP_INPUT);
    CU_ASSERT(Mock_idx == 1);

}
    
#endif//_UT_APP_H_
