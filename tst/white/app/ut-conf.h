#ifndef _UT_CONF_H_ 
#define _UT_CONF_H_

void ut_conf()
{
  config_t conf = init_config();
  //defaults
  CU_ASSERT(conf->nt == 4);
  CU_ASSERT(conf->timers[3].index == 0);
  
  CU_ASSERT(conf->ns == 4);
  CU_ASSERT(conf->pulses[3].index == 0);
  
  CU_ASSERT(conf->nm == 4);
  CU_ASSERT(conf->mregs[3].index == 0);
  
  CU_ASSERT(conf->nr == 4);
  CU_ASSERT(conf->mvars[3].index == 0);
 
  CU_ASSERT(conf->di == 8);
  CU_ASSERT(conf->dinps[7].index == 0);
 
  CU_ASSERT(conf->dq == 8);
  CU_ASSERT(conf->douts[7].index == 0);
 
  CU_ASSERT(conf->ai == 4);
  CU_ASSERT(conf->ainps[3].index == 0);
 
  CU_ASSERT(conf->aq == 4);
  CU_ASSERT(conf->aouts[3].index == 0);
 
  CU_ASSERT(conf->sigenable == 36);
  CU_ASSERT(conf->page_width == 80);
  CU_ASSERT(conf->page_len == 24);
  
  CU_ASSERT(conf->hw[MAXSTR-1] == 0);
  
  CU_ASSERT(conf->base == ADVANTECH_HISTORICAL_BASE);
  CU_ASSERT(conf->wr_offs == 0);
  CU_ASSERT(conf->rd_offs == 8);
  
  CU_ASSERT(conf->comedi_file == 0);
  CU_ASSERT(conf->comedi_subdev_i == 0);
  CU_ASSERT(conf->comedi_subdev_q == 1);
  CU_ASSERT(conf->comedi_subdev_ai == 2);
  CU_ASSERT(conf->comedi_subdev_aq == 3);
  
  CU_ASSERT(conf->sim_in_file[MAXSTR-1] == 0);
  CU_ASSERT(conf->sim_out_file[MAXSTR-1] == 0);
  
  CU_ASSERT(conf->step == 1);
  CU_ASSERT(conf->pipe[MAXSTR-1] == 0);
  
  CU_ASSERT(conf->response_file[MAXSTR-1] == 0);
  
  clear_config(&conf);
  CU_ASSERT(conf == NULL);
}



