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

module tb_tx_frontend
(
  input   int          testcase,

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

tx_frontend dut (
  .clk_i           (clk_i),
  .rst_i           (rst_i),
                 
  .cr_acc_incr_i   (cr_acc_incr_i),
  .cr_ds_i         (cr_ds_i),
  .cr_p_i          (cr_p_i),
  .cr_s_i          (cr_s_i),

  .transmit_i      (transmit_i),
  .dr_i            (dr_i),
                 
  .done_o          (done_o),

  .uart_tx_o       (uart_tx_o)
);

endmodule // tb_tx_frontend

`verilator_config

public -module "tx_frontend" -var "state_q"
