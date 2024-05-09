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

#include "Vtb_rx_frontend.h"
#include "Vtb_rx_frontend_rx_frontend.h"
#include "Vtb_rx_frontend_tb_rx_frontend.h"
#include "testbench.h"

enum CondId {
  COND_state,
  COND_frame,
  COND_valid,
  COND_errors,
  __CondIdEnd
};

enum TestcaseId {
  T_IDLE        = 1,
  T_VALID_7N1   = 2,
  T_VALID_7N2   = 3,
  T_VALID_7E1   = 4,
  T_VALID_7E2   = 5,
  T_VALID_7O1   = 6,
  T_VALID_7O2   = 7,
  T_VALID_8N1   = 8,
  T_VALID_8N2   = 9,
  T_VALID_8E1   = 10,
  T_VALID_8E2   = 11,
  T_VALID_8O1   = 12,
  T_VALID_8O2   = 13,
  T_BAUDRATE    = 14,
  T_PARITY_EVEN = 15,
  T_PARITY_ODD  = 16,
  T_FRAMING     = 17
};

enum StateId {
  S_IDLE = 0,
  S_START,
  S_DATA,
  S_STOP
};

class TB_Rx_frontend : public Testbench<Vtb_rx_frontend> {
public:
  void reset() {
    this->_nop();

    this->core->rst_i = 1;
    for(int i = 0; i < 5; i++) {
      this->tick();
    }
    this->core->rst_i = 0;

    Testbench<Vtb_rx_frontend>::reset();
  }
  
  void _nop() {
    core->uart_rx_i = 1;
    core->cr_clk_div_i = 0;
    core->cr_ds_i = 0;
    core->cr_p_i = 0;
    core->cr_s_i = 0;
  }

  void n_tick(int n) {
    for(int i = 0; i < n; i++) {
      this->tick();
    }
  }
};

void tb_rx_frontend_idle(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
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
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_valid, (core->output_valid_o == 0));
  
  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (2)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (3)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_rx_frontend.idle.01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_rx_frontend.idle.02",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

