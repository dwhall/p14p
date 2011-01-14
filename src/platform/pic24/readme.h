/** 
\page PIC24_dsPIC33_port The PIC24 and dsPIC33 port
 
\author Bryan A. Jones

\section purpose Purpose

This is a sample project that demonstrates the use of PyMite on the
<a href="http://www.microchip.com/stellent/idcplg?IdcService=SS_GET_PAGE&nodeId=2629&param=en533465">PIC24
and dsPIC33 family</a> of microcontrollers.  Any PIC with at least 128 KiB of
program memory and 4 KiB of RAM can run PyMite.


\section details Details

This sample project demonstrates a simple use case of 
python-on-a-chip (p14p) on a microcontroller.  The file 
\ref src/platform/pic24/main.c "main.c" is used to initialize p14p
and indicate the name of the module it should run.  In this case, it
will run the module \ref src/platform/pic24/main.py "main.py".
Examining the module \ref src/platform/pic24/main.py "main.py",
the program prints a "Hello world" message then executes \ref ipm.py.
Additional sample code in sample_lib.py illustrates use of the
\ref src/platform/pic24/pic24_dspic33.py "PIC24/dsPIC33" library.
The program \ref src/platform/pic24/robot.py "robot.py" 
gives code to operate a simple robot.


\section building Building the Project

To build documentation,
install <a href="http://docutils.sourceforge.net">docutils</a> and
<a href="http://www.doxygen.org">Doxygen</a>, then use the system-wide
<code>make html</code> target. You'll need make from Cygwin installed;
see item 1 below for more information.

\subsection mplab_building Building with MPLAB

The preferreed method for building the project relies on using the
<a href="http://www.microchip.com/stellent/idcplg?IdcService=SS_GET_PAGE&nodeId=1406&dDocName=en019469&part=SW007002">Microchip
MPLAB IDE</a>. After installing it, either:

-# Best: Install <a href="http://www.cygwin.com">Cygwin</a>, manually
   selecting installation of make. Place the path to Cygwin's
   make.exe in your Windows path. Then, simply open the p14p.mcp
   project file with the Microchip MPLAB IDE and build the project.
-# If you don't want to install Cygwin, double-click on the py2c.bat
   file before building, then open the project in the MPLAB IDE and
   compile. Ignore the error about the pre-build step (which tries to use
   make.exe from Cygwin). <b>Important</b>: Any changes to a .py file
   used in the build will not be included until <code>py2c.bat</code> is
   re-run. For example, after changing <code>pmfeatures.py</code>, re-run!

\subsection make_building Building with make

Alternatively, you can compile from the command line.
Start with a clean PyMite tree.  Edit 
<a href="../../../src/platform/pic24/Makefile">src/platform/pic24/Makefile</a>
near line 14 to set the part number of your PIC. Then, execute the following
at the command prompt, which will build the PyMite VM archive as well as the 
sample application.

@verbatim
    $ cd src/platform/pic24
    $ make
@endverbatim


\section flashing Flashing the Binary

The steps above result in the binary file that need to go on the PIC.
Use a PICKit2/3 to program your PIC.

*/
