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
#(
  localparam UART_SR   = 0,
  localparam UART_CR   = 1,
  localparam UART_RXDR = 2,
  localparam UART_TXDR = 3,

  // The minimum frame size is :
  //   - 7 data bits
  //   - 1 stop bit
  localparam MIN_FRAME_SIZE = 8,
  
  // The maximum frame size is :
  //   - 8 data bits
  //   - 1 parity bit
  //   - 2 stop bit
  localparam MAX_FRAME_SIZE = 11
)
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

/*****************************************/
/*           Internal signals            */
/*****************************************/

logic frontend_rst;

logic[7:0]  mem_addr;
logic       mem_read, mem_write;
logic[31:0] mem_read_data_d, mem_read_data_q, 
            mem_write_data;

logic[MAX_FRAME_SIZE-1:0] rx_frame;
logic rx_parity;
logic rx_valid;

logic tx_transmit_d, tx_transmit_q,
      tx_done;

/*****************************************/
/*        Memory mapped registers        */
/*****************************************/

logic[15:0] cr_clk_div_d, cr_clk_div_q;
logic       cr_ds_d, cr_ds_q,
            cr_s_d, cr_s_q;
logic[1:0]  cr_p_d, cr_p_q;

logic sr_pe_d, sr_pe_q,
      sr_fe_d, sr_fe_q,
      sr_rxoe_d, sr_rxoe_q,
      sr_txe_d, sr_txe_q,
      sr_rxne_d, sr_rxne_q;

logic[7:0] rxdr_rxd_d, rxdr_rxd_q;
logic[7:0] txdr_txd_d, txdr_txd_q;

/*****************************************/

rx_frontend rx_frontend_inst (
  .clk_i (clk_i),   .rst_i (frontend_rst),

  .cr_clk_div_i   (cr_clk_div_q),
  .cr_ds_i        (cr_ds_q),
  .cr_s_i         (cr_s_q),
  .cr_p_i         (cr_p_q),

  .uart_rx_i      (uart_rx_i),
  
  .frame_o        (rx_frame),
  .parity_o       (rx_parity),
  .output_valid_o (rx_valid)
);

tx_frontend tx_frontend_inst (
  .clk_i (clk_i),   .rst_i (frontend_rst),

  .cr_clk_div_i   (cr_clk_div_q),
  .cr_ds_i        (cr_ds_q),
  .cr_s_i         (cr_s_q),
  .cr_p_i         (cr_p_q),

  .transmit_i     (tx_transmit_q),
  .dr_i           (txdr_txd_q),

  .done_o         (tx_done),

  .uart_tx_o      (uart_tx_o)
);

wb_interface wb_interface_inst (
  .clk_i (clk_i),   .rst_i (rst_i),
  
  .wb_adr_i (wb_adr_i),  .wb_dat_o (wb_dat_o),  .wb_dat_i   (wb_dat_i),
  .wb_we_i  (wb_we_i),   .wb_sel_i (wb_sel_i),  .wb_stb_i   (wb_stb_i),
  .wb_ack_o (wb_ack_o),  .wb_cyc_i (wb_cyc_i),  .wb_stall_o (wb_stall_o),

  .addr_o       (mem_addr),
  .read_o       (mem_read),
  .read_data_i  (mem_read_data_q),
  .write_o      (mem_write),
  .write_data_o (mem_write_data)
);

always_comb begin : register_access
  cr_clk_div_d = cr_clk_div_q;
  cr_ds_d      = cr_ds_q;
  cr_s_d       = cr_s_q;
  cr_p_d       = cr_p_q;

  sr_pe_d      = sr_pe_q;
  sr_fe_d      = sr_fe_q;
  sr_rxoe_d    = sr_rxoe_q;
  sr_txe_d     = sr_txe_q;
  sr_rxne_d    = sr_rxne_q;

  rxdr_rxd_d   = rxdr_rxd_q;
  txdr_txd_d   = txdr_txd_q;

  // Set the data output for R and R/W registers
  mem_read_data_d = 0;
  case(mem_addr[7:2])
    UART_SR:   mem_read_data_d = {27'b0, sr_pe_q, sr_fe_q, sr_rxoe_q, sr_txe_q, sr_rxne_q};
    UART_CR:   mem_read_data_d = {cr_clk_div_q, 11'b0, cr_s_q, cr_p_q, cr_ds_q, 1'b1};
    UART_RXDR: mem_read_data_d = {24'b0, rxdr_rxd_q};
    default:   mem_read_data_d = '0;
  endcase

  // Set the register data for R/W and W registers
  if(mem_write) begin
    case(mem_addr[7:2])
      UART_CR: begin
        cr_clk_div_d = mem_write_data[31:16];
        cr_s_d = mem_write_data[4];
        cr_p_d = mem_write_data[3:2];
        cr_ds_d = mem_write_data[1];
      end
      UART_TXDR: txdr_txd_d = mem_write_data[7:0];
      default: begin end
    endcase 
  end

  // Set the TXE field
  if(tx_done) begin
    sr_txe_d = 1;
  end

  // Reset the TXE field
  if(mem_write && (mem_addr[7:2] == UART_TXDR)) begin
    sr_txe_d = 0;
  end

  // Set the RXDR register and RXNE field
  if(rx_valid) begin
    rxdr_rxd_d = rx_frame[7:0];
    sr_rxne_d = 1;
  end

  // Reset UART_RXDR after reading
  if(mem_read && mem_addr[7:2] == UART_RXDR) begin
    rxdr_rxd_d = '0;
    sr_rxne_d = 0;
  end
end

always_comb begin : frontend_interface
  frontend_rst = rst_i || (mem_write && mem_addr[7:2] == UART_CR);

  tx_transmit_d = mem_write && (mem_addr[7:2] == UART_TXDR);
end

always_ff @(posedge clk_i) begin
  if(rst_i) begin
    cr_clk_div_q <= '0;
    cr_ds_q <= 0;
    cr_s_q <= 0;
    cr_p_q <= '0;

    sr_pe_q <= 0;
    sr_fe_q <= 0;
    sr_rxoe_q <= 0;
    sr_txe_q <= 1;
    sr_rxne_q <= 0;

    rxdr_rxd_q <= '0;
    txdr_txd_q <= '0;

    tx_transmit_q <= 0;

    mem_read_data_q <= '0;
  end else begin
    cr_clk_div_q <= cr_clk_div_d;
    cr_ds_q <= cr_ds_d;
    cr_s_q <= cr_s_d;
    cr_p_q <= cr_p_d;

    sr_pe_q <= sr_pe_d;
    sr_fe_q <= sr_fe_d;
    sr_rxoe_q <= sr_rxoe_d;
    sr_txe_q <= sr_txe_d;
    sr_rxne_q <= sr_rxne_d;

    rxdr_rxd_q <= rxdr_rxd_d;
    txdr_txd_q <= txdr_txd_d;

    tx_transmit_q <= tx_transmit_d;

    mem_read_data_q <= mem_read_data_d;
  end
end

endmodule // ecap5_wbuart
