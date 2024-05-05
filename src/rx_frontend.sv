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

  input   logic[15:0]   clk_div_i,
  input   logic         uart_rx_i,

  output  logic[10:0]   data_o,
  output  logic         output_valid_o
);
localparam logic[3:0] MAX_PACKET_SIZE = 11;

typedef enum {
  IDLE,    // 0
  START,   // 1
  DATA     // 2
} state_t;
state_t state_d, state_q;

logic uart_rx_q, uart_rx_qq, uart_rx_qqq;

logic[15:0] half_baud_cnt_d, half_baud_cnt_q, 
                 baud_cnt_d,      baud_cnt_q;
logic[$size(MAX_PACKET_SIZE)-1:0]  data_cnt_d, data_cnt_q;

// The shift register data of size 11 :
//   - max 8 data bits
//   - max 1 parity bit
//   - max 2 stop bits
logic[MAX_PACKET_SIZE-1:0] data_q;

logic output_valid_d, output_valid_q;

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
      if(data_cnt_q == '0) begin
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
      // We initialize the half_baud_rate counter at the start of the 
      // start bit
      if(uart_rx_qqq == 0) begin
        half_baud_cnt_d = {1'b0, clk_div_i[15:1]} - 2;
      end
    end
    START: begin
      half_baud_cnt_d = half_baud_cnt_q - 2;
      // We initialize the data counter when reaching the middle of the 
      // start bit
      if(half_baud_cnt_q == '0) begin
        baud_cnt_d = clk_div_i - 1;
        data_cnt_d = MAX_PACKET_SIZE - 1;
      end
    end
    DATA: begin
      if(baud_cnt_q == '0) begin
        // When the baud interval has elapsed
        // reset the baud counter
        baud_cnt_d = clk_div_i - 1;
        // decrement the data counter
        data_cnt_d = data_cnt_q - 1;
      end else begin
        baud_cnt_d = baud_cnt_q - 1;
      end
    end
    default: begin end
  endcase
end

always_comb begin : output_valid
  output_valid_d = 0;
  if(state_q == DATA && data_cnt_q == '0) begin
    output_valid_d = 1;
  end
end

always_ff @(posedge clk_i) begin
  if(rst_i) begin
    state_q <= IDLE;

    uart_rx_q <= 1;
    uart_rx_qq <= 1;
    uart_rx_qqq <= 1;

    half_baud_cnt_q <= '0;
    baud_cnt_q <= '0;
    data_q <= '0;
  end else begin
    state_q <= state_d;

    // The receive input is registered three times to prevent
    // metastability issues
    uart_rx_q <= uart_rx_i;
    uart_rx_qq <= uart_rx_q;
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
    data_cnt_q <= data_cnt_d;

    output_valid_q <= output_valid_d;
  end
end

assign data_o = data_q;
assign output_valid_o = output_valid_q;

endmodule // rx_frontend