void ut_store()
{
    config_t conf = init_config();
    int r = store_value(N_CONFIG_VARIABLES, "",  &conf);
    CU_ASSERT(r == PLC_ERR);
  
 /*dont test deprecated shit
  CU_ASSERT(conf->sigenable == 36);
  CU_ASSERT(conf->page_width == 80);
  CU_ASSERT(conf->page_len == 24);
  
   CU_ASSERT(conf->response_file[MAXSTR-1] == 0);
  */
  
  r = store_value(CONFIG_HW, "simulation",  &conf);
  CU_ASSERT(r == PLC_OK);
  CU_ASSERT_STRING_EQUAL(conf->hw, "simulation");
  
  r = store_value(CONFIG_PIPE, "pipe",  &conf);
  CU_ASSERT(r == PLC_OK);
  CU_ASSERT_STRING_EQUAL(conf->pipe, "pipe");

  r = store_value(CONFIG_STEP, "1000000",  &conf);
  CU_ASSERT(conf->step == 1000000);
  CU_ASSERT(r == PLC_OK);

  r = store_value(CONFIG_USPACE_BASE, "12345678",  &conf);
  CU_ASSERT(r == PLC_OK);
  CU_ASSERT(conf->base == 12345678);  
  
  r = store_value(CONFIG_USPACE_WR, "9",  &conf);
  CU_ASSERT(r == PLC_OK);
  CU_ASSERT(conf->wr_offs == 9);
  
  r = store_value(CONFIG_USPACE_RD, "9",  &conf);
  CU_ASSERT(r == PLC_OK);
  CU_ASSERT(conf->rd_offs == 9);  
  
  r = store_value(CONFIG_COMEDI_FILE, "1",  &conf);
  CU_ASSERT(r == PLC_OK);
  CU_ASSERT(conf->comedi_file == 1);  
  
  r = store_value(CONFIG_SUBDEV_IN, "5",  &conf);
  CU_ASSERT(r == PLC_OK);
  CU_ASSERT(conf->comedi_subdev_i == 5);
  
  r = store_value(CONFIG_SUBDEV_OUT, "6",  &conf);
  CU_ASSERT(r == PLC_OK);
  CU_ASSERT(conf->comedi_subdev_q == 6);  
  
  r = store_value(CONFIG_SUBDEV_ADC, "7",  &conf);
  CU_ASSERT(r == PLC_OK);
  CU_ASSERT(conf->comedi_subdev_ai == 7);
  
  r = store_value(CONFIG_SUBDEV_DAC, "8",  &conf);
  CU_ASSERT(r == PLC_OK);
  CU_ASSERT(conf->comedi_subdev_aq == 8);  
  
  r = store_value(CONFIG_SIM_INPUT, "sim_in.txt",  &conf);
  CU_ASSERT(r == PLC_OK);
  CU_ASSERT_STRING_EQUAL(conf->sim_in_file, "sim_in.txt");
  
  r = store_value(CONFIG_SIM_OUTPUT, "sim_out.txt",  &conf);
  CU_ASSERT(r == PLC_OK);
  CU_ASSERT_STRING_EQUAL(conf->sim_out_file, "sim_out.txt");
  
  r = store_value(CONFIG_NT, "10",  &conf);
  CU_ASSERT(conf->nt == 10);
  CU_ASSERT(conf->timers[9].index == 0);
  CU_ASSERT(r == PLC_OK);
  
  r = store_value(CONFIG_NS, "11",  &conf);
  CU_ASSERT(conf->ns == 11);
  CU_ASSERT(conf->pulses[10].index == 0);
  CU_ASSERT(r == PLC_OK);
  
  r = store_value(CONFIG_NM, "12",  &conf);
  CU_ASSERT(conf->nm == 12);
  CU_ASSERT(conf->mregs[11].index == 0);
  CU_ASSERT(r == PLC_OK);
  
  r = store_value(CONFIG_NR, "13",  &conf);
  CU_ASSERT(conf->nr == 13);
  CU_ASSERT(conf->mvars[12].index == 0);
  CU_ASSERT(r == PLC_OK);
  
  r = store_value(CONFIG_NDI, "14",  &conf);
  CU_ASSERT(conf->di == 14);
  CU_ASSERT(conf->dinps[13].index == 0);
  CU_ASSERT(r == PLC_OK);
  
  r = store_value(CONFIG_NDQ, "15",  &conf);
  CU_ASSERT(conf->dq == 15);
  CU_ASSERT(conf->douts[14].index == 0);
  CU_ASSERT(r == PLC_OK);
  
  r = store_value(CONFIG_NAI, "16",  &conf);
  CU_ASSERT(conf->ai == 16);
  CU_ASSERT(conf->ainps[15].index == 0);

  CU_ASSERT(r == PLC_OK);
  
  r = store_value(CONFIG_NAQ, "17",  &conf);
  CU_ASSERT(conf->aq == 17);
  CU_ASSERT(conf->aouts[16].index == 0);
 
  CU_ASSERT(r == PLC_OK);
  
  r = store_value(CONFIG_PROGRAM_IL, "program.il",  &conf);
  CU_ASSERT(r == PLC_OK);
  CU_ASSERT_STRING_EQUAL(conf->program_file, "program.il");
  
  r = store_value(CONFIG_PROGRAM_LD, "program.ld",  &conf);
  CU_ASSERT(r == PLC_OK);
  CU_ASSERT_STRING_EQUAL(conf->program_file, "program.ld");
  
  r = store_seq_value(N_SEQUENCES, 0, 0, "", &conf);
  CU_ASSERT(r == PLC_ERR);
  
  r = store_seq_value(SEQ_AI, 0, VARIABLE_MAX, "1.0", &conf);
  CU_ASSERT_STRING_EQUAL(conf->ainps[0].max,"1.0");
  CU_ASSERT(r == PLC_OK);
  
  r = store_seq_value(SEQ_AQ, 0, N_VARIABLE_PARAMS, "", &conf);
  CU_ASSERT(r == PLC_ERR);
  
  r = store_seq_value(SEQ_DI, 1, VARIABLE_ID, "var1", &conf);
  CU_ASSERT_STRING_EQUAL(conf->dinps[1].name, "var1");
  CU_ASSERT(r == PLC_OK);
  
  r = store_seq_value(SEQ_DQ, conf->dq, VARIABLE_ID, "var1", &conf);
  CU_ASSERT(r == PLC_ERR);
  
  r = store_seq_value(SEQ_TIMER, 1, VARIABLE_VALUE, "0x100", &conf);
  CU_ASSERT_STRING_EQUAL(conf->timers[1].value, "0x100");
  CU_ASSERT(r == PLC_OK);
  
  r = store_seq_value(SEQ_MVAR, 1, VARIABLE_INDEX, "2", &conf);
  CU_ASSERT(conf->mvars[1].index == 2);
  CU_ASSERT(r == PLC_OK);
  
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
    CU_ASSERT(conf->step == 100);
    
    char * input = 
"#SIMULATION IO \n\
SIM: \n\
    SIM_INPUT:   sim.in \n\
    SIM_OUTPUT:  sim.out \n\
";
    memset(&parser, 0, sizeof(parser));
    yaml_parser_initialize(&parser);
    yaml_parser_set_input_string(&parser, input, strlen(input)); 

    r = process(PLC_ERR, &parser, conf);
    CU_ASSERT(r == PLC_OK);
    CU_ASSERT_STRING_EQUAL(conf->sim_in_file, "sim.in");
    CU_ASSERT_STRING_EQUAL(conf->sim_out_file, "sim.out");
    
    input = 
"#COMEDI interface:\n\
COMEDI: \n\
    COMEDI_FILE:     1 \n\
    COMEDI_SUBDEV :     \n\
        SUBDEV_IN :   5 \n\
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
    CU_ASSERT(conf->comedi_file == 1);  
    CU_ASSERT(conf->comedi_subdev_i == 5);
    CU_ASSERT(conf->comedi_subdev_q == 6);  
    CU_ASSERT(conf->comedi_subdev_ai == 7);
    CU_ASSERT(conf->comedi_subdev_aq == 8);

    input = "\n\
AI:  \n\
 - INDEX  :     1    \n\
   ID     :    var1\n\
   MIN    :    0.0\n\
   MAX    :   24.0\n";
    memset(&parser, 0, sizeof(parser));      
    yaml_parser_initialize(&parser);
    yaml_parser_set_input_string(&parser, input, strlen(input)); 

    r = process(PLC_ERR, &parser, conf);
    CU_ASSERT(r == PLC_OK);
    CU_ASSERT_STRING_EQUAL(conf->ainps[1].name, "var1");
    CU_ASSERT_STRING_EQUAL(conf->ainps[1].min,"0.0");
    CU_ASSERT_STRING_EQUAL(conf->ainps[1].max,"24.0");
                      
    yaml_parser_delete(&parser);
}

