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
  T_RACE_SR               = 8,
  T_RACE_TXDR             = 9,
  T_RACE_RXDR             = 10,
  T_ERROR_RETENTION       = 11
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

  void generate_read() {
    // (2**16)/4 = 16384 = 1 bit every 4 clk cycles
    uint32_t cr = (16384 << 16) | (1 << 3) | 1;
    this->write(0x4, cr);
    this->tick();

    this->_nop();
    this->core->wb_cyc_i = 1;
    this->tick();

    this->_nop();
    this->tick();

    this->write(0xC, 0xA5FA5FA5);
    this->tick();

    this->_nop();
    this->core->wb_cyc_i = 1;
    this->tick();

    this->_nop();
    this->core->wb_cyc_i = 0;
    // 1 start bit, 8 data bits, 1 parity bit, 0.5 stop bit
    // the receive is valid at the half of the stop bit
    uint32_t number_of_tx_bits = (1 + 8 + 1 + 1 + 0.5) * 4;
    // This call is shifted by 1 tick from the emission start,
    // and this function shall put its caller the cycle before the valid signal is asserted
    this->n_tick(number_of_tx_bits - 2);
  }

  void generate_fe() {
    // (2**16)/4 = 16384 = 1 bit every 4 clk cycles
    uint32_t cr = (16384 << 16) | (1 << 3) | 1;
    this->write(0x4, cr);

    this->tick();

    this->_nop();
    this->core->wb_cyc_i = 1;

    this->tick();

    this->_nop();

    this->tick();

    // Send the write data
    this->write(0xC, 0xA5FA5FA5);

    this->tick();

    this->_nop();
    this->core->wb_cyc_i = 1;

    this->tick();

    this->_nop();
    this->core->wb_cyc_i = 0;

    // 1 start bit, 8 data bits, 1 parity bit
    // the receive is valid at the half of the stop bit
    uint32_t number_of_tx_bits = (1 + 8 + 1 + 1) * 4;
    // This call is shifted by 1 tick from the emission start
    // and we want to be before the sending of the stop bit (3 cycles in advance)
    this->n_tick(number_of_tx_bits - 1 - 3);

    // Inject the frame error
    this->core->inj_frame_error = 1;
    this->n_tick(3 + 2);
    this->core->inj_frame_error = 0;
  }

  void generate_pe() {
    // (2**16)/4 = 16384 = 1 bit every 4 clk cycles
    uint32_t cr = (16384 << 16) | (1 << 3) | 1;
    this->write(0x4, cr);

    this->tick();

    this->_nop();
    this->core->wb_cyc_i = 1;
    this->tick();

    this->_nop();
    this->tick();

    this->write(0xC, 0xA5FA5FA5);
    this->tick();

    this->_nop();
    this->core->wb_cyc_i = 1;
    this->tick();

    this->_nop();
    this->core->wb_cyc_i = 0;

    // 1 start bit, 8 data bits 
    uint32_t number_of_tx_bits = (1 + 8 + 1) * 4;
    // This call is shifted by 1 tick from the emission start
    // and we want to be before the sending of the parity bit (3 cycles in advance)
    this->n_tick(number_of_tx_bits - 1 - 3);

    // Inject the frame error for 3/4 of a bit
    this->core->inj_parity_error = 1;
    this->n_tick(3);
    this->core->inj_parity_error = 0;

    // Stop injecting and tick for 
    //   - 1 cycle for 1/4 of the previous bit
    //   - 2 cycles for half of the stop bit 
    //   - 3 cycles delay due to the input registering
    this->n_tick(1 + 2 + 3);
  }

  void generate_rxoe() {
    // (2**16)/4 = 16384 = 1 bit every 4 clk cycles
    uint32_t cr = (16384 << 16) | (1 << 3) | 1;
    this->write(0x4, cr);

    this->tick();

    this->_nop();
    this->core->wb_cyc_i = 1;
    this->tick();

    this->_nop();
    this->tick();

    this->write(0xC, 0xA5FA5FA5);
    this->tick();

    this->_nop();
    this->core->wb_cyc_i = 1;
    this->tick();

    this->_nop();
    this->core->wb_cyc_i = 0;
    // 1 start bit, 8 data bits, 1 parity bit, 0.5 stop bit
    // the receive is valid at the half of the stop bit
    uint32_t number_of_tx_bits = (1 + 8 + 1 + 1 + 0.5) * 4;
    // This call is shifted by 1 tick from the emission start,
    this->n_tick(number_of_tx_bits - 1);

    this->write(0xC, 0xA5FAA5FA);
    this->tick();

    this->_nop();
    this->core->wb_cyc_i = 1;
    this->tick();

    this->_nop();
    this->core->wb_cyc_i = 0;
    // and this function shall put its caller the cycle before the valid signal is asserted
    this->n_tick(number_of_tx_bits - 2);
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

  //=================================
  //      Tick (1-49)
  
  tb->generate_read();

  //=================================
  //      Tick (50)
  
  tb->tick();

  //=================================
  //      Tick (51)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_rx, (core->tb_ecap5_dwbuart->dut->rx_valid == 1));

  //=================================
  //      Tick (52)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_rx, (core->tb_ecap5_dwbuart->dut->rx_valid == 0));
  tb->check(COND_registers, (tb->uart_rxdr() == 0xA5) &&
                            (tb->uart_sr() & 0x1));

  //`````````````````````````````````
  //      Set inputs
  
  tb->read(0x8);

  //=================================
  //      Tick (53)
  
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

  //=================================
  //      Tick (1-96)
  
  tb->generate_rxoe();

  //=================================
  //      Tick (97)
  
  tb->tick();

  //=================================
  //      Tick (98)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_rx, (core->tb_ecap5_dwbuart->dut->rx_valid == 1));

  //=================================
  //      Tick (99)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_registers, ((tb->uart_sr() >> 2) & 0x1) &&
                             (tb->uart_rxdr() == 0xFA));

  //`````````````````````````````````
  //      Set inputs

  tb->read(0x8);

  //=================================
  //      Tick (100)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_rx, (core->tb_ecap5_dwbuart->dut->rx_valid == 0));
  tb->check(COND_mem, (core->tb_ecap5_dwbuart->dut->mem_read_data_q == 0xFA));
  tb->check(COND_registers, ((tb->uart_sr() >> 2) & 0x1) &&
                             (tb->uart_rxdr() == 0x0));

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;

  //=================================
  //      Tick (101)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();

  //=================================
  //      Tick (102)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs

  tb->read(0x0);

  uint32_t sr = tb->uart_sr();

  //=================================
  //      Tick (103)
  
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
  //      Tick (104)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();

  //=================================
  //      Tick (105)
  
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

  //=================================
  //      Tick (1-47)
  
  tb->generate_fe();

  //=================================
  //      Tick (48)
  
  tb->tick();

  //=================================
  //      Tick (49)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs

  tb->read(0x0);

  uint32_t sr = tb->uart_sr();

  //=================================
  //      Tick (50)
  
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
  //      Tick (51)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();

  //=================================
  //      Tick (52)
  
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

  //=================================
  //      Tick (1-47)
  
  tb->generate_pe();

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

