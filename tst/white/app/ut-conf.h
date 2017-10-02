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
    CU_ASSERT_STRING_EQUAL(got->name, "SIM_INPUT");
    CU_ASSERT_STRING_EQUAL(got->e.scalar_str, "sim.in");
    
    got = get_entry(
            SUBDEV_IN, 
            get_entry(
                COMEDI_SUBDEV,
                get_entry(
                     CONFIG_COMEDI,
                     conf)->e.conf)->e.conf);
    
    CU_ASSERT(got->type_tag == ENTRY_INT);
    CU_ASSERT_STRING_EQUAL(got->name, "SUBDEV_IN");
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
  
  updated = store_seq_value(CONFIG_AI, 0, "MAX", "1.0", conf);
  CU_ASSERT_STRING_EQUAL(
    get_param_val("MAX",
        get_entry(CONFIG_AI, conf)->e.seq->vars[0].params), 
    "1.0");
  CU_ASSERT(updated->err == PLC_OK);
  
  updated = store_seq_value(CONFIG_AQ, 0, "", NULL, conf);
  CU_ASSERT(updated->err == PLC_ERR);  
  clear_config(conf);
    
}

void ut_process()
{
    config_t conf = process(PLC_ERR, NULL, NULL);
    CU_ASSERT(conf == NULL);
    
    conf = init_config();
    conf = process(PLC_ERR, NULL, conf);
    CU_ASSERT(conf->err == PLC_ERR);
    
    conf->err = PLC_OK;
    
    yaml_parser_t parser;
    conf = process(PLC_ERR, &parser, conf);
    CU_ASSERT(conf->err == PLC_ERR);
    conf->err = PLC_OK;
    
    memset(&parser, 0, sizeof(parser));
    yaml_parser_initialize(&parser);
    
    yaml_parser_set_input_string(&parser, "STEP: 100", 9); 
    conf = process(PLC_ERR, &parser, conf);
    CU_ASSERT(conf->err == PLC_OK);
    
    entry_t got = get_entry(CONFIG_STEP, conf);
    CU_ASSERT(got->e.scalar_int == 100);
    
    char * input = 
"#SIMULATION IO \n\
SIM: \n\
    SIM_INPUT:   simin.txt \n\
    SIM_OUTPUT:  simout.txt \n\
";
    memset(&parser, 0, sizeof(parser));
    yaml_parser_initialize(&parser);
    yaml_parser_set_input_string(&parser, input, strlen(input)); 

    conf = process(PLC_ERR, &parser, conf);
    CU_ASSERT(conf->err == PLC_OK);
    
    got = get_entry(
            SIM_INPUT, 
            get_entry(
                CONFIG_SIM, 
                conf)->e.conf);
    
    CU_ASSERT(got->type_tag == ENTRY_STR);
    CU_ASSERT_STRING_EQUAL(got->name, "SIM_INPUT");
    CU_ASSERT_STRING_EQUAL(got->e.scalar_str, "simin.txt");
    printf("%s\n", got->e.scalar_str);

    input = 
"#COMEDI interface:\n\
COMEDI: \n\
    COMEDI_FILE:     1 \n\
    COMEDI_SUBDEV :     \n\
        SUBDEV_IN :  5 \n\
        SUBDEV_OUT:  6 \n\
        SUBDEV_ADC:  7 \n\
        SUBDEV_DAC:  8 \n\
";  
    memset(&parser, 0, sizeof(parser));
    yaml_parser_initialize(&parser);
    yaml_parser_set_input_string(
    &parser, 
    (yaml_char_t * )input, 
    strlen(input)); 

    conf = process(PLC_ERR, &parser, conf);
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
 - INDEX  :     1    \n\
   ID     :    var1\n\
   VALUE  :    5.0\n\
   MIN    :    0.0\n\
   MAX    :   24.0\n";
    memset(&parser, 0, sizeof(parser));      
    yaml_parser_initialize(&parser);
    yaml_parser_set_input_string(&parser, input, strlen(input)); 

    conf = process(PLC_ERR, &parser, conf);
    CU_ASSERT(conf->err == PLC_OK);
    
    got = get_entry(CONFIG_AI,conf);
    CU_ASSERT(got->type_tag == ENTRY_SEQ);
    
    CU_ASSERT_STRING_EQUAL(
    get_param_val("MAX",
        get_entry(CONFIG_AI, conf)->e.seq->vars[1].params), 
    "24.0");
               
    yaml_parser_delete(&parser);
}

void ut_save(){
    config_t conf = init_config();
    conf = store_seq_value(CONFIG_AI, 0, "MAX", "1.0", conf);
    conf = store_seq_value(CONFIG_AI, 0, "ID", "var1", conf);
    yaml_emitter_t emitter;
    yaml_emitter_initialize(&emitter);
    yaml_event_t event;
    BYTE output[MAXSTR];
    memset(output,0, MAXSTR);
    size_t written;
    
    yaml_emitter_set_output_string(
        &emitter,
  	    output,
		MAXSTR,
		&written);
		
	yaml_stream_start_event_initialize(&event, YAML_UTF8_ENCODING);
	yaml_emitter_emit(&emitter, &event);
	
	int r = emit(&emitter, conf);
	
	yaml_stream_end_event_initialize(&event);
	yaml_emitter_emit(&emitter, &event); 		
    
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
    CU_ASSERT(r == PLC_OK);
}

void ut_get(){

    config_t conf = init_config();
    conf = store_seq_value(CONFIG_AI, 3, "MAX", "1.0", conf);
    conf = store_seq_value(CONFIG_AI, 3, "ID", "var1", conf);
 
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
    
    sequence_t seq =  get_sequence_entry(CONFIG_AI, conf);
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
}    
    
#endif//_UT_CONF_H_
