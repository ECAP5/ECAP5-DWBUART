Status register (UART_SR)
"""""""""""""""""""""""""

.. bitfield::
    :bits: 32
    :lanes: 2
    :vspace: 70
    :hspace: 700

        [
            { "name": "RXNE", "bits": 1},
            { "name": "TXE", "bits": 1},
            { "name": "RXOE", "bits": 1},
            { "name": "FE", "bits": 1},
            { "name": "PE", "bits": 1},
            { "name": "reserved", "bits": 27, "type": 1}
        ]

|

.. list-table::
  :header-rows: 1
  :widths: 1 1 99
  
  * - Position
    - Field
    - Description

  * - 31-5
    - Reserved
    - *This field is reserved.*

      This read-only field is reserved and always has the value 0.
  * - 4
    - PE
    - *Parity Error*

      This bit is cleared after reading it.
       
      0 |tab| No parity error

      1 |tab| Parity error detected
  * - 3
    - FE
    - *Framing Error*

      This bit is cleared after reading it.

      0 |tab| No framing error

      1 |tab| Framing error detected
  * - 2
    - RXOE
    - *Receive Overrun Error*

      This bit is cleared after reading it.

      0 |tab| No received overrun error

      1 |tab| A packet was received but RXNE is asserted
  * - 1
    - TXE
    - *Transmit register Empty*

      0 |tab| The transmit register is full (being sent)

      1 |tab| The transmit register is empty
  * - 0
    - RXNE
    - *Receive register Not Empty*

      0 |tab| The receive register is empty (no data)

      1 |tab| The receive register is not empty empty
