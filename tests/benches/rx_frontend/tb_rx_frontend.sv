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

module tb_rx_frontend
(
  input   int          testcase,

  input   logic         clk_i,
  input   logic         rst_i,

  input   logic[15:0]   clk_div_i,
  input   logic         uart_rx_i,

  output  logic[10:0]   data_o,
  output  logic         output_valid_o
);

rx_frontend dut (
  .clk_i           (clk_i),
  .rst_i           (rst_i),
                 
  .clk_div_i       (clk_div_i),
  .uart_rx_i       (uart_rx_i),
                 
  .data_o          (data_o),
  .output_valid_o  (output_valid_o)
);

endmodule // tb_rx_frontend

`verilator_config

public -module "rx_frontend" -var "state_q"
