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

typedef struct {
  uint32_t baudrate;
  uint32_t data;
  uint8_t ds;
  uint8_t p;
  uint8_t s;
  uint8_t inject_frame_error;
  uint8_t inject_parity_error;
} test_configuration_t;

class TB_Rx_frontend : public Testbench<Vtb_rx_frontend> {
public:
  uint32_t accumulator_increment;
  uint32_t accumulator;

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
    core->cr_acc_incr_i = 0;
    core->cr_ds_i = 0;
    core->cr_p_i = 0;
    core->cr_s_i = 0;
  }
  
  void set_injected_baudrate(uint32_t acc_increment) {
    this->accumulator_increment = acc_increment;
    this->accumulator = 0;
  }

  uint32_t get_cycles_before_next_accumulator_overflow() {
    uint32_t cycles = 0;
    while(this->accumulator < (1 << 16)) {
      cycles += 1;
      this->accumulator += this->accumulator_increment;
    }

    this->accumulator = this->accumulator & ((1 << 16) - 1);

    return cycles;
  }

  void test_with_injected_frame(test_configuration_t config) {
    this->core->cr_acc_incr_i = ((double)config.baudrate * (1 << 16)) / 24000000;
    this->core->cr_ds_i = config.ds;
    this->core->cr_p_i = config.p;
    this->core->cr_s_i = config.s;

    this->tick();

    set_injected_baudrate(this->core->cr_acc_incr_i);

    // Send start bit
    this->core->uart_rx_i = 0;

    uint32_t cycles_to_run = get_cycles_before_next_accumulator_overflow();
    uint32_t sampling_offset = 0;
    for(int i = 0; i < cycles_to_run; i++) {
      this->tick();

      // Count the number of cycles before sampling the first bit
      if(core->tb_rx_frontend->dut->state_q == S_START) {
        sampling_offset += 1;
      }
    }
    
    // Send data bits
    uint32_t num_data_bits = config.ds ? 8 : 7;
    for(int i = 0; i < num_data_bits; i++) {
      this->core->uart_rx_i = (config.data >> i) & 1;

      cycles_to_run = get_cycles_before_next_accumulator_overflow();
      this->n_tick(cycles_to_run);
    }

    // Send parity bit
    uint32_t num_parity_bits = config.p ? 1 : 0;
    uint8_t parity = (config.p == 2) ? 0 : 1;
    if(config.p) {
      // Compute parity, starting with either 1 or 0 depending on ODD or EVEN
      for(int j = 0; j < num_data_bits; j++) {
        parity ^= (config.data >> j) & 1;
      }

      this->core->uart_rx_i = parity & 1;

      // Inject a parity error
      this->core->uart_rx_i ^= config.inject_parity_error; 

      cycles_to_run = get_cycles_before_next_accumulator_overflow();
      this->n_tick(cycles_to_run);
    }

    // Send stop bits
    uint32_t num_stop_bits = config.s ? 2 : 1;

    this->core->uart_rx_i = 1;
    
    // Inject a frame error
    this->core->uart_rx_i ^= config.inject_frame_error;

    for(int i = 0; i < num_stop_bits; i++) {
      // If this is the last stop bit
      if(i == (num_stop_bits - 1)) {
        // Send half of the first cycle + 3 as the used signal is delayed by three
        // clock cycles. Ceil is as sometimes cycles to run is not equal to the first
        // cycle which created the sampling offset.
        // This is done in order to catch the tx_done signal
        cycles_to_run = get_cycles_before_next_accumulator_overflow();
        this->n_tick(sampling_offset + 3);

        // Check the result
        uint32_t stop_bits = config.s ? 3 : 1;
        uint32_t parity_bit = config.p ? parity : 0;
        uint32_t data_bits = config.data & ((1 << num_data_bits) - 1);
        uint32_t expected_frame = (stop_bits << (num_parity_bits + num_data_bits)) | (parity_bit << num_data_bits) | data_bits;
        this->check(COND_frame,  (core->frame_o == expected_frame));
        this->check(COND_errors, (core->parity_err_o == config.inject_parity_error) &&
                                 (core->frame_err_o == config.inject_frame_error));
        this->check(COND_valid, (core->output_valid_o == 1));

        // Send the remaining of the first stop bit
        this->n_tick(cycles_to_run - sampling_offset - 3);
      } else {
        cycles_to_run = get_cycles_before_next_accumulator_overflow();
        this->n_tick(cycles_to_run);
      }
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
  //      Checks 
  
  test_configuration_t config = {
    .baudrate = 2500000,
    .data = 0b10100101,
    .ds = 0,
    .p = 0,
    .s = 0,
    .inject_frame_error = 0,
    .inject_parity_error = 0
  };
  tb->test_with_injected_frame(config);

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
  //      Checks 
  
  test_configuration_t config = {
    .baudrate = 2500000,
    .data = 0b10100101,
    .ds = 0,
    .p = 0,
    .s = 1,
    .inject_frame_error = 0,
    .inject_parity_error = 0
  };
  tb->test_with_injected_frame(config);
  
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
  //      Checks 
  
  test_configuration_t config = {
    .baudrate = 2500000,
    .data = 0b10100101,
    .ds = 0,
    .p = 2,
    .s = 0,
    .inject_frame_error = 0,
    .inject_parity_error = 0
  };
  tb->test_with_injected_frame(config);

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
  //      Checks 
  
  test_configuration_t config = {
    .baudrate = 2500000,
    .data = 0b10100101,
    .ds = 0,
    .p = 2,
    .s = 1,
    .inject_frame_error = 0,
    .inject_parity_error = 0
  };
  tb->test_with_injected_frame(config);
  
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
  //      Checks 
  
  test_configuration_t config = {
    .baudrate = 2500000,
    .data = 0b10100101,
    .ds = 0,
    .p = 1,
    .s = 0,
    .inject_frame_error = 0,
    .inject_parity_error = 0
  };
  tb->test_with_injected_frame(config);

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
  //      Checks 
  
  test_configuration_t config = {
    .baudrate = 2500000,
    .data = 0b10100101,
    .ds = 0,
    .p = 1,
    .s = 1,
    .inject_frame_error = 0,
    .inject_parity_error = 0
  };
  tb->test_with_injected_frame(config);

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
  //      Checks 
  
  test_configuration_t config = {
    .baudrate = 2500000,
    .data = 0b10100101,
    .ds = 1,
    .p = 0,
    .s = 0,
    .inject_frame_error = 0,
    .inject_parity_error = 0
  };
  tb->test_with_injected_frame(config);

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
  //      Checks 
  
  test_configuration_t config = {
    .baudrate = 2500000,
    .data = 0b10100101,
    .ds = 1,
    .p = 0,
    .s = 1,
    .inject_frame_error = 0,
    .inject_parity_error = 0
  };
  tb->test_with_injected_frame(config);

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
  //      Checks 
  
  test_configuration_t config = {
    .baudrate = 2500000,
    .data = 0b10100101,
    .ds = 1,
    .p = 2,
    .s = 0,
    .inject_frame_error = 0,
    .inject_parity_error = 0
  };
  tb->test_with_injected_frame(config);

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
  //      Checks 
  
  test_configuration_t config = {
    .baudrate = 2500000,
    .data = 0b10100101,
    .ds = 1,
    .p = 2,
    .s = 1,
    .inject_frame_error = 0,
    .inject_parity_error = 0
  };
  tb->test_with_injected_frame(config);

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
  //      Checks 
  
  test_configuration_t config = {
    .baudrate = 2500000,
    .data = 0b10100101,
    .ds = 1,
    .p = 1,
    .s = 0,
    .inject_frame_error = 0,
    .inject_parity_error = 0
  };
  tb->test_with_injected_frame(config);

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
  //      Checks 
  
  test_configuration_t config = {
    .baudrate = 2500000,
    .data = 0b10100101,
    .ds = 1,
    .p = 1,
    .s = 1,
    .inject_frame_error = 0,
    .inject_parity_error = 0
  };
  tb->test_with_injected_frame(config);

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

void tb_rx_frontend_parity_even(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
  core->testcase = T_PARITY_EVEN;

  //=================================
  //      Tick (0)
  
  tb->reset();

  test_configuration_t config = {
    .baudrate = 2500000,
    .data = 0b10100101,
    .ds = 0,
    .p = 2,
    .s = 0,
    .inject_frame_error = 0,
    .inject_parity_error = 1
  };
  tb->test_with_injected_frame(config);

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

  test_configuration_t config = {
    .baudrate = 2500000,
    .data = 0b10100101,
    .ds = 0,
    .p = 1,
    .s = 0,
    .inject_frame_error = 0,
    .inject_parity_error = 1
  };
  tb->test_with_injected_frame(config);

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
  
  tb->reset();

  test_configuration_t config = {
    .baudrate = 2500000,
    .data = 0b10100101,
    .ds = 0,
    .p = 0,
    .s = 0,
    .inject_frame_error = 1,
    .inject_parity_error = 0
  };
  tb->test_with_injected_frame(config);

  //`````````````````````````````````
  //      Formal Checks 
  
  CHECK("tb_rx_frontend.framing.01",
      tb->conditions[COND_state],
      "Failed to implement the state machine", tb->err_cycles[COND_state]);

  CHECK("tb_rx_frontend.framing.02",
      tb->conditions[COND_errors],
      "Failed to implement the errors computation", tb->err_cycles[COND_errors]);

  CHECK("tb_rx_frontend.framing.03",
      tb->conditions[COND_valid],
      "Failed to implement the valid signal", tb->err_cycles[COND_valid]);
}

void tb_rx_frontend_baudrate(TB_Rx_frontend * tb) {
  Vtb_rx_frontend * core = tb->core;
  core->testcase = T_BAUDRATE;

  tb->reset();

  uint32_t baudrates[] = {
    4800,
    9600,
    19200,
    38400,
    57600,
    115200,
    230400,
    460800,
    921600,
    1000000,
    2000000,
    3000000
  };
  size_t num_baudrates = sizeof(baudrates)/sizeof(uint32_t);

  for(size_t i = 0; i < num_baudrates; i++) {
    test_configuration_t config = {
      .baudrate = baudrates[i],
      .data = 0b10100101,
      .ds = 1,
      .p = 1,
      .s = 1,
      .inject_frame_error = 0,
      .inject_parity_error = 0
    };
    tb->test_with_injected_frame(config);
  }

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

int main(int argc, char ** argv, char ** env) {
  srand(time(NULL));
  Verilated::traceEverOn(true);

  bool verbose = parse_verbose(argc, argv);

  TB_Rx_frontend * tb = new TB_Rx_frontend;
  tb->open_trace("waves/rx_frontend.vcd");
  tb->open_testdata("testdata/rx_frontend.csv");
  tb->set_debug_log(verbose);
  tb->init_conditions(__CondIdEnd);

  // 24MHz
  tb->clk_period_in_ps = 41667;

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

  tb_rx_frontend_parity_even(tb);
  tb_rx_frontend_parity_odd(tb);
  tb_rx_frontend_framing(tb);

  tb_rx_frontend_baudrate(tb);

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
