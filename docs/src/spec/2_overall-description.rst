.. |tab|   unicode:: U+00A0 .. NO-BREAK SPACE

Overall Description
===================

User needs
----------

ECAP5 is the primary user for ECAP5-DWBUART, but it could be used as a standalone Wishbone UART peripheral as well.

Serial Interface
^^^^^^^^^^^^^^^^

.. requirement:: U_UART_01

   The peripheral shall perform parallel-to-serial conversion of the provided data.

.. requirement:: U_UART_02

   The peripheral shall perform serial-to-parallel conversion of the received data.

.. requirement:: U_UART_03

   The peripheral shall support full-duplex communications.

.. requirement:: U_UART_04

   The peripheral shall detect parity errors.

.. requirement:: U_UART_05

   The peripheral shall detect framing errors.

.. requirement:: U_UART_06

   The peripheral shall detect overrrun errors.

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

   The peripheral shall provide memory-mapped configuration and status registers.

.. requirement:: U_MEMORY_INTERFACE_01

   The peripheral memory-mapped registers shall be accessible through a wishbone interface.

Assumptions and Dependencies
----------------------------

.. todo:: Describe what the assumptions for the product are

  Targeting the ecp5 family, based around opensource toolchains.
