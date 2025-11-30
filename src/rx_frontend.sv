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

module rx_frontend #(
  parameter MIN_FRAME_SIZE = 8,
  parameter MAX_FRAME_SIZE = 11
)(
  input   logic         clk_i,
  input   logic         rst_i,

  input   logic[15:0]   cr_acc_incr_i,
  input   logic         cr_ds_i,
  input   logic[1:0]    cr_p_i,
  input   logic         cr_s_i,

  input   logic         uart_rx_i,

  output  logic[10:0]   frame_o,
  output  logic         parity_err_o,
  output  logic         frame_err_o,
  output  logic         output_valid_o
);

/*****************************************/
/*           Internal signals            */
/*****************************************/

typedef enum {
  IDLE,    // 0
  START,   // 1
  DATA     // 2
} state_t;
state_t state_d, state_q;

logic uart_rx_q, uart_rx_qq, uart_rx_qqq;

logic[16:0] baud_acc_d, baud_acc_q;
logic baud_acc_half_overflow, baud_acc_overflow;;

logic[MAX_FRAME_SIZE:0]  frame_bit_cnt_d, frame_bit_cnt_q;

logic[$clog2(MAX_FRAME_SIZE)-1:0] frame_size;
logic[$clog2(MAX_FRAME_SIZE)-1:0] frame_start_index;
logic frame_bit_cnt_done;
logic data_bit_cnt_done_d, data_bit_cnt_done_q;

/*****************************************/
/*            Output signals             */
/*****************************************/

// Computed parity
logic parity_d, parity_q;
// Received parity
logic parity_bit;

logic[MAX_FRAME_SIZE-1:0] frame_d, frame_q, frame_shifted0, frame_shifted;

/*****************************************/

always_comb begin : state_machine
  state_d = state_q;

  case(state_q)
    IDLE: begin
      // Wait for the beginning of the start bit
      if(uart_rx_qq == 0) begin
        state_d = START;
      end
    end
    START: begin
      // Wait for the middle of the start bit
      if(baud_acc_half_overflow) begin
        state_d = DATA;
      end
    end
    DATA: begin
      // Wait for the frame to be received
      if(frame_bit_cnt_done) begin
        state_d = IDLE;
      end
    end
    default: begin end
  endcase
end

always_comb begin : sampling
  baud_acc_d = 0;
  frame_bit_cnt_d = frame_bit_cnt_q;
  frame_d = frame_q;
  parity_d = parity_q;

  // The frame size is computed based on the given configuration
  frame_size = MIN_FRAME_SIZE + {3'b0, cr_ds_i} + {2'b0, (cr_p_i == '0 ? 1'b0 : 1'b1)} + {3'b0, cr_s_i};
  // Index of bit0 in the frame_q shift register
  frame_start_index = MAX_FRAME_SIZE - frame_size;
  // A frame is terminated when this bit is set
  frame_bit_cnt_done = frame_bit_cnt_q[frame_size];
  // The data field of the frame is terminated when this bit is set
  data_bit_cnt_done_d = data_bit_cnt_done_q | (cr_ds_i ? frame_bit_cnt_q[8] : frame_bit_cnt_q[7]);

  parity_bit = cr_ds_i ? frame_shifted[8] : frame_shifted[7];

  // Baudrate accumulator overflow
  baud_acc_overflow = baud_acc_q[16];
  baud_acc_half_overflow = baud_acc_q[15];

  case(state_q)
    IDLE: begin
      // We initialize the baud_rate counter at the start of the start bit
      if(uart_rx_qq == 0) begin
        // This is initialized to (2**15) as we want it to overflow in half the baud period
        // so that we sample in the middle of the bits
        baud_acc_d = {2'b0, cr_acc_incr_i[14:0]};
      end
    end
    START: begin
      baud_acc_d = {2'b0, baud_acc_q[14:0]} + {2'b0, cr_acc_incr_i[14:0]};
      // We initialize the data counter when reaching the middle of the start bit
      // This is reached when the 15th bit is set.
      if(baud_acc_half_overflow) begin
        // It takes one cycle for logic to detect this counter is null
        // The counter is therefore initialized to acc_incr (1 cycle)
        baud_acc_d = {1'b0, cr_acc_incr_i[15:0]};
        // Initialize the frame size ring counter
        frame_bit_cnt_d[0] = 1'b1;
        // Initialize the parity bit with the parity configuration bit
        // the parity is still computed when disabled but shall be ignored by the user
        parity_d = cr_p_i[0];
      end
    end
    DATA: begin
      if(baud_acc_overflow) begin
        // When the baud interval has elapsed
        //   1. decrement the frame bit counter
        //   1. sample the input
        //   1. update the computed parity
        frame_bit_cnt_d = {frame_bit_cnt_d[MAX_FRAME_SIZE-1:0], 1'b0};
        frame_d = {uart_rx_qqq, frame_q[10:1]};
        parity_d = parity_q ^ (uart_rx_qqq & (~data_bit_cnt_done_q));
      end
      baud_acc_d = {1'b0, baud_acc_q[15:0]} + cr_acc_incr_i;

      // Reset the counter as it will not be incremented further
      if(frame_bit_cnt_done) begin
        frame_bit_cnt_d = '0;
        data_bit_cnt_done_d = 0;
      end
    end
    default: begin end
  endcase
end

always_comb begin : frame_align
  // Barrel shifter to align the frame_q shift register output
  frame_shifted0 = frame_start_index[0] ? {1'b0, frame_q[MAX_FRAME_SIZE-1:1]} : frame_q;
  frame_shifted  = frame_start_index[1] ? {2'b0, frame_shifted0[MAX_FRAME_SIZE-1:2]} : frame_shifted0;
end

always_ff @(posedge clk_i) begin
  if(rst_i) begin
    state_q         <= IDLE;

    uart_rx_q       <= 1;
    uart_rx_qq      <= 1;
    uart_rx_qqq     <= 1;

    baud_acc_q          <= '0;
    frame_q             <= '0;
    frame_bit_cnt_q     <= '0;
    data_bit_cnt_done_q <=  0;
    parity_q            <=  0;
  end else begin
    state_q <= state_d;

    // The receive input is registered three times to prevent
    // metastability issues
    uart_rx_q   <= uart_rx_i;
    uart_rx_qq  <= uart_rx_q;
    uart_rx_qqq <= uart_rx_qq;

    // baudrate counter used to sample the serial signal
    baud_acc_q <= baud_acc_d;

    // The frame shift register
    frame_q <= frame_d;
    
    // The counter used to detect the end of frame
    frame_bit_cnt_q <= frame_bit_cnt_d;

    // Signal used to end the parity computation
    data_bit_cnt_done_q <= data_bit_cnt_done_d;

    // Computed parity
    parity_q <= parity_d;
  end
end

/*****************************************/
/*         Assign output signals         */
/*****************************************/

assign frame_o = frame_shifted;
// A parity error is detected when
//  - The computed parity is different than the received parity bit
//  - Parity detection is enabled
assign parity_err_o = (parity_q ^ parity_bit) & (cr_p_i[0] | cr_p_i[1]);
assign frame_err_o = (frame_q[MAX_FRAME_SIZE-1] == 0);
assign output_valid_o = frame_bit_cnt_done;

endmodule // rx_frontend
