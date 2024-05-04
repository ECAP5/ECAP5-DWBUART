Control register (UART_CR)
``````````````````````````

UART_CR contains the control for selecting the UART baudrate, parity, size and stop bits.

.. bitfield::
    :bits: 32
    :lanes: 2
    :vspace: 70
    :hspace: 700

        [
            { "name": "EN", "bits": 1},
            { "name": "DS", "bits": 1},
            { "name": "P", "bits": 2},
            { "name": "S", "bits": 1},
            { "name": "reserved", "bits": 11, "type": 1},
            { "name": "CLK_DIV", "bits": 16}
        ]

|

.. list-table::
  :header-rows: 1
  :widths: 1 1 99
  
  * - Position
    - Field
    - Description

  * - 31-16
    - CLK_DIV
    - *Clock Divider/Baud Rate selector*

      The specified clock divider determines the baud rate with the formula Baud Rate = Freq / CLK_DIV.
  * - 15-5
    - reserved
    - *This field is reserved.*

      This read-only field is reserved and always has the value 0.
  * - 4
    - S
    - *Stop bit selector*
       
      0 |tab| One stop bit

      2 |tab| Two stop bits
  * - 3-2
    - P
    - *Parity bit selector*

      00 |tab| No parity checks

      01 |tab| Odd parity

      10 |tab| Even parity

      11 |tab| *reserved*
  * - 1
    - DS
    - *Data Size selector*

      0 |tab| 7-bit data

      1 |tab| 8-bit data
  * - 0
    - EN
    - *UART Enable*

      0 |tab| The peripheral is disabled

      1 |tab| The peripheral is enabled
