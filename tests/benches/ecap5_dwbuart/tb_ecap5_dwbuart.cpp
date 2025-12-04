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

#include "Vtb_ecap5_dwbuart.h"
#include "Vtb_ecap5_dwbuart_tb_ecap5_dwbuart.h"
#include "Vtb_ecap5_dwbuart_ecap5_dwbuart.h"
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
  T_IDLE                  = 1,
  T_WRITE_CR              = 2,
  T_WRITE_TXDR            = 3,
  T_READ_RXDR             = 4,
  T_RXOE                  = 5,
  T_FE                    = 6,
  T_PE                    = 7,
  T_FULL_DUPLEX           = 8
};

enum StateId {
};

class TB_Ecap5_dwbuart : public Testbench<Vtb_ecap5_dwbuart> {
public:
  void reset() {
    this->_nop();

    this->core->rst_i = 1;
    for(int i = 0; i < 5; i++) {
      this->tick();
    }
    this->core->rst_i = 0;

    Testbench<Vtb_ecap5_dwbuart>::reset();
  }
  
  void _nop() {
    this->core->wb_adr_i = 0;
    this->core->wb_dat_i = 0;
    this->core->wb_we_i = 0;
    this->core->wb_sel_i = 0;
    this->core->wb_stb_i = 0;
    this->core->wb_cyc_i = 0;

//    this->core->uart_rx_i = 1;
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

  uint32_t uart_sr() {
    uint32_t reg = 0;
    reg |= core->tb_ecap5_dwbuart->dut->sr_pe_q << 4;
    reg |= core->tb_ecap5_dwbuart->dut->sr_fe_q << 3;
    reg |= core->tb_ecap5_dwbuart->dut->sr_rxoe_q << 2;
    reg |= core->tb_ecap5_dwbuart->dut->sr_txe_q << 1;
    reg |= core->tb_ecap5_dwbuart->dut->sr_rxne_q;
    return reg;
  }

  uint32_t uart_cr() {
    uint32_t reg = 0;
    reg |= core->tb_ecap5_dwbuart->dut->cr_acc_incr_q << 16;
    reg |= core->tb_ecap5_dwbuart->dut->cr_ds_q << 3;
    reg |= core->tb_ecap5_dwbuart->dut->cr_s_q << 2;
    reg |= core->tb_ecap5_dwbuart->dut->cr_p_q;
    return reg;
  }

  uint32_t uart_rxdr() {
    return core->tb_ecap5_dwbuart->dut->rxdr_rxd_q;
  }

  uint32_t uart_txdr() {
    return core->tb_ecap5_dwbuart->dut->txdr_txd_q;
  }
};

void tb_ecap5_dwbuart_idle(TB_Ecap5_dwbuart * tb) {
  Vtb_ecap5_dwbuart * core = tb->core;
  core->testcase = T_IDLE;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_reset,     (core->tb_ecap5_dwbuart->dut->frontend_rst == 1));
  tb->check(COND_mem,       (core->tb_ecap5_dwbuart->dut->mem_read == 0) &&
                            (core->tb_ecap5_dwbuart->dut->mem_write == 0));
  tb->check(COND_rx,        (core->tb_ecap5_dwbuart->dut->rx_valid == 0));
  tb->check(COND_tx,        (core->tb_ecap5_dwbuart->dut->tx_transmit_q == 0) &&
                            (core->tb_ecap5_dwbuart->dut->tx_done == 0) &&
                            (core->uart_tx_o == 1));
  tb->check(COND_registers, (tb->uart_sr() == 2) &&
                            (tb->uart_cr() == 0) &&
                            (tb->uart_rxdr() == 0) &&
                            (tb->uart_txdr() == 0));

