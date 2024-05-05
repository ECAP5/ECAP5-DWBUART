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
  T_IDLE,
  T_VALID_PACKET
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
    core->clk_div_i = 0;
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

void tb_rx_frontend_valid_packet(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
  core->testcase = T_VALID_PACKET;

  // The following actions are performed in this test :
  //    tick 0. Set clk_div to 4 (core is in IDLE)
  //    tick 1. Send start bit (core is in START)
  //    tick 2. Hold start bit (core is in DATA)
  //    tick 3. Hold start bit (core is in DATA)
  //    tick 4. Hold start bit (core is in DATA)
  //    tick 5. Send d0 bit (core is in DATA)
  //    tick 6. Hold d0 bit (core is in DATA)
  //    tick 7. Hold d0 bit (core is in DATA)
  //    tick 8. Hold d0 bit (core is in DATA)
  //    tick 9. Send d1 bit (core is in DATA)
  //    tick 10. Hold d1 bit (core is in DATA)
  //    tick 11. Hold d1 bit (core is in DATA)
  //    tick 12. Hold d1 bit (core is in DATA)
  //    tick 13. Send d2 bit (core is in DATA)
  //    tick 14. Hold d2 bit (core is in DATA)
  //    tick 15. Hold d2 bit (core is in DATA)
  //    tick 16. Hold d2 bit (core is in DATA)
  //    tick 17. Send d3 bit (core is in DATA)
  //    tick 18. Hold d3 bit (core is in DATA)
  //    tick 19. Hold d3 bit (core is in DATA)
  //    tick 20. Hold d3 bit (core is in DATA)
  //    tick 21. Send d4 bit (core is in DATA)
  //    tick 22. Hold d4 bit (core is in DATA)
  //    tick 23. Hold d4 bit (core is in DATA)
  //    tick 24. Hold d4 bit (core is in DATA)
  //    tick 25. Send d5 bit (core is in DATA)
  //    tick 26. Hold d5 bit (core is in DATA)
  //    tick 27. Hold d5 bit (core is in DATA)
  //    tick 28. Hold d5 bit (core is in DATA)
  //    tick 29. Send d6 bit (core is in DATA)
  //    tick 30. Hold d6 bit (core is in DATA)
  //    tick 31. Hold d6 bit (core is in DATA)
  //    tick 32. Hold d6 bit (core is in DATA)
  //    tick 33. Send d7 bit (core is in DATA)
  //    tick 34. Hold d7 bit (core is in DATA)
  //    tick 35. Hold d7 bit (core is in DATA)
  //    tick 36. Hold d7 bit (core is in DATA)
  //    tick 37. Send parity bit (core is in DATA)
  //    tick 38. Hold parity bit (core is in DATA)
  //    tick 39. Hold parity bit (core is in DATA)
  //    tick 40. Hold parity bit (core is in DATA)
  //    tick 41. Send stop bit (core is in DATA)
  //    tick 42. Hold stop bit (core is in IDLE)
  //    tick 43. Hold stop bit (core is in IDLE)
  //    tick 44. Hold stop bit (core is in IDLE)
  //    tick 45. Send stop bit (core is in IDLE)
  //    tick 46. Hold stop bit (core is in IDLE)
  //    tick 47. Hold stop bit (core is in IDLE)
  //    tick 48. Hold stop bit (core is in IDLE)
  //    tick 49. Nothing (core is in IDLE)

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  core->clk_div_i = 4;

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (2)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (3)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (4)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (5)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (6)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (7)
  
  tb->tick();

  //=================================
  //      Tick (8)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (9)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (10)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (11)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (12)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (13)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (14)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (15)
  
  tb->tick();

  //=================================
  //      Tick (16)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (17)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (18)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (19)
  
  tb->tick();

  //=================================
  //      Tick (20)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (21)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (22)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (23)
  
  tb->tick();

  //=================================
  //      Tick (24)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (25)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (26)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (27)
  
  tb->tick();

  //=================================
  //      Tick (28)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (29)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (30)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (31)
  
  tb->tick();

  //=================================
  //      Tick (32)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (33)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 0;

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (34)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (35)
  
  tb->tick();

  //=================================
  //      Tick (36)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (37)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (38)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (39)
  
  tb->tick();

  //=================================
  //      Tick (40)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (41)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (42)
  
  tb->tick();

  //=================================
  //      Tick (43)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (44)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (45)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  core->uart_rx_i = 1;

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (46)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (47)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (48)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (49)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (50)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  //=================================
  //      Tick (51)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  

  //`````````````````````````````````
  //      Formal Checks 
  
//  CHECK("tb_rx_frontend.idle.01",
//      tb->conditions[COND_state],
//      "Failed to implement the state machine", tb->err_cycles[COND_state]);
//
//  CHECK("tb_rx_frontend.idle.02",
//      tb->conditions[COND_valid],
//      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
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
  tb_rx_frontend_valid_packet(tb);

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
