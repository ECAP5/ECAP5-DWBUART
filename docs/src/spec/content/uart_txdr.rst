Transmit Data register (UART_TXDR)
""""""""""""""""""""""""""""""""""

.. bitfield::
    :bits: 32
    :lanes: 2
    :vspace: 70
    :hspace: 700

        [
            { "name": "TXD", "bits": 8},
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
  * - 7-0
    - TXD
    - *Transmit Data*

      Data written to this field is sent through the serial link.
      
      The TXE field of the UART_SR shall be sample before writing again to this field to prevent data loss.
