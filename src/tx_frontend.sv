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

module tx_frontend #(
  parameter MIN_FRAME_SIZE = 8,
  parameter MAX_FRAME_SIZE = 11
)(
  input   logic         clk_i,
  input   logic         rst_i,

  input   logic[15:0]   cr_acc_incr_i,
  input   logic         cr_ds_i,
  input   logic[1:0]    cr_p_i,
  input   logic         cr_s_i,

  input   logic         transmit_i,
  input   logic[7:0]    dr_i,

  output  logic         done_o,

  output  logic         uart_tx_o
);

/*****************************************/
/*           Internal signals            */
/*****************************************/

typedef enum {
  IDLE,     // 0
  START,    // 1
  DATA,     // 2
  PARITY,   // 3
  STOP      // 4
} state_t;
state_t state_d, state_q;

// Baudrate accumulator
logic[16:0] baud_acc_d, baud_acc_q;
logic baud_acc_overflow;

// Frame bit counter
logic[$size(dr_i)-1:0] bit_cnt_d, bit_cnt_q;

// Data shift-register
logic[$size(dr_i)-1:0] dr_d, dr_q;

logic parity_d, parity_q;

/*****************************************/
/*            Output signals             */
/*****************************************/

logic uart_tx_d;

logic done_d, done_q;

/*****************************************/

always_comb begin : baudrate_generation
  // Reset the baud accumulator when in the IDLE state
  if(state_q == IDLE && transmit_i) begin
    baud_acc_d = 0;
  end else begin
    // Increment the accumulator
    // In this case, baud_acc_q[15] is not used as we want this bit
    // to be set to one only when the increment overflows.
    // In that case, on the next cycle this bit is not used but the 
    // remaining bits are.
    baud_acc_d = {1'b0, baud_acc_q[15:0]} + {1'b0, cr_acc_incr_i};
  end
  baud_acc_overflow = baud_acc_d[16];
end

always_comb begin : state_machine
  state_d = state_q;

  case(state_q)
    IDLE: begin
      // If a transmit is initiated
      if(transmit_i) begin
        state_d = START;
      end 
    end
    START: begin
      // Wait for a baud interval (1-bit)
      if(baud_acc_overflow) begin
        state_d = DATA;
      end
    end
    DATA: begin
      // Wait for the end of the last baud interval (all data-bits)
      if(baud_acc_overflow && bit_cnt_q[0]) begin
        // Bypass the parity bit based on configuration
        if(cr_p_i == '0) begin
          state_d = STOP;
        end else begin
          state_d = PARITY;
        end
      end
    end
    PARITY: begin
      // Wait for a baud interval (1-bit)
      if(baud_acc_overflow) begin
        state_d = STOP;
      end
    end
    STOP: begin
      // Wait for the last baud interval (n stop bits)
      if(baud_acc_overflow && bit_cnt_q[0]) begin
        state_d = IDLE;
      end
    end
    default: begin end
  endcase
end

always_comb begin : transmit
  dr_d = dr_q;

  uart_tx_d = 1;

  bit_cnt_d = bit_cnt_q;
  parity_d = parity_q;

  case(state_q)
    IDLE: begin
      // If a transmit is initiated
      if(transmit_i) begin
        // Initialize the number of bits to send
        bit_cnt_d = cr_ds_i ? (1 << 7) : (1 << 6);
        // Initialize the data shift register
        dr_d = dr_i;
        // Initialize the parity
        parity_d = cr_p_i[0];
      end
    end
    START: begin
      uart_tx_d = 1'b0;
    end 
    DATA: begin
      uart_tx_d = dr_q[0];

      // After each data bit is sent (one baud period)
      if(baud_acc_overflow) begin
        // Updates the number of remaining data bits
        bit_cnt_d = {1'b0, bit_cnt_q[$size(dr_i)-1:1]};
        // Shift the data register
        dr_d = {1'b0, dr_q[$size(dr_i)-1:1]};

        // Update the parity with the sent bit
        parity_d = parity_q ^ dr_q[0];

        // Set the number of stop bits for the stop state
        if(bit_cnt_q[0]) begin
          bit_cnt_d = cr_s_i ? (1 << 1) : (1 << 0);
        end
      end
    end
    PARITY: begin
      uart_tx_d = parity_q;
    end
    STOP: begin
      uart_tx_d = 1'b1;

      // After each stop bit is sent (one baud period)
      if(baud_acc_overflow) begin
        // Update the number of remaining stop bits
        bit_cnt_d = {1'b0, bit_cnt_q[$size(dr_i)-1:1]};
      end
    end
    default: begin end
  endcase
end

always_comb begin : done
  done_d = 0;
  // The done signal is asserted while 
  //   - in the stop STATE
  //   - at the end of a baud interval
  //   - during the last stop bit
  if(state_q == STOP && baud_acc_overflow && bit_cnt_q[0]) begin
    done_d = 1;
  end
end

always_ff @(posedge clk_i) begin
  if(rst_i) begin
    state_q <= IDLE;

    dr_q <= '0;
    baud_acc_q <= 0;
    bit_cnt_q <= 0;
    parity_q <= 0;

    done_q <= 0;
  end else begin
    state_q <= state_d;

    // Shift register for the dr_i signal
    dr_q <= dr_d;

    // baudrate counter used to sample the serial signal
    baud_acc_q <= baud_acc_d;

    // Computed parity
    parity_q <= parity_d;

    // The bit counter used to detect the end of multi-bit transmit states
    bit_cnt_q <= bit_cnt_d;

    // Asserted to indicate the end of a transmission
    done_q <= done_d;
  end
end

/*****************************************/
/*         Assign output signals         */
/*****************************************/

assign uart_tx_o = rst_i ? 1 : uart_tx_d;
assign done_o = done_q;

endmodule // tx_frontend
