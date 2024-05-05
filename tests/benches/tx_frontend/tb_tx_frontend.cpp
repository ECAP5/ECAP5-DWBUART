/*           __        _
 *  ________/ /  ___ _(_)__  ___
 * / __/ __/ _ \/ _ `/ / _ \/ -_)
 * \__/\__/_//_/\_,_/_/_//_/\__/
 * 
 * Copyright (C) Clément Chaine
 * This file is part of ECAP5-WBUART <https://github.com/ecap5/ECAP5-WBUART>
 *
 * ECAP5-WBUART is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ECAP5-WBUART is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ECAP5-WBUART.  If not, see <http://www.gnu.org/licenses/>.
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
    core->cr_clk_div_i = 0;
    core->cr_ds_i = 0;
    core->cr_p_i = 0;
    core->cr_s_i = 0;

    core->transmit_i = 0;
    core->dr_i = 0;
  }

  void n_tick(int n) {
    for(int i = 0; i < n; i++) {
      this->tick();
    }
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
  
  core->cr_clk_div_i = 4;
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
  //      Tick (2-5)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (6-9)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (10-13)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (14-17)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (18-21)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (22-25)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (26-29)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (30-33)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (34-37)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 1));

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
  
  core->cr_clk_div_i = 4;
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
  //      Tick (2-5)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (6-9)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (10-13)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (14-17)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (18-21)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (22-25)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (26-29)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (30-33)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (34-37)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (38-41)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 1));

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
  
  core->cr_clk_div_i = 4;
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
  //      Tick (2-5)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (6-9)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (10-13)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (14-17)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (18-21)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (22-25)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (26-29)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (30-33)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_PARITY));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (34-37)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (38-41)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 1));

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
  core->testcase = T_7E1;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  core->cr_clk_div_i = 4;
  core->cr_ds_i = 0;
  core->cr_p_i = 2;
  core->cr_s_i = 0;

  core->transmit_i = 1;
  core->dr_i = 0x4A;

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
  //      Tick (2-5)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (6-9)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (10-13)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (14-17)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (18-21)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (22-25)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (26-29)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (30-33)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_PARITY));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (34-37)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (38-41)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 1));

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
  
  core->cr_clk_div_i = 4;
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
  //      Tick (2-5)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (6-9)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (10-13)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (14-17)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (18-21)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (22-25)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (26-29)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (30-33)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_PARITY));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (34-37)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (38-41)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 1));

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
  
  core->cr_clk_div_i = 4;
  core->cr_ds_i = 0;
  core->cr_p_i = 1;
  core->cr_s_i = 0;

  core->transmit_i = 1;
  core->dr_i = 0x4A;

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
  //      Tick (2-5)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (6-9)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (10-13)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (14-17)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (18-21)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (22-25)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (26-29)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (30-33)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_PARITY));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (34-37)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (38-41)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 1));

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
  
  core->cr_clk_div_i = 4;
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
  //      Tick (2-5)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (6-9)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (10-13)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (14-17)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (18-21)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (22-25)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (26-29)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (30-33)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (34-37)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (38-41)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 1));

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
  
  core->cr_clk_div_i = 4;
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
  //      Tick (2-5)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (6-9)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (10-13)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (14-17)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (18-21)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (22-25)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (26-29)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (30-33)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (34-37)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (38-41)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (42-45)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 1));

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
  
  core->cr_clk_div_i = 4;
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
  //      Tick (2-5)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (6-9)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (10-13)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (14-17)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (18-21)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (22-25)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (26-29)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (30-33)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (34-37)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_PARITY));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (38-41)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (42-45)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 1));

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
  
  core->cr_clk_div_i = 4;
  core->cr_ds_i = 1;
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
  //      Tick (2-5)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (6-9)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (10-13)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (14-17)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (18-21)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (22-25)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (26-29)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (30-33)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (34-37)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_PARITY));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (38-41)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (42-45)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (46-49)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 1));

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
  
  core->cr_clk_div_i = 4;
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
  //      Tick (2-5)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (6-9)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (10-13)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (14-17)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (18-21)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (22-25)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (26-29)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (30-33)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (34-37)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_PARITY));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (38-41)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (42-45)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 1));

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
  
  core->cr_clk_div_i = 4;
  core->cr_ds_i = 1;
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
  //      Tick (2-5)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (6-9)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (10-13)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (14-17)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (18-21)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (22-25)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (26-29)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (30-33)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (34-37)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_PARITY));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (38-41)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (42-45)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (46-49)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 1));

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

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  core->cr_clk_div_i = 100;
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
  //      Tick (2-5)
  
  tb->n_tick(100);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (6-9)
  
  tb->n_tick(100);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (10-13)
  
  tb->n_tick(100);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (14-17)
  
  tb->n_tick(100);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (18-21)
  
  tb->n_tick(100);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (22-25)
  
  tb->n_tick(100);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (26-29)
  
  tb->n_tick(100);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_DATA));
  tb->check(COND_output, (core->uart_tx_o == 0));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (30-33)
  
  tb->n_tick(100);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_STOP));
  tb->check(COND_output, (core->uart_tx_o == 1));
  tb->check(COND_done,   (core->done_o == 0));

  //=================================
  //      Tick (34-37)
  
  tb->n_tick(100);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state,  (core->tb_tx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_done,   (core->done_o == 1));

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_tx_frontend.baudrate.01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_tx_frontend.baudrate.02",
      tb->conditions[COND_output],
      "Failed to implement the output signal", tb->err_cycles[COND_output]);

  CHECK("tb_tx_frontend.baudrate.03",
      tb->conditions[COND_done],
      "Failed to implement the done signal", tb->err_cycles[COND_done]);
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
