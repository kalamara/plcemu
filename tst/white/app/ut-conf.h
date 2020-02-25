#ifndef _UT_CONF_H_ 
#define _UT_CONF_H_

extern struct entry ConfigSchema[];
extern struct entry HwSchema[];
void ut_conf()
{
   
   config_t conf = init_config(ConfigSchema, N_CONFIG_VARIABLES);
  //defaults

  //bullshit should return NULL
    entry_t got = get_entry(-1, conf);
    CU_ASSERT(got == NULL);
      
    got = get_entry(CONFIG_STEP, conf);
    CU_ASSERT(got->type_tag == ENTRY_INT);
    CU_ASSERT_STRING_EQUAL(got->name, "STEP");
    CU_ASSERT(got->e.scalar_int == 100);
    
    
    got = get_entry(CONFIG_TIMER, conf);
    CU_ASSERT(got->type_tag == ENTRY_SEQ);
    CU_ASSERT_STRING_EQUAL(got->name, "TIMERS");
    CU_ASSERT(got->e.seq->size == 2);
    CU_ASSERT_PTR_NULL(got->e.seq->vars[1].name);
    
    //get by value
    int got_key = get_key("", conf);
    
    CU_ASSERT(got_key == PLC_ERR);
    
    got_key = get_key("HW", conf);
    CU_ASSERT(got_key == CONFIG_HW);
    clear_config(conf);
}

void ut_store()
{
    config_t conf = init_config(ConfigSchema, N_CONFIG_VARIABLES);
    config_t updated = store_value(N_CONFIG_VARIABLES, "",  conf);
    CU_ASSERT(updated->err == PLC_ERR);
    updated->err = PLC_OK;
/*  
    updated = store_value(CONFIG_HW, "simulation",  conf);
    CU_ASSERT(updated->err == PLC_OK);
    CU_ASSERT_STRING_EQUAL(
    get_entry(CONFIG_HW,updated)->e.scalar_str, "simulation");

*/
    updated = store_value(CONFIG_STEP, "1000000",  conf);
    CU_ASSERT(
        get_entry(CONFIG_STEP, conf)->e.scalar_int == 1000000);
    
    CU_ASSERT(updated->err == PLC_OK);
    
/*store in a config within a config*/


/*store in a sequence*/
  updated = store_seq_value(NULL, 0, NULL, "", conf);

  CU_ASSERT(updated->err == PLC_ERR);
  updated->err = PLC_OK;
  sequence_t seq = conf->map[CONFIG_AI]->e.seq;
  updated = store_seq_value(seq, 0, "MAX", "1.0", conf);
  CU_ASSERT_STRING_EQUAL(
    get_param_val("MAX",
        get_entry(CONFIG_AI, conf)->e.seq->vars[0].params), 
    "1.0");
  CU_ASSERT(updated->err == PLC_OK);
  seq = conf->map[CONFIG_AQ]->e.seq;
  updated = store_seq_value(seq, CONFIG_AQ, "", NULL, conf);
  CU_ASSERT(updated->err == PLC_ERR);  
  clear_config(conf);
    
}
/*
void ut_process()
{
    config_t conf = process(PLC_ERR, NULL, NULL);
    CU_ASSERT(conf == NULL);
    
    config_t def = init_config();
    conf = process(PLC_ERR, NULL, def);
    CU_ASSERT(conf->err == PLC_ERR);
    
    conf->err = PLC_OK;
    conf = deserialize_config(NULL, def);

    CU_ASSERT(conf->err == PLC_ERR);
    conf->err = PLC_OK;
   
    conf = deserialize_config("STEP: 100", def);
    CU_ASSERT(conf->err == PLC_OK);
    
    entry_t got = get_entry(CONFIG_STEP, def);
    CU_ASSERT(got->e.scalar_int == 100);
    
    char * input = 
"#SIMULATION IO \n\
SIM: \n\
    INPUT:   simin.txt \n\
    OUTPUT:  simout.txt \n\
";
    conf = deserialize_config(input, def);
    CU_ASSERT(conf->err == PLC_OK);
   
    got = get_entry(
            SIM_INPUT, 
            get_entry(
                CONFIG_SIM, 
                conf)->e.conf);
    
    CU_ASSERT(got->type_tag == ENTRY_STR);
    CU_ASSERT_STRING_EQUAL(got->name, "INPUT");
    CU_ASSERT_STRING_EQUAL(got->e.scalar_str, "simin.txt");
    printf("%s\n", got->e.scalar_str);

    input = 
"#COMEDI interface:\n\
COMEDI: \n\
    FILE:     1 \n\
    SUBDEV :     \n\
        IN :  5 \n\
        OUT:  6 \n\
        ADC:  7 \n\
        DAC:  8 \n\
";  

    conf = deserialize_config(input, def);
    CU_ASSERT(conf->err == PLC_OK);
   
    got = get_entry(
            SUBDEV_IN, 
            get_entry(
                COMEDI_SUBDEV,
                get_entry(
                     CONFIG_COMEDI,
                     conf)->e.conf)->e.conf);
    
    CU_ASSERT_EQUAL(got->e.scalar_int, 5);
   
    input = "\n\
AI:  \n\
 - 5 \n\
 - INDEX  :     1    \n\
   ID     :    var1\n\
   VALUE  :    5.0\n\
   MIN    :    0.0\n\
   MAX    :   24.0\n";
 
    conf = deserialize_config(input, def);
    CU_ASSERT(conf->err == PLC_OK);
    
    got = get_entry(CONFIG_AI,conf);
    CU_ASSERT(got->type_tag == ENTRY_SEQ);
    CU_ASSERT(got->e.seq->size == 5);
    char * max = get_param_val("MAX",
        got->e.seq->vars[1].params);
    CU_ASSERT_STRING_EQUAL(max, "24.0");
}

void ut_save(){
    config_t conf = init_config();
    sequence_t seq = conf->map[CONFIG_AI]->e.seq;
    conf = store_seq_value(seq, 0, "MAX", "1.0", conf);
    conf = store_seq_value(seq, 0, "ID", "var1", conf);

    char * output = serialize_config( conf);    
    char * expected = "\
---\n\
STEP: 1\n\
PIPE: plcpipe\n\
HW: STDI/O\n\
USPACE:\n\
  BASE: 50176\n\
  WR: 0\n\
  RD: 8\n\
COMEDI:\n\
  FILE: 0\n\
  SUBDEV:\n\
    IN: 0\n\
    OUT: 1\n\
    ADC: 2\n\
    DAC: 3\n\
SIM:\n\
  INPUT: sim.in\n\
  OUTPUT: sim.out\n\
PROGRAM:\n\
- 2\n\
AI:\n\
- 8\n\
- INDEX: 0\n\
  ID: var1\n\
  MAX: 1.0\n\
AQ:\n\
- 8\n\
DI:\n\
- 8\n\
DQ:\n\
- 8\n\
MVAR:\n\
- 4\n\
MREG:\n\
- 4\n\
TIMERS:\n\
- 4\n\
PULSES:\n\
- 4\n\
...\n\
";
    
	CU_ASSERT_STRING_EQUAL(output,expected);
	printf("%s\n", output);
}
*/
void ut_get(){

    config_t conf = init_config(ConfigSchema, N_CONFIG_VARIABLES);
    sequence_t seq =  get_sequence_entry(CONFIG_AI, conf);
    conf = store_seq_value(seq, 1, "MAX", "1.0", conf);
    conf = store_seq_value(seq, 1, "ID", "var1", conf);
 
    //bullshit should return -1
    int step = get_numeric_entry(-523, conf);
    CU_ASSERT(step == CONF_ERR);
    
    step = get_numeric_entry(CONFIG_STEP, conf);
    CU_ASSERT(step == 100);
    
    //wrong type should return NULL
    char * hw = get_string_entry(CONFIG_STEP, conf);
    CU_ASSERT(hw == NULL);
   
    
    int i = 0;
    char * name = NULL;
    char * max = NULL;
    for(; i < seq->size; i++){
        if(i == 1){
            name = seq->vars[i].name;
            CU_ASSERT_STRING_EQUAL(name, "var1");
            max = get_param_val("MAX", seq->vars[i].params);
            CU_ASSERT_STRING_EQUAL(max, "1.0");
        }        
    }
    
    variable_t v = get_variable("var1", seq);
    max = get_param_val("MAX", v->params);
    CU_ASSERT_STRING_EQUAL(max, "1.0")
    clear_config(conf);
}    

