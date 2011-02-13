=================
P14p Event System
=================
A Step Above Interrupts
=======================


:Author: Dean Hall


Introduction
------------

An important part of using microcontrollers is handling interrupts provided
by the MCU core and peripherals.  This document discusses how Python-on-a-Chip
(P14p) intends to use interrupts and connect interrupt service routines through
the VM to user-defined event-handlers.


Perspective
-----------

Interrupt service routines are meant to be small, fast snippets of code and
should be written in C or assembly.  The bytecode interpreter in P14p's VM
is not suitable for executing Python code inside an ISR.  Instead, I propose
creating a lightweight method of notifying the VM of an event from within an
ISR (or other C-language runtime such as a separate thread of execution).

Just as Python is a higher-level language than C, so too should P14p event
handlers be designed at a higher-level than ISRs.  For example, a UART
peripheral typically provides an interrupt when a character is received.  
In this situation, I would recommend the ISR collect a number of characters in
a buffer and then notify the P14p even system.  The P14p event handler would
use a native function to copy the bytes from the ISR's character buffer into a
Python bytearray or String object.  Then, the characters are available for use
in Python code.  The rest of this document explains how this system will work.


Platform ISRs
-------------

Notifying the VM
----------------

Declaring an Event Handler
--------------------------

Setting an Event Handler
------------------------
