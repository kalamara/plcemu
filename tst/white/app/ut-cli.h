#ifndef _UT_CLI_H_ 
#define _UT_CLI_H_
extern struct entry ConfigSchema[];
void ut_cli()
{
//any null should not make it crash, and return null
    config_t result = cli_parse(NULL, NULL);
    int c = get_numeric_entry(CLI_COM, result);
    CU_ASSERT(c==CONF_ERR);
    char input[MAXSTR];
    memset(input,0,MAXSTR);
    config_t conf = init_config(ConfigSchema, N_CONFIG_VARIABLES);
     
    sprintf(input, "%s\n", "HELP");      
    result = cli_parse(input, NULL);
    c = get_numeric_entry(CLI_COM, result);
 
    CU_ASSERT(c==CONF_ERR);
        
    config_t command = cli_init_command(conf);
//bull should be ignored
    sprintf(input, "%s\n", "lolol");          
    result = cli_parse(input, command);
    c = get_numeric_entry(CLI_COM, result);
 
    CU_ASSERT(c==COM_NONE);
//simulate console input by printfs with newline on a buffer    
    sprintf(input, "%s\n", "HELP");      
    result = cli_parse(input, command);
    c = get_numeric_entry(CLI_COM, result);
    
    CU_ASSERT(c==COM_HELP);
//case insensitive    
    sprintf(input, "%s\n", "quit");      
    result = cli_parse(input, command);
    c = get_numeric_entry(CLI_COM, result);
 
    CU_ASSERT(c==COM_QUIT);
//all the commands    
    sprintf(input, "%s\n", "Start");
    result = cli_parse(input, command);
    c = get_numeric_entry(CLI_COM, result);
    
    CU_ASSERT(c==COM_START);

    sprintf(input, "%s\n", "sToP");    
    result = cli_parse(input, command);
    c = get_numeric_entry(CLI_COM, result);
 
    CU_ASSERT(c==COM_STOP);
//some commands carry a string argument
    sprintf(input, "%s\n", "save");    
    result = cli_parse(input, command);
    c = get_numeric_entry(CLI_COM, result);
    char * a = get_string_entry(CLI_ARG, result);
    
    CU_ASSERT(c==COM_SAVE);
    CU_ASSERT_PTR_NULL(a);
 /*   
    sprintf(input, "%s\n", "SAVE lol.yml");
    result = cli_parse(input, command);
    c = get_numeric_entry(CLI_COM, result);
    a = get_string_entry(CLI_ARG, result);
    //printf("Got:-->%s<-----", a);
    
    CU_ASSERT_STRING_EQUAL(a, "lol.yml");           
    CU_ASSERT(c==COM_SAVE);
    
    sprintf(input, "%s\n", "LOAD lolol.yml");
    result = cli_parse(input, command);
    c = get_numeric_entry(CLI_COM, result);
    a = get_string_entry(CLI_ARG, result);
    //printf("Got:-->%s<-----", a);
    
    CU_ASSERT_STRING_EQUAL(a, "lolol.yml");      
    CU_ASSERT(c==COM_LOAD);
*/    
//some commands carry multiple arguments that match logic block sequences
//ignore missing arguments    
    sprintf(input, "%s\n", "FORCE");
    result = cli_parse(input, command);
    c = get_numeric_entry(CLI_COM, result);
    CU_ASSERT(c==COM_NONE);
    //FORCE
    sprintf(input, "%s\n", "FORCE DI ");
    result = cli_parse(input, command);
    c = get_numeric_entry(CLI_COM, result);
    CU_ASSERT(c==COM_NONE);
    
    sprintf(input, "%s\n", "FORCE 3 1");
    result = cli_parse(input, command);
    c = get_numeric_entry(CLI_COM, result);
    CU_ASSERT(c==COM_NONE);
//arguments are mapped to existing block sequences
    sprintf(input, "%s\n", "FORCE 3 2 1");
    result = cli_parse(input, command);
    c = get_numeric_entry(CLI_COM, result);
    CU_ASSERT(c==COM_NONE);
        
    sprintf(input, "%s\n", "FORCE DI 1 1");
    result = cli_parse(input, command);
    c = get_numeric_entry(CLI_COM, result);
  
    config_t arg = get_recursive_entry(CLI_ARG, result);
    int k = get_key("DI", arg);
    sequence_t s = get_sequence_entry(k, arg);
    CU_ASSERT_STRING_EQUAL(get_param_val("FORCE", s->vars[1].params), "1");
    CU_ASSERT(c==COM_FORCE);   
    //UNFORCE
    sprintf(input, "%s\n", "UNFORCE 3 2");
    result = cli_parse(input, command);
    c = get_numeric_entry(CLI_COM, result);
    CU_ASSERT(c==COM_NONE);
        
    sprintf(input, "%s\n", "UNFORCE AI 1");
    result = cli_parse(input, command);
    c = get_numeric_entry(CLI_COM, result);
    arg = get_recursive_entry(CLI_ARG, result);
  
    k = get_key("AI", arg);
    s = get_sequence_entry(k, arg);
    CU_ASSERT_STRING_EQUAL(get_param_val("FORCE", s->vars[1].params), "UNFORCE");
    CU_ASSERT(c==COM_UNFORCE);  
    //EDIT
    sprintf(input, "%s\n", "EDIT 3 2 1");
    result = cli_parse(input, command);
    c = get_numeric_entry(CLI_COM, result);
    CU_ASSERT(c==COM_NONE);
        
    sprintf(input, "%s\n", "EDIT AQ 1 NAME LOL");
    result = cli_parse(input, command);
    c = get_numeric_entry(CLI_COM, result);
    arg = get_recursive_entry(CLI_ARG, result);
  
    k = get_key("AQ", arg);
    s = get_sequence_entry(k, arg);
    CU_ASSERT_STRING_EQUAL(get_param_val("NAME", s->vars[1].params), "LOL");
    CU_ASSERT(c==COM_EDIT);  
}
    
#endif//_UT_CLI_H_