void ut_set(){
    config_t conf = init_config(ConfigSchema, N_CONFIG_VARIABLES);
//garbage in garbage out
    config_t res = set_numeric_entry(-1, 2, NULL);
    CU_ASSERT_PTR_NULL(res);
//numeric happy path    
    res = set_numeric_entry(CONFIG_STEP, 2, conf);
    int step = get_numeric_entry(CONFIG_STEP, res);
    CU_ASSERT(step == 2);
//more garbage in garbage out    
    res = set_recursive_entry(CONFIG_HW, NULL, NULL);
    CU_ASSERT_PTR_NULL(res);
//set sub config    
    config_t hw = init_config(HwSchema, N_HW_VARS);
    res = set_recursive_entry(CONFIG_HW, hw, conf);
    config_t got = get_recursive_entry(CONFIG_HW, conf);
    char * label = get_string_entry(HW_LABEL, got);
    
    CU_ASSERT_STRING_EQUAL(label, "DRY");
    clear_config(conf);    
    clear_config(hw);
}

void ut_copy(){

    sequence_t seq = new_sequence(5);
    seq->vars[0].name = "lol";
    seq->vars[0].index = 1;
    
    seq->vars[0].params = 
        append_param(seq->vars[0].params, "name", "a");    
    seq->vars[0].params = 
        append_param(seq->vars[0].params, "value", "m");
        
    sequence_t seqond = copy_sequence(seq);
    CU_ASSERT(seqond->size == 5);    
    CU_ASSERT_STRING_EQUAL(seqond->vars[0].name, "lol");
    CU_ASSERT(seqond->vars[0].index == 1);
    CU_ASSERT_STRING_EQUAL(seqond->vars[0].params->key, "name");
    CU_ASSERT_STRING_EQUAL(seqond->vars[0].params->next->value, "m");
        
    config_t conf = init_config(ConfigSchema, N_CONFIG_VARIABLES);
    
    entry_t numeric = copy_entry(get_entry(CONFIG_STEP, conf));
    CU_ASSERT(numeric->e.scalar_int == 100);
    
    entry_t sequence = copy_entry(get_entry(CONFIG_AI, conf));
    CU_ASSERT(sequence->type_tag == ENTRY_SEQ);
    CU_ASSERT(sequence->e.seq->size == 2);
    clear_config(conf);    
}

    
#endif//_UT_CONF_H_