/**
 * @brief Generate all possible errors one by one and then
 *        generate a successfull read.
 *        The errors are expected to have accumulated.
 */
void tb_ecap5_dwbuart_error_retention(TB_Ecap5_dwbuart * tb) {
  Vtb_ecap5_dwbuart * core = tb->core;
  core->testcase = T_ERROR_RETENTION;

  //=================================
  //      Tick (0)
  
  tb->reset();

  // Generate the three errors
  tb->generate_rxoe();
  tb->tick();
  tb->generate_fe();
  tb->tick();
  tb->generate_pe();
  tb->tick();

  // Send a clean frame
  tb->generate_read();
  tb->tick();

  //=================================
  //      Tick (250)

  tb->tick();
  
  //`````````````````````````````````
  //      Checks 
  
  // Check that every errors is still present
  tb->check(COND_registers, ((tb->uart_sr() >> 2) & 0x7) == 0x7);

  //=================================
  //      Tick (251)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->read(0x0);
  uint32_t sr = tb->uart_sr();

  //=================================
  //      Tick (252)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_mem, (core->tb_ecap5_dwbuart->dut->mem_read_data_q == sr));
  tb->check(COND_registers, (((tb->uart_sr() >> 2) & 0x7) == 0));

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;

  //=================================
  //      Tick (253)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();

  //=================================
  //      Tick (254)
  
  tb->tick();

  //`````````````````````````````````
  //      Formal Checks 

  CHECK("tb_ecap5_dwbuart.error_retention.01",
      tb->conditions[COND_mem],
      "Failed to integrate the memory", tb->err_cycles[COND_mem]);

  CHECK("tb_ecap5_dwbuart.error_retention.02",
      tb->conditions[COND_registers],
      "Failed to implement the memory-mapped registers", tb->err_cycles[COND_registers]);
}

/**
 * @brief Read SR at the same time as status bits are written to it
 *        Instead of reseting SR after reading, the bits are properly written
 */
