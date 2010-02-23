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


// Documentation for this file. If the \file tag isn't present,
// this file won't be documented.
/** \file
 *  Implementation of serial I/O functions prototyped in pic24_serial.h
 */

#include "pic24_all.h"
#include <libpic30.h>

/*********************************************************
 * Public functions intended to be called by other files *
 *********************************************************/

#ifdef BUILT_ON_ESOS
#define outChar			esos_PutUint8ToCommOut
#else
/** Write a character to the serial port.
 *  This function blocks until a character is
 *  written. The UART used
 *  is determined by the __C30_UART variable, which
 *  defaults to 1.
 *  \param u8_c Character to write
 */
void outChar(uint8 u8_c) {
  switch (__C30_UART) {
#if (NUM_UART_MODS >= 1)
    case 1 :
      outChar1(u8_c);
      break;
#endif
    default :
      REPORT_ERROR("Invalid UART");
  }
}
#endif

/** Write a null-terminated string to the serial port.
See file documentation for End-of-line behavior when passed a "\n" (newline).
\param psz_s  Pointer to null-terminated string to print.
*/
void outString(const char* psz_s) {
  while (*psz_s) {

#if (SERIAL_EOL_DEFAULT==SERIAL_EOL_CR_LF)
	if (*psz_s == '\n') outChar(0x0D);
    outChar(*psz_s);    
#endif
#if (SERIAL_EOL_DEFAULT==SERIAL_EOL_CR)
    if (*psz_s == '\n') outChar(0x0D);
    else outChar(*psz_s);
#endif
#if (SERIAL_EOL_DEFAULT==SERIAL_EOL_LF)
//no translation
    outChar(*psz_s);
#endif
    psz_s++;
  }
}




void outUint8NoLeader(uint8 u8_x) {
  uint8 u8_c;
  u8_c = (u8_x>>4)& 0xf;
  if (u8_c > 9) outChar('A'+u8_c-10);
  else outChar('0'+u8_c);
  //LSDigit
  u8_c= u8_x & 0xf;
  if (u8_c > 9) outChar('A'+u8_c-10);
  else outChar('0'+u8_c);
}

/**
Output u8_x as formatted hex value with leading "0x".
\param u8_x value to output.
*/
void outUint8(uint8 u8_x) {
  outString("0x");
  outUint8NoLeader(u8_x);
}

/**
Output u16_x as formatted hex value with leading "0x".
\param u16_x value to output.
*/
void outUint16(uint16 u16_x) {
  uint8 u8_c;

  outString("0x");
  u8_c = (u16_x >> 8);
  outUint8NoLeader(u8_c);
  u8_c = (uint8) u16_x;
  outUint8NoLeader(u8_c);
}

/**
Output u32_x as formatted hex value with leading "0x".
\param u32_x value to output.
*/
void outUint32(uint32 u32_x) {
  uint8 u8_c;
  outString("0x");
  u8_c = (u32_x >> 24);
  outUint8NoLeader(u8_c);
  u8_c = (u32_x >> 16);
  outUint8NoLeader(u8_c);
  u8_c = (u32_x >> 8);
  outUint8NoLeader(u8_c);
  u8_c = u32_x;
  outUint8NoLeader(u8_c);
}

/** Configures a UART based compiler setting of DEFAULT_UART
 *  and sets __C30_UART to the default UART.
 *  If you want to configure a different UART, then call the configUARTx function explicitly.
 *  \param u32_baudRate The baud rate to use.
 */
void configDefaultUART(uint32 u32_baudRate) {
  switch (DEFAULT_UART) {
#if (NUM_UART_MODS >= 1)
    case 1 :
      __C30_UART = 1;   //this is the default UART
      configUART1(u32_baudRate);
      break;
#endif
    default :
      REPORT_ERROR("Invalid UART");
  }
}

