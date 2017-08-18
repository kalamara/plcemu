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
    int r = store_value(N_CONFIG_VARIABLES, "",  &conf);
    CU_ASSERT(r == PLC_ERR);
  
    r = store_value(CONFIG_HW, "simulation",  &conf);
    CU_ASSERT(r == PLC_OK);
    CU_ASSERT_STRING_EQUAL(
    get_entry(CONFIG_HW,conf)->e.scalar_str, "simulation");
  
    r = store_value(CONFIG_PIPE, "pipe",  &conf);
    CU_ASSERT(r == PLC_OK);
    CU_ASSERT_STRING_EQUAL(
    get_entry(CONFIG_PIPE,conf)->e.scalar_str, "pipe");

    r = store_value(CONFIG_STEP, "1000000",  &conf);
    CU_ASSERT(
        get_entry(CONFIG_STEP, conf)->e.scalar_int == 1000000);
    
    CU_ASSERT(r == PLC_OK);
    
/*store in a config within a config*/
    r = store_value(
        USPACE_BASE, 
        "12345678",  
        &(get_entry(CONFIG_USPACE, conf)->e.conf));

    CU_ASSERT(r == PLC_OK);
    CU_ASSERT(
        get_entry(
            USPACE_BASE,
            get_entry(
                CONFIG_USPACE, 
                conf)->e.conf)->e.scalar_int == 12345678);

/*store in a sequence*/
  r = store_seq_value(-1, 0, 0, "", &conf);

  CU_ASSERT(r == PLC_ERR);
  
  r = store_seq_value(CONFIG_AI, 0, VARIABLE_MAX, "1.0", &conf);
  CU_ASSERT_STRING_EQUAL(
    get_entry(CONFIG_AI, conf)->e.seq->vars[0].max, 
    "1.0");
  CU_ASSERT(r == PLC_OK);
  
  r = store_seq_value(CONFIG_AQ, 0, N_VARIABLE_PARAMS, "", &conf);
  CU_ASSERT(r == PLC_ERR);  
  clear_config(&conf);
    
}

void ut_process()
{
    int r = process(PLC_ERR, NULL, NULL);
    CU_ASSERT(r == PLC_ERR);
    
    config_t conf = init_config();
    r = process(PLC_ERR, NULL, conf);
    CU_ASSERT(r == PLC_ERR);
    
    yaml_parser_t parser;
    r = process(PLC_ERR, &parser, conf);
    CU_ASSERT(r == PLC_ERR);
    
    memset(&parser, 0, sizeof(parser));
    yaml_parser_initialize(&parser);
    
    yaml_parser_set_input_string(&parser, "STEP: 100", 9); 
    r = process(PLC_ERR, &parser, conf);
    CU_ASSERT(r == PLC_OK);
    
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

    r = process(PLC_ERR, &parser, conf);
    CU_ASSERT(r == PLC_OK);
    
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

    r = process(PLC_ERR, &parser, conf);
    CU_ASSERT(r == PLC_OK);
    
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

    r = process(PLC_ERR, &parser, conf);
    CU_ASSERT(r == PLC_OK);
    
    got = get_entry(CONFIG_AI,conf);
    CU_ASSERT(got->type_tag == ENTRY_SEQ);
    
    CU_ASSERT_STRING_EQUAL(
    get_entry(CONFIG_AI, conf)->e.seq->vars[1].max, 
    "24.0");
               
    yaml_parser_delete(&parser);
}

void ut_save(){
    config_t conf = init_config();
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
  USPACE_BASE: 50176\n\
  USPACE_WR: 0\n\
  USPACE_RD: 8\n\
COMEDI:\n\
  COMEDI_FILE: 0\n\
  COMEDI_SUBDEV:\n\
    SUBDEV_IN: 0\n\
    SUBDEV_OUT: 1\n\
    SUBDEV_ADC: 2\n\
    SUBDEV_DAC: 3\n\
SIM:\n\
  SIM_INPUT: sim.in\n\
  SIM_OUTPUT: sim.out\n\
AI:\n\
- 8\n\
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
//	printf("%s\n", output);
    CU_ASSERT(r == PLC_OK);

}

#endif//_UT_CONF_H_