  //=================================
  //      Tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_reset,     (core->tb_ecap5_dwbuart->dut->frontend_rst == 0));
  tb->check(COND_mem,       (core->tb_ecap5_dwbuart->dut->mem_read == 0) &&
                            (core->tb_ecap5_dwbuart->dut->mem_write == 0));
  tb->check(COND_rx,        (core->tb_ecap5_dwbuart->dut->rx_valid == 0));
  tb->check(COND_tx,        (core->tb_ecap5_dwbuart->dut->tx_transmit_q == 0) &&
                            (core->tb_ecap5_dwbuart->dut->tx_done == 0) &&
                            (core->uart_tx_o == 1));
  tb->check(COND_registers, (tb->uart_sr() == 2) &&
                            (tb->uart_cr() == 0) &&
                            (tb->uart_rxdr() == 0) &&
                            (tb->uart_txdr() == 0));

  //=================================
  //      Tick (2)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_reset,     (core->tb_ecap5_dwbuart->dut->frontend_rst == 0));
  tb->check(COND_mem,       (core->tb_ecap5_dwbuart->dut->mem_read == 0) &&
                            (core->tb_ecap5_dwbuart->dut->mem_write == 0));
  tb->check(COND_rx,        (core->tb_ecap5_dwbuart->dut->rx_valid == 0));
  tb->check(COND_tx,        (core->tb_ecap5_dwbuart->dut->tx_transmit_q == 0) &&
                            (core->tb_ecap5_dwbuart->dut->tx_done == 0) &&
                            (core->uart_tx_o == 1));
  tb->check(COND_registers, (tb->uart_sr() == 2) &&
                            (tb->uart_cr() == 0) &&
                            (tb->uart_rxdr() == 0) &&
                            (tb->uart_txdr() == 0));

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_ecap5_dwbuart.idle.01",
      tb->conditions[COND_reset],
      "Failed to implement the frontend reset", tb->err_cycles[COND_reset]);

  CHECK("tb_ecap5_dwbuart.idle.02",
      tb->conditions[COND_mem],
      "Failed to integrate the memory", tb->err_cycles[COND_mem]);

  CHECK("tb_ecap5_dwbuart.idle.03",
      tb->conditions[COND_rx],
      "Failed to integrate the rx frontend", tb->err_cycles[COND_rx]);

  CHECK("tb_ecap5_dwbuart.idle.04",
      tb->conditions[COND_tx],
      "Failed to integrate the tx frontend", tb->err_cycles[COND_tx]);

  CHECK("tb_ecap5_dwbuart.idle.05",
      tb->conditions[COND_registers],
      "Failed to implement the memory-mapped registers", tb->err_cycles[COND_registers]);
}

void tb_ecap5_dwbuart_write_cr(TB_Ecap5_dwbuart * tb) {
  Vtb_ecap5_dwbuart * core = tb->core;
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
  
  tb->check(COND_reset,     (core->tb_ecap5_dwbuart->dut->frontend_rst == 1));
  tb->check(COND_mem,       (core->wb_ack_o == 1));
  tb->check(COND_registers, (tb->uart_cr() == 0xA5FA0005));

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;
  
  //=================================
  //      Tick (2)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_reset,     (core->tb_ecap5_dwbuart->dut->frontend_rst == 0));

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_ecap5_dwbuart.write_cr.01",
      tb->conditions[COND_reset],
      "Failed to implement the frontend reset", tb->err_cycles[COND_reset]);

  CHECK("tb_ecap5_dwbuart.write_cr.02",
      tb->conditions[COND_mem],
      "Failed to integrate the memory", tb->err_cycles[COND_mem]);

  CHECK("tb_ecap5_dwbuart.write_cr.03",
      tb->conditions[COND_registers],
      "Failed to implement the memory-mapped registers", tb->err_cycles[COND_registers]);
}

