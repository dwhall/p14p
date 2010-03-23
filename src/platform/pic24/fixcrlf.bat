@echo off
rem This batch file changes CR/LF line endings to pure LF line endings to be Unix-compliant.
rem It relies on crlf.py, taken from the Windows Python 2.5 distribution.
rem Python MUST reside in the path for this to work.
python crlf.py *.c *.h *.py Makefile README common/*.c include/*.h include/devices/*.h include/devices/*.pl
