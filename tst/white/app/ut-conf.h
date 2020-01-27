#ifndef _UT_CONF_H_ 
#define _UT_CONF_H_

void ut_conf()
{
   config_t conf = init_config();
  //defaults

  //bullshit should return NULL
    entry_t got = get_entry(-1, conf);
    CU_ASSERT(got == NULL);
      
    got = get_entry(CONFIG_STEP, conf);
    CU_ASSERT(got->type_tag == ENTRY_INT);
    CU_ASSERT_STRING_EQUAL(got->name, "STEP");
    CU_ASSERT(got->e.scalar_int == 1);
    
    got = get_entry(CONFIG_PIPE, conf);
    CU_ASSERT(got->type_tag == ENTRY_STR);
    CU_ASSERT_STRING_EQUAL(got->name, "PIPE");
    CU_ASSERT_STRING_EQUAL(got->e.scalar_str, "plcpipe");
    
    got = get_entry(
            SIM_INPUT, 
            get_entry(
                CONFIG_SIM, 
                conf)->e.conf);
    
    CU_ASSERT(got->type_tag == ENTRY_STR);
    CU_ASSERT_STRING_EQUAL(got->name, "INPUT");
    CU_ASSERT_STRING_EQUAL(got->e.scalar_str, "sim.in");
    
    got = get_entry(
            SUBDEV_IN, 
            get_entry(
                COMEDI_SUBDEV,
                get_entry(
                     CONFIG_COMEDI,
                     conf)->e.conf)->e.conf);
    
    CU_ASSERT(got->type_tag == ENTRY_INT);
    CU_ASSERT_STRING_EQUAL(got->name, "IN");
    CU_ASSERT_EQUAL(got->e.scalar_int, 0);
    
    got = get_entry(CONFIG_TIMER, conf);
    CU_ASSERT(got->type_tag == ENTRY_SEQ);
    CU_ASSERT_STRING_EQUAL(got->name, "TIMERS");
    CU_ASSERT(got->e.seq->size == 4);
    CU_ASSERT_PTR_NULL(got->e.seq->vars[3].name);
    
    //get by value
    int got_key = get_key("", conf);
    
    CU_ASSERT(got_key == PLC_ERR);
    
    got_key = get_key("HW", conf);
    CU_ASSERT(got_key == CONFIG_HW);
    clear_config(&conf);
}

void ut_store()
{
    config_t conf = init_config();
    config_t updated = store_value(N_CONFIG_VARIABLES, "",  conf);
    CU_ASSERT(updated->err == PLC_ERR);
    updated->err = PLC_OK;
  
    updated = store_value(CONFIG_HW, "simulation",  conf);
    CU_ASSERT(updated->err == PLC_OK);
    CU_ASSERT_STRING_EQUAL(
    get_entry(CONFIG_HW,updated)->e.scalar_str, "simulation");
  
    updated = store_value(CONFIG_PIPE, "pipe",  conf);
    CU_ASSERT(updated->err == PLC_OK);
    CU_ASSERT_STRING_EQUAL(
    get_entry(CONFIG_PIPE,conf)->e.scalar_str, "pipe");

    updated = store_value(CONFIG_STEP, "1000000",  conf);
    CU_ASSERT(
        get_entry(CONFIG_STEP, conf)->e.scalar_int == 1000000);
    
    CU_ASSERT(updated->err == PLC_OK);
    
/*store in a config within a config*/
    updated = store_value(
        USPACE_BASE, 
        "12345678",  
        (get_entry(CONFIG_USPACE, conf)->e.conf));

    CU_ASSERT(updated->err == PLC_OK);
    CU_ASSERT(
        get_entry(
            USPACE_BASE,
            get_entry(
                CONFIG_USPACE, 
                conf)->e.conf)->e.scalar_int == 12345678);

/*store in a sequence*/
  updated = store_seq_value(-1, 0, 0, "", conf);

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
  updated = store_seq_value(CONFIG_AQ, 0, "", NULL, conf);
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
	//printf("%s\n", output);
}

void ut_get(){

    config_t conf = init_config();
    sequence_t seq =  get_sequence_entry(CONFIG_AI, conf);
    conf = store_seq_value(seq, 3, "MAX", "1.0", conf);
    conf = store_seq_value(seq, 3, "ID", "var1", conf);
 
    //bullshit should return -1
    int step = get_numeric_entry(-523, conf);
    CU_ASSERT(step == CONF_ERR);
    
    step = get_numeric_entry(CONFIG_STEP, conf);
    CU_ASSERT(step == 1);
    
    //wrong type should return NULL
    char * hw = get_string_entry(CONFIG_STEP, conf);
    CU_ASSERT(hw == NULL);
    
    hw = get_string_entry(CONFIG_HW, conf);
    CU_ASSERT_STRING_EQUAL(hw, "STDI/O");
    
    
    int i = 0;
    char * name = NULL;
    char * max = NULL;
    for(; i < seq->size; i++){
        if(i == 3){
            name = seq->vars[i].name;
            CU_ASSERT_STRING_EQUAL(name, "var1");
            max = get_param_val("MAX", seq->vars[i].params);
            CU_ASSERT_STRING_EQUAL(max, "1.0");
        }        
    }
    
    variable_t v = get_variable("var1", seq);
    max = get_param_val("MAX", v->params);
    CU_ASSERT_STRING_EQUAL(max, "1.0")
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
        
    config_t conf = init_config();
    
    entry_t numeric = copy_entry(get_entry(CONFIG_STEP, conf));
    CU_ASSERT(numeric->e.scalar_int == 1);
    
    entry_t string = copy_entry(get_entry(CONFIG_HW, conf));
    CU_ASSERT_STRING_EQUAL(string->e.scalar_str, "STDI/O");
    
    entry_t sequence = copy_entry(get_entry(CONFIG_AI, conf));
    CU_ASSERT(sequence->type_tag == ENTRY_SEQ);
    CU_ASSERT(sequence->e.seq->size == 8);
    
    entry_t map = copy_entry(get_entry(CONFIG_SIM, conf));
}
*/
    
#endif//_UT_CONF_H_
