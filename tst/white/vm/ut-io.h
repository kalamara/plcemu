#ifndef _UT_IO_
#define _UT_IO_
void ut_read(){

    extern unsigned char Mock_din;
    
    //nulls shouldn't crash
    read_inputs(NULL);
    struct PLC_regs p;
    memset(&p,0,sizeof(struct PLC_regs));
    read_inputs(&p);
    init_mock_plc(&p);
    //mock fetch makes all digitals 1s, and all analogs magic
    //so magic numbers should appear in input regs
    read_inputs(&p);
    CU_ASSERT(p.inputs[p.ni - 1] == 0xFF);
    CU_ASSERT(p.real_in[p.nai - 1] == 0xABCDEF01);
    
    plc_start(NULL);
    plc_start(&p);
    //this should reset inputs
    CU_ASSERT(Mock_din == 0);    
    
    
}

void ut_write(){
    extern int Mock_flush_count;
    extern unsigned char Mock_dout;
    extern uint64_t Mock_aout;
    //nulls shouldn't crash
    write_outputs(NULL);
    struct PLC_regs p;
    memset(&p,0,sizeof(struct PLC_regs));
    write_outputs(&p);
    init_mock_plc(&p);
    unsigned char out[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    memcpy(p.outputs, out, 8);
    
    uint64_t rout[2] = {
        0xABCDEF01,
        0xABCDEF01
    };
    memcpy(p.real_out, rout, 16);
    //write outputs copies magic numbers to hardware
    //mock flush also should be called
    write_outputs(&p);
    CU_ASSERT(Mock_dout == 0xFF);
    CU_ASSERT(Mock_aout == 0xABCDEF01);
    CU_ASSERT(Mock_flush_count == 1);
    
    plc_stop(NULL);
    plc_stop(&p);
    //this should reset outputs
    CU_ASSERT(Mock_flush_count == 0);
}

#endif //_UT_IO_
