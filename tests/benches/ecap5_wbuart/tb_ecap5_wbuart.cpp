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
#include "Vtb_ecap5_wbuart_tb_ecap5_wbuart.h"
#include "Vtb_ecap5_wbuart_ecap5_wbuart.h"
#include "testbench.h"

enum CondId {
  COND_reset,
  COND_mem,
  COND_rx,
  COND_tx,
  COND_registers,
  __CondIdEnd
};

enum TestcaseId {
  T_IDLE        = 1,
  T_WRITE_CR    = 2,
  T_WRITE_TXDR  = 3,
  T_READ_RXDR   = 4,
  T_RXNE        = 5,
  T_TXE         = 6,
  T_RXOE        = 7,
  T_FE          = 8,
  T_PE          = 9,
  T_DISABLE     = 10
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

  void n_tick(int n) {
    for(int i = 0; i < n; i++) {
      this->tick();
    }
  } 

  void check_tx(uint8_t expected, uint32_t clk_div, uint8_t ds, uint8_t s, uint8_t p) {
    // Wait half clk_div
    this->n_tick(clk_div / 2);
    // Start bit
    this->check(COND_tx, (this->core->uart_tx_o == 0));
    this->n_tick(clk_div);
    // Sample the bits
    int num_bits = ds ? 8 : 7;
    uint8_t data = 0;
    uint8_t parity = p & 0x1;
    for(int i = 0; i < num_bits; i++) {
      data = data | ((this->core->uart_tx_o & 0x1) << i);
      parity ^= this->core->uart_tx_o;
      this->n_tick(clk_div);
    }
    this->check(COND_tx, (data == expected));
    // Parity bit
    this->check(COND_tx, (this->core->uart_tx_o == parity));
    this->n_tick(clk_div);
    // Stop bits 
    int num_stop_bits = s ? 2 : 1;
    for(int i = 0; i < num_stop_bits; i++) {
      this->check(COND_tx, (this->core->uart_tx_o == 1));
      if((i + 1) < num_stop_bits ) {
        this->n_tick(clk_div);
      }
    }
    this->n_tick(clk_div / 2);
    this->check(COND_tx, (this->core->tb_ecap5_wbuart->dut->tx_done == 1));
    this->tick();
    this->check(COND_tx, (this->core->tb_ecap5_wbuart->dut->tx_done == 0));
  }
};

void tb_ecap5_wbuart_idle(TB_Ecap5_wbuart * tb) {
  Vtb_ecap5_wbuart * core = tb->core;
  core->testcase = T_IDLE;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_reset,     (core->tb_ecap5_wbuart->dut->frontend_rst == 1));
  tb->check(COND_mem,       (core->tb_ecap5_wbuart->dut->mem_read == 0) &&
                            (core->tb_ecap5_wbuart->dut->mem_write == 0));
  tb->check(COND_rx,        (core->tb_ecap5_wbuart->dut->rx_valid == 0));
  tb->check(COND_tx,        (core->tb_ecap5_wbuart->dut->tx_transmit_q == 0) &&
                            (core->tb_ecap5_wbuart->dut->tx_done == 0) &&
                            (core->uart_tx_o == 1));
  tb->check(COND_registers, (core->tb_ecap5_wbuart->dut->registers_q[0] == 0) &&
                            (core->tb_ecap5_wbuart->dut->registers_q[1] == 0) &&
                            (core->tb_ecap5_wbuart->dut->registers_q[2] == 0) &&
                            (core->tb_ecap5_wbuart->dut->registers_q[3] == 0));

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_reset,     (core->tb_ecap5_wbuart->dut->frontend_rst == 0));
  tb->check(COND_mem,       (core->tb_ecap5_wbuart->dut->mem_read == 0) &&
                            (core->tb_ecap5_wbuart->dut->mem_write == 0));
  tb->check(COND_rx,        (core->tb_ecap5_wbuart->dut->rx_valid == 0));
  tb->check(COND_tx,        (core->tb_ecap5_wbuart->dut->tx_transmit_q == 0) &&
                            (core->tb_ecap5_wbuart->dut->tx_done == 0) &&
                            (core->uart_tx_o == 1));
  tb->check(COND_registers, (core->tb_ecap5_wbuart->dut->registers_q[0] == 0) &&
                            (core->tb_ecap5_wbuart->dut->registers_q[1] == 0) &&
                            (core->tb_ecap5_wbuart->dut->registers_q[2] == 0) &&
                            (core->tb_ecap5_wbuart->dut->registers_q[3] == 0));

  //=================================
  //      Tick (2)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_reset,     (core->tb_ecap5_wbuart->dut->frontend_rst == 0));
  tb->check(COND_mem,       (core->tb_ecap5_wbuart->dut->mem_read == 0) &&
                            (core->tb_ecap5_wbuart->dut->mem_write == 0));
  tb->check(COND_rx,        (core->tb_ecap5_wbuart->dut->rx_valid == 0));
  tb->check(COND_tx,        (core->tb_ecap5_wbuart->dut->tx_transmit_q == 0) &&
                            (core->tb_ecap5_wbuart->dut->tx_done == 0) &&
                            (core->uart_tx_o == 1));
  tb->check(COND_registers, (core->tb_ecap5_wbuart->dut->registers_q[0] == 0) &&
                            (core->tb_ecap5_wbuart->dut->registers_q[1] == 0) &&
                            (core->tb_ecap5_wbuart->dut->registers_q[2] == 0) &&
                            (core->tb_ecap5_wbuart->dut->registers_q[3] == 0));

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_ecap5_wbuart.idle.01",
      tb->conditions[COND_reset],
      "Failed to implement the frontend reset", tb->err_cycles[COND_reset]);

  CHECK("tb_ecap5_wbuart.idle.02",
      tb->conditions[COND_mem],
      "Failed to integrate the memory", tb->err_cycles[COND_mem]);

  CHECK("tb_ecap5_wbuart.idle.03",
      tb->conditions[COND_rx],
      "Failed to integrate the rx frontend", tb->err_cycles[COND_rx]);

  CHECK("tb_ecap5_wbuart.idle.04",
      tb->conditions[COND_tx],
      "Failed to integrate the tx frontend", tb->err_cycles[COND_tx]);

  CHECK("tb_ecap5_wbuart.idle.05",
      tb->conditions[COND_registers],
      "Failed to implement the memory-mapped registers", tb->err_cycles[COND_registers]);
}