void tb_ecap5_dwbuart_write_txdr(TB_Ecap5_dwbuart * tb) {
  Vtb_ecap5_dwbuart * core = tb->core;
  core->testcase = T_WRITE_TXDR;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_registers, (tb->uart_sr() & 0x2));

  //`````````````````````````````````
  //      Set inputs

  // (2**16)/4 = 16384 = 1 bit every 4 clk cycles
  uint32_t cr = (16384 << 16) | (1 << 3) | 1;
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
  
  tb->check(COND_registers, ((tb->uart_sr() & 0x2) == 0));
  tb->check(COND_reset,     (core->tb_ecap5_dwbuart->dut->frontend_rst == 0));
  tb->check(COND_mem,       (core->wb_ack_o == 1));
  tb->check(COND_registers, (tb->uart_txdr() == 0x000000A5));
  tb->check(COND_tx,        (core->tb_ecap5_dwbuart->dut->tx_transmit_q == 1));

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
  //      Tick (6-49)
  
  // 1 start bit, 8 data bits, 1 parity bit, 1 stop bit
  uint32_t number_of_tx_bits = (1 + 8 + 1 + 1) * 4;
  tb->n_tick(number_of_tx_bits);
  
  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_tx, (core->tb_ecap5_dwbuart->dut->tx_done == 1));

  //=================================
  //      Tick (50)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_tx, (core->tb_ecap5_dwbuart->dut->tx_done == 0));
  tb->check(COND_registers, (tb->uart_sr() & 0x2));

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_ecap5_dwbuart.write_txdr.01",
      tb->conditions[COND_reset],
      "Failed to implement the frontend reset", tb->err_cycles[COND_reset]);

  CHECK("tb_ecap5_dwbuart.write_txdr.02",
      tb->conditions[COND_mem],
      "Failed to integrate the memory", tb->err_cycles[COND_mem]);

  CHECK("tb_ecap5_dwbuart.write_txdr.03",
      tb->conditions[COND_tx],
      "Failed to integrate the tx frontend", tb->err_cycles[COND_tx]);

  CHECK("tb_ecap5_dwbuart.write_txdr.04",
      tb->conditions[COND_registers],
      "Failed to implement the memory-mapped registers", tb->err_cycles[COND_registers]);
}

void tb_ecap5_dwbuart_read_rxdr(TB_Ecap5_dwbuart * tb) {
  Vtb_ecap5_dwbuart * core = tb->core;
  core->testcase = T_READ_RXDR;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs

  // (2**16)/4 = 16384 = 1 bit every 4 clk cycles
  uint32_t cr = (16384 << 16) | (1 << 3) | 1;
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
  //      Tick (6-50)
  
  // 1 start bit, 8 data bits, 1 parity bit, 0.5 stop bit
  // the receive is valid at the half of the stop bit
  uint32_t number_of_tx_bits = (1 + 8 + 1 + 1 + 0.5) * 4;
  // This call is shifted by 1 tick from the emission start,
  tb->n_tick(number_of_tx_bits - 1);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_rx, (core->tb_ecap5_dwbuart->dut->rx_valid == 1));

  //=================================
  //      Tick (51)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_rx, (core->tb_ecap5_dwbuart->dut->rx_valid == 0));
  tb->check(COND_registers, (tb->uart_rxdr() == 0xA5) &&
                            (tb->uart_sr() & 0x1));

  //=================================
  //      Tick (52)
  
  core->probe = 0;
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_registers, (tb->uart_rxdr() == 0xA5) &&
                            (tb->uart_sr() & 0x1));

  //`````````````````````````````````
  //      Set inputs
  
  tb->read(0x8);

  //=================================
  //      Tick (53)
  
  core->probe = 1;
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_mem, (core->tb_ecap5_dwbuart->dut->mem_read_data_q == 0xA5));
  tb->check(COND_registers, (tb->uart_rxdr() == 0) &&
                            ((tb->uart_sr() & 0x1) == 0));

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;

  //=================================
  //      Tick (52)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();

  //=================================
  //      Tick (53)
  
  tb->tick();

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_ecap5_dwbuart.read_rxdr.01",
      tb->conditions[COND_mem],
      "Failed to integrate the memory", tb->err_cycles[COND_mem]);

  CHECK("tb_ecap5_dwbuart.read_rxdr.02",
      tb->conditions[COND_rx],
      "Failed to integrate the rx frontend", tb->err_cycles[COND_rx]);

  CHECK("tb_ecap5_dwbuart.read_rxdr.03",
      tb->conditions[COND_registers],
      "Failed to implement the memory-mapped registers", tb->err_cycles[COND_registers]);
}

