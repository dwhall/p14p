/*
 * "Copyright (c) 2008 Robert B. Reese, Bryan A. Jones, J. W. Bruce ("AUTHORS")"
 * All rights reserved.
 * (R. Reese, reese_AT_ece.msstate.edu, Mississippi State University)
 * (B. A. Jones, bjones_AT_ece.msstate.edu, Mississippi State University)
 * (J. W. Bruce, jwbruce_AT_ece.msstate.edu, Mississippi State University)
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice, the following
 * two paragraphs and the authors appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE "AUTHORS" BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE "AUTHORS"
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE "AUTHORS" SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE "AUTHORS" HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 * Please maintain this header in its entirety when copying/modifying
 * these files.
 *
 *
 */

#ifndef __DATA_XFER_IMPL_H__
#define __DATA_XFER_IMPL_H__

/** \file
 *  \brief Routines which implement the \ref dataXfer "uC comm protocol".
 *
 *  This implements code which receives data from a uC sent
 *  either as individual characters or as data packets. For a
 *  complete specification, see the \ref dataXfer "uC comm protocol".
 *
 *  Two state machines implement this spec. The receive state
 *  machine uses the command-finding state machine in its operation,
 *  so that a user of this code will not normally need access
 *  to the command-finding machine.
 *
 *  Both machines can be reset (resetCommandFindMachine(),
 *  resetReceiveMachine()) and advanced by one state
 *  (stepCommandFindMachine(), stepReceiveMachine()).
 *
 *  Two mechanisms provide read access to the receive
 *  state machine. Low-level calls (getReceiveMachineOutChar(),
 *  getReceiveMachineIndex(), getReceiveMachineError(),
 *  getReceiveMachineIsSpec()) report machine
 *  state, while high-level calls (isReceiveMachineChar(),
 *  isReceiveMachineData(), isReceiveMachineSpec()) aggreate that state into more
 *  meaningful information.
 *
 *  Note: this is implemented in C (not C++), so that similar code can be re-used on
 *  a uC.
 *
 *  \section implementation Implementation
 *  The uC keeps state in \ref xferVar;
 *  a NULL address indicates nothing is present.
 *
 *  On receive: implement as a state machine (see sketch below).
 *
 *  \section receiveSketch Receive state machine sketch
 *  For simplicity, "getch" really means one invocation of the receive
 *  state machine, which is runs until the next getch. Timeouts are not
 *  shown: if too much time passes, the state machine is reset to the
 *  top. Likewise, receiving a command at any unexpected point causes
 *  a state machine reset.
<pre>
varBits = getch
if !command:
	c = varBits  // This was just a normal char; return it
	return index = 0
	restart
// varBits is a command, not a char
if isLongVar(varBits):
	varNum, len = getch
else
	assign varNum, len from varBits
if isVarSpec and !PIC
	getch len times to buf
	parse to var spec
	report any errors, restart
else if isValid(varNum, len)
	getch len times to var
	return varIndex
else
	getch len times and discard
	report error, restart
</pre>
 *
 *  \author Bryan A. Jones, bjones AT ece DOT msstate DOT edu.
 */

#include <stdlib.h>

// Determine if we're compiling for the PIC or not.
#if defined(__PIC24H__) || defined(__PIC24F__) || defined(__dsPIC33F__) || defined(__PIC24FK__)
#define __PIC__
#include "pic24_all.h"
#endif


