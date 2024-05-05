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

module tb_ecap5_wbuart
(
  input   int          testcase,

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

ecap5_wbuart dut (
  .clk_i           (clk_i),
  .rst_i           (rst_i),

  .wb_adr_i   (wb_adr_i),
  .wb_dat_o   (wb_dat_o),
  .wb_dat_i   (wb_dat_i),
  .wb_we_i    (wb_we_i),
  .wb_sel_i   (wb_sel_i),
  .wb_stb_i   (wb_stb_i),
  .wb_ack_o   (wb_ack_o),
  .wb_cyc_i   (wb_cyc_i),
  .wb_stall_o (wb_stall_o),

  .uart_rx_i       (uart_rx_i),
  .uart_tx_o       (uart_tx_o)
);

endmodule // tb_ecap5_wbuart
