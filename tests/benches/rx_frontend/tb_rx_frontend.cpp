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
  T_VALID_8O2 = 13
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

  // The following actions are performed in this test :
  //    tick 0. Nothing (core is in IDLE)

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
  
  //=================================
  //      Tick (4-5)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (6-7)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (8-9)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (10-11)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (12-13)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (14-15)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (16-17)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (18-19)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (20-21)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (22-23)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (24-25)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (26-27)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (28-29)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (30-31)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (32-33)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //=================================
  //      Tick (34-35)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (36-37)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //=================================
  //      Tick (38-39)
  
  tb->n_tick(8);

  //`````````````````````````````````
  //      Checks 
}

void tb_rx_frontend_valid_packet_7N2(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
  core->testcase = T_VALID_7N2;
}

void tb_rx_frontend_valid_packet_7E1(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
  core->testcase = T_VALID_7E1;
}

void tb_rx_frontend_valid_packet_7E2(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
  core->testcase = T_VALID_7E2;
}

void tb_rx_frontend_valid_packet_7O1(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
  core->testcase = T_VALID_7O1;
}

void tb_rx_frontend_valid_packet_7O2(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
  core->testcase = T_VALID_7O2;
}

void tb_rx_frontend_valid_packet_8N1(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
  core->testcase = T_VALID_8N1;
}

void tb_rx_frontend_valid_packet_8N2(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
  core->testcase = T_VALID_8N2;
}

void tb_rx_frontend_valid_packet_8E1(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
  core->testcase = T_VALID_8E1;
}

void tb_rx_frontend_valid_packet_8E2(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
  core->testcase = T_VALID_8E2;
}

void tb_rx_frontend_valid_packet_8O1(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
  core->testcase = T_VALID_8O1;
}

void tb_rx_frontend_valid_packet_8O2(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
  core->testcase = T_VALID_8O1;
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
