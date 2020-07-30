#ifndef _UT_INIT_H_ 
#define _UT_INIT_H_

void ut_construct()
{
    plc_t plc = new_plc(8,8,4,4,4,4,4,4,100,NULL); 
   
    CU_ASSERT_PTR_NULL(plc->hw);
    //printf("hw: %s\n", plc.hw);
    CU_ASSERT(plc->ni == 8);
    CU_ASSERT(plc->di[63].I == 0);
    CU_ASSERT(plc->inputs[7] == 0);
   
    CU_ASSERT(plc->nq == 8);
    CU_ASSERT(plc->dq[63].Q == 0);
    CU_ASSERT(plc->outputs[7] == 0);
   
    CU_ASSERT(plc->nai == 4);
    CU_ASSERT(plc->real_in[3] == 0);
    CU_ASSERT(plc->ai[3].V < FLOAT_PRECISION);
    
    CU_ASSERT(plc->naq == 4);
    CU_ASSERT(plc->real_out[3] == 0);
    CU_ASSERT(plc->aq[3].V < FLOAT_PRECISION);
    
    CU_ASSERT(plc->nt == 4);
    CU_ASSERT(plc->t[3].Q == 0);
    
    CU_ASSERT(plc->ns == 4);
    CU_ASSERT(plc->s[3].Q == 0);
    
    CU_ASSERT(plc->nm == 4);
    CU_ASSERT(plc->m[3].V == 0);
    
    CU_ASSERT(plc->nmr == 4);
    CU_ASSERT(plc->mr[3].V < FLOAT_PRECISION);

    CU_ASSERT(plc->old->ni == 8);
    CU_ASSERT(plc->old->di[63].I == 0);
    CU_ASSERT(plc->old->inputs[7] == 0);
   
    CU_ASSERT(plc->old->nq == 8);
    CU_ASSERT(plc->old->dq[63].Q == 0);
    CU_ASSERT(plc->old->outputs[7] == 0);
   
    CU_ASSERT(plc->old->nai == 4);
    CU_ASSERT(plc->old->real_in[3] == 0);
    CU_ASSERT(plc->old->ai[3].V < FLOAT_PRECISION);
    
    CU_ASSERT(plc->old->naq == 4);
    CU_ASSERT(plc->old->real_out[3] == 0);
    CU_ASSERT(plc->old->aq[3].V < FLOAT_PRECISION);
    
    CU_ASSERT(plc->old->nt == 4);
    CU_ASSERT(plc->old->t[3].Q == 0);
    
    CU_ASSERT(plc->old->ns == 4);
    CU_ASSERT(plc->old->s[3].Q == 0);
    
    CU_ASSERT(plc->old->nm == 4);
    CU_ASSERT(plc->old->m[3].V == 0);
    
    CU_ASSERT(plc->old->nmr == 4);
    CU_ASSERT(plc->old->mr[3].V < FLOAT_PRECISION);

    CU_ASSERT(plc->step == 100);
    CU_ASSERT(plc->command == 0);
    CU_ASSERT(plc->status == ST_STOPPED);	
    
    clear_plc(plc);
}

