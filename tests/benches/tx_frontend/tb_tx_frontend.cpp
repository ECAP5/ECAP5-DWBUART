/*           __        _
 *  ________/ /  ___ _(_)__  ___
 * / __/ __/ _ \/ _ `/ / _ \/ -_)
 * \__/\__/_//_/\_,_/_/_//_/\__/
 * 
 * Copyright (C) Clément Chaine
 * This file is part of ECAP5-DWBUART <https://github.com/ecap5/ECAP5-DWBUART>
 *
 * ECAP5-DWBUART is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ECAP5-DWBUART is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ECAP5-DWBUART.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include <svdpi.h>

#include "Vtb_tx_frontend.h"
#include "Vtb_tx_frontend_tx_frontend.h"
#include "Vtb_tx_frontend_tb_tx_frontend.h"
#include "testbench.h"

enum CondId {
  COND_state,
  COND_output,
  COND_done,
  COND_baudrate,
  __CondIdEnd
};

enum TestcaseId {
  T_IDLE     = 1,
  T_7N1      = 2,
  T_7N2      = 3,
  T_7E1      = 4,
  T_7E2      = 5,
  T_7O1      = 6,
  T_7O2      = 7,
  T_8N1      = 8,
  T_8N2      = 9,
  T_8E1      = 10,
  T_8E2      = 11,
  T_8O1      = 12,
  T_8O2      = 13,
  T_BAUDRATE = 14
};

enum StateId {
  S_IDLE   = 0,
  S_START  = 1,
  S_DATA   = 2,
  S_PARITY = 3,
  S_STOP   = 4
};

class TB_Tx_frontend : public Testbench<Vtb_tx_frontend> {
public:
  void reset() {
    this->_nop();

    this->core->rst_i = 1;
    for(int i = 0; i < 5; i++) {
      this->tick();
    }
    this->core->rst_i = 0;

    Testbench<Vtb_tx_frontend>::reset();
  }
  
  void _nop() {
    core->cr_acc_incr_i = 0;
    core->cr_ds_i = 0;
    core->cr_p_i = 0;
    core->cr_s_i = 0;

    core->transmit_i = 0;
    core->dr_i = 0;
  }

  float get_generated_baudrate(uint32_t baudrate) {
    this->reset();

    core->cr_acc_incr_i = ((double)baudrate * (1 << 16)) / 24000000;
    core->cr_ds_i = 1;
    core->cr_p_i = 1;
    core->cr_s_i = 1;

    core->transmit_i = 1;
    core->dr_i = 0x7A;

    // Wait for the tx signal to go down
    while(core->uart_tx_o == 1) {
      this->tick();
    }

    // Initializes the clock counter
    uint32_t counter = 0;

    // Wait for the end of the transfer
    while(core->done_o != 1) {
      counter += 1; 

      this->tick();
    }

    // Compute the baudrate
    float duration = (counter / 24000000.0);
    float baud_period = duration / 12;
    float generated_baudrate = (1.0 / baud_period);
    return generated_baudrate;
  }
};

void tb_tx_frontend_idle(TB_Tx_frontend * tb) {
  Vtb_tx_frontend * core = tb->core;
  core->testcase = T_IDLE;

  // The following actions are performed in this test :
  //    tick 0. Nothing (core is in IDLE)
  //    tick 1. Nothing (core is in IDLE)
  //    tick 2. Nothing (core is in IDLE)
  //    tick 3. Nothing (core is in IDLE)

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));
  
  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (2)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (3)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_tx_frontend.idle.01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_tx_frontend.idle.02",
      tb->conditions[COND_output],
      "Failed to implement the output signal", tb->err_cycles[COND_output]);

  CHECK("tb_tx_frontend.idle.03",
      tb->conditions[COND_done],
      "Failed to implement the done signal", tb->err_cycles[COND_done]);
}

void tb_tx_frontend_7N1(TB_Tx_frontend * tb) {
  Vtb_tx_frontend * core = tb->core;
  core->testcase = T_7N1;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  // Baudrate = 2500000 -> 9.6 cycles d'horloge
  core->cr_acc_incr_i = 6827;
  core->cr_ds_i = 0;
  core->cr_p_i = 0;
  core->cr_s_i = 0;

  core->transmit_i = 1;
  core->dr_i = 0x3A;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_START));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->transmit_i = 0;

  //=================================
  //      Tick (2-10)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_START));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (11-20)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (21-29)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (30-39)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (40-49)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (50-58)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (59-68)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (69-77)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (78-87)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (88)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 1));

  //=================================
  //      Tick (89)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 0));

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_tx_frontend.7N1.01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_tx_frontend.7N1.02",
      tb->conditions[COND_output],
      "Failed to implement the output signal", tb->err_cycles[COND_output]);

  CHECK("tb_tx_frontend.7N1.03",
      tb->conditions[COND_done],
      "Failed to implement the done signal", tb->err_cycles[COND_done]);
}

void tb_tx_frontend_7N2(TB_Tx_frontend * tb) {
  Vtb_tx_frontend * core = tb->core;
  core->testcase = T_7N2;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  // Baudrate = 2500000 -> 9.6 cycles d'horloge
  core->cr_acc_incr_i = 6827;
  core->cr_ds_i = 0;
  core->cr_p_i = 0;
  core->cr_s_i = 1;

  core->transmit_i = 1;
  core->dr_i = 0x3A;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_START));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->transmit_i = 0;

  //=================================
  //      Tick (2-10)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_START));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (11-20)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (21-29)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (30-39)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (40-49)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (50-58)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (59-68)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (69-77)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (78-87)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (88-96)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (97)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 1));

  //=================================
  //      Tick (98)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 0));

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_tx_frontend.7N2.01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_tx_frontend.7N2.02",
      tb->conditions[COND_output],
      "Failed to implement the output signal", tb->err_cycles[COND_output]);

  CHECK("tb_tx_frontend.7N2.03",
      tb->conditions[COND_done],
      "Failed to implement the done signal", tb->err_cycles[COND_done]);
}

void tb_tx_frontend_7E1(TB_Tx_frontend * tb) {
  Vtb_tx_frontend * core = tb->core;
  core->testcase = T_7E1;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  // Baudrate = 2500000 -> 9.6 cycles d'horloge
  core->cr_acc_incr_i = 6827;
  core->cr_ds_i = 0;
  core->cr_p_i = 2;
  core->cr_s_i = 0;

  core->transmit_i = 1;
  core->dr_i = 0x3A;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_START));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->transmit_i = 0;

  //=================================
  //      Tick (2-10)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_START));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (11-20)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (21-29)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (30-39)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (40-49)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (50-58)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (59-68)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (69-77)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (78-87)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_PARITY));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (88-96)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (97)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 1));

  //=================================
  //      Tick (98)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 0));


  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_tx_frontend.7E1.01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_tx_frontend.7E1.02",
      tb->conditions[COND_output],
      "Failed to implement the output signal", tb->err_cycles[COND_output]);

  CHECK("tb_tx_frontend.7E1.03",
      tb->conditions[COND_done],
      "Failed to implement the done signal", tb->err_cycles[COND_done]);
  
}

void tb_tx_frontend_7E2(TB_Tx_frontend * tb) {
  Vtb_tx_frontend * core = tb->core;
  core->testcase = T_7E2;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  // Baudrate = 2500000 -> 9.6 cycles d'horloge
  core->cr_acc_incr_i = 6827;
  core->cr_ds_i = 0;
  core->cr_p_i = 2;
  core->cr_s_i = 1;

  core->transmit_i = 1;
  core->dr_i = 0x3A;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_START));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->transmit_i = 0;

  //=================================
  //      Tick (2-10)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_START));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (11-20)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (21-29)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (30-39)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (40-49)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (50-58)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (59-68)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (69-77)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (78-87)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_PARITY));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (88-96)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (97-106)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (107)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 1));

  //=================================
  //      Tick (108)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 0));

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_tx_frontend.7E2.01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_tx_frontend.7E2.02",
      tb->conditions[COND_output],
      "Failed to implement the output signal", tb->err_cycles[COND_output]);

  CHECK("tb_tx_frontend.7E2.03",
      tb->conditions[COND_done],
      "Failed to implement the done signal", tb->err_cycles[COND_done]);
}

void tb_tx_frontend_7O1(TB_Tx_frontend * tb) {
  Vtb_tx_frontend * core = tb->core;
  core->testcase = T_7O1;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  // Baudrate = 2500000 -> 9.6 cycles d'horloge
  core->cr_acc_incr_i = 6827;
  core->cr_ds_i = 0;
  core->cr_p_i = 1;
  core->cr_s_i = 0;

  core->transmit_i = 1;
  core->dr_i = 0x3A;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_START));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->transmit_i = 0;

  //=================================
  //      Tick (2-10)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_START));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (11-20)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (21-29)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (30-39)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (40-49)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (50-58)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (59-68)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (69-77)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (78-87)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_PARITY));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (88-96)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (97)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 1));

  //=================================
  //      Tick (98)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 0));

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_tx_frontend.7O1.01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_tx_frontend.7O1.02",
      tb->conditions[COND_output],
      "Failed to implement the output signal", tb->err_cycles[COND_output]);

  CHECK("tb_tx_frontend.7O1.03",
      tb->conditions[COND_done],
      "Failed to implement the done signal", tb->err_cycles[COND_done]);
}

void tb_tx_frontend_7O2(TB_Tx_frontend * tb) {
  Vtb_tx_frontend * core = tb->core;
  core->testcase = T_7O2;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  // Baudrate = 2500000 -> 9.6 cycles d'horloge
  core->cr_acc_incr_i = 6827;
  core->cr_ds_i = 0;
  core->cr_p_i = 1;
  core->cr_s_i = 1;

  core->transmit_i = 1;
  core->dr_i = 0x3A;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_START));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->transmit_i = 0;

  //=================================
  //      Tick (2-10)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_START));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (11-20)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (21-29)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (30-39)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (40-49)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (50-58)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (59-68)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (69-77)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (78-87)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_PARITY));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (88-96)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (97-106)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (107)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 1));

  //=================================
  //      Tick (108)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 0));

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_tx_frontend.7O2.01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_tx_frontend.7O2.02",
      tb->conditions[COND_output],
      "Failed to implement the output signal", tb->err_cycles[COND_output]);

  CHECK("tb_tx_frontend.7O2.03",
      tb->conditions[COND_done],
      "Failed to implement the done signal", tb->err_cycles[COND_done]);
}

void tb_tx_frontend_8N1(TB_Tx_frontend * tb) {
  Vtb_tx_frontend * core = tb->core;
  core->testcase = T_8N1;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  // Baudrate = 2500000 -> 9.6 cycles d'horloge
  core->cr_acc_incr_i = 6827;
  core->cr_ds_i = 1;
  core->cr_p_i = 0;
  core->cr_s_i = 0;

  core->transmit_i = 1;
  core->dr_i = 0x7A;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_START));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->transmit_i = 0;

  //=================================
  //      Tick (2-10)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_START));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (11-20)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (21-29)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (30-39)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (40-49)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (50-58)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (59-68)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (69-77)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (78-87)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (88-96)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (97)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 1));

  //=================================
  //      Tick (98)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 0));

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_tx_frontend.8N1.01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_tx_frontend.8N1.02",
      tb->conditions[COND_output],
      "Failed to implement the output signal", tb->err_cycles[COND_output]);

  CHECK("tb_tx_frontend.8N1.03",
      tb->conditions[COND_done],
      "Failed to implement the done signal", tb->err_cycles[COND_done]);
}

void tb_tx_frontend_8N2(TB_Tx_frontend * tb) {
  Vtb_tx_frontend * core = tb->core;
  core->testcase = T_8N2;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  // Baudrate = 2500000 -> 9.6 cycles d'horloge
  core->cr_acc_incr_i = 6827;
  core->cr_ds_i = 1;
  core->cr_p_i = 0;
  core->cr_s_i = 1;

  core->transmit_i = 1;
  core->dr_i = 0x7A;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_START));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->transmit_i = 0;

  //=================================
  //      Tick (2-10)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_START));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (11-20)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (21-29)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (30-39)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (40-49)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (50-58)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (59-68)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (69-77)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (78-87)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (88-96)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (97-106)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (107)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 1));

  //=================================
  //      Tick (108)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 0));

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_tx_frontend.8N2.01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_tx_frontend.8N2.02",
      tb->conditions[COND_output],
      "Failed to implement the output signal", tb->err_cycles[COND_output]);

  CHECK("tb_tx_frontend.8N2.03",
      tb->conditions[COND_done],
      "Failed to implement the done signal", tb->err_cycles[COND_done]);
}

void tb_tx_frontend_8E1(TB_Tx_frontend * tb) {
  Vtb_tx_frontend * core = tb->core;
  core->testcase = T_8E1;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  // Baudrate = 2500000 -> 9.6 cycles d'horloge
  core->cr_acc_incr_i = 6827;
  core->cr_ds_i = 1;
  core->cr_p_i = 2;
  core->cr_s_i = 0;

  core->transmit_i = 1;
  core->dr_i = 0x7A;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_START));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->transmit_i = 0;

  //=================================
  //      Tick (2-10)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_START));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (11-20)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (21-29)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (30-39)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (40-49)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (50-58)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (59-68)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (69-77)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (78-87)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (88-96)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_PARITY));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (97-106)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (107)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 1));

  //=================================
  //      Tick (108)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 0));


  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_tx_frontend.8E1.01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_tx_frontend.8E1.02",
      tb->conditions[COND_output],
      "Failed to implement the output signal", tb->err_cycles[COND_output]);

  CHECK("tb_tx_frontend.8E1.03",
      tb->conditions[COND_done],
      "Failed to implement the done signal", tb->err_cycles[COND_done]);
}

void tb_tx_frontend_8E2(TB_Tx_frontend * tb) {
  Vtb_tx_frontend * core = tb->core;
  core->testcase = T_8E2;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  // Baudrate = 2500000 -> 9.6 cycles d'horloge
  core->cr_acc_incr_i = 6827;
  core->cr_ds_i = 1;
  core->cr_p_i = 2;
  core->cr_s_i = 1;

  core->transmit_i = 1;
  core->dr_i = 0x7A;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_START));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->transmit_i = 0;

  //=================================
  //      Tick (2-10)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_START));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (11-20)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (21-29)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (30-39)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (40-49)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (50-58)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (59-68)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (69-77)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (78-87)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (88-96)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_PARITY));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (97-106)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (107-116)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (117)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 1));

  //=================================
  //      Tick (118)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 0));

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_tx_frontend.8E2.01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_tx_frontend.8E2.02",
      tb->conditions[COND_output],
      "Failed to implement the output signal", tb->err_cycles[COND_output]);

  CHECK("tb_tx_frontend.8E2.03",
      tb->conditions[COND_done],
      "Failed to implement the done signal", tb->err_cycles[COND_done]);
}

void tb_tx_frontend_8O1(TB_Tx_frontend * tb) {
  Vtb_tx_frontend * core = tb->core;
  core->testcase = T_8O1;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  // Baudrate = 2500000 -> 9.6 cycles d'horloge
  core->cr_acc_incr_i = 6827;
  core->cr_ds_i = 1;
  core->cr_p_i = 1;
  core->cr_s_i = 0;

  core->transmit_i = 1;
  core->dr_i = 0x7A;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_START));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->transmit_i = 0;

  //=================================
  //      Tick (2-10)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_START));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (11-20)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (21-29)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (30-39)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (40-49)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (50-58)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (59-68)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (69-77)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (78-87)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (88-96)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_PARITY));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (97-106)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (107)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 1));

  //=================================
  //      Tick (108)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 0));

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_tx_frontend.8O1.01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_tx_frontend.8O1.02",
      tb->conditions[COND_output],
      "Failed to implement the output signal", tb->err_cycles[COND_output]);

  CHECK("tb_tx_frontend.8O1.03",
      tb->conditions[COND_done],
      "Failed to implement the done signal", tb->err_cycles[COND_done]);
}

void tb_tx_frontend_8O2(TB_Tx_frontend * tb) {
  Vtb_tx_frontend * core = tb->core;
  core->testcase = T_8O2;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  // Baudrate = 2500000 -> 9.6 cycles d'horloge
  core->cr_acc_incr_i = 6827;
  core->cr_ds_i = 1;
  core->cr_p_i = 1;
  core->cr_s_i = 1;

  core->transmit_i = 1;
  core->dr_i = 0x7A;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_START));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->transmit_i = 0;

  //=================================
  //      Tick (2-10)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_START));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (11-20)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (21-29)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (30-39)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (40-49)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (50-58)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (59-68)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (69-77)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (78-87)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (88-96)
  
  for(size_t i = 0; i < 9; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_PARITY));
    tb->check(COND_output, (core->uart_tx_o == 0));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (97-106)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (107-116)
  
  for(size_t i = 0; i < 10; i++) {
    tb->tick();

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
    tb->check(COND_output, (core->uart_tx_o == 1));
    tb->check(COND_done,   (core->done_o == 0));
  }

  //=================================
  //      Tick (117)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 1));

  //=================================
  //      Tick (118)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 0));

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_tx_frontend.8O2.01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_tx_frontend.8O2.02",
      tb->conditions[COND_output],
      "Failed to implement the output signal", tb->err_cycles[COND_output]);

  CHECK("tb_tx_frontend.8O2.03",
      tb->conditions[COND_done],
      "Failed to implement the done signal", tb->err_cycles[COND_done]);
}

void tb_tx_frontend_baudrate(TB_Tx_frontend * tb) {
  Vtb_tx_frontend * core = tb->core;
  core->testcase = T_BAUDRATE;

  uint32_t baudrates[] = {
    4800,
    9600,
    19200,
    38400,
    57600,
    115200,
    230400,
    460800,
    921600,
    1000000,
    2000000,
    3000000
  };
  size_t num_baudrates = sizeof(baudrates)/sizeof(uint32_t);

  for(size_t i = 0; i < num_baudrates; i++) {
    float generated_baudrate = tb->get_generated_baudrate(baudrates[i]);
    float precision = (1.0 - (generated_baudrate / baudrates[i]));

    //`````````````````````````````````
    //      Checks 
    
    tb->check(COND_baudrate,  (precision < 0.02));
  }

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_tx_frontend.baudrate.01",
      tb->conditions[COND_baudrate],
      "Failed to comply with the baudrate precision", tb->err_cycles[COND_baudrate]);
}

int main(int argc, char ** argv, char ** env) {
  srand(time(NULL));
  Verilated::traceEverOn(true);

  bool verbose = parse_verbose(argc, argv);

  TB_Tx_frontend * tb = new TB_Tx_frontend;
  tb->open_trace("waves/tx_frontend.vcd");
  tb->open_testdata("testdata/tx_frontend.csv");
  tb->set_debug_log(verbose);
  tb->init_conditions(__CondIdEnd);

  // 24MHz
  tb->clk_period_in_ps = 41667;

  /************************************************************/

  tb_tx_frontend_idle(tb);

  tb_tx_frontend_7N1(tb);
  tb_tx_frontend_7N2(tb);
  tb_tx_frontend_7E1(tb);
  tb_tx_frontend_7E2(tb);
  tb_tx_frontend_7O1(tb);
  tb_tx_frontend_7O2(tb);
  tb_tx_frontend_8N1(tb);
  tb_tx_frontend_8N2(tb);
  tb_tx_frontend_8E1(tb);
  tb_tx_frontend_8E2(tb);
  tb_tx_frontend_8O1(tb);
  tb_tx_frontend_8O2(tb);

  tb_tx_frontend_baudrate(tb);

  /************************************************************/

  printf("[TX_FRONTEND]: ");
  if(tb->success) {
    printf("Done\n");
  } else {
    printf("Failed\n");
  }

  delete tb;
  exit(EXIT_SUCCESS);
}