void tb_ecap5_wbuart_write_cr(TB_Ecap5_wbuart * tb) {
  Vtb_ecap5_wbuart * core = tb->core;
  core->testcase = T_WRITE_CR;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs
  
  tb->write(0x4, 0xA5FA5FA5);

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_reset,     (core->tb_ecap5_wbuart->dut->frontend_rst == 1));
  tb->check(COND_mem,       (core->wb_ack_o == 1));
  tb->check(COND_registers, (core->tb_ecap5_wbuart->dut->registers_q[1] == 0xA5FA5FA5));

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;
  
  //=================================
  //      Tick (2)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_reset,     (core->tb_ecap5_wbuart->dut->frontend_rst == 0));

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_ecap5_wbuart.write_cr.01",
      tb->conditions[COND_reset],
      "Failed to implement the frontend reset", tb->err_cycles[COND_reset]);

  CHECK("tb_ecap5_wbuart.write_cr.02",
      tb->conditions[COND_mem],
      "Failed to integrate the memory", tb->err_cycles[COND_mem]);

  CHECK("tb_ecap5_wbuart.write_cr.03",
      tb->conditions[COND_rx],
      "Failed to integrate the rx frontend", tb->err_cycles[COND_rx]);

  CHECK("tb_ecap5_wbuart.write_cr.04",
      tb->conditions[COND_tx],
      "Failed to integrate the tx frontend", tb->err_cycles[COND_tx]);

  CHECK("tb_ecap5_wbuart.write_cr.05",
      tb->conditions[COND_registers],
      "Failed to implement the memory-mapped registers", tb->err_cycles[COND_registers]);
}

void tb_ecap5_wbuart_write_txdr(TB_Ecap5_wbuart * tb) {
  Vtb_ecap5_wbuart * core = tb->core;
  core->testcase = T_WRITE_TXDR;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs

  uint32_t cr = (4 << 16) | (1 << 4) | (1 << 1);
  tb->write(0x4, cr);

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;

  //=================================
  //      Tick (2)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();

  //=================================
  //      Tick (3)
  
  tb->tick();


  //`````````````````````````````````
  //      Set inputs
  
  tb->write(0xC, 0xA5FA5FA5);

  //=================================
  //      Tick (4)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_reset,     (core->tb_ecap5_wbuart->dut->frontend_rst == 0));
  tb->check(COND_mem,       (core->wb_ack_o == 1));
  tb->check(COND_registers, (core->tb_ecap5_wbuart->dut->registers_q[3] == 0xA5FA5FA5));
  tb->check(COND_tx,        (core->tb_ecap5_wbuart->dut->tx_transmit_q == 1));

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;

  //=================================
  //      Tick (5)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 0;
  
  //=================================
  //      Tick (6-7)
  
  // Wait half clk_div
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_tx, (core->uart_tx_o == 0));
  
  //=================================
  //      Tick (8-11)
  
  tb->n_tick(4);

  //=================================
  //      Tick (11-43)
  
  uint8_t expected = 0xA5;
  for(int i = 0; i < 8; i++) {
    tb->check(COND_tx, (core->uart_tx_o == ((expected >> i) & 0x1)));

    //=================================
    //      Tick 
    
    tb->n_tick(4);
  }

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_tx, (core->uart_tx_o == 1));

  //=================================
  //      Tick (44-47)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_tx, (core->uart_tx_o == 1));

  //=================================
  //      Tick (48-49)
  
  tb->n_tick(2);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_tx, (core->tb_ecap5_wbuart->dut->tx_done == 1));

  //=================================
  //      Tick (50)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_tx, (core->tb_ecap5_wbuart->dut->tx_done == 0));

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_ecap5_wbuart.write_txdr.01",
      tb->conditions[COND_reset],
      "Failed to implement the frontend reset", tb->err_cycles[COND_reset]);

  CHECK("tb_ecap5_wbuart.write_txdr.02",
      tb->conditions[COND_mem],
      "Failed to integrate the memory", tb->err_cycles[COND_mem]);

  CHECK("tb_ecap5_wbuart.write_txdr.03",
      tb->conditions[COND_rx],
      "Failed to integrate the rx frontend", tb->err_cycles[COND_rx]);

  CHECK("tb_ecap5_wbuart.write_txdr.04",
      tb->conditions[COND_tx],
      "Failed to integrate the tx frontend", tb->err_cycles[COND_tx]);

  CHECK("tb_ecap5_wbuart.write_txdr.05",
      tb->conditions[COND_registers],
      "Failed to implement the memory-mapped registers", tb->err_cycles[COND_registers]);
}

void tb_ecap5_wbuart_read_rxdr(TB_Ecap5_wbuart * tb) {
  Vtb_ecap5_wbuart * core = tb->core;
  core->testcase = T_READ_RXDR;

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

  tb_ecap5_wbuart_write_cr(tb);
  tb_ecap5_wbuart_write_txdr(tb);
  tb_ecap5_wbuart_read_rxdr(tb);

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
