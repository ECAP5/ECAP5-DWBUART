.. |tab|   unicode:: U+00A0 .. NO-BREAK SPACE

Overall Description
===================

User needs
----------

ECAP5 is the primary user for ECAP5-DWBUART, but it could be used as a standalone Wishbone UART peripheral as well.

Serial Interface
^^^^^^^^^^^^^^^^

.. requirement:: U_UART_01

   The peripheral shall perform serial-to-parallel conversion of the provided data.

.. requirement:: U_UART_02

   The peripheral shall perform parallel-to-serial conversion of the received data.

Configuration
^^^^^^^^^^^^^

.. requirement:: U_BAUD_RATE_01

   The baud rate of the peripheral shall be software-configurable.

.. requirement:: U_PARITY_BIT_01

   The parity bit of the peripheral shall be software-configurable.

.. requirement:: U_DATA_SIZE_01

   The data packet size of the peripheral shall be software-configurable.

.. requirement:: U_STOP_BIT_01

   The stop bits of the peripheral shall be software-configurable.

UART flow control is not supported in version 1.0.0.

Memory-Mapped Interface
^^^^^^^^^^^^^^^^^^^^^^^

.. requirement:: U_REGISTERS_01

   The peripheral shall provide memory-mapped registers described in the table below.

.. requirement:: U_MEMORY_INTERFACE_01

   The peripheral memory-mapped registers shall be accessible through a wishbone interface.

  .. _Memory_Mapping_and_Registers:
.. list-table:: Memory Mapping and Registers
  :header-rows: 1
  :widths: 1 94 1 1 1 1
  
  * - Address Offset
    - Register name
    - Width (in bits)
    - Access
    - Reset value
    - Section/page

  * - 0000_0000h
    - Status register (UART_SR)
    - 32
    - R
    - 0000_0000h
    - :ref:`UART_SR <ARCH_UART_SR>`
  * - 0000_0004h
    - Control register (UART_CR)
    - 32
    - R/W
    - 0000_0000h
    - :ref:`UART_CR <ARCH_UART_CR>`
  * - 0000_0008h
    - Receive Data register (UART_RXDR)
    - 32
    - R
    - 0000_0000h
    - :ref:`UART_RXDR <ARCH_UART_RXDR>`
  * - 0000_000Ch
    - Transmit Data register (UART_TXDR)
    - 32
    - W
    - 0000_0000h
    - :ref:`UART_TXDR <ARCH_UART_TXDR>`

.. _ARCH_UART_SR:
.. include:: ../arch/content/uart_sr.rst

.. _ARCH_UART_CR:
.. include:: ../arch/content/uart_cr.rst

.. _ARCH_UART_RXDR:
.. include:: ../arch/content/uart_rxdr.rst

.. _ARCH_UART_TXDR:
.. include:: ../arch/content/uart_txdr.rst


Assumptions and Dependencies
----------------------------

.. todo:: Describe what the assumptions for the product are

  Targeting the ecp5 family, based around opensource toolchains.