void tb_ecap5_dwbuart_rxoe(TB_Ecap5_dwbuart * tb) {
  Vtb_ecap5_dwbuart * core = tb->core;
  core->testcase = T_RXOE;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs

  // (2**16)/4 = 16384 = 1 bit every 4 clk cycles
  uint32_t cr = (16384 << 16) | (1 << 3) | 1;
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
  //      Tick (6-50)
  
  // 1 start bit, 8 data bits, 1 parity bit, 0.5 stop bit
  // the receive is valid at the half of the stop bit
  uint32_t number_of_tx_bits = (1 + 8 + 1 + 1 + 0.5) * 4;
  // This call is shifted by 1 tick from the emission start,
  tb->n_tick(number_of_tx_bits - 1);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_rx, (core->tb_ecap5_dwbuart->dut->rx_valid == 1));

  //`````````````````````````````````
  //      Set inputs
  
  tb->write(0xC, 0xA5FAA5FA);

  //=================================
  //      Tick (51)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;

  //=================================
  //      Tick (52)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 0;

  //=================================
  //      Tick (53-97)
  
  // This call is shifted by 1 tick from the emission start,
  tb->n_tick(number_of_tx_bits - 1);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_rx, (core->tb_ecap5_dwbuart->dut->rx_valid == 1));

  //=================================
  //      Tick (98)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_rx, (core->tb_ecap5_dwbuart->dut->rx_valid == 0));
  tb->check(COND_registers, ((tb->uart_sr() >> 2) & 0x1) &&
                             (tb->uart_rxdr() == 0xFA));

  //`````````````````````````````````
  //      Set inputs

  tb->read(0x8);

  //=================================
  //      Tick (99)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_mem, (core->tb_ecap5_dwbuart->dut->mem_read_data_q == 0xFA));
  tb->check(COND_registers, ((tb->uart_sr() >> 2) & 0x1) &&
                             (tb->uart_rxdr() == 0x0));

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;

  //=================================
  //      Tick (100)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();

  //=================================
  //      Tick (101)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs

  tb->read(0x0);

  uint32_t sr = tb->uart_sr();

  //=================================
  //      Tick (102)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_mem, (core->tb_ecap5_dwbuart->dut->mem_read_data_q == sr));
  tb->check(COND_registers, (((tb->uart_sr() >> 2) & 0x1) == 0));

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;

  //=================================
  //      Tick (103)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();

  //=================================
  //      Tick (104)
  
  tb->tick();

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_ecap5_dwbuart.rxoe.01",
      tb->conditions[COND_mem],
      "Failed to integrate the memory", tb->err_cycles[COND_mem]);

  CHECK("tb_ecap5_dwbuart.rxoe.02",
      tb->conditions[COND_rx],
      "Failed to integrate the rx frontend", tb->err_cycles[COND_rx]);

  CHECK("tb_ecap5_dwbuart.rxoe.03",
      tb->conditions[COND_registers],
      "Failed to implement the memory-mapped registers", tb->err_cycles[COND_registers]);
}

