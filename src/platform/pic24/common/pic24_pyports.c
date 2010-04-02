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
 *  \brief This file supports Python-based configuration of 
 *  IO ports on the PIC24.
 */

#include "pic24_all.h"

// These macro auto-generate the bits by testing for their presense
// as macros in the chip-specific include file.
const uint16_t u16_digitalPinPresent[NUM_DIGITAL_PORTS] = { 
  #if NUM_DIGITAL_PORTS >= 1
    #ifdef _RA0
      0x0001 |
    #endif
    #ifdef _RA1
      0x0002 |
    #endif
    #ifdef _RA2
      0x0004 |
    #endif
    #ifdef _RA3
      0x0008 |
    #endif
    #ifdef _RA4
      0x0010 |
    #endif
    #ifdef _RA5
      0x0020 |
    #endif
    #ifdef _RA6
      0x0040 |
    #endif
    #ifdef _RA7
      0x0080 |
    #endif
    #ifdef _RA8
      0x0100 |
    #endif
    #ifdef _RA9
      0x0200 |
    #endif
    #ifdef _RA10
      0x0400 |
    #endif
    #ifdef _RA11
      0x0800 |
    #endif
    #ifdef _RA12
      0x1000 |
    #endif
    #ifdef _RA13
      0x2000 |
    #endif
    #ifdef _RA14
      0x4000 |
    #endif
    #ifdef _RA15
      0x8000 |
    #endif
    0x0000,
  #endif

  #if NUM_DIGITAL_PORTS >= 2
    #ifdef _RB0
      0x0001 |
    #endif
    #ifdef _RB1
      0x0002 |
    #endif
    #ifdef _RB2
      0x0004 |
    #endif
    #ifdef _RB3
      0x0008 |
    #endif
    #ifdef _RB4
      0x0010 |
    #endif
    #ifdef _RB5
      0x0020 |
    #endif
    #ifdef _RB6
      0x0040 |
    #endif
    #ifdef _RB7
      0x0080 |
    #endif
    #ifdef _RB8
      0x0100 |
    #endif
    #ifdef _RB9
      0x0200 |
    #endif
    #ifdef _RB10
      0x0400 |
    #endif
    #ifdef _RB11
      0x0800 |
    #endif
    #ifdef _RB12
      0x1000 |
    #endif
    #ifdef _RB13
      0x2000 |
    #endif
    #ifdef _RB14
      0x4000 |
    #endif
    #ifdef _RB15
      0x8000 |
    #endif
    0x0000,
  #endif

  #if NUM_DIGITAL_PORTS >= 3
    #ifdef _RC0
      0x0001 |
    #endif
    #ifdef _RC1
      0x0002 |
    #endif
    #ifdef _RC2
      0x0004 |
    #endif
    #ifdef _RC3
      0x0008 |
    #endif
    #ifdef _RC4
      0x0010 |
    #endif
    #ifdef _RC5
      0x0020 |
    #endif
    #ifdef _RC6
      0x0040 |
    #endif
    #ifdef _RC7
      0x0080 |
    #endif
    #ifdef _RC8
      0x0100 |
    #endif
    #ifdef _RC9
      0x0200 |
    #endif
    #ifdef _RC10
      0x0400 |
    #endif
    #ifdef _RC11
      0x0800 |
    #endif
    #ifdef _RC12
      0x1000 |
    #endif
    #ifdef _RC13
      0x2000 |
    #endif
    #ifdef _RC14
      0x4000 |
    #endif
    #ifdef _RC15
      0x8000 |
    #endif
    0x0000,
  #endif

  #if NUM_DIGITAL_PORTS >= 4
    #ifdef _RD0
      0x0001 |
    #endif
    #ifdef _RD1
      0x0002 |
    #endif
    #ifdef _RD2
      0x0004 |
    #endif
    #ifdef _RD3
      0x0008 |
    #endif
    #ifdef _RD4
      0x0010 |
    #endif
    #ifdef _RD5
      0x0020 |
    #endif
    #ifdef _RD6
      0x0040 |
    #endif
    #ifdef _RD7
      0x0080 |
    #endif
    #ifdef _RD8
      0x0100 |
    #endif
    #ifdef _RD9
      0x0200 |
    #endif
    #ifdef _RD10
      0x0400 |
    #endif
    #ifdef _RD11
      0x0800 |
    #endif
    #ifdef _RD12
      0x1000 |
    #endif
    #ifdef _RD13
      0x2000 |
    #endif
    #ifdef _RD14
      0x4000 |
    #endif
    #ifdef _RD15
      0x8000 |
    #endif
    0x0000,
  #endif

  #if NUM_DIGITAL_PORTS >= 5
    #ifdef _RE0
      0x0001 |
    #endif
    #ifdef _RE1
      0x0002 |
    #endif
    #ifdef _RE2
      0x0004 |
    #endif
    #ifdef _RE3
      0x0008 |
    #endif
    #ifdef _RE4
      0x0010 |
    #endif
    #ifdef _RE5
      0x0020 |
    #endif
    #ifdef _RE6
      0x0040 |
    #endif
    #ifdef _RE7
      0x0080 |
    #endif
    #ifdef _RE8
      0x0100 |
    #endif
    #ifdef _RE9
      0x0200 |
    #endif
    #ifdef _RE10
      0x0400 |
    #endif
    #ifdef _RE11
      0x0800 |
    #endif
    #ifdef _RE12
      0x1000 |
    #endif
    #ifdef _RE13
      0x2000 |
    #endif
    #ifdef _RE14
      0x4000 |
    #endif
    #ifdef _RE15
      0x8000 |
    #endif
    0x0000,
  #endif

  #if NUM_DIGITAL_PORTS >= 6
    #ifdef _RF0
      0x0001 |
    #endif
    #ifdef _RF1
      0x0002 |
    #endif
    #ifdef _RF2
      0x0004 |
    #endif
    #ifdef _RF3
      0x0008 |
    #endif
    #ifdef _RF4
      0x0010 |
    #endif
    #ifdef _RF5
      0x0020 |
    #endif
    #ifdef _RF6
      0x0040 |
    #endif
    #ifdef _RF7
      0x0080 |
    #endif
    #ifdef _RF8
      0x0100 |
    #endif
    #ifdef _RF9
      0x0200 |
    #endif
    #ifdef _RF10
      0x0400 |
    #endif
    #ifdef _RF11
      0x0800 |
    #endif
    #ifdef _RF12
      0x1000 |
    #endif
    #ifdef _RF13
      0x2000 |
    #endif
    #ifdef _RF14
      0x4000 |
    #endif
    #ifdef _RF15
      0x8000 |
    #endif
    0x0000,
  #endif

  #if NUM_DIGITAL_PORTS >= 7
    #ifdef _RG0
      0x0001 |
    #endif
    #ifdef _RG1
      0x0002 |
    #endif
    #ifdef _RG2
      0x0004 |
    #endif
    #ifdef _RG3
      0x0008 |
    #endif
    #ifdef _RG4
      0x0010 |
    #endif
    #ifdef _RG5
      0x0020 |
    #endif
    #ifdef _RG6
      0x0040 |
    #endif
    #ifdef _RG7
      0x0080 |
    #endif
    #ifdef _RG8
      0x0100 |
    #endif
    #ifdef _RG9
      0x0200 |
    #endif
    #ifdef _RG10
      0x0400 |
    #endif
    #ifdef _RG11
      0x0800 |
    #endif
    #ifdef _RG12
      0x1000 |
    #endif
    #ifdef _RG13
      0x2000 |
    #endif
    #ifdef _RG14
      0x4000 |
    #endif
    #ifdef _RG15
      0x8000 |
    #endif
    0x0000,
  #endif
  };






