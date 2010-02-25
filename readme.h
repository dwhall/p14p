/**
\mainpage

<h1>featuring PyMite</h1>


@author Dean Hall

Copyright 2003, 2006, 2007, 2009, 2010 Dean Hall.
See the <a href="../../../LICENSE">License</a> for details.

Site: http://www.pythononachip.org


\section purpose Purpose

The Python-on-a-Chip program consists of the PyMite virtual machine,
ports to specific platforms, libraries, tests, tools and documentation.
PyMite is a flyweight Python bytecode interpreter written from scratch
to execute in low memory on microcontrollers without an operating system.
PyMite supports a subset of Python 2.5 and can also be compiled, tested
and executed on a desktop computer.


\section license License

See the <a href="../../../LICENSE">LICENSE</a> file to understand the terms of licensing
for each component of the Python-on-a-Chip program.

The Python-on-a-Chip program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.


\section build Build

You must build PyMite on a system with a POSIX-like terminal:
Mac OS X, Linux or Cygwin.  GNU Make should be installed.
The development system should have installed the
<a href="http://gcc.gnu.org/">GNU Compiler Collection</a> 
version 3.4 or later and 
<a href="http://www.python.org/download">Python</a> 2.5 or 2.6.
The SCons build system is experimental.
Please read the <a href="../BuildSystem.html">build system 
documentation</a> for more details.

To build PyMite for the desktop platform and run the interactive prompt
(press Control+D when done):

@verbatim
    $ make ipm
    ...
    ipm> print "Hello world."
    Hello world.
    ipm> ^D
@endverbatim

To run the system tests on the desktop:

@verbatim
    $ make check
@endverbatim

To cross-compile for a different platform, type <code>make PLATFORM=\<plat></code>
where <code>\<plat></code> is the name of a subdirectory in <code>src/platform/</code>.
For example:

@verbatim
    $ make PLATFORM=avr
@endverbatim

The author uses avr-gcc to compile for Atmel AVR microcontrollers
and arm-elf-gcc to compile for ARM7TDMI microcontrollers.

Since PyMite is so deeply embedded, it does not rely on many libraries.
When compiling for the AVR, it will need avr-libc.


\section discussion Discussion

Please browse the 
<a href="http://code.google.com/p/python-on-a-chip/">Python-on-a-chip project</a>
site for up-to-date information and join the 
<a href="http://groups.google.com/group/python-on-a-chip">Python-on-a-chip maillist</a> for discussion about the project and
the PyMite virtual machine.


\section thanks Thanks

My personal thanks go out to these people for their recent contribution to PyMite:

- Trammell Hudson: Patches for string concatenation and backtick operator (UNARY_CONVERT).
- Duncan McGreggor: Creating an excellent Python-on-a-Chip logo.
- Tyler Wilson: Porting PyMite to the Windows platform and making good VM improvement suggestions.
- Thomas Fenzl: Patching and testing for 64-bit systems.
- Alex Clausen: Reporting a build break and including a patch for 64-bit systems.
- Diego Martins: Reporting a build break and including a patch.
- Mark Eichin: Feedback on system tests and Makefile


\section releaseNotes Release Notes

This is PyMite release 08

- Release 08, 2009/04/20

    New issue tracker:

	- 9  Add "IPM=true" to the Makefile mods in docs/src/HowToPortPyMite.txt
	- 11 Create a sizeof module
	- 12 Fix float endian issue   float
	- 16 Fix 64-bit FTBFS
	- 17 Make pyserial module optional for ipm-host
	- 18 Make ipm help messages cross-platform
	- 19 Document a useful scons option
	- 24 Update DevelopmentProcess.txt for new repos and issue tracker
	- 25 Create release 08
	- 26 Update HowToReleasePyMite.txt

    Old Trac system:

    - #196 Use snprintf() in float_print() and int_print() since -ansi was lifted
    - #195 Restored \#include <math.h> to float.c
    - #194 Added -h/--help args to ipm
    - #193 Updated ipm usage message to comply with GPL
    - #192 Updated ipm docs with pyserial dependency and win32 args
    - #180 Fixed gc bug during ipm
    - #190 Fixed segfault when num args is fewer than expected
    - #189 Updated README with correct build instructions
    - #72  Implemented plat module for other targets
    - #182 Created I/O funcs so plat_reportError can be removed from plat interface
    - #187 Allowed removal of __doc__ from co_names
    - #185 Created porting document
    - #184 Added scons build capability
    - #179 Unified plat and samples directories
    - #183 Updated the FAQ
    - #181 Fixed AT91SAM7 demo

- Release 07, 2009/03/08
- Release 06, 2009/01/30
- Release 05, 2007/05/10
- Release 04, 2006/12/14
- Release 03, 2006/09/17
- Release 02, 2006/08/11
- Release 01, 2003/03/18

*/
