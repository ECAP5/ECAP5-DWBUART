Receive Data register (UART_RXDR)
"""""""""""""""""""""""""""""""""

.. bitfield::
    :bits: 32
    :lanes: 2
    :vspace: 70
    :hspace: 700

        [
            { "name": "RXD", "bits": 8},
            { "name": "reserved", "bits": 24, "type": 1}
        ]

|

.. list-table::
  :header-rows: 1
  :widths: 1 1 99
  
  * - Position
    - Field
    - Description

  * - 31-8
    - reserved
    - *This field is reserved.*

      This read-only field is reserved and always has the value 0.
  * - 7-0
    - RXD
    - *Receive Data*

      The received data is written to this field.
      
      The field is cleared by hardware after being read.
