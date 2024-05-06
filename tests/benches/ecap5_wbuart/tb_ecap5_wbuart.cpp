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

#include "Vtb_ecap5_wbuart.h"
#include "testbench.h"

enum CondId {
  __CondIdEnd
};

enum TestcaseId {
  T_IDLE        = 1,
  T_TX_7N1_B4   = 2,
  T_TX_8O2_B100 = 3,
  T_RX_7O1_B5   = 4,
  T_RX_8E1_B101 = 5,
  T_CR_RST      = 6,
  T_RXDR_CLR    = 7,
  T_RXNE        = 8,
  T_TXE         = 9,
  T_RXOE        = 10,
  T_FE          = 11,
  T_PE          = 12,
  T_DISABLE     = 13
};

enum StateId {
};

class TB_Ecap5_wbuart : public Testbench<Vtb_ecap5_wbuart> {
public:
  void reset() {
    this->_nop();

    this->core->rst_i = 1;
    for(int i = 0; i < 5; i++) {
      this->tick();
    }
    this->core->rst_i = 0;

    Testbench<Vtb_ecap5_wbuart>::reset();
  }
  
  void _nop() {
    this->core->wb_adr_i = 0;
    this->core->wb_dat_i = 0;
    this->core->wb_we_i = 0;
    this->core->wb_sel_i = 0;
    this->core->wb_stb_i = 0;
    this->core->wb_cyc_i = 0;

    this->core->uart_rx_i = 1;
  }
};

void tb_ecap5_wbuart_idle(TB_Ecap5_wbuart * tb) {
  Vtb_ecap5_wbuart * core = tb->core;
  core->testcase = T_IDLE;

  tb->reset();
}

void tb_ecap5_wbuart_tx_7N1_B4(TB_Ecap5_wbuart * tb) {
  Vtb_ecap5_wbuart * core = tb->core;
  core->testcase = T_TX_7N1_B4;

  tb->reset();
}

void tb_ecap5_wbuart_tx_8O2_B100(TB_Ecap5_wbuart * tb) {
  Vtb_ecap5_wbuart * core = tb->core;
  core->testcase = T_TX_8O2_B100;

  tb->reset();
}

void tb_ecap5_wbuart_rx_7O1_B5(TB_Ecap5_wbuart * tb) {
  Vtb_ecap5_wbuart * core = tb->core;
  core->testcase = T_RX_7O1_B5;

  tb->reset();
}

void tb_ecap5_wbuart_rx_8E1_B101(TB_Ecap5_wbuart * tb) {
  Vtb_ecap5_wbuart * core = tb->core;
  core->testcase = T_RX_8E1_B101;

  tb->reset();
}

void tb_ecap5_wbuart_cr_rst(TB_Ecap5_wbuart * tb) {
  Vtb_ecap5_wbuart * core = tb->core;
  core->testcase = T_CR_RST;

  tb->reset();
}

void tb_ecap5_wbuart_rxdr_clr(TB_Ecap5_wbuart * tb) {
  Vtb_ecap5_wbuart * core = tb->core;
  core->testcase = T_RXDR_CLR;

  tb->reset();
}

void tb_ecap5_wbuart_rxne(TB_Ecap5_wbuart * tb) {
  Vtb_ecap5_wbuart * core = tb->core;
  core->testcase = T_RXNE;

  tb->reset();
}

void tb_ecap5_wbuart_txe(TB_Ecap5_wbuart * tb) {
  Vtb_ecap5_wbuart * core = tb->core;
  core->testcase = T_TXE;

  tb->reset();
}

void tb_ecap5_wbuart_rxoe(TB_Ecap5_wbuart * tb) {
  Vtb_ecap5_wbuart * core = tb->core;
  core->testcase = T_RXOE;

  tb->reset();
}

void tb_ecap5_wbuart_fe(TB_Ecap5_wbuart * tb) {
  Vtb_ecap5_wbuart * core = tb->core;
  core->testcase = T_FE;

  tb->reset();
}

void tb_ecap5_wbuart_pe(TB_Ecap5_wbuart * tb) {
  Vtb_ecap5_wbuart * core = tb->core;
  core->testcase = T_PE;

  tb->reset();
}

void tb_ecap5_wbuart_disable(TB_Ecap5_wbuart * tb) {
  Vtb_ecap5_wbuart * core = tb->core;
  core->testcase = T_DISABLE;

  tb->reset();
}

int main(int argc, char ** argv, char ** env) {
  srand(time(NULL));
  Verilated::traceEverOn(true);

  bool verbose = parse_verbose(argc, argv);

  TB_Ecap5_wbuart * tb = new TB_Ecap5_wbuart;
  tb->open_trace("waves/ecap5_wbuart.vcd");
  tb->open_testdata("testdata/ecap5_wbuart.csv");
  tb->set_debug_log(verbose);
  tb->init_conditions(__CondIdEnd);

  /************************************************************/

  tb_ecap5_wbuart_idle(tb);

  tb_ecap5_wbuart_tx_7N1_B4(tb);
  tb_ecap5_wbuart_tx_8O2_B100(tb);
  tb_ecap5_wbuart_rx_7O1_B5(tb);
  tb_ecap5_wbuart_rx_8E1_B101(tb);

  tb_ecap5_wbuart_cr_rst(tb);

  tb_ecap5_wbuart_rxdr_clr(tb);

  tb_ecap5_wbuart_rxne(tb);
  tb_ecap5_wbuart_txe(tb);
  tb_ecap5_wbuart_rxoe(tb);
  tb_ecap5_wbuart_fe(tb);
  tb_ecap5_wbuart_pe(tb);

  tb_ecap5_wbuart_disable(tb);

  /************************************************************/

  printf("[ECAP5_WBUART]: ");
  if(tb->success) {
    printf("Done\n");
  } else {
    printf("Failed\n");
  }

  delete tb;
  exit(EXIT_SUCCESS);
}
