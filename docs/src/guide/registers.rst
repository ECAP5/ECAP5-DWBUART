Memory Map and Registers
========================

.. |tab|   unicode:: U+00A0 .. NO-BREAK SPACE

.. list-table::
  :header-rows: 1
  :widths: 1 94 1 1 1 1
  
  * - Address Offset
    - Register name
    - Width (in bits)
    - Access
    - Reset value
    - Section/page

  * - 00000000h
    - Status register (UART_SR)
    - 32
    - R
    - 00000000h
    - :ref:`UART_SR <GUIDE_UART_SR>`
  * - 00000004h
    - Control register (UART_CR)
    - 32
    - R/W
    - 00000000h
    - :ref:`UART_CR <GUIDE_UART_CR>`
  * - 00000008h
    - Receive Data register (UART_RXDR)
    - 32
    - R
    - 00000000h
    - :ref:`UART_RXDR <GUIDE_UART_RXDR>`
  * - 0000000Ch
    - Transmit Data register (UART_TXDR)
    - 32
    - W
    - 00000000h
    - :ref:`UART_TXDR <GUIDE_UART_TXDR>`

.. _GUIDE_UART_SR:
.. include:: ../arch/content/uart_sr.rst

.. _GUIDE_UART_CR:
.. include:: ../arch/content/uart_cr.rst

.. _GUIDE_UART_RXDR:
.. include:: ../arch/content/uart_rxdr.rst

.. _GUIDE_UART_TXDR:
.. include:: ../arch/content/uart_txdr.rst