void ut_apply()
{
    struct PLC_regs plc;
    memset(&plc, 0, sizeof(struct PLC_regs));
    config_t conf = init_config();
    store_value(CONFIG_HW, "simulated_hardware",  &conf);
    
    configure(conf, &plc);
    CU_ASSERT_STRING_EQUAL(plc.hw, "simulated_hardware");
    //printf("hw: %s\n", plc.hw);
    CU_ASSERT(plc.ni == 8);
    CU_ASSERT(plc.di[63].I == 0);
    CU_ASSERT(plc.inputs[7] == 0);
    CU_ASSERT(plc.edgein[7] == 0);
    CU_ASSERT(plc.maskin[7] == 0);
    CU_ASSERT(plc.maskin_N[7] == 0);
   
    CU_ASSERT(plc.nq == 8);
    CU_ASSERT(plc.dq[63].Q == 0);
    CU_ASSERT(plc.outputs[7] == 0);
    CU_ASSERT(plc.maskout[7] == 0);
    CU_ASSERT(plc.maskout_N[7] == 0);
   
    CU_ASSERT(plc.nai == 4);
    CU_ASSERT(plc.real_in[3] == 0);
    CU_ASSERT(plc.ai[3].V < FLOAT_PRECISION);
    
    CU_ASSERT(plc.naq == 4);
    CU_ASSERT(plc.real_out[3] == 0);
    CU_ASSERT(plc.aq[3].V < FLOAT_PRECISION);
    
    CU_ASSERT(plc.nt == 4);
    CU_ASSERT(plc.t[3].Q == 0);
    
    CU_ASSERT(plc.ns == 4);
    CU_ASSERT(plc.s[3].Q == 0);
    
    CU_ASSERT(plc.nm == 4);
    CU_ASSERT(plc.m[3].V == 0);
    
    CU_ASSERT(plc.nmr == 4);
    CU_ASSERT(plc.mr[3].V < FLOAT_PRECISION);

    CU_ASSERT(plc.old->ni == 8);
    CU_ASSERT(plc.old->di[63].I == 0);
    CU_ASSERT(plc.old->inputs[7] == 0);
    CU_ASSERT(plc.old->edgein[7] == 0);
    CU_ASSERT(plc.old->maskin[7] == 0);
    CU_ASSERT(plc.old->maskin_N[7] == 0);
   
    CU_ASSERT(plc.old->nq == 8);
    CU_ASSERT(plc.old->dq[63].Q == 0);
    CU_ASSERT(plc.old->outputs[7] == 0);
    CU_ASSERT(plc.old->maskout[7] == 0);
    CU_ASSERT(plc.old->maskout_N[7] == 0);
   
    CU_ASSERT(plc.old->nai == 4);
    CU_ASSERT(plc.old->real_in[3] == 0);
    CU_ASSERT(plc.old->ai[3].V < FLOAT_PRECISION);
    
    CU_ASSERT(plc.old->naq == 4);
    CU_ASSERT(plc.old->real_out[3] == 0);
    CU_ASSERT(plc.old->aq[3].V < FLOAT_PRECISION);
    
    CU_ASSERT(plc.old->nt == 4);
    CU_ASSERT(plc.old->t[3].Q == 0);
    
    CU_ASSERT(plc.old->ns == 4);
    CU_ASSERT(plc.old->s[3].Q == 0);
    
    CU_ASSERT(plc.old->nm == 4);
    CU_ASSERT(plc.old->m[3].V == 0);
    
    CU_ASSERT(plc.old->nmr == 4);
    CU_ASSERT(plc.old->mr[3].V < FLOAT_PRECISION);

 
    CU_ASSERT(plc.step == 1);
    CU_ASSERT(plc.command == 0);
	CU_ASSERT(plc.status = 1);
	
    CU_ASSERT(conf->response_file[MAXSTR-1] == 0);  
    clear_config(&conf);
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
NT: 4\n\
NS: 4\n\
NM: 4\n\
NR: 4\n\
HW: STDI/O\n\
NDI: 8\n\
NDQ: 8\n\
NAI: 4\n\
NAQ: 4\n\
...\n\
";
	CU_ASSERT_STRING_EQUAL(output,expected);
	//printf("%s\n", output);
    CU_ASSERT(r == PLC_OK);

}

#endif//_UT_CONF_H_
