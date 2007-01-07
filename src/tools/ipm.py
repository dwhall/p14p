#!/usr/bin/env python

# PyMite - A flyweight Python interpreter for 8-bit microcontrollers and more.
# Copyright 2002 Dean Hall
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#

"""
==================
Interactive PyMite
==================

An interactive command line that runs on a host computer that is connected
to a target device that is running PyMite.  The host computer compiles the
interactive statement and converts it to a form that PyMite can handle,
sends that over the connection where the target device loads and interprets it.
The target device then packages any result, sends it to the host computer
and the host computer prints the result.

Log
---

==========      ================================================================
Date            Action
==========      ================================================================
2006/12/21      Initial creation
"""


import cmd, getopt, popen2, sys
import pmImgCreator


__usage__ = """USAGE:
    ipm.py -d

    -d      Specifies the desktop connection; uses a pipe to send/receive bytes
            to/from the target, which is the vm also running on the desktop.
            ipm will spawn the process and run the vm.
    """

# Exit error codes (from /usr/include/sysexits.h)
EX_USAGE = 64

PMVM_EXE = "../tests/interactive/t067.out"
IPM_PROMPT = "ipm> "
COMPILE_FN = "<ipm>"
COMPILE_MODE = "exec"
HELP_MESSAGE = "This is the interactive PyMite command line.\n" \
               "Just type the code that you want the target device to run.\n" \
               "Type another return if you see no prompt to exit multiline mode.\n" \
               "Type ctrl+D to quit.\n"


class Connection(object):
    pass


class PipeConnection(Connection):
    def __init__(self, target=PMVM_EXE):
        self.open(target)


    def open(self, target):
        (self.child_stdout,
         self.child_stdin,
         self.child_stderr) = popen2.popen3(target)

        # The write function shall write to the child's stdin
        self.write = self.child_stdin.write

        # The read function shall read from the child's stdout
        self.read = self.child_stdout.read

        # The error function shall write to the child's stderr
        self.error = self.child_stderr.write


    def close(self,):
        self.child_stdout.close()
        self.child_stdin.close()
        self.child_stderr.close()


class Interactive(cmd.Cmd):
    """The interactive command line parser accepts typed input line-by-line.
    If a statement requires multiple lines to complete,  the input
    is collected until two sequential end-of-line characters are received.
    """
    ipmcommands = ("?", "help", "load",)


    def __init__(self, conn):
        cmd.Cmd.__init__(self,)
        self.prompt = IPM_PROMPT
        self.conn = conn


    def do_help(self, *args):
        """Prints the help message.
        """
        self.stdout.write(HELP_MESSAGE)


    def do_load(self, *args):
        """Loads a module from the host to the target device.
        """

        # Ensure the filename arg names a python source file
        fn = args[0]
        if not fn.endswith(".py"):
            self.stdout.write('Module must be a ".py" source file.\n')
            return

        print "Loading module %s" % (args[0]) #DBG
        # TODO: load module, send to target


    def do_input(self, line):
        """Handles input from the interactive interface.
        Compiles and converts the interactive statement.
        """

        codeobj = compile(line, COMPILE_FN, COMPILE_MODE)

        # Convert to a code image
        pic = pmImgCreator.PmImgCreator()
        try:
            codeimg = pic.co_to_str(codeobj)

        # Print any conversion errors
        except Exception, e:
            print e.__class__.__name__, ":", e

        # TODO: Otherwise send the image and print the reply
        else:
            print "TODO: Send code image (%d bytes)." % len(codeimg)#DBG
            print "DEBUG: img = ", repr(codeimg)
            self.conn.write(codeimg)


    def onecmd(self, line):
        """Parses one line of input (grabs more lines if needed).
        """

        # Ignore empty line, continue interactive prompt
        if not line:
            return

        # Handle ctrl+D (End Of File) input, stop interactive prompt
        if line == "EOF":
            # Send disconnect command to target
            self.conn.write("\0")

            # Close the connection
            self.conn.close()

            self.stdout.write("\n")
            self.stop = True
            return True

        # Handle ipm-specific commands
        if line.split()[0] in Interactive.ipmcommands:
            cmd.Cmd.onecmd(self, line)
            return

        codeobj = None
        while not codeobj:

            # Try to compile the given line
            try:
                codeobj = compile(line, COMPILE_FN, COMPILE_MODE)

            # Get more input if syntax error reports unexpected end of file
            except SyntaxError, se:

                # Print any other syntax error
                if not se.msg.startswith("unexpected EOF while parsing"):
                    print se.__class__.__name__, ":", se
                    return

                # Restore the newline chopped by cmd.py:140
                line += "\n"

                # Get more input if needed
                while not line.endswith("\n\n"):
                    line += self.stdin.readline()

            # Print any other exception
            except Exception, e:
                print e.__class__.__name__, ":", e
                return

        # Process the input
        self.do_input(line)


    def run(self,):
        """Runs the command loop and handles keyboard interrupts (ctrl+C)."""

        print HELP_MESSAGE,

        self.stop = False
        while not self.stop:
            try:
                stop = self.cmdloop()
            except KeyboardInterrupt, ki:
                print "\n", ki.__class__.__name__
                # TODO: check connection?


def parse_cmdline():
    """Parses the command line for options.
    """

    try:
        opts, args = getopt.getopt(sys.argv[1:], "d", [])
    except Exception, e:
        raise e
        print __usage__
        sys.exit(EX_USAGE)

    if not opts:
        print __usage__
        sys.exit(EX_USAGE)

    for opt in opts:
        if opt[0] == "-d":
            conn = PipeConnection()

    return (conn,)


def main():
    (conn,) = parse_cmdline()
    i = Interactive(conn)
    i.run()


if __name__ == "__main__":
    main()
