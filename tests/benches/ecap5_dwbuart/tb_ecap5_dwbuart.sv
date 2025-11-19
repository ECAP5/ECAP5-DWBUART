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

module tb_ecap5_dwbuart
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

ecap5_dwbuart dut (
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

endmodule // tb_ecap5_dwbuart

`verilator_config

public -module "ecap5_dwbuart" -var "frontend_rst"

public -module "ecap5_dwbuart" -var "mem_addr"
public -module "ecap5_dwbuart" -var "mem_read"
public -module "ecap5_dwbuart" -var "mem_write"
public -module "ecap5_dwbuart" -var "mem_read_data_q"
public -module "ecap5_dwbuart" -var "mem_write_data"

public -module "ecap5_dwbuart" -var "rx_frame"
public -module "ecap5_dwbuart" -var "rx_parity"
public -module "ecap5_dwbuart" -var "rx_valid"

public -module "ecap5_dwbuart" -var "tx_transmit_q"
public -module "ecap5_dwbuart" -var "tx_done"

public -module "ecap5_dwbuart" -var "cr_clk_div_q"
public -module "ecap5_dwbuart" -var "cr_ds_q"
public -module "ecap5_dwbuart" -var "cr_s_q"
public -module "ecap5_dwbuart" -var "cr_p_q"
public -module "ecap5_dwbuart" -var "sr_pe_q"
public -module "ecap5_dwbuart" -var "sr_fe_q"
public -module "ecap5_dwbuart" -var "sr_rxoe_q"
public -module "ecap5_dwbuart" -var "sr_txe_q"
public -module "ecap5_dwbuart" -var "sr_rxne_q"
public -module "ecap5_dwbuart" -var "rxdr_rxd_q"
public -module "ecap5_dwbuart" -var "txdr_txd_q"
