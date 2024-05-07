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

logic[31:0] registers_d[0:3],
            registers_q[0:3];

/*****************************************/

rx_frontend rx_frontend_inst (
  .clk_i (clk_i),   .rst_i (frontend_rst),

  .cr_clk_div_i   (registers_q[UART_CR][31:16]),
  .cr_ds_i        (registers_q[UART_CR][4]),
  .cr_s_i         (registers_q[UART_CR][3]),
  .cr_p_i         (registers_q[UART_CR][2:1]),

  .uart_rx_i      (uart_rx_i),
  
  .frame_o        (rx_frame),
  .parity_o       (rx_parity),
  .output_valid_o (rx_valid)
);

tx_frontend tx_frontend_inst (
  .clk_i (clk_i),   .rst_i (frontend_rst),

  .cr_clk_div_i   (registers_q[UART_CR][31:16]),
  .cr_ds_i        (registers_q[UART_CR][4]),
  .cr_s_i         (registers_q[UART_CR][3]),
  .cr_p_i         (registers_q[UART_CR][2:1]),

  .transmit_i     (tx_transmit_q),
  .dr_i           (registers_q[UART_TXDR][7:0]),

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
  registers_d[UART_SR]   = registers_q[UART_SR];
  registers_d[UART_CR]   = registers_q[UART_CR];
  registers_d[UART_RXDR] = registers_q[UART_RXDR];
  registers_d[UART_TXDR] = registers_q[UART_TXDR];

  // Set the data output for R and R/W registers
  mem_read_data_d = 0;
  case(mem_addr[7:2])
    UART_SR:   mem_read_data_d = registers_q[UART_SR];
    UART_CR:   mem_read_data_d = registers_q[UART_CR];
    UART_RXDR: mem_read_data_d = registers_q[UART_RXDR];
    default:   mem_read_data_d = '0;
  endcase

  // Set the register data for R/W and W registers
  if(mem_write) begin
    case(mem_addr[7:2])
      UART_CR:   registers_d[UART_CR]   = mem_write_data;
      UART_TXDR: registers_d[UART_TXDR] = mem_write_data;
      default: begin end
    endcase 
  end

  // Set the TXE field
  if(tx_done) begin
    registers_d[UART_SR][1] = 1;
  end

  // Reset the TXE field
  if(mem_write && (mem_addr[7:2] == UART_TXDR)) begin
    registers_d[UART_SR][1] = 0;
  end

  // Set the RXDR register and RXNE field
  if(rx_valid) begin
    registers_d[UART_RXDR][MAX_FRAME_SIZE-1:0] = rx_frame;
    registers_d[UART_SR][0] = 1;
  end

  // Reset UART_RXDR after reading
  if(mem_read && mem_addr[7:2] == UART_RXDR) begin
    registers_d[UART_RXDR] = '0;
    registers_d[UART_SR][0] = 0;
  end
end

always_comb begin : frontend_interface
  frontend_rst = rst_i || (mem_write && mem_addr[7:2] == UART_CR);

  tx_transmit_d = mem_write && (mem_addr[7:2] == UART_TXDR);
end

always_ff @(posedge clk_i) begin
  if(rst_i) begin
    registers_q[UART_SR] <= 32'h2;
    registers_q[UART_CR] <= '0;
    registers_q[UART_RXDR] <= '0;
    registers_q[UART_TXDR] <= '0;

    tx_transmit_q <= 0;

    mem_read_data_q <= '0;
  end else begin
    registers_q[UART_SR] <= registers_d[UART_SR];
    registers_q[UART_CR] <= registers_d[UART_CR];
    registers_q[UART_RXDR] <= registers_d[UART_RXDR];
    registers_q[UART_TXDR] <= registers_d[UART_TXDR];

    tx_transmit_q <= tx_transmit_d;

    mem_read_data_q <= mem_read_data_d;
  end
end

endmodule // ecap5_wbuart
