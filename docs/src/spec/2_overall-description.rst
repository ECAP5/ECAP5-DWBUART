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

   The peripheral memory-mapped registers shall be accessible through a memory interface compliant with the Wishbone specification.

.. list-table:: Wishbone Datasheet for the memory interface
  :header-rows: 1
  :width: 100%
  :widths: 70 30
  
  * - Description
    - Specification

  * - Revision level of the WISHBONE specification
    - B4
  * - Type of interface
    - SLAVE
  * - Signal names for the WISHBONE interface
    - Wishbone signals are prefixed with ``wb_``.
  * - ERR_I support
    - No
  * - RTY_I support
    - No
  * - Supported tags
    - None
  * - Port size
    - 32-bit
  * - Port granularity
    - 8-bit
  * - Maximum operand size
    - 32-bit
  * - Data transfer ordering
    - Little Endian
  * - Sequence of data transfer
    - Undefined
  * - Clock constraints
    - Clocked on clk_i

Assumptions and Dependencies
----------------------------

.. todo:: Describe what the assumptions for the product are

  Targeting the ecp5 family, based around opensource toolchains.