void tb_ecap5_dwbuart_fe(TB_Ecap5_dwbuart * tb) {
  Vtb_ecap5_dwbuart * core = tb->core;
  core->testcase = T_FE;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs

  // (2**16)/4 = 16384 = 1 bit every 4 clk cycles
  uint32_t cr = (16384 << 16) | (1 << 3) | 1;
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
  //      Tick (6-45)
  
  // 1 start bit, 8 data bits, 1 parity bit
  // the receive is valid at the half of the stop bit
  uint32_t number_of_tx_bits = (1 + 8 + 1 + 1) * 4;
  // This call is shifted by 1 tick from the emission start
  // and we want to be before the sending of the stop bit (3 cycles in advance)
  tb->n_tick(number_of_tx_bits - 1 - 3);

  //=================================
  //      Tick (46-50)

  // Inject the frame error
  core->inj_frame_error = 1;
  tb->n_tick(3 + 2);
  core->inj_frame_error = 0;

  //=================================
  //      Tick (51)

  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_registers, (tb->uart_sr() >> 3) & 0x1);

  //`````````````````````````````````
  //      Set inputs

  tb->read(0x0);

  uint32_t sr = tb->uart_sr();

  //=================================
  //      Tick (49)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_mem, (core->tb_ecap5_dwbuart->dut->mem_read_data_q == sr));
  tb->check(COND_registers, (((tb->uart_sr() >> 3) & 0x1) == 0));

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;

  //=================================
  //      Tick (50)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();

  //=================================
  //      Tick (51)
  
  tb->tick();

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_ecap5_dwbuart.fe.01",
      tb->conditions[COND_mem],
      "Failed to integrate the memory", tb->err_cycles[COND_mem]);

  CHECK("tb_ecap5_dwbuart.fe.02",
      tb->conditions[COND_registers],
      "Failed to implement the memory-mapped registers", tb->err_cycles[COND_registers]);
}

void tb_ecap5_dwbuart_pe(TB_Ecap5_dwbuart * tb) {
  Vtb_ecap5_dwbuart * core = tb->core;
  core->testcase = T_PE;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      Set inputs

  // (2**16)/4 = 16384 = 1 bit every 4 clk cycles
  uint32_t cr = (16384 << 16) | (1 << 3) | 1;
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
  //      Tick (6-45)
  
  // 1 start bit, 8 data bits 
  uint32_t number_of_tx_bits = (1 + 8 + 1) * 4;
  // This call is shifted by 1 tick from the emission start
  // and we want to be before the sending of the parity bit (3 cycles in advance)
  tb->n_tick(number_of_tx_bits - 1 - 3);

  //=================================
  //      Tick (46-50)

  // Inject the frame error for 3/4 of a bit
  core->inj_parity_error = 1;
  tb->n_tick(3);
  core->inj_parity_error = 0;

  // Stop injecting and tick for 
  //   - 1 cycle for 1/4 of the previous bit
  //   - 2 cycles for half of the stop bit 
  //   - 1 cycle delay before the register is updated
  //   - 3 cycles delay due to the input registering
  tb->n_tick(1 + 2 + 1 + 3);

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_registers, (tb->uart_sr() >> 4) & 0x1);

  //`````````````````````````````````
  //      Set inputs

  tb->read(0x0);

  uint32_t sr = tb->uart_sr();

  //=================================
  //      Tick (49)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_mem, (core->tb_ecap5_dwbuart->dut->mem_read_data_q == sr));
  tb->check(COND_registers, (((tb->uart_sr() >> 3) & 0x1) == 0));

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;

  //=================================
  //      Tick (50)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();

  //=================================
  //      Tick (51)
  
  tb->tick();

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_ecap5_dwbuart.pe.01",
      tb->conditions[COND_mem],
      "Failed to integrate the memory", tb->err_cycles[COND_mem]);

  CHECK("tb_ecap5_dwbuart.pe.02",
      tb->conditions[COND_registers],
      "Failed to implement the memory-mapped registers", tb->err_cycles[COND_registers]);
}

