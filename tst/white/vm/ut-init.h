#ifndef _UT_INIT_H_ 
#define _UT_INIT_H_

struct PLC_regs Plc;

void ut_extract() 
{
    char * line = "I 1 lol";
    char name[SMALLSTR];
    int index = 0;
    int r = extract_name(NULL, NULL, -1 );
    CU_ASSERT(r == PLC_ERR);
    
    r = extract_name(line, NULL, -1 );
    CU_ASSERT(r == PLC_ERR);
    
    r = extract_name(line, name, 0);
    CU_ASSERT(r == 1);
    CU_ASSERT_STRING_EQUAL(name, "I");
    
    //
    
    line = "  \t\n   I_MAX 1 lol";
    r = extract_name(line, name, 0);  
    //ignore trailing spaces
    CU_ASSERT(r == 12);
    CU_ASSERT_STRING_EQUAL(name, "I_MAX");
    
    //printf("found %s up to %d\n", name, r);
    
    r = extract_index(NULL, NULL, -1 );
    CU_ASSERT(r == PLC_ERR);
    
    r = extract_index(line, NULL, -1 );
    CU_ASSERT(r == PLC_ERR);
    
    line = "I 5 lol";
    r = extract_index(line, &index, 1);
    CU_ASSERT(r == 3);
    CU_ASSERT(index == 5);
    //
    line = "I\t\t \n\r57 lol";
    r = extract_index(line, &index, 1);
    CU_ASSERT(r == 8);
    CU_ASSERT(index == 57);
    //printf("found %d up to %d\n", index, r);
    
    r = extract_value(NULL, NULL, -1 );
    CU_ASSERT(r == PLC_ERR);
    
    r = extract_value(line, NULL, -1 );
    CU_ASSERT(r == PLC_ERR);
    
    line = "I 5 lol";
    r = extract_value(line, name, 3 );
    CU_ASSERT(r == 7);
    CU_ASSERT_STRING_EQUAL(name, "lol");
    
    line = "I 5 lol\n\r\n\n\n";
    r = extract_value(line, name, 3 );
    CU_ASSERT(r == 7);
    CU_ASSERT_STRING_EQUAL(name, "lol");
    //printf("found %s up to %d\n", name, r);
   
}

void ut_load() 
{
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
/*    CU_ASSERT(plc.ni == 8);
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

 */
    CU_ASSERT(plc.step == 1);
    CU_ASSERT(plc.command == 0);
	CU_ASSERT(plc.status = 1);
	
//    CU_ASSERT(conf->response_file[MAXSTR-1] == 0);  
    clear_config(&conf);
}

#endif //_UT_INIT_H_