void tb_ecap5_dwbuart_race_sr(TB_Ecap5_dwbuart * tb) {
  Vtb_ecap5_dwbuart * core = tb->core;
  core->testcase = T_RACE_SR;

  //=================================
  //      Tick (0)
  
  tb->reset();

  tb->generate_pe();

  //`````````````````````````````````
  //      Set inputs

  tb->read(0x0);

  //=================================
  //      Tick (51)
  
  tb->tick();

  //=================================
  //      Tick (52)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_mem, ((core->tb_ecap5_dwbuart->dut->mem_read_data_q >> 4) & 0x1) == 0);
  tb->check(COND_registers, (((tb->uart_sr() >> 4) & 0x1) == 1));

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;

  //=================================
  //      Tick (53)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();

  //=================================
  //      Tick (54)
  
  tb->tick();
  
  //`````````````````````````````````
  //      Set inputs

  tb->read(0x0);

  //=================================
  //      Tick (55)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_mem, ((core->tb_ecap5_dwbuart->dut->mem_read_data_q >> 4) & 0x1) == 1);
  tb->check(COND_registers, (((tb->uart_sr() >> 4) & 0x1) == 0));

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;

  //=================================
  //      Tick (56)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();

  //=================================
  //      Tick (57)
  
  tb->tick();

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_ecap5_dwbuart.race_sr.01",
      tb->conditions[COND_mem],
      "Failed to integrate the memory", tb->err_cycles[COND_mem]);

  CHECK("tb_ecap5_dwbuart.race_sr.02",
      tb->conditions[COND_registers],
      "Failed to implement the memory-mapped registers", tb->err_cycles[COND_registers]);
}

/**
 * @brief Write to TXDR at the same time as the previous transfer is done
 *        Instead of setting TXE, TXE shall stay low
 */
void tb_ecap5_dwbuart_race_txdr(TB_Ecap5_dwbuart * tb) {
  Vtb_ecap5_dwbuart * core = tb->core;
  core->testcase = T_RACE_TXDR;

  //=================================
  //      Tick (0)
  
  tb->reset();

  tb->generate_read();

  //=================================
  //      Tick (50)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->write(0xC, 0xA5FA5FA5);

  //=================================
  //      Tick (51)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  tb->check(COND_registers, ((tb->uart_sr() & 0x2) == 0));
  tb->check(COND_registers, (tb->uart_txdr() == 0x000000A5));
  tb->check(COND_tx,        (core->tb_ecap5_dwbuart->dut->tx_transmit_q == 1));

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

  tb->tick();

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_ecap5_dwbuart.race_txdr.01",
      tb->conditions[COND_rx],
      "Failed to integrate the rx frontend", tb->err_cycles[COND_rx]);

  CHECK("tb_ecap5_dwbuart.race_txdr.02",
      tb->conditions[COND_tx],
      "Failed to integrate the tx frontend", tb->err_cycles[COND_tx]);

  CHECK("tb_ecap5_dwbuart.race_txdr.03",
      tb->conditions[COND_registers],
      "Failed to implement the memory-mapped registers", tb->err_cycles[COND_registers]);
}

void tb_ecap5_dwbuart_race_rxdr(TB_Ecap5_dwbuart * tb) {
  Vtb_ecap5_dwbuart * core = tb->core;
  core->testcase = T_RACE_RXDR;

  //=================================
  //      Tick (0)
  
  tb->reset();

  tb->generate_read();

  //=================================
  //      Tick (50)
  
  tb->tick();

  //=================================
  //      Tick (51)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->read(0x8);

  //=================================
  //      Tick (52)
  
  tb->tick();

  //`````````````````````````````````
  //      Checks 
  
  // Expected to be true even if a read was performed
  tb->check(COND_registers, (tb->uart_sr() & 0x1) == 1);

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();
  core->wb_cyc_i = 1;

  //=================================
  //      Tick (53)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();

  //=================================
  //      Tick (54)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->read(0x8);

  //=================================
  //      Tick (55)
  
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
  //      Tick (56)
  
  tb->tick();

  //`````````````````````````````````
  //      Set inputs
  
  tb->_nop();

  //=================================
  //      Tick (57)
  
  tb->tick();

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_ecap5_dwbuart.race_rxdr.01",
      tb->conditions[COND_mem],
      "Failed to integrate the memory", tb->err_cycles[COND_mem]);

  CHECK("tb_ecap5_dwbuart.race_rxdr.02",
      tb->conditions[COND_registers],
      "Failed to implement the memory-mapped registers", tb->err_cycles[COND_registers]);
}

int main(int argc, char ** argv, char ** env) {
  srand(time(NULL));
  Verilated::traceEverOn(true);

  bool verbose = parse_verbose(argc, argv);

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

  tb_ecap5_dwbuart_error_retention(tb);
  tb_ecap5_dwbuart_race_sr(tb);
  tb_ecap5_dwbuart_race_txdr(tb);
  tb_ecap5_dwbuart_race_rxdr(tb);

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
