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

module rx_shiftreg import ecap5_dproc_pkg::*;
(
  input   logic         clk_i,
  input   logic         rst_i,

  input  logic uart_rx_i
);

typedef enum {
  IDLE,    // 0
  START,   // 1
  DATA,    // 2
  PARITY,  // 3
  STOP     // 4
} state_t;
state_t state_d, state_q;

logic uart_rx_q, uart_rx_qq, uart_rx_qqq;

always_comb begin : state_machine
  state_d = state_q;

  case(state_q)
    IDLE: begin
      // Wait for the beginning of the start bit
    end
    START: begin
      // Wait for the middle of the start bit
    end
    default: begin end
  endcase
end

// The shift register data of size 11 :
//   - max 8 data bits
//   - max 1 parity bit
//   - max 2 stop bits
logic[10:0] data_q;

always_ff @(posedge clk_i) begin : shift_register
  // Only when we are in the middle of the bit
  data_q <= {uart_rx_qqq, data_q[10:1]};
end

always_ff @(posedge clk_i) begin : async_to_clocked_input
  if(rst_i) begin
    state_q <= IDLE;

    uart_rx_q <= 0;
    uart_rx_qq <= 0;
    uart_rx_qqq <= 0;
  end else begin
    state_q <= state_d;

    // The receive input is registered three times to prevent
    // metastability issues
    uart_rx_q <= uart_rx_i;
    uart_rx_qq <= uart_rx_q;
    uart_rx_qqq <= uart_rx_qq;
  end
end

endmodule // rx_shiftreg
