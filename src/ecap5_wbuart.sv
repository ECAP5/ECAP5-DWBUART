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

module ecap5_wbuart
(
  input   logic         clk_i,
  input   logic         rst_i,

  //=================================
  //    Memory interface

  input   logic[31:0]  wb_adr_i,
  output  logic[31:0]  wb_dat_o,
  input   logic[31:0]  wb_dat_i,
  input   logic        wb_we_i,
  input   logic[3:0]   wb_sel_i,
  input   logic        wb_stb_i,
  output  logic        wb_ack_o,
  input   logic        wb_cyc_i,
  output  logic        wb_stall_o,

  //=================================
  //    Serial interface
  
  input  logic uart_rx_i,
  output logic uart_tx_o
);
localparam UART_SR = 0;
localparam UART_CR = 1;
localparam UART_RXDR = 2;
localparam UART_TXDR = 3;

/*****************************************/
/*           Internal signals            */
/*****************************************/

typedef enum logic[1:0] {
  IDLE,
  RESPONSE
} state_t;
state_t state_d, state_q;

/*****************************************/
/*        Memory mapped registers        */
/*****************************************/

logic[31:0] registers_d[0:3], 
            registers_q[0:3];

/*****************************************/
/*            Output signals             */
/*****************************************/

logic       wb_ack_d, wb_ack_q;
logic[31:0] wb_dat_d, wb_dat_q;

/*****************************************/

always_comb begin : wishbone
  state_d = state_q;
  wb_ack_d = 0;

  case(state_q)
    IDLE: begin
      if(wb_stb_i && wb_cyc_i) begin
        wb_ack_d = 1;
        state_d = RESPONSE;
      end
    end
    RESPONSE: begin
      state_d = IDLE;
    end
    default: begin end
  endcase
end

always_comb begin : register_access
  if(wb_stb_i && wb_cyc_i) begin
    if(wb_we_i) begin
    end else begin
      case(wb_adr_i[7:0])
        8'h00: wb_dat_d = registers_q[UART_SR];
        8'h04: wb_dat_d = registers_q[UART_CR];
        8'h08: wb_dat_d = registers_q[UART_RXDR];
        default: wb_dat_d = '0;
      endcase
    end
  end
end

always_ff @(posedge clk_i) begin
  if(rst_i) begin
    state_q <= IDLE;
    wb_ack_q <= 0;
    wb_dat_q <= '0;

    registers_q[UART_SR] <= '0;
    registers_q[UART_CR] <= '0;
    registers_q[UART_RXDR] <= '0;
    registers_q[UART_TXDR] <= '0;
  end else begin
    state_q  <=  state_d;
    wb_ack_q <= wb_ack_d;
    wb_dat_q <= wb_dat_d;

    foreach(registers_q[i]) begin
      registers_q[i] <= registers_d[i];
    end
  end
end

assign wb_ack_o = wb_ack_q;
assign wb_dat_o = wb_dat_q;
assign wb_stall_o = 0;

endmodule // ecap5_wbuart
