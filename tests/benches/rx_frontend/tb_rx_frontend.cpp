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
  COND_data,
  COND_valid,
  __CondIdEnd
};

enum TestcaseId {
  T_IDLE      = 1,
  T_VALID_7N1 = 2,
  T_VALID_7N2 = 3,
  T_VALID_7E1 = 4,
  T_VALID_7E2 = 5,
  T_VALID_7O1 = 6,
  T_VALID_7O2 = 7,
  T_VALID_8N1 = 8,
  T_VALID_8N2 = 9,
  T_VALID_8E1 = 10,
  T_VALID_8E2 = 11,
  T_VALID_8O1 = 12,
  T_VALID_8O2 = 13,
  T_BAUDRATE = 14
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

void tb_rx_frontend_valid_packet_7N1(TB_Rx_frontend * tb) {
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
  tb->check(COND_data,  (core->data_o == 0b10100101));
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
      tb->conditions[COND_data],
      "Failed to implement the data output", tb->err_cycles[COND_data]);

  CHECK("tb_rx_frontend.valid.7N1_03",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

void tb_rx_frontend_valid_packet_7N2(TB_Rx_frontend * tb) {
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
  tb->check(COND_data,  (core->data_o == 0b110100101));
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
      tb->conditions[COND_data],
      "Failed to implement the data output", tb->err_cycles[COND_data]);

  CHECK("tb_rx_frontend.valid.7N2_03",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

void tb_rx_frontend_valid_packet_7E1(TB_Rx_frontend * tb) {
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
  tb->check(COND_data,  (core->data_o == 0b110100101));
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
      tb->conditions[COND_data],
      "Failed to implement the data output", tb->err_cycles[COND_data]);

  CHECK("tb_rx_frontend.valid.7E1_03",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

void tb_rx_frontend_valid_packet_7E2(TB_Rx_frontend * tb) {
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
  tb->check(COND_data,  (core->data_o == 0b1110100101));
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
      tb->conditions[COND_data],
      "Failed to implement the data output", tb->err_cycles[COND_data]);

  CHECK("tb_rx_frontend.valid.7E2_03",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

void tb_rx_frontend_valid_packet_7O1(TB_Rx_frontend * tb) {
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
  tb->check(COND_data,  (core->data_o == 0b100100101));
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
      tb->conditions[COND_data],
      "Failed to implement the data output", tb->err_cycles[COND_data]);

  CHECK("tb_rx_frontend.valid.7O1_03",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

void tb_rx_frontend_valid_packet_7O2(TB_Rx_frontend * tb) {
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
  tb->check(COND_data,  (core->data_o == 0b1100100101));
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
      tb->conditions[COND_data],
      "Failed to implement the data output", tb->err_cycles[COND_data]);

  CHECK("tb_rx_frontend.valid.7O2_03",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

void tb_rx_frontend_valid_packet_8N1(TB_Rx_frontend * tb) {
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
  tb->check(COND_data,  (core->data_o == 0b110100101));
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
      tb->conditions[COND_data],
      "Failed to implement the data output", tb->err_cycles[COND_data]);

  CHECK("tb_rx_frontend.valid.8N1_03",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

void tb_rx_frontend_valid_packet_8N2(TB_Rx_frontend * tb) {
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
  tb->check(COND_data,  (core->data_o == 0b1110100101));
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
      tb->conditions[COND_data],
      "Failed to implement the data output", tb->err_cycles[COND_data]);

  CHECK("tb_rx_frontend.valid.8N2_03",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

void tb_rx_frontend_valid_packet_8E1(TB_Rx_frontend * tb) {
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
  tb->check(COND_data,  (core->data_o == 0b1010100101));
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
      tb->conditions[COND_data],
      "Failed to implement the data output", tb->err_cycles[COND_data]);

  CHECK("tb_rx_frontend.valid.8E1_03",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

void tb_rx_frontend_valid_packet_8E2(TB_Rx_frontend * tb) {
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
  tb->check(COND_data,  (core->data_o == 0b11010100101));
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
      tb->conditions[COND_data],
      "Failed to implement the data output", tb->err_cycles[COND_data]);

  CHECK("tb_rx_frontend.valid.8E2_03",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

void tb_rx_frontend_valid_packet_8O1(TB_Rx_frontend * tb) {
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
  tb->check(COND_data,  (core->data_o == 0b1110100101));
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
      tb->conditions[COND_data],
      "Failed to implement the data output", tb->err_cycles[COND_data]);

  CHECK("tb_rx_frontend.valid.8O1_03",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

void tb_rx_frontend_valid_packet_8O2(TB_Rx_frontend * tb) {
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
  tb->check(COND_data,  (core->data_o == 0b11110100101));
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
      tb->conditions[COND_data],
      "Failed to implement the data output", tb->err_cycles[COND_data]);

  CHECK("tb_rx_frontend.valid.8O2_03",
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
  
  tb->n_tick(3);

  //=================================
  //      Tick (5-7)
  
  tb->n_tick(3); // 3 cycle delay due to metastability countermeasures

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_START));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (8-53)
  
  tb->n_tick(46);

  //=================================
  //      Tick (54-56)
  
  tb->n_tick(3); // 3 cycle delay due to metastability countermeasures

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (57-103)
  
  tb->n_tick(47);

  //`````````````````````````````````
  //      Checks 
  

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (104-153)
  
  tb->n_tick(50);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //=================================
  //      Tick (154-203)
  
  tb->n_tick(50);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (204-303)
  
  tb->n_tick(100);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (304-403)
  
  tb->n_tick(100);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (404-503)
  
  tb->n_tick(100);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (504-603)
  
  tb->n_tick(100);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (604-703)
  
  tb->n_tick(100);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (704-803)
  
  tb->n_tick(100);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (804-853)
  
  tb->n_tick(50);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_valid, (core->output_valid_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (854)
  
  tb->n_tick(1);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_state, (core->tb_rx_frontend->dut->state_q == S_DATA));
  tb->check(COND_data,  (core->data_o == 0b10100101));
  tb->check(COND_valid, (core->output_valid_o == 1));

  //=================================
  //      Tick (855-903)
  
  tb->n_tick(49);

  //`````````````````````````````````
  //      Checks 
  
  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_rx_frontend.valid.7N1_01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_rx_frontend.valid.7N1_02",
      tb->conditions[COND_data],
      "Failed to implement the data output", tb->err_cycles[COND_data]);

  CHECK("tb_rx_frontend.valid.7N1_03",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

void tb_rx_frontend_parity(TB_Rx_frontend * tb) {

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

  tb_rx_frontend_valid_packet_7N1(tb);
  tb_rx_frontend_valid_packet_7N2(tb);
  tb_rx_frontend_valid_packet_7E1(tb);
  tb_rx_frontend_valid_packet_7E2(tb);
  tb_rx_frontend_valid_packet_7O1(tb);
  tb_rx_frontend_valid_packet_7O2(tb);
  tb_rx_frontend_valid_packet_8N1(tb);
  tb_rx_frontend_valid_packet_8N2(tb);
  tb_rx_frontend_valid_packet_8E1(tb);
  tb_rx_frontend_valid_packet_8E2(tb);
  tb_rx_frontend_valid_packet_8O1(tb);
  tb_rx_frontend_valid_packet_8O2(tb);

  tb_rx_frontend_baudrate(tb);

  tb_rx_frontend_parity(tb);

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