// Specify that this is C code for proper C++ linking
#ifdef __cplusplus
extern "C" {
#endif

  /** \name Constants
   *  These values are \#defined as necessary so that the work under:
   *  - Pre-C99 (no bool, true, or false); this is MSVC++ in C mode
   *    and GCC for the PIC24.
   *  - C++ (bool, true, and false built in)
   *  - C++/CLI (NULL not defined)
   */
// @{
#if defined(__cplusplus)
#ifndef NULL
/// Define NULL under C++/CLI for portability
#define NULL nullptr
#endif

#ifndef FALSE
/// The boolean value false.
#define FALSE false
#endif

#ifndef TRUE
/// The boolean value true.
#define TRUE true
#endif

/// A boolean data type, which can take on values of
/// \ref TRUE and \ref FALSE.
#define BOOL bool

/// On MSVC under C++, use a throw for an assert. This
/// is defined already for the PIC.
#ifndef ASSERT
#ifdef _NOASSERT
#define ASSERT(placeholder) (void)0
#else
#define ASSERT(x) if (!(x)) throw #x
#endif
#endif

/// An assert with message macro
#ifndef ASSERTM
#ifdef _NOASSERT
#define ASSERT(msg, expr) (void)0
#else
#define ASSERTM(msg, expr) if (! (expr)) throw msg ": " #expr
#endif
#endif


#else

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef ASSERT
#ifdef _NOASSERT
#define ASSERT(placeholder) (void)0
#else
#include <assert.h>
#define ASSERT(x) assert(x)
#endif
#endif

/// An assert with message macro; the msg isn't used in C
#ifndef ASSERTM
#define ASSERTM(msg, expr) ASSERT(expr)
#endif

#define BOOL unsigned char
#endif
// @}

/// An abbreviation for an unsigned integer.
typedef unsigned int uint;

#ifndef __PIC__
/// An abbreviation for an 8-bit unsigned integer.
typedef unsigned char uint8;
#endif


// Unit testing: uncomment the following line to run PIC tests on PC.
//#define __PIC__

/// \name Command-finding state machine
//@{

/// The character used to begin a command. If this is not a command,
/// then use the two-character sequence \ref CMD_TOKEN
/// \ref ESCAPED_CMD; a \ref CMD_TOKEN followed
/// by any other value is a command.
#define CMD_TOKEN ((char) 0xAA)

/// After a \ref CMD_TOKEN "command token", this value specifies that
/// the character \ref CMD_TOKEN was sent.
#define ESCAPED_CMD  ((char) 0xFC)

/// After a \ref CMD_TOKEN "command token", this value specifies that
/// the command is a long variable.
#define CMD_LONG_VAR  ((char) 0xFD)

/// After a \ref CMD_TOKEN "command token", this value specifies that
/// the command is a send-only var.
#define CMD_SEND_ONLY ((char) 0xFE)

/// After a \ref CMD_TOKEN "command token", this value specifies that
/// the command is a send/receive var.
#define CMD_SEND_RECEIVE_VAR ((char) 0xFF)

/// The number of bits in the variable size field of the varBits field, following a
/// command token.
#define VAR_SIZE_BITS 2

/// A mask which removes all but the variable size bits in the varBits field.
#define VAR_SIZE_MASK ((1 << VAR_SIZE_BITS) - 1)

/// The maximum size of a short variable (which must fit in \ref VAR_SIZE_BITS number
/// of bits).
#define SHORT_VAR_MAX_LEN (1 << VAR_SIZE_BITS)

/// State of the command-finding state machine. See \ref stepCommandFindMachine
/// for more information.
typedef enum {
  /// The machine is in its starting state.
  STATE_CMD_START,
  /// The machine is waiting for another character; c_outChar is not valid.
  STATE_CMD_WAIT1,
  /// The machine is waiting for an additional character; c_outChar is not valid.
  STATE_CMD_WAIT2
} CMD_STATE;

/// The output of the command-finding state machine. See \ref stepCommandFindMachine
/// for more information.
typedef enum {
  /// The state machine produced no output, but is waiting for additional input.
  OUTPUT_CMD_NONE,
  /// A character was received; c_outChar contains the character.
  OUTPUT_CMD_CHAR,
  /// A command was received; c_outChar contains the command.
  OUTPUT_CMD_CMD,
  /// A repeated command was received; c_outChar contains the command.
  OUTPUT_CMD_REPEATED_CMD,
  /// The machine received a \ref CMD_TOKEN \ref CMD_TOKEN \ref CMD_TOKEN,
  /// so report a repeated command and wait for the next character to
  /// finish decoding.
  OUTPUT_CMD_REPEATED_WAIT
} CMD_OUTPUT;

/// Resets the command-finding state machine; see \ref stepCommandFindMachine
/// for more information.
void resetCommandFindMachine();

/** The command-finding state machine looks for commands in the data
 *  passed to it. Sequences it recognizes:
 *  - c, where c != \ref CMD_TOKEN, outputs the character c.
 *  - \ref CMD_TOKEN \ref ESCAPED_CMD outputs the character \ref CMD_TOKEN
 *    (known as an escaped command).
 *  - \ref CMD_TOKEN c, where c != \ref CMD_TOKEN and c != \ref ESCAPED_CMD,
 *    outputs the command c.
 *  - \ref CMD_TOKEN \ref CMD_TOKEN \ref ESCAPED_CMD outputs the command
 *    \ref CMD_TOKEN (note that the second \ref CMD_TOKEN is "escaped",
 *    so it is treated as a character specifying the command, not as the
 *    beginning of a command).
 *  - \ref CMD_TOKEN \ref CMD_TOKEN \ref CMD_TOKEN outputs a repeated
 *    command then waits for the next character. See the state machine
 *    sketch below for more information.
 *  - \ref CMD_TOKEN \ref CMD_TOKEN c, where c != \ref CMD_TOKEN and
 *    c != \ref ESCAPED_CMD, outputs
 *    a repeated command c. This is a protocol violation, but must be
 *    reported at a higher level; this routine merely reports a repeated command.
 *  A sketch of the state machine: <pre>
 *  case START :
 *    if (c == CMD_TOKEN) state = WAIT1
 *    else output c as a character
 *  case WAIT1 :
 *    if (c == CMD_TOKEN) state = WAIT2
 *    if (c == ESCAPED_CMD) state = START, output CMD_TOKEN as a character
 *    else output c as a command
 *  case WAIT2 :
 *    if (c == ESCAPED_CMD) state = START, output command CMD_TOKEN
 *    if (c == CMD_TOKEN) output repeated command, remain in this state
 *    else output repeated command c
 *  </pre>
 *  \param c_inChar A character input to the machine.
 *  \param c_outChar The character/command output by the machine when
 *                   the returned state is not CMD_WAIT.
 *  \return The output of the machine, which indicates if a
 *          command or character is available.
 */
CMD_OUTPUT stepCommandFindMachine(char c_inChar, char* c_outChar);
//@}



/// \name Data structures to store received variables
//@{

/// Struct to hold send/receive data. An array of these entries holds all the
/// necessary state.
typedef struct {
  /// Pointer to the data to be exchanged. NULL means this entry is not
  /// defined.
  uint8* pu8_data;
  /// Size of data in bytes – 1: 0 = 1 byte, etc.
  uint8 u8_size;
#if !defined(__PIC__) || defined(__DOXYGEN__)
  /// printf format string to use in displaying the variable. <b>PC only.</b>
  char* psz_format;
  /// Name of this variable, typically the same as used
  /// in the code. <b>PC only.</b>
  char* psz_name;
  /// Description of this variable. <b>PC only.</b>
  char* psz_desc;
#endif
} XFER_VAR;

/// Maximum number of transfer variables supported
#define MAX_NUM_XFER_VARS ((1 << (8 - VAR_SIZE_BITS)) - 1)

/// Number of transfer variables supported. Must be less than
/// the \ref MAX_NUM_XFER_VARS.
#define NUM_XFER_VARS 8
#if NUM_XFER_VARS > MAX_NUM_XFER_VARS
#error Too many transfer variables; there must be MAX_NUM_XFER_VARS or fewer.
#endif

/// A table to hold the state of transfer variables.
extern XFER_VAR xferVar[NUM_XFER_VARS];

/// An array of isWriteable bits for each var. Each bit is true if the PC is
/// allowed to change this variable; false otherwise. This does *NOT*
/// restrict the PIC to read-only access to this variable.
extern uint8 au8_xferVarWriteable[NUM_XFER_VARS/8 + ((NUM_XFER_VARS % 8) > 0)];

//@}



/// \name Receive state machine
//@{

/// This value retured for the index from the receive state
/// machine (see, e.g., \ref isReceiveMachineData) indicates
/// that a character, not a command, was received.
#define CHAR_RECEIVED_INDEX 0xFF

/// States of the receive state machine. See
/// \ref stepReceiveMachine for more information.
typedef enum {
  /// At the start of the machine
  STATE_RECV_START,
  /// Waiting for a command or escaped \ref CMD_TOKEN
  STATE_RECV_CMD_WAIT,
  /// Reading data bytes in from a command
  STATE_RECV_READ_BYTES,
  /// Reading the variable index for a long var command
  STATE_RECV_LONG_INDEX,
  /// Reading the variable length for a long/specification command
  STATE_RECV_LONG_LENGTH,
  /// Reading the variable index for a specification command
  STATE_RECV_SPEC_INDEX,
} RECEIVE_STATE;


/// Protocol errors produced by the receive state machine.
/// Internal errors (invalid state transitions, etc.) are
/// detected via ASSERTs.
typedef enum {
  /// No error; all state machine outputs are valid
  /// when the state is \ref STATE_RECV_START after execution of
  /// \ref stepReceiveMachine.
  ERR_NONE = 0,
  /// A repeated command (the sequence \ref CMD_TOKEN
  /// \ref CMD_TOKEN c, where c != \ref ESCAPED_CMD),
  /// was received.
  ERR_REPEATED_CMD,
  /// A timeout occurred in the middle of receiving a
  /// command.
  ERR_TIMEOUT,
  /// A command occurred in the middle of receiving
  /// data belonging to an earlier command.
  ERR_INTERRUPTED_CMD,
  /// Data was sent to a variable that has not been
  /// specified: the pointer to its data is NULL.
  ERR_UNSPECIFIED_INDEX,
  /// Data was sent to a variable which exceeds the
  /// \ref NUM_XFER_VARS.
  ERR_INDEX_TOO_HIGH,
  /// The size of data sent to a variable does not
  /// match the size specified earlier.
  ERR_VAR_SIZE_MISMATCH,
  /// The destination variable is read-only.
  ERR_READ_ONLY_VAR,
  /// The PIC is sent a variable specification
  ERR_PIC_VAR_SPEC
} RECEIVE_ERROR;

/// Number of error codes in the \ref RECEIVE_ERROR enum.
#define NUM_ERROR_CODES (ERR_PIC_VAR_SPEC + 1)

/// Return the current receive machine state. See \ref stepReceiveMachine
/// for more information.
RECEIVE_STATE getReceiveMachineState();

/// Return the character output by the receive state machine.
/// See \ref stepReceiveMachine for more information.
char getReceiveMachineOutChar();

/// Return the index output by the receive state machine.
/// See \ref stepReceiveMachine for more information.
uint getReceiveMachineIndex();

/// Return the error last encountered by the receive state machine.
/// See \ref stepReceiveMachine for more information. This also
/// clears the error status.
RECEIVE_ERROR getReceiveMachineError();

#if !defined(__PIC__) || defined(__DOXYGEN__)
/// Determine if the last data found by the receive state machine
/// was a specification; if not, it was data.
/// See \ref stepReceiveMachine for more information. <b>PC only.</b>
BOOL getReceiveMachineIsSpec();
#endif

/// Reset the receive state machine to its initial state and clear the
/// error status. The outputs are not reset, because they will not be
/// valid until after an invocation of the state machine.
void resetReceiveMachine();

/// Clear the current receive machine error status; the caller should
/// therefore handle or report this error to a higher level of the program.
void clearReceiveMachineError();

/// Clear the received data structure, so that no variables are
/// specified.
void clearReceiveStruct();

/** Determines if the receive state machine just received a character.
 *  \return True when the machine just received a character.
 */
BOOL isReceiveMachineChar();

/** Determines if the receive state machine just received some data.
 *  \return True when the machine just received some data.
 */
BOOL isReceiveMachineData();

#if !defined(__PIC__) || defined(__DOXYGEN__)
/** Determines if the receive state machine just received an updated specification. 
 *  <b>PC only.</b>
 *  \return True when the machine just received an updated spec.
 */
BOOL isReceiveMachineSpec();
#endif

/** Return the index of a variable in a command byte.
 *  \param c_cmd Command byte.
 *  \return Index of the variable.
 */
uint getVarIndex(char c_cmd);

/** Return the number of bytes of a variable in a command byte.
 * \param c_cmd Command byte.
 * \return Number of bytes.
 */
uint getVarLength(char c_cmd);

/** Assign a bit in the \ref au8_xferVarWriteable bit field.
 *  \param u_index The index of the variable to set.
 *  \param b_bitVal Bit value to set at this index.
 */
void assignBit(uint u_index, BOOL b_bitVal);

/** Read a bit in the \ref au8_xferVarWriteable bit field.
 *  \param u_index The index of the variable to set.
 *  \return The bit value at this index. TRUE indicated the
 *     variable is writeable; FALSE indicates a read-only
 *     variable: only the PIC, but not the PC, may change
 *     its value.
 */
BOOL isVarWriteable(uint u_index);

/** Notify the state machine that a timeout occurred between receiving
 *  the previous and next character. A timeout may not lead to an error;
 *  for example, between two received data packets or two received characters,
 *  timeout are allowed.
 *  \return An error code; ERROR_NONE (which is false) means no error
 *          occurred.
 */
RECEIVE_ERROR notifyOfTimeout();

/** This state machine receives data from the PIC. It takes a character
 *  received plus an indication if a timeout occurred since the last
 *  invocation of this function and advances the machine. The machine
 *  produces outputs when the returned state is \ref STATE_RECV_START. Outputs:
 *  - c_outChar, set if a character (versus a data packet) was received
 *  - u_index, set to the index of the data received
 *  - receiveError, an error code
 *  - b_isSpec, true if a specification packet was received.
 *  \param c_inChar A character for the state machine to process.
 *  \return An error code; ERROR_NONE (which is false) means no error
 *          occurred.
 */
RECEIVE_ERROR stepReceiveMachine(char c_inChar);

/// Returns an error string matching the last error code.
const char* getReceiveErrorString();

#ifdef __cplusplus
}
#endif

#endif
