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

module rx_frontend
(
  input   logic         clk_i,
  input   logic         rst_i,

  input   logic[15:0]   cr_clk_div_i,
  input   logic         cr_ds_i,
  input   logic[1:0]    cr_p_i,
  input   logic         cr_s_i,
  input   logic         uart_rx_i,

  output  logic[10:0]   data_o,
  output  logic         output_valid_o
);
// The minimum packet size is :
//   - 7 data bits
//   - 1 stop bit
localparam MIN_PACKET_SIZE = 8;

// The maximum packet size is :
//   - 8 data bits
//   - 1 parity bit
//   - 2 stop bit
localparam MAX_PACKET_SIZE = 11;

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

logic[15:0] half_baud_cnt_d, half_baud_cnt_q, 
                 baud_cnt_d,      baud_cnt_q;

logic[MAX_PACKET_SIZE:0]  data_cnt_d, data_cnt_q;

logic[MAX_PACKET_SIZE-1:0] data_q, data_shifted0, data_shifted;

logic[$clog2(MAX_PACKET_SIZE)-1:0] packet_size;
logic[$clog2(MAX_PACKET_SIZE)-1:0] data_start_index;
logic data_cnt_done;

/*****************************************/

always_comb begin
  // The packet size is computed based on the given configuration
  packet_size = MIN_PACKET_SIZE + {3'b0, cr_ds_i} + {2'b0, (cr_p_i == '0 ? 1'b0 : 1'b1)} + {3'b0, cr_s_i};
  // Index of bit0 in the data_q shift register
  data_start_index = MAX_PACKET_SIZE - packet_size;
  // A packet is terminated when this bit is set
  data_cnt_done = data_cnt_q[packet_size];
end
   
always_comb begin : state_machine
  state_d = state_q;

  case(state_q)
    IDLE: begin
      // Wait for the beginning of the start bit
      if(uart_rx_qqq == 0) begin
        state_d = START;
      end
    end
    START: begin
      // Wait for the middle of the start bit
      if(half_baud_cnt_q == '0) begin
        state_d = DATA;
      end
    end
    DATA: begin
      // Wait for the packet to be received
      if(data_cnt_done) begin
        state_d = IDLE;
      end
    end
    default: begin end
  endcase
end

always_comb begin : counters
  half_baud_cnt_d = 0;
  baud_cnt_d = 0;
  data_cnt_d = data_cnt_q;

  case(state_q)
    IDLE: begin
      // We initialize the half_baud_rate counter at the start of the start bit
      if(uart_rx_qqq == 0) begin
        half_baud_cnt_d = {1'b0, cr_clk_div_i[15:1]} - 2;
      end
    end
    START: begin
      half_baud_cnt_d = half_baud_cnt_q - 1;
      // We initialize the data counter when reaching the middle of the start bit
      if(half_baud_cnt_q == '0) begin
        baud_cnt_d = cr_clk_div_i - 1;
        data_cnt_d[0] = 1'b1;
      end
    end
    DATA: begin
      if(baud_cnt_q == '0) begin
        // When the baud interval has elapsed, reset the baud counter and decrement the data counter
        baud_cnt_d = cr_clk_div_i - 1;
        data_cnt_d = {data_cnt_d[MAX_PACKET_SIZE-1:0], 1'b0};
      end else begin
        baud_cnt_d = baud_cnt_q - 1;
      end
      // Reset the counter as it will not be incremented further
      if(data_cnt_done) begin
        data_cnt_d = '0;
      end
    end
    default: begin end
  endcase
end

always_comb begin : data_align
  // Barrel shifter to align the data_q shift register output
  data_shifted0 = data_start_index[0] ? {1'b0, data_q[MAX_PACKET_SIZE-1:1]} : data_q;
  data_shifted  = data_start_index[1] ? {2'b0, data_shifted0[MAX_PACKET_SIZE-1:2]} : data_shifted0;
end

always_ff @(posedge clk_i) begin
  if(rst_i) begin
    state_q         <= IDLE;

    uart_rx_q       <= 1;
    uart_rx_qq      <= 1;
    uart_rx_qqq     <= 1;

    half_baud_cnt_q <= '0;
    baud_cnt_q      <= '0;
    data_q          <= '0;
    data_cnt_q      <= '0;
  end else begin
    state_q <= state_d;

    // The receive input is registered three times to prevent
    // metastability issues
    uart_rx_q   <= uart_rx_i;
    uart_rx_qq  <= uart_rx_q;
    uart_rx_qqq <= uart_rx_qq;

    // half baudrate counter used at the start of a packet to
    // sample the serial signal in the middle of a bit
    half_baud_cnt_q <= half_baud_cnt_d;

    // baudrate counter used to sample the serial signal
    baud_cnt_q <= baud_cnt_d;

    // Sample the serial signal while in the data state and
    // the baud interval has elapsed
    if(state_q == DATA && baud_cnt_q == '0) begin
      data_q <= {uart_rx_qqq, data_q[10:1]};
    end
    
    // The data counter used to detect the end of packet
    data_cnt_q <= data_cnt_d;
  end
end

/*****************************************/
/*         Assign output signals         */
/*****************************************/

assign data_o = data_shifted;
assign output_valid_o = data_cnt_done;

endmodule // rx_frontend