void tb_rx_frontend_valid_frame_7N1(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
  core->testcase = T_VALID_7N1;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  core->cr_clk_div_i = 4;
  core->cr_ds_i = 0;
  core->cr_p_i = 0;
  core->cr_s_i = 0;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (2-3)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (4-5)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_START));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (6-7)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (8-9)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (10-13)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (14-17)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (18-21)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (22-25)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (26-29)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (30-33)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (34-37)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (38)
  
  tb->n_tick(1);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_frame,  (core->frame_o == 0b10100101));
  tb->check(COND_errors, (core->parity_err_o == 0) &&
                         (core->frame_err_o == 0));
  tb->check(COND_valid, (core->output_valid_o == 1));

  //=================================
  //      Tick (39-41)
  
  tb->n_tick(3);

  //`````````````````````````````````
  //      Checks 
  
  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_rx_frontend.valid.7N1_01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_rx_frontend.valid.7N1_02",
      tb->conditions[COND_frame],
      "Failed to implement the frame output", tb->err_cycles[COND_frame]);

  CHECK("tb_rx_frontend.valid.7N1_03",
      tb->conditions[COND_errors],
      "Failed to implement the errors computation", tb->err_cycles[COND_errors]);

  CHECK("tb_rx_frontend.valid.7N1_04",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

void tb_rx_frontend_valid_frame_7N2(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
  core->testcase = T_VALID_7N2;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  core->cr_clk_div_i = 4;
  core->cr_ds_i = 0;
  core->cr_p_i = 0;
  core->cr_s_i = 1;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (2-3)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (4-5)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_START));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (6-7)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (8-9)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (10-13)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (14-17)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (18-21)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (22-25)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (26-29)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (30-33)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (34-37)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (38-41)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (42)
  
  tb->n_tick(1);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_frame,  (core->frame_o == 0b110100101));
  tb->check(COND_errors, (core->parity_err_o == 0) &&
                         (core->frame_err_o == 0));
  tb->check(COND_valid, (core->output_valid_o == 1));

  //=================================
  //      Tick (43-45)
  
  tb->n_tick(3);

  //`````````````````````````````````
  //      Checks 
  
  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_rx_frontend.valid.7N2_01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_rx_frontend.valid.7N2_02",
      tb->conditions[COND_frame],
      "Failed to implement the frame output", tb->err_cycles[COND_frame]);

  CHECK("tb_rx_frontend.valid.7N2_03",
      tb->conditions[COND_errors],
      "Failed to implement the errors computation", tb->err_cycles[COND_errors]);

  CHECK("tb_rx_frontend.valid.7N2_04",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

void tb_rx_frontend_valid_frame_7E1(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
  core->testcase = T_VALID_7E1;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  core->cr_clk_div_i = 4;
  core->cr_ds_i = 0;
  core->cr_p_i = 2;
  core->cr_s_i = 0;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (2-3)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (4-5)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_START));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (6-7)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (8-9)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (10-13)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (14-17)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (18-21)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (22-25)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (26-29)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (30-33)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (34-37)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (38-41)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (42)
  
  tb->n_tick(1);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_frame,  (core->frame_o == 0b110100101));
  tb->check(COND_errors, (core->parity_err_o == 0) &&
                         (core->frame_err_o == 0));
  tb->check(COND_valid, (core->output_valid_o == 1));

  //=================================
  //      Tick (43-45)
  
  tb->n_tick(3);

  //`````````````````````````````````
  //      Checks 
  
  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_rx_frontend.valid.7E1_01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_rx_frontend.valid.7E1_02",
      tb->conditions[COND_frame],
      "Failed to implement the frame output", tb->err_cycles[COND_frame]);

  CHECK("tb_rx_frontend.valid.7E1_03",
      tb->conditions[COND_errors],
      "Failed to implement the errors computation", tb->err_cycles[COND_errors]);

  CHECK("tb_rx_frontend.valid.7E1_04",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

void tb_rx_frontend_valid_frame_7E2(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
  core->testcase = T_VALID_7E2;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  core->cr_clk_div_i = 4;
  core->cr_ds_i = 0;
  core->cr_p_i = 2;
  core->cr_s_i = 1;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (2-3)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (4-5)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_START));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (6-7)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (8-9)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (10-13)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (14-17)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (18-21)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (22-25)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (26-29)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (30-33)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (34-37)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (38-41)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (42-45)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (46)
  
  tb->n_tick(1);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_frame,  (core->frame_o == 0b1110100101));
  tb->check(COND_errors, (core->parity_err_o == 0) &&
                         (core->frame_err_o == 0));
  tb->check(COND_valid, (core->output_valid_o == 1));

  //=================================
  //      Tick (47-49)
  
  tb->n_tick(3);

  //`````````````````````````````````
  //      Checks 
  
  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_rx_frontend.valid.7E2_01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_rx_frontend.valid.7E2_02",
      tb->conditions[COND_frame],
      "Failed to implement the frame output", tb->err_cycles[COND_frame]);

  CHECK("tb_rx_frontend.valid.7E2_03",
      tb->conditions[COND_errors],
      "Failed to implement the errors computation", tb->err_cycles[COND_errors]);

  CHECK("tb_rx_frontend.valid.7E2_04",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

void tb_rx_frontend_valid_frame_7O1(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
  core->testcase = T_VALID_7O1;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  core->cr_clk_div_i = 4;
  core->cr_ds_i = 0;
  core->cr_p_i = 1;
  core->cr_s_i = 0;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (2-3)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (4-5)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_START));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (6-7)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (8-9)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (10-13)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (14-17)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (18-21)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (22-25)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (26-29)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (30-33)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (34-37)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (38-41)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (42)
  
  tb->n_tick(1);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_frame,  (core->frame_o == 0b100100101));
  tb->check(COND_errors, (core->parity_err_o == 0) &&
                         (core->frame_err_o == 0));
  tb->check(COND_valid, (core->output_valid_o == 1));

  //=================================
  //      Tick (43-45)
  
  tb->n_tick(3);

  //`````````````````````````````````
  //      Checks 
  
  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_rx_frontend.valid.7O1_01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_rx_frontend.valid.7O1_02",
      tb->conditions[COND_frame],
      "Failed to implement the frame output", tb->err_cycles[COND_frame]);

  CHECK("tb_rx_frontend.valid.7O1_03",
      tb->conditions[COND_errors],
      "Failed to implement the errors computation", tb->err_cycles[COND_errors]);

  CHECK("tb_rx_frontend.valid.7O1_04",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

void tb_rx_frontend_valid_frame_7O2(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
  core->testcase = T_VALID_7O2;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  core->cr_clk_div_i = 4;
  core->cr_ds_i = 0;
  core->cr_p_i = 1;
  core->cr_s_i = 1;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (2-3)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (4-5)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_START));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (6-7)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (8-9)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (10-13)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (14-17)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (18-21)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (22-25)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (26-29)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (30-33)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (34-37)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (38-41)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (42-45)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (46)
  
  tb->n_tick(1);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_frame,  (core->frame_o == 0b1100100101));
  tb->check(COND_errors, (core->parity_err_o == 0) &&
                         (core->frame_err_o == 0));
  tb->check(COND_valid, (core->output_valid_o == 1));

  //=================================
  //      Tick (47-49)
  
  tb->n_tick(3);

  //`````````````````````````````````
  //      Checks 
  
  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_rx_frontend.valid.7O2_01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_rx_frontend.valid.7O2_02",
      tb->conditions[COND_frame],
      "Failed to implement the frame output", tb->err_cycles[COND_frame]);

  CHECK("tb_rx_frontend.valid.7O2_03",
      tb->conditions[COND_errors],
      "Failed to implement the errors computation", tb->err_cycles[COND_errors]);

  CHECK("tb_rx_frontend.valid.7O2_04",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

void tb_rx_frontend_valid_frame_8N1(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
  core->testcase = T_VALID_8N1;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  core->cr_clk_div_i = 4;
  core->cr_ds_i = 1;
  core->cr_p_i = 0;
  core->cr_s_i = 0;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (2-3)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (4-5)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_START));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (6-7)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (8-9)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (10-13)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (14-17)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (18-21)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (22-25)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (26-29)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (30-33)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (34-37)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (38-41)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (42)
  
  tb->n_tick(1);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_frame,  (core->frame_o == 0b110100101));
  tb->check(COND_errors, (core->parity_err_o == 0) &&
                         (core->frame_err_o == 0));
  tb->check(COND_valid, (core->output_valid_o == 1));

  //=================================
  //      Tick (43-45)
  
  tb->n_tick(3);

  //`````````````````````````````````
  //      Checks 
  
  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_rx_frontend.valid.8N1_01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_rx_frontend.valid.8N1_02",
      tb->conditions[COND_frame],
      "Failed to implement the frame output", tb->err_cycles[COND_frame]);

  CHECK("tb_rx_frontend.valid.8N1_03",
      tb->conditions[COND_errors],
      "Failed to implement the errors computation", tb->err_cycles[COND_errors]);

  CHECK("tb_rx_frontend.valid.8N1_04",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

void tb_rx_frontend_valid_frame_8N2(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
  core->testcase = T_VALID_8N2;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  core->cr_clk_div_i = 4;
  core->cr_ds_i = 1;
  core->cr_p_i = 0;
  core->cr_s_i = 1;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (2-3)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (4-5)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_START));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (6-7)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (8-9)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (10-13)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (14-17)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (18-21)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (22-25)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (26-29)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (30-33)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (34-37)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (38-41)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (42-45)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (46)
  
  tb->n_tick(1);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_frame,  (core->frame_o == 0b1110100101));
  tb->check(COND_errors, (core->parity_err_o == 0) &&
                         (core->frame_err_o == 0));
  tb->check(COND_valid, (core->output_valid_o == 1));

  //=================================
  //      Tick (47-49)
  
  tb->n_tick(3);

  //`````````````````````````````````
  //      Checks 
  
  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_rx_frontend.valid.8N2_01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_rx_frontend.valid.8N2_02",
      tb->conditions[COND_frame],
      "Failed to implement the frame output", tb->err_cycles[COND_frame]);

  CHECK("tb_rx_frontend.valid.8N2_03",
      tb->conditions[COND_errors],
      "Failed to implement the errors computation", tb->err_cycles[COND_errors]);

  CHECK("tb_rx_frontend.valid.8N2_04",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

void tb_rx_frontend_valid_frame_8E1(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
  core->testcase = T_VALID_8E1;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  core->cr_clk_div_i = 4;
  core->cr_ds_i = 1;
  core->cr_p_i = 2;
  core->cr_s_i = 0;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (2-3)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (4-5)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_START));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (6-7)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (8-9)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (10-13)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (14-17)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (18-21)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (22-25)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (26-29)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (30-33)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (34-37)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (38-41)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (42-45)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (46)
  
  tb->n_tick(1);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_frame,  (core->frame_o == 0b1010100101));
  tb->check(COND_errors, (core->parity_err_o == 0) &&
                         (core->frame_err_o == 0));
  tb->check(COND_valid, (core->output_valid_o == 1));

  //=================================
  //      Tick (47-49)
  
  tb->n_tick(3);

  //`````````````````````````````````
  //      Checks 
  
  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_rx_frontend.valid.8E1_01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_rx_frontend.valid.8E1_02",
      tb->conditions[COND_frame],
      "Failed to implement the frame output", tb->err_cycles[COND_frame]);

  CHECK("tb_rx_frontend.valid.8E1_03",
      tb->conditions[COND_errors],
      "Failed to implement the errors computation", tb->err_cycles[COND_errors]);

  CHECK("tb_rx_frontend.valid.8E1_04",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

void tb_rx_frontend_valid_frame_8E2(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
  core->testcase = T_VALID_8E2;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  core->cr_clk_div_i = 4;
  core->cr_ds_i = 1;
  core->cr_p_i = 2;
  core->cr_s_i = 1;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (2-3)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (4-5)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_START));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (6-7)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (8-9)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (10-13)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (14-17)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (18-21)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (22-25)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (26-29)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (30-33)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (34-37)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (38-41)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (42-45)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (46-49)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (50)
  
  tb->n_tick(1);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_frame,  (core->frame_o == 0b11010100101));
  tb->check(COND_errors, (core->parity_err_o == 0) &&
                         (core->frame_err_o == 0));
  tb->check(COND_valid, (core->output_valid_o == 1));

  //=================================
  //      Tick (51-53)
  
  tb->n_tick(3);

  //`````````````````````````````````
  //      Checks 
  
  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_rx_frontend.valid.8E2_01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_rx_frontend.valid.8E2_02",
      tb->conditions[COND_frame],
      "Failed to implement the frame output", tb->err_cycles[COND_frame]);

  CHECK("tb_rx_frontend.valid.8E2_03",
      tb->conditions[COND_errors],
      "Failed to implement the errors computation", tb->err_cycles[COND_errors]);

  CHECK("tb_rx_frontend.valid.8E2_04",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

void tb_rx_frontend_valid_frame_8O1(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
  core->testcase = T_VALID_8O1;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  core->cr_clk_div_i = 4;
  core->cr_ds_i = 1;
  core->cr_p_i = 1;
  core->cr_s_i = 0;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (2-3)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (4-5)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_START));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (6-7)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (8-9)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (10-13)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (14-17)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (18-21)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (22-25)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (26-29)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (30-33)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (34-37)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (38-41)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (42-45)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (46)
  
  tb->n_tick(1);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_frame,  (core->frame_o == 0b1110100101));
  tb->check(COND_errors, (core->parity_err_o == 0) &&
                         (core->frame_err_o == 0));
  tb->check(COND_valid, (core->output_valid_o == 1));

  //=================================
  //      Tick (47-49)
  
  tb->n_tick(3);

  //`````````````````````````````````
  //      Checks 
  
  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_rx_frontend.valid.8O1_01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_rx_frontend.valid.8O1_02",
      tb->conditions[COND_frame],
      "Failed to implement the frame output", tb->err_cycles[COND_frame]);

  CHECK("tb_rx_frontend.valid.8O1_03",
      tb->conditions[COND_errors],
      "Failed to implement the errors computation", tb->err_cycles[COND_errors]);

  CHECK("tb_rx_frontend.valid.8O1_04",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

void tb_rx_frontend_valid_frame_8O2(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
  core->testcase = T_VALID_8O2;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  core->cr_clk_div_i = 4;
  core->cr_ds_i = 1;
  core->cr_p_i = 1;
  core->cr_s_i = 1;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (2-3)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (4-5)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_START));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (6-7)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (8-9)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (10-13)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (14-17)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (18-21)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (22-25)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (26-29)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (30-33)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (34-37)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (38-41)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (42-45)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (46-49)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (50)
  
  tb->n_tick(1);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_frame,  (core->frame_o == 0b11110100101));
  tb->check(COND_errors, (core->parity_err_o == 0) &&
                         (core->frame_err_o == 0));
  tb->check(COND_valid, (core->output_valid_o == 1));

  //=================================
  //      Tick (51-53)
  
  tb->n_tick(3);

  //`````````````````````````````````
  //      Checks 
  
  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_rx_frontend.valid.8O2_01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_rx_frontend.valid.8O2_02",
      tb->conditions[COND_frame],
      "Failed to implement the frame output", tb->err_cycles[COND_frame]);

  CHECK("tb_rx_frontend.valid.8O2_03",
      tb->conditions[COND_errors],
      "Failed to implement the errors computation", tb->err_cycles[COND_errors]);

  CHECK("tb_rx_frontend.valid.8O2_04",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

void tb_rx_frontend_baudrate(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
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

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (2-4)
  
  tb->n_tick(3); // 3 cycle delay due to metastability countermeasures

  //=================================
  //      Tick (5)
  
  tb->n_tick(1); // 1 cycle delay between idle and start

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_START));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (6-54)
  
  tb->n_tick(49);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (55-104)
  
  tb->n_tick(50);

  //`````````````````````````````````
  //      Checks 
  

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (105-154)
  
  tb->n_tick(50);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (155-204)
  
  tb->n_tick(50);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (205-304)
  
  tb->n_tick(100);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (305-404)
  
  tb->n_tick(100);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (405-504)
  
  tb->n_tick(100);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (505-604)
  
  tb->n_tick(100);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (605-704)
  
  tb->n_tick(100);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (705-804)
  
  tb->n_tick(100);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (805-854)
  
  tb->n_tick(50);

  //=================================
  //      Tick (855-854)
  
  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_frame,  (core->frame_o == 0b10100101));
  tb->check(COND_errors, (core->parity_err_o == 0) &&
                         (core->frame_err_o == 0));
  tb->check(COND_valid, (core->output_valid_o == 1));

  //=================================
  //      Tick (955-904)
  
  tb->n_tick(50);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (905)
  
  tb->n_tick(1);

  //=================================
  //      Tick (906-954)
  
  tb->n_tick(49);

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_rx_frontend.baudrate.01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_rx_frontend.baudrate.02",
      tb->conditions[COND_frame],
      "Failed to implement the frame output", tb->err_cycles[COND_frame]);

  CHECK("tb_rx_frontend.baudrate.03",
      tb->conditions[COND_errors],
      "Failed to implement the errors computation", tb->err_cycles[COND_errors]);

  CHECK("tb_rx_frontend.baudrate.04",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

void tb_rx_frontend_parity_even(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
  core->testcase = T_PARITY_EVEN;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  core->cr_clk_div_i = 4;
  core->cr_ds_i = 0;
  core->cr_p_i = 2;
  core->cr_s_i = 0;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (2-5)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (6-9)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (10-13)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (14-17)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (18-21)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (22-25)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (26-29)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (30-33)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (34-37)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (38-39)
  
  tb->n_tick(2);
  
  //=================================
  //      Tick (40-42)
  
  tb->n_tick(3); // 3 cycle delay due to metastability countermeasures

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_errors, (core->parity_err_o == 0));
  tb->check(COND_valid,  (core->output_valid_o == 1));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (43-46)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (47-50)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (51-54)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (55-58)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (59-62)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (63-66)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (67-70)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (71-74)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (75-78)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (79-80)
  
  tb->n_tick(2);
  
  //=================================
  //      Tick (81-83)
  
  tb->n_tick(3); // 3 cycle delay due to metastability countermeasures

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_errors, (core->parity_err_o == 0));
  tb->check(COND_valid,  (core->output_valid_o == 1));

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_rx_frontend.parity.EVEN_01",
      tb->conditions[COND_errors],
      "Failed to implement the errors computation", tb->err_cycles[COND_errors]);

  CHECK("tb_rx_frontend.parity.EVEN_02",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

void tb_rx_frontend_parity_odd(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
  core->testcase = T_PARITY_ODD;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  core->cr_clk_div_i = 4;
  core->cr_ds_i = 0;
  core->cr_p_i = 1;
  core->cr_s_i = 0;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (2-5)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (6-9)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (10-13)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (14-17)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (18-21)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (22-25)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (26-29)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (30-33)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (34-37)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (38-39)
  
  tb->n_tick(2);
  
  //=================================
  //      Tick (40-42)
  
  tb->n_tick(3); // 3 cycle delay due to metastability countermeasures

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_errors, (core->parity_err_o == 0));
  tb->check(COND_valid,  (core->output_valid_o == 1));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (43-46)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (47-50)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (51-54)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (55-58)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (59-62)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (63-66)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (67-70)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (71-74)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (75-78)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (79-80)
  
  tb->n_tick(2);
  
  //=================================
  //      Tick (81-83)
  
  tb->n_tick(3); // 3 cycle delay due to metastability countermeasures

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_errors, (core->parity_err_o == 1));
  tb->check(COND_valid,  (core->output_valid_o == 1));

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_rx_frontend.parity.ODD_01",
      tb->conditions[COND_errors],
      "Failed to implement the errors computation", tb->err_cycles[COND_errors]);

  CHECK("tb_rx_frontend.parity.ODD_02",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

void tb_rx_frontend_framing(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
  core->testcase = T_FRAMING;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (2-3)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_IDLE));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (4-5)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_START));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (6-7)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (8-9)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (10-13)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (14-17)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (18-21)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (22-25)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (26-29)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (30-33)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (34-37)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (38-41)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (42)
  
  tb->n_tick(1);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_errors, (core->frame_err_o == 1));

  //=================================
  //      Tick (43-45)
  
  tb->n_tick(3);

  //`````````````````````````````````
  //      Checks 
  
  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_rx_frontend.framing.01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_rx_frontend.framing.02",
      tb->conditions[COND_frame],
      "Failed to implement the frame output", tb->err_cycles[COND_frame]);

  CHECK("tb_rx_frontend.framing.03",
      tb->conditions[COND_errors],
      "Failed to implement the errors computation", tb->err_cycles[COND_errors]);

  CHECK("tb_rx_frontend.framing.04",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

int main(int argc, char ** argv, char ** env) {
  srand(time(NULL));
  Verilated::traceEverOn(true);

  bool verbose = parse_verbose(argc, argv);

  TB_Rx_frontend * tb = new TB_Rx_frontend;
  tb->open_trace("waves/rx_frontend.vcd");
  tb->open_testdata("testdata/rx_frontend.csv");
  tb->set_debug_log(verbose);
  tb->init_conditions(__CondIdEnd);

  /************************************************************/

  tb_rx_frontend_idle(tb);

  tb_rx_frontend_valid_frame_7N1(tb);
  tb_rx_frontend_valid_frame_7N2(tb);
  tb_rx_frontend_valid_frame_7E1(tb);
  tb_rx_frontend_valid_frame_7E2(tb);
  tb_rx_frontend_valid_frame_7O1(tb);
  tb_rx_frontend_valid_frame_7O2(tb);
  tb_rx_frontend_valid_frame_8N1(tb);
  tb_rx_frontend_valid_frame_8N2(tb);
  tb_rx_frontend_valid_frame_8E1(tb);
  tb_rx_frontend_valid_frame_8E2(tb);
  tb_rx_frontend_valid_frame_8O1(tb);
  tb_rx_frontend_valid_frame_8O2(tb);

  tb_rx_frontend_baudrate(tb);

  tb_rx_frontend_parity_even(tb);
  tb_rx_frontend_parity_odd(tb);
  tb_rx_frontend_framing(tb);

  /************************************************************/

  printf("[RX_FRONTEND]: ");
  if(tb->success) {
    printf("Done\n");
  } else {
    printf("Failed\n");
  }

  delete tb;
  exit(EXIT_SUCCESS);
}
