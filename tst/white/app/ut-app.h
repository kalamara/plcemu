#ifndef _UT_APP_H_ 
#define _UT_APP_H_

void ut_apply_command()
{
    app_t r = apply_command(NULL, NULL);
    
    config_t conf = init_config();
    config_t com = cli_init_command(conf);
    
    com = set_numeric_entry(CLI_COM, COM_START, com);
    r = apply_command(com, NULL);
    
    app_t a = (app_t)malloc(sizeof(struct app));
    memset(a,0,sizeof(struct app));
    
    a = configure(conf, a);
    r = apply_command(com, a);
    
    CU_ASSERT(r->plc->status == ST_RUNNING);
    
    com = set_numeric_entry(CLI_COM, COM_STOP, com);
    r = apply_command(com, a);
    
    CU_ASSERT(r->plc->status == ST_STOPPED);
    
    //"FORCE DI 2 1"
    sequence_t s = edit_seq_param(com, 
                                        "DI", 
                                        2, 
                                        "FORCE",
                                        "1");
    com = set_numeric_entry(CLI_COM, COM_FORCE, com);
    r = apply_command(com, a);
    
    
}
    
#endif//_UT_APP_H_