void ut_config(){
    plc_t plc = new_plc(8,8,4,4,4,4,4,4,100,NULL);

    plc = declare_variable(plc, 0, 99, "input_1");
    CU_ASSERT(plc->status == ERR_BADOPERAND);

    plc->status = PLC_OK;
    plc = declare_variable(plc, OP_INPUT, 99, "input_1");
    CU_ASSERT(plc->status == ERR_BADINDEX);

    plc->status = PLC_OK;
    plc = declare_variable(plc, OP_INPUT, 1, "input_1");
    CU_ASSERT_STRING_EQUAL(plc->di[1].nick, "input_1");
    CU_ASSERT(plc->status == PLC_OK);
    
    plc = declare_variable(plc, OP_OUTPUT, 2, "output_1");
    CU_ASSERT_STRING_EQUAL(plc->dq[2].nick, "output_1");
    
    plc = declare_variable(plc, OP_REAL_INPUT, 5, "input_1");
    CU_ASSERT(plc->status == ERR_BADINDEX);

    plc->status = PLC_OK;

    plc = declare_variable(plc, OP_REAL_OUTPUT, 2, "output_1");
    CU_ASSERT_STRING_EQUAL(plc->aq[2].nick, "output_1");

/*******************************************************************/

    plc = configure_io_limit(plc, 0, 99, "0.0", TRUE);
    CU_ASSERT(plc->status == ERR_BADOPERAND);

    plc->status = PLC_OK;
    plc = configure_io_limit(plc, OP_REAL_OUTPUT, 99, "", FALSE);
    CU_ASSERT(plc->status == ERR_BADINDEX);

    plc->status = PLC_OK;
    plc = configure_io_limit(plc, OP_REAL_INPUT, 1, "-10.0", FALSE);
    CU_ASSERT_DOUBLE_EQUAL(plc->ai[1].min, -10.0, FLOAT_PRECISION);
    CU_ASSERT(plc->status == PLC_OK);
    
/*******************************************************************/    
    
    plc = init_variable(plc, 0, 99, "0.0");
    CU_ASSERT(plc->status == ERR_BADOPERAND);

    plc->status = PLC_OK;
    plc = init_variable(plc, OP_MEMORY, 99, "");
    CU_ASSERT(plc->status == ERR_BADINDEX);

    plc->status = PLC_OK;
    plc = init_variable(plc, OP_REAL_MEMORY, 1, "-10.0");
    CU_ASSERT_DOUBLE_EQUAL(plc->mr[1].V, -10.0, FLOAT_PRECISION);
    CU_ASSERT(plc->status == PLC_OK); 
    
/*******************************************************************/    
    
    plc = configure_variable_readonly(plc, 0, 99, "FALSE");
    CU_ASSERT(plc->status == ERR_BADOPERAND);

    plc = configure_variable_readonly(plc, OP_MEMORY, 99, "TRUE");
    CU_ASSERT(plc->status == ERR_BADINDEX);

    plc->status = PLC_OK;
    plc = configure_variable_readonly(plc, OP_REAL_MEMORY, 1, "TRUE");
    CU_ASSERT(plc->mr[1].RO == TRUE);
    CU_ASSERT(plc->status == PLC_OK);  

/*******************************************************************/

    plc = configure_counter_direction(plc, 99, "UP");
    CU_ASSERT(plc->status == ERR_BADINDEX);

    plc->status = PLC_OK;
    plc = configure_counter_direction(plc, 1, "DOWN");
    CU_ASSERT(plc->m[1].DOWN == TRUE);
    CU_ASSERT(plc->status == PLC_OK); 

/*******************************************************************/

    plc = configure_timer_scale(plc, 99, "105");
    CU_ASSERT(plc->status == ERR_BADINDEX);

    plc->status = PLC_OK;
    plc = configure_timer_scale(plc, 1, "105");
    CU_ASSERT(plc->t[1].S == 105);
    CU_ASSERT(plc->status == PLC_OK); 

/*******************************************************************/
    
    plc = configure_timer_preset(plc, 99, "105");
    CU_ASSERT(plc->status == ERR_BADINDEX);

    plc->status = PLC_OK;
    plc = configure_timer_preset(plc, 1, "105");
    CU_ASSERT(plc->t[1].P == 105);
    CU_ASSERT(plc->status == PLC_OK);     

/*******************************************************************/

    plc = configure_timer_delay_mode(plc, 99, "OFF");
    CU_ASSERT(plc->status == ERR_BADINDEX);

    plc->status = PLC_OK;
    plc = configure_timer_delay_mode(plc, 1, "ON");
    CU_ASSERT(plc->t[1].ONDELAY == TRUE);
    CU_ASSERT(plc->status == PLC_OK); 
   
/******************************************************************/  
   
    plc = configure_pulse_scale(plc, 99, "105");
    CU_ASSERT(plc->status == ERR_BADINDEX);

    plc->status = PLC_OK;
    plc = configure_pulse_scale(plc, 1, "105");
    CU_ASSERT(plc->s[1].S == 105);
    CU_ASSERT(plc->status == PLC_OK);    

    clear_plc(plc);
}

int stub_enable_fails();
int stub_enable();

void ut_start_stop()
{
     extern unsigned char Mock_din;   
     extern unsigned char Mock_dout;
//degenerates
     plc_t r = plc_start(NULL);
     CU_ASSERT_PTR_NULL(r);
     plc_t plc = new_plc(8,8,4,4,4,4,4,4,100,NULL);
     r = plc_start(plc);
     CU_ASSERT(r->status == ERR_HARDWARE);
     
     r = plc_stop(NULL);
     CU_ASSERT_PTR_NULL(r);
     
     r = plc_stop(plc);
     CU_ASSERT(r->status == ERR_HARDWARE);
     
     //hardware is not configured correctly
     plc->hw = get_hardware(HW_SIM);
     plc->hw->status = PLC_ERR;

     r = plc_start(plc);
     CU_ASSERT(plc->status == ERR_HARDWARE);
     
     //stop should have no effect
     r = plc_stop(plc);        
     CU_ASSERT(plc->status == ERR_HARDWARE);
     
     //hardware is configured correctly but fails to start
     plc->hw->enable = stub_enable_fails;
     plc->hw->status = PLC_OK;
     plc->status = ST_STOPPED;
     
     //stop should have no effect
     r = plc_stop(plc);        
     CU_ASSERT(plc->status == ST_STOPPED);
     
     r = plc_start(plc);
     
     CU_ASSERT(plc->status == ERR_HARDWARE);
     plc->hw->enable = stub_enable;
     
     //status other than ST_STOPPED
     plc->status = ST_RUNNING;
        
     r = plc_start(plc);
     
     CU_ASSERT(plc->status == ST_RUNNING);
     CU_ASSERT(plc->update == 0);
     
     //finally all is well
     plc->status = ST_STOPPED;
     r = plc_start(plc);
      
    //this should reset inputs
     CU_ASSERT(Mock_din == 0);    
     CU_ASSERT(plc->status == ST_RUNNING);
     CU_ASSERT(plc->update == CHANGED_STATUS);
     
     plc->update = 0;
     r = plc_stop(plc);
     CU_ASSERT(plc->status == ST_STOPPED);
     CU_ASSERT(plc->update == CHANGED_STATUS);
     
     //should reset outputs
     CU_ASSERT(Mock_dout == 0); 
      
     clear_plc(plc);
}
#endif //_UT_INIT_H_

