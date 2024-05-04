Functional Description
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

Memory-Mapped Interface
^^^^^^^^^^^^^^^^^^^^^^^

.. todo:: Add register table and table ref in req
.. requirement:: U_REGISTERS_01

   The peripheral shall provide memory-mapped registers described in table .

.. requirement:: U_WISHBONE_01

   The peripheral memory-mapped registers shall be accessible through a wishbone interface.

UART flow control is not supported in version 1.0.0.

Assumptions and Dependencies
----------------------------

.. todo:: Describe what the assumptions for the product are

  Targeting the ecp5 family, based around opensource toolchains.
