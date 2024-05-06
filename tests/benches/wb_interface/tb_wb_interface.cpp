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

#include "Vtb_wb_interface.h"
#include "testbench.h"

enum CondId {
  COND_output,
  COND_wishbone,
  __CondIdEnd
};

enum TestcaseId {
  T_READ,
  T_WRITE
};

class TB_Wb_interface : public Testbench<Vtb_wb_interface> {
public:
  void reset() {
    this->_nop();

    this->core->rst_i = 1;
    for(int i = 0; i < 5; i++) {
      this->tick();
    }
    this->core->rst_i = 0;

    Testbench<Vtb_wb_interface>::reset();
  }
  
  void _nop() {
    this->core->wb_adr_i = 0;
    this->core->wb_dat_i = 0;
    this->core->wb_we_i = 0;
    this->core->wb_sel_i = 0;
    this->core->wb_stb_i = 0;
    this->core->wb_cyc_i = 0;
  }

  void read(uint32_t addr) {
    this->core->wb_adr_i = addr;
    this->core->wb_dat_i = 0;
    this->core->wb_we_i = 0;
    this->core->wb_sel_i = 0xF;
    this->core->wb_stb_i = 1;
    this->core->wb_cyc_i = 1;
  }

  void write(uint32_t addr, uint32_t data) {
    this->core->wb_adr_i = addr;
    this->core->wb_dat_i = data;
    this->core->wb_we_i = 1;
    this->core->wb_sel_i = 0xF;
    this->core->wb_stb_i = 1;
    this->core->wb_cyc_i = 1;
  }
};

void tb_wb_interface_read(TB_Wb_interface * tb) {
  Vtb_wb_interface * core = tb->core;
  core->testcase = T_READ;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  tb->check(COND_output, (core->read_o == 0) &&
                         (core->write_o == 0));

  uint32_t addr = rand();
  tb->read(addr);

  //=================================
  //      Tick (0)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_output, (core->read_o == 1) &&
                         (core->addr_o == addr) &&
                         (core->write_o == 0));

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;
  
  uint32_t data = rand();
  core->read_data_i = data;

  //=================================
  //      Tick (0)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_output, (core->read_o == 0) &&
                         (core->write_o == 0));
  tb->check(COND_wishbone, (core->wb_ack_o == 1));

  //`````````````````````````````````
  //      Set inputs
  
  core->wb_cyc_i = 0;

  //=================================
  //      Tick (0)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_output, (core->read_o == 0) &&
                         (core->write_o == 0));
  tb->check(COND_wishbone, (core->wb_ack_o == 0));

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_wb_uart.read.01",
      tb->conditions[COND_output],
      "Failed to implement the output logic", tb->err_cycles[COND_output]);

  CHECK("tb_wb_uart.read.02",
      tb->conditions[COND_wishbone],
      "Failed to implement the wishbone protocol", tb->err_cycles[COND_wishbone]);
}

int main(int argc, char ** argv, char ** env) {
  srand(time(NULL));
  Verilated::traceEverOn(true);

  bool verbose = parse_verbose(argc, argv);

  TB_Wb_interface * tb = new TB_Wb_interface;
  tb->open_trace("waves/wb_interface.vcd");
  tb->open_testdata("testdata/wb_interface.csv");
  tb->set_debug_log(verbose);
  tb->init_conditions(__CondIdEnd);

  /************************************************************/


  /************************************************************/

  printf("[WB_INTERFACE]: ");
  if(tb->success) {
    printf("Done\n");
  } else {
    printf("Failed\n");
  }

  delete tb;
  exit(EXIT_SUCCESS);
}
