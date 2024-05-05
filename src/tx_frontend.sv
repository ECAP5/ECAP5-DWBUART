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

module tx_frontend
(
  input   logic         clk_i,
  input   logic         rst_i,

  input   logic[15:0]   cr_clk_div_i,
  input   logic         cr_ds_i,
  input   logic[1:0]    cr_p_i,
  input   logic         cr_s_i,

  input   logic         transmit_i,
  input   logic[7:0]    dr_i,

  output  logic         done_o,

  output  logic         uart_tx_o
);

/*****************************************/
/*           Internal signals            */
/*****************************************/
localparam MAX_DATA_SIZE = 8;

typedef enum {
  IDLE,     // 0
  START,    // 1
  DATA,     // 2
  PARITY,   // 3
  STOP      // 4
} state_t;
state_t state_d, state_q;

logic[15:0] baud_cnt_d, baud_cnt_q;
logic[MAX_DATA_SIZE:0] bit_cnt_d, bit_cnt_q;
logic[$size(dr_i)-1:0]  dr_d, dr_q;

logic parity_d, parity_q;

logic uart_tx_d;

logic done_d, done_q;

/*****************************************/

always_comb begin : state_machine
  case(state_q)
    IDLE: begin
      if(transmit_i) begin
        state_d = START;
      end 
    end
    START: begin
      if(baud_cnt_q == '0) begin
        state_d = DATA;
      end
    end
    DATA: begin
      if(baud_cnt_q == '0 && bit_cnt_q[1]) begin
        if(cr_p_i == '0) begin
          state_d = STOP;
        end else begin
          state_d = PARITY;
        end
      end
    end
    PARITY: begin
      if(baud_cnt_q == '0) begin
        state_d = STOP;
      end
    end
    STOP: begin
      if(baud_cnt_q == '0 && bit_cnt_q[1]) begin
        state_d = IDLE;
      end
    end
  endcase
end

always_comb begin
  baud_cnt_d = baud_cnt_q - 1;
  dr_d = dr_q;

  uart_tx_d = 1;

  if(baud_cnt_q == '0) begin
    baud_cnt_d = cr_clk_div_i - 1;
  end

  case(state_q)
    IDLE: begin
      if(transmit_i) begin
        baud_cnt_d = cr_clk_div_i - 1;
        dr_d = dr_i;
        bit_cnt_d = cr_ds_i ? (1 << 8) : (1 << 7);
      end
    end
    START: begin
      uart_tx_d = 1'b0;
    end 
    DATA: begin
      uart_tx_d = dr_q[0];
      if(baud_cnt_q == '0) begin
        bit_cnt_d = {1'b0, bit_cnt_q[MAX_DATA_SIZE:1]};
        dr_d = {1'b0, dr_q[$size(dr_i)-1:1]};
        if(bit_cnt_q[1]) begin
          bit_cnt_d = cr_s_i ? (1 << 2) : (1 << 1);
        end
      end
    end
    PARITY: begin
      uart_tx_d = parity_q;
    end
    STOP: begin
      uart_tx_d = 1'b1;
      if(baud_cnt_q == '0) begin
        bit_cnt_d = {1'b0, bit_cnt_q[MAX_DATA_SIZE:1]};
      end
    end
  endcase
end

always_comb begin : done
  done_d = 0;

  if(state_q == STOP && baud_cnt_q == '0 && bit_cnt_q[1]) begin
    done_d = 1;
  end
end

always_ff @(posedge clk_i) begin
  if(rst_i) begin
    state_q <= IDLE;

    dr_q <= '0;
    baud_cnt_q <= 0;

    parity_q <= 0;

    bit_cnt_q <= 0;

    done_q <= 0;
  end else begin
    state_q <= state_d;

    dr_q <= dr_d;

    baud_cnt_q <= baud_cnt_d;

    parity_q <= parity_d;

    bit_cnt_q <= bit_cnt_d;

    done_q <= done_d;
  end
end

assign uart_tx_o = rst_i ? 1 : uart_tx_d;
assign done_o = done_q;

endmodule // tx_frontend