// These macro auto-generate the bits by testing for their presense
// as macros in the chip-specific include file. Note that the bit
// name for PIC24H and dsPIC33 processors is _ODCxy, while PIC24F 
// uses _ODxy. Therefore, these macros test for both.
const uint16_t u16_digitalPinOpenDrainPresent[NUM_DIGITAL_PORTS] = {
  #if NUM_DIGITAL_PORTS >= 1
    #if defined(_ODCA0)  || defined(_ODA0)
      0x0001 |
    #endif
    #if defined(_ODCA1)  || defined(_ODA1)
      0x0002 |
    #endif
    #if defined(_ODCA2)  || defined(_ODA2)
      0x0004 |
    #endif
    #if defined(_ODCA3)  || defined(_ODA3)
      0x0008 |
    #endif
    #if defined(_ODCA4)  || defined(_ODA4)
      0x0010 |
    #endif
    #if defined(_ODCA5)  || defined(_ODA5)
      0x0020 |
    #endif
    #if defined(_ODCA6)  || defined(_ODA6)
      0x0040 |
    #endif
    #if defined(_ODCA7)  || defined(_ODA7)
      0x0080 |
    #endif
    #if defined(_ODCA8)  || defined(_ODA8)
      0x0100 |
    #endif
    #if defined(_ODCA9)  || defined(_ODA9)
      0x0200 |
    #endif
    #if defined(_ODCA10) || defined(_ODA10)
      0x0400 |
    #endif
    #if defined(_ODCA11) || defined(_ODA11)
      0x0800 |
    #endif
    #if defined(_ODCA12) || defined(_ODA12)
      0x1000 |
    #endif
    #if defined(_ODCA13) || defined(_ODA13)
      0x2000 |
    #endif
    #if defined(_ODCA14) || defined(_ODA14)
      0x4000 |
    #endif
    #if defined(_ODCA15) || defined(_ODA15)
      0x8000 |
    #endif
    0x0000,
  #endif 

  #if NUM_DIGITAL_PORTS >= 2
    #if defined(_ODCB0)  || defined(_ODB0)
      0x0001 |
    #endif
    #if defined(_ODCB1)  || defined(_ODB1)
      0x0002 |
    #endif
    #if defined(_ODCB2)  || defined(_ODB2)
      0x0004 |
    #endif
    #if defined(_ODCB3)  || defined(_ODB3)
      0x0008 |
    #endif
    #if defined(_ODCB4)  || defined(_ODB4)
      0x0010 |
    #endif
    #if defined(_ODCB5)  || defined(_ODB5)
      0x0020 |
    #endif
    #if defined(_ODCB6)  || defined(_ODB6)
      0x0040 |
    #endif
    #if defined(_ODCB7)  || defined(_ODB7)
      0x0080 |
    #endif
    #if defined(_ODCB8)  || defined(_ODB8)
      0x0100 |
    #endif
    #if defined(_ODCB9)  || defined(_ODB9)
      0x0200 |
    #endif
    #if defined(_ODCB10) || defined(_ODB10)
      0x0400 |
    #endif
    #if defined(_ODCB11) || defined(_ODB11)
      0x0800 |
    #endif
    #if defined(_ODCB12) || defined(_ODB12)
      0x1000 |
    #endif
    #if defined(_ODCB13) || defined(_ODB13)
      0x2000 |
    #endif
    #if defined(_ODCB14) || defined(_ODB14)
      0x4000 |
    #endif
    #if defined(_ODCB15) || defined(_ODB15)
      0x8000 |
    #endif
    0x0000,
  #endif 

  #if NUM_DIGITAL_PORTS >= 3
    #if defined(_ODCC0)  || defined(_ODC0)
      0x0001 |
    #endif
    #if defined(_ODCC1)  || defined(_ODC1)
      0x0002 |
    #endif
    #if defined(_ODCC2)  || defined(_ODC2)
      0x0004 |
    #endif
    #if defined(_ODCC3)  || defined(_ODC3)
      0x0008 |
    #endif
    #if defined(_ODCC4)  || defined(_ODC4)
      0x0010 |
    #endif
    #if defined(_ODCC5)  || defined(_ODC5)
      0x0020 |
    #endif
    #if defined(_ODCC6)  || defined(_ODC6)
      0x0040 |
    #endif
    #if defined(_ODCC7)  || defined(_ODC7)
      0x0080 |
    #endif
    #if defined(_ODCC8)  || defined(_ODC8)
      0x0100 |
    #endif
    #if defined(_ODCC9)  || defined(_ODC9)
      0x0200 |
    #endif
    #if defined(_ODCC10) || defined(_ODC10)
      0x0400 |
    #endif
    #if defined(_ODCC11) || defined(_ODC11)
      0x0800 |
    #endif
    #if defined(_ODCC12) || defined(_ODC12)
      0x1000 |
    #endif
    #if defined(_ODCC13) || defined(_ODC13)
      0x2000 |
    #endif
    #if defined(_ODCC14) || defined(_ODC14)
      0x4000 |
    #endif
    #if defined(_ODCC15) || defined(_ODC15)
      0x8000 |
    #endif
    0x0000,
  #endif 

  #if NUM_DIGITAL_PORTS >= 4
    #if defined(_ODCD0)  || defined(_ODD0)
      0x0001 |
    #endif
    #if defined(_ODCD1)  || defined(_ODD1)
      0x0002 |
    #endif
    #if defined(_ODCD2)  || defined(_ODD2)
      0x0004 |
    #endif
    #if defined(_ODCD3)  || defined(_ODD3)
      0x0008 |
    #endif
    #if defined(_ODCD4)  || defined(_ODD4)
      0x0010 |
    #endif
    #if defined(_ODCD5)  || defined(_ODD5)
      0x0020 |
    #endif
    #if defined(_ODCD6)  || defined(_ODD6)
      0x0040 |
    #endif
    #if defined(_ODCD7)  || defined(_ODD7)
      0x0080 |
    #endif
    #if defined(_ODCD8)  || defined(_ODD8)
      0x0100 |
    #endif
    #if defined(_ODCD9)  || defined(_ODD9)
      0x0200 |
    #endif
    #if defined(_ODCD10) || defined(_ODD10)
      0x0400 |
    #endif
    #if defined(_ODCD11) || defined(_ODD11)
      0x0800 |
    #endif
    #if defined(_ODCD12) || defined(_ODD12)
      0x1000 |
    #endif
    #if defined(_ODCD13) || defined(_ODD13)
      0x2000 |
    #endif
    #if defined(_ODCD14) || defined(_ODD14)
      0x4000 |
    #endif
    #if defined(_ODCD15) || defined(_ODD15)
      0x8000 |
    #endif
    0x0000,
  #endif 

  #if NUM_DIGITAL_PORTS >= 5
    #if defined(_ODCE0)  || defined(_ODE0)
      0x0001 |
    #endif
    #if defined(_ODCE1)  || defined(_ODE1)
      0x0002 |
    #endif
    #if defined(_ODCE2)  || defined(_ODE2)
      0x0004 |
    #endif
    #if defined(_ODCE3)  || defined(_ODE3)
      0x0008 |
    #endif
    #if defined(_ODCE4)  || defined(_ODE4)
      0x0010 |
    #endif
    #if defined(_ODCE5)  || defined(_ODE5)
      0x0020 |
    #endif
    #if defined(_ODCE6)  || defined(_ODE6)
      0x0040 |
    #endif
    #if defined(_ODCE7)  || defined(_ODE7)
      0x0080 |
    #endif
    #if defined(_ODCE8)  || defined(_ODE8)
      0x0100 |
    #endif
    #if defined(_ODCE9)  || defined(_ODE9)
      0x0200 |
    #endif
    #if defined(_ODCE10) || defined(_ODE10)
      0x0400 |
    #endif
    #if defined(_ODCE11) || defined(_ODE11)
      0x0800 |
    #endif
    #if defined(_ODCE12) || defined(_ODE12)
      0x1000 |
    #endif
    #if defined(_ODCE13) || defined(_ODE13)
      0x2000 |
    #endif
    #if defined(_ODCE14) || defined(_ODE14)
      0x4000 |
    #endif
    #if defined(_ODCE15) || defined(_ODE15)
      0x8000 |
    #endif
    0x0000,
  #endif 

  #if NUM_DIGITAL_PORTS >= 6
    #if defined(_ODCF0)  || defined(_ODF0)
      0x0001 |
    #endif
    #if defined(_ODCF1)  || defined(_ODF1)
      0x0002 |
    #endif
    #if defined(_ODCF2)  || defined(_ODF2)
      0x0004 |
    #endif
    #if defined(_ODCF3)  || defined(_ODF3)
      0x0008 |
    #endif
    #if defined(_ODCF4)  || defined(_ODF4)
      0x0010 |
    #endif
    #if defined(_ODCF5)  || defined(_ODF5)
      0x0020 |
    #endif
    #if defined(_ODCF6)  || defined(_ODF6)
      0x0040 |
    #endif
    #if defined(_ODCF7)  || defined(_ODF7)
      0x0080 |
    #endif
    #if defined(_ODCF8)  || defined(_ODF8)
      0x0100 |
    #endif
    #if defined(_ODCF9)  || defined(_ODF9)
      0x0200 |
    #endif
    #if defined(_ODCF10) || defined(_ODF10)
      0x0400 |
    #endif
    #if defined(_ODCF11) || defined(_ODF11)
      0x0800 |
    #endif
    #if defined(_ODCF12) || defined(_ODF12)
      0x1000 |
    #endif
    #if defined(_ODCF13) || defined(_ODF13)
      0x2000 |
    #endif
    #if defined(_ODCF14) || defined(_ODF14)
      0x4000 |
    #endif
    #if defined(_ODCF15) || defined(_ODF15)
      0x8000 |
    #endif
    0x0000,
  #endif 

  #if NUM_DIGITAL_PORTS >= 7
    #if defined(_ODCG0)  || defined(_ODG0)
      0x0001 |
    #endif
    #if defined(_ODCG1)  || defined(_ODG1)
      0x0002 |
    #endif
    #if defined(_ODCG2)  || defined(_ODG2)
      0x0004 |
    #endif
    #if defined(_ODCG3)  || defined(_ODG3)
      0x0008 |
    #endif
    #if defined(_ODCG4)  || defined(_ODG4)
      0x0010 |
    #endif
    #if defined(_ODCG5)  || defined(_ODG5)
      0x0020 |
    #endif
    #if defined(_ODCG6)  || defined(_ODG6)
      0x0040 |
    #endif
    #if defined(_ODCG7)  || defined(_ODG7)
      0x0080 |
    #endif
    #if defined(_ODCG8)  || defined(_ODG8)
      0x0100 |
    #endif
    #if defined(_ODCG9)  || defined(_ODG9)
      0x0200 |
    #endif
    #if defined(_ODCG10) || defined(_ODG10)
      0x0400 |
    #endif
    #if defined(_ODCG11) || defined(_ODG11)
      0x0800 |
    #endif
    #if defined(_ODCG12) || defined(_ODG12)
      0x1000 |
    #endif
    #if defined(_ODCG13) || defined(_ODG13)
      0x2000 |
    #endif
    #if defined(_ODCG14) || defined(_ODG14)
      0x4000 |
    #endif
    #if defined(_ODCG15) || defined(_ODG15)
      0x8000 |
    #endif
    0x0000,
  #endif 
 };



// The AN_CN_MAP macro specifies the mapping. It is defined in
// the device/<devicename>_pyports.h file.
const anCnMap_t anCnMap[NUM_DIGITAL_PORTS * 16] = { AN_CN_MAP };