void tb_ecap5_dwbuart_full_duplex(TB_Ecap5_dwbuart * tb) {
  Vtb_ecap5_dwbuart * core = tb->core;
  core->testcase = T_FULL_DUPLEX;

  //=================================
  //      Tick (0)
  
  tb->reset();

  //`````````````````````````````````
  //      set inputs

  uint32_t cr = (4 << 16) | (1 << 3) | 1;
  tb->write(0x4, cr);

  //=================================
  //      tick (1)
  
  tb->tick();

  //`````````````````````````````````
  //      set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;

  //=================================
  //      tick (2)
  
  tb->tick();

  //`````````````````````````````````
  //      set inputs
  
  tb->_nop();

  //=================================
  //      tick (3-46)
  
//  tb->send(0xFF, 4, 1, 0, 1, 0, 0);

  //=================================
  //      tick (47)
  
  tb->tick();

  //`````````````````````````````````
  //      set inputs

  tb->write(0xC, 0xAF);

  //=================================
  //      tick (48)
  
  tb->tick();

  //`````````````````````````````````
  //      set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;

//  core->uart_rx_i = 0;

  //=================================
  //      tick (49)
  
  tb->tick();

  //`````````````````````````````````
  //      set inputs
  
  core->wb_cyc_i = 0;

  //=================================
  //      Tick (50-52)
  
  tb->n_tick(3);

  //=================================
  //      Tick (53-84)
  
  uint32_t data = 0x5F;
  for(int i = 0; i < 8; i++) {
    //`````````````````````````````````
    //      Set inputs
    
    //core->uart_rx_i = ((data >> i) & 0x1);

    //=================================
    //      Tick (...)
    
    tb->n_tick(4);
  }

  //`````````````````````````````````
  //      Set inputs
  
  //core->uart_rx_i = 0;

  //=================================
  //      Tick (85-88)
  
  tb->n_tick(4);

  //`````````````````````````````````
  //      Set inputs
  
  //core->uart_rx_i = 0;

  //=================================
  //      Tick (89-92)
  
  tb->n_tick(4);

  //=================================
  //      Tick (93)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_rx, (core->tb_ecap5_dwbuart->dut->rx_valid == 1));
  tb->check(COND_tx, (core->tb_ecap5_dwbuart->dut->tx_done == 1));

  //=================================
  //      Tick (94)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_rx, (core->tb_ecap5_dwbuart->dut->rx_valid == 0));
  tb->check(COND_registers, (tb->uart_rxdr() == 0x5F) &&
                            (tb->uart_sr() == 0x1F));

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_ecap5_dwbuart.full_duplex.01",
      tb->conditions[COND_rx],
      "Failed to integrate the rx frontend", tb->err_cycles[COND_rx]);

  CHECK("tb_ecap5_dwbuart.full_duplex.02",
      tb->conditions[COND_tx],
      "Failed to integrate the tx frontend", tb->err_cycles[COND_tx]);

  CHECK("tb_ecap5_dwbuart.full_duplex.03",
      tb->conditions[COND_registers],
      "Failed to implement the memory-mapped registers", tb->err_cycles[COND_registers]);
}

int main(int argc, char ** argv, char ** env) {
  srand(time(NULL));
  Verilated::traceEverOn(true);

  bool verbose = parse_verbose(argc, argv);
  verbose = 1;

  TB_Ecap5_dwbuart * tb = new TB_Ecap5_dwbuart;
  tb->open_trace("waves/ecap5_dwbuart.vcd");
  tb->open_testdata("testdata/ecap5_dwbuart.csv");
  tb->set_debug_log(verbose);
  tb->init_conditions(__CondIdEnd);

  /************************************************************/

  tb_ecap5_dwbuart_idle(tb);

  tb_ecap5_dwbuart_write_cr(tb);
  tb_ecap5_dwbuart_write_txdr(tb);
  tb_ecap5_dwbuart_read_rxdr(tb);

  tb_ecap5_dwbuart_rxoe(tb);
  tb_ecap5_dwbuart_fe(tb);
  tb_ecap5_dwbuart_pe(tb);

  tb_ecap5_dwbuart_full_duplex(tb);

  /************************************************************/

  printf("[ECAP5_DWBUART]: ");
  if(tb->success) {
    printf("Done\n");
  } else {
    printf("Failed\n");
  }

  delete tb;
  exit(EXIT_SUCCESS);
}
