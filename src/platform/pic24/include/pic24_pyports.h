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


#ifndef _PIC24_PYPORTS_H_
#define _PIC24_PYPORTS_H_
// Documentation for this file. If the \file tag isn't present,
// this file won't be documented.
/** \file
 *  \brief This file supports configuration of IO ports on the PIC24.
 */

/** This macro tells how many digital I/O ports exist on the selected processor. */
#if   defined(_RG0)  || defined(_RG1)  || defined(_RG2)  || defined(_RG3)  || \
      defined(_RG4)  || defined(_RG5)  || defined(_RG6)  || defined(_RG7)  || \
      defined(_RG8)  || defined(_RG9)  || defined(_RG10) || defined(_RG11) || \
      defined(_RG12) || defined(_RG13) || defined(_RG14) || defined(_RG15)
  #define NUM_DIGITAL_PORTS 7
#elif defined(_RF0)  || defined(_RF1)  || defined(_RF2)  || defined(_RF3)  || \
      defined(_RF4)  || defined(_RF5)  || defined(_RF6)  || defined(_RF7)  || \
      defined(_RF8)  || defined(_RF9)  || defined(_RF10) || defined(_RF11) || \
      defined(_RF12) || defined(_RF13) || defined(_RF14) || defined(_RF15)
  #define NUM_DIGITAL_PORTS 6
#elif defined(_RE0)  || defined(_RE1)  || defined(_RE2)  || defined(_RE3)  || \
      defined(_RE4)  || defined(_RE5)  || defined(_RE6)  || defined(_RE7)  || \
      defined(_RE8)  || defined(_RE9)  || defined(_RE10) || defined(_RE11) || \
      defined(_RE12) || defined(_RE13) || defined(_RE14) || defined(_RE15)
  #define NUM_DIGITAL_PORTS 5
#elif defined(_RD0)  || defined(_RD1)  || defined(_RD2)  || defined(_RD3)  || \
      defined(_RD4)  || defined(_RD5)  || defined(_RD6)  || defined(_RD7)  || \
      defined(_RD8)  || defined(_RD9)  || defined(_RD10) || defined(_RD11) || \
      defined(_RD12) || defined(_RD13) || defined(_RD14) || defined(_RD15)
  #define NUM_DIGITAL_PORTS 4
#elif defined(_RC0)  || defined(_RC1)  || defined(_RC2)  || defined(_RC3)  || \
      defined(_RC4)  || defined(_RC5)  || defined(_RC6)  || defined(_RC7)  || \
      defined(_RC8)  || defined(_RC9)  || defined(_RC10) || defined(_RC11) || \
      defined(_RC12) || defined(_RC13) || defined(_RC14) || defined(_RC15)
  #define NUM_DIGITAL_PORTS 3
#elif defined(_RB0)  || defined(_RB1)  || defined(_RB2)  || defined(_RB3)  || \
      defined(_RB4)  || defined(_RB5)  || defined(_RB6)  || defined(_RB7)  || \
      defined(_RB8)  || defined(_RB9)  || defined(_RB10) || defined(_RB11) || \
      defined(_RB12) || defined(_RB13) || defined(_RB14) || defined(_RB15)
  #define NUM_DIGITAL_PORTS 2
#elif defined(_RA0)  || defined(_RA1)  || defined(_RA2)  || defined(_RA3)  || \
      defined(_RA4)  || defined(_RA5)  || defined(_RA6)  || defined(_RA7)  || \
      defined(_RA8)  || defined(_RA9)  || defined(_RA10) || defined(_RA11) || \
      defined(_RA12) || defined(_RA13) || defined(_RA14) || defined(_RA15) || \
      defined(__DOXYGEN__)
  #define NUM_DIGITAL_PORTS 1
#else
  #error No digital I/O ports were found on the current microprocessor.
  #error Something is very confused.
#endif

// Define HAS_PULL_DOWNS if this chip has pull-down ability.
#if defined(_CN0PDE)  || defined(_CN1PDE)  || \
    defined(_CN2PDE)  || defined(_CN3PDE)  || \
    defined(_CN4PDE)  || defined(_CN5PDE)  || \
    defined(_CN6PDE)  || defined(_CN7PDE)  || \
    defined(_CN8PDE)  || defined(_CN9PDE)  || \
    defined(_CN10PDE) || defined(_CN11PDE) || \
    defined(_CN12PDE) || defined(_CN13PDE) || \
    defined(_CN14PDE) || defined(_CN15PDE) || \
    defined(_CN15PDE) || defined(_CN17PDE) || \
    defined(_CN18PDE) || defined(_CN19PDE) || \
    defined(_CN20PDE) || defined(_CN21PDE) || \
    defined(_CN22PDE) || defined(_CN23PDE) || \
    defined(_CN24PDE) || defined(_CN25PDE) || \
    defined(_CN26PDE) || defined(_CN27PDE) || \
    defined(_CN28PDE) || defined(_CN29PDE) || \
    defined(_CN30PDE) || defined(_CN31PDE)
#define HAS_PULL_DOWNS
#endif

/** This variable stores a bitmap describing which digitial I/O pins exist
 *  on the current processor. Port A is stored at [0], B at [1], etc. A value
 *  of 1 for a gvien bit indicates the corresponding pin of that port exists.
 *  For example, 0x0001 indicates that only pin 0 of the selected port exists, 
 *  while pins 1-15 do not.
 */
const extern uint16_t u16_digitalPinPresent[NUM_DIGITAL_PORTS];

/** This variable stores a bitmap describing which digitial I/O pins can be
 *  configured as open-drain outputs on the current processor. Port A is 
 *  stored at [0], B at [1], etc. A value of 1 for a gvien bit indicates 
 *  the corresponding pin of that port can be made open-drain. For example, 
 *  0x0001 indicates that only pin 0 of the selected port can be made, 
 *  open-drain, while pins 1-15 operator only in the standard, totem-pole
 *  output configuration.
 */
const extern uint16_t u16_digitalPinOpenDrainPresent[NUM_DIGITAL_PORTS];

/** The values state that a given digital I/O pin does not 
 *  have analog capability. It is used in \ref anCnMap.
 */
#define UNDEF_AN_PIN 255

/** The values state that a given digital I/O pin does not 
 *  have change notification capability. It is used in \ref anCnMap.
 */
#define UNDEF_CN_PIN 255

/** Define a structure which contains a mapping from
 *  a digital I/O port/pin to an analog pin and to
 *  a change notification pin.
 */
typedef struct {
  /// The analog pin (ANxx) corresponding to a given digital port/pin,
  /// or \ref UNDEF_AN_PIN if the pin has no analog capability.
  uint8_t u8_anPin;
  /// The analog pin (ANxx) corresponding to a given digital port/pin,
  /// or \ref UNDEF_CN_PIN if the pin has no change notification capability.
  uint8_t u8_cnPin;
} anCnMap_t;

/** An array of the \ref anCnMap_t structure with an entry for each of
 *  the (up to) 16 pins on each digitial I/O port for this device.
 */
const extern anCnMap_t anCnMap[NUM_DIGITAL_PORTS * 16];

// Include appropriate ports file for the device in use.
// These definitions are then used below to map digital I/O ports to the
// corresponding analog and change notification pin
#if defined(__PIC24HJ128GP202__)

#include "devices/pic24hj128gp202_pyports.h"

#elif defined(__PIC24HJ128GP204__)

#include "devices/pic24hj128gp204_pyports.h"

#elif defined(__PIC24HJ128GP206__)

#include "devices/pic24hj128gp206_pyports.h"

#elif defined(__PIC24HJ128GP210__)

#include "devices/pic24hj128gp210_pyports.h"

#elif defined(__PIC24HJ128GP306__)

#include "devices/pic24hj128gp306_pyports.h"

#elif defined(__PIC24HJ128GP310__)

#include "devices/pic24hj128gp310_pyports.h"

#elif defined(__PIC24HJ128GP502__)

#include "devices/pic24hj128gp502_pyports.h"

#elif defined(__PIC24HJ128GP504__)

#include "devices/pic24hj128gp504_pyports.h"

#elif defined(__PIC24HJ128GP506__)

#include "devices/pic24hj128gp506_pyports.h"

#elif defined(__PIC24HJ128GP510__)

#include "devices/pic24hj128gp510_pyports.h"

#elif defined(__PIC24HJ12GP201__)

#include "devices/pic24hj12gp201_pyports.h"

#elif defined(__PIC24HJ12GP202__)

#include "devices/pic24hj12gp202_pyports.h"

#elif defined(__PIC24HJ16GP304__)

#include "devices/pic24hj16gp304_pyports.h"

#elif defined(__PIC24HJ256GP206__)

#include "devices/pic24hj256gp206_pyports.h"

#elif defined(__PIC24HJ256GP210__)

#include "devices/pic24hj256gp210_pyports.h"

#elif defined(__PIC24HJ256GP610__)

#include "devices/pic24hj256gp610_pyports.h"

#elif defined(__PIC24HJ32GP202__)

#include "devices/pic24hj32gp202_pyports.h"

#elif defined(__PIC24HJ32GP204__)

#include "devices/pic24hj32gp204_pyports.h"

#elif defined(__PIC24HJ32GP302__)

#include "devices/pic24hj32gp302_pyports.h"

#elif defined(__PIC24HJ32GP304__)

#include "devices/pic24hj32gp304_pyports.h"

#elif defined(__PIC24HJ64GP202__)

#include "devices/pic24hj64gp202_pyports.h"

#elif defined(__PIC24HJ64GP204__)

#include "devices/pic24hj64gp204_pyports.h"

#elif defined(__PIC24HJ64GP206__)

#include "devices/pic24hj64gp206_pyports.h"

#elif defined(__PIC24HJ64GP210__)

#include "devices/pic24hj64gp210_pyports.h"

#elif defined(__PIC24HJ64GP502__)

#include "devices/pic24hj64gp502_pyports.h"

#elif defined(__PIC24HJ64GP504__)

#include "devices/pic24hj64gp504_pyports.h"

#elif defined(__PIC24HJ64GP506__)

#include "devices/pic24hj64gp506_pyports.h"

#elif defined(__PIC24HJ64GP510__)

#include "devices/pic24hj64gp510_pyports.h"

#elif defined(__PIC24F04KA200__)

#include "devices/pic24f04ka200_pyports.h"

#elif defined(__PIC24F04KA201__)

#include "devices/pic24f04ka201_pyports.h"

#elif defined(__PIC24F08KA101__)

#include "devices/pic24f08ka101_pyports.h"

#elif defined(__PIC24F08KA102__)

#include "devices/pic24f08ka102_pyports.h"

#elif defined(__PIC24F16KA101__)

#include "devices/pic24f16ka101_pyports.h"

#elif defined(__PIC24F16KA102__)

#include "devices/pic24f16ka102_pyports.h"

#elif defined(__PIC24F32KA101__)

#include "devices/pic24f32ka101_pyports.h"

#elif defined(__PIC24F32KA102__)

#include "devices/pic24f32ka102_pyports.h"

#elif defined(__PIC24FJ128GA006__)

#include "devices/pic24fj128ga006_pyports.h"

#elif defined(__PIC24FJ128GA008__)

#include "devices/pic24fj128ga008_pyports.h"

#elif defined(__PIC24FJ128GA010__)

#include "devices/pic24fj128ga010_pyports.h"

#elif defined(__PIC24FJ128GA106__)

#include "devices/pic24fj128ga106_pyports.h"

#elif defined(__PIC24FJ128GA108__)

#include "devices/pic24fj128ga108_pyports.h"

#elif defined(__PIC24FJ128GA110__)

#include "devices/pic24fj128ga110_pyports.h"

#elif defined(__PIC24FJ128GB106__)

#include "devices/pic24fj128gb106_pyports.h"

#elif defined(__PIC24FJ128GB108__)

#include "devices/pic24fj128gb108_pyports.h"

#elif defined(__PIC24FJ128GB110__)

#include "devices/pic24fj128gb110_pyports.h"

#elif defined(__PIC24FJ16GA002__)

#include "devices/pic24fj16ga002_pyports.h"

#elif defined(__PIC24FJ16GA004__)

#include "devices/pic24fj16ga004_pyports.h"

#elif defined(__PIC24FJ192GA106__)

#include "devices/pic24fj192ga106_pyports.h"

#elif defined(__PIC24FJ192GA108__)

#include "devices/pic24fj192ga108_pyports.h"

#elif defined(__PIC24FJ192GA110__)

#include "devices/pic24fj192ga110_pyports.h"

#elif defined(__PIC24FJ192GB106__)

#include "devices/pic24fj192gb106_pyports.h"

#elif defined(__PIC24FJ192GB108__)

#include "devices/pic24fj192gb108_pyports.h"

#elif defined(__PIC24FJ192GB110__)

#include "devices/pic24fj192gb110_pyports.h"

#elif defined(__PIC24FJ256GA106__)

#include "devices/pic24fj256ga106_pyports.h"

#elif defined(__PIC24FJ256GA108__)

#include "devices/pic24fj256ga108_pyports.h"

#elif defined(__PIC24FJ256GA110__)

#include "devices/pic24fj256ga110_pyports.h"

#elif defined(__PIC24FJ256GB106__)

#include "devices/pic24fj256gb106_pyports.h"

#elif defined(__PIC24FJ256GB108__)

#include "devices/pic24fj256gb108_pyports.h"

#elif defined(__PIC24FJ256GB110__)

#include "devices/pic24fj256gb110_pyports.h"

#elif defined(__PIC24FJ32GA002__)

#include "devices/pic24fj32ga002_pyports.h"

#elif defined(__PIC24FJ32GA004__)

#include "devices/pic24fj32ga004_pyports.h"

#elif defined(__PIC24FJ48GA002__)

#include "devices/pic24fj48ga002_pyports.h"

#elif defined(__PIC24FJ48GA004__)

#include "devices/pic24fj48ga004_pyports.h"

#elif defined(__PIC24FJ64GA002__)

#include "devices/pic24fj64ga002_pyports.h"

#elif defined(__PIC24FJ64GA004__)

#include "devices/pic24fj64ga004_pyports.h"

#elif defined(__PIC24FJ64GA006__)

#include "devices/pic24fj64ga006_pyports.h"

#elif defined(__PIC24FJ64GA008__)

#include "devices/pic24fj64ga008_pyports.h"

#elif defined(__PIC24FJ64GA010__)

#include "devices/pic24fj64ga010_pyports.h"

#elif defined(__PIC24FJ64GA106__)

#include "devices/pic24fj64ga106_pyports.h"

#elif defined(__PIC24FJ64GA108__)

#include "devices/pic24fj64ga108_pyports.h"

#elif defined(__PIC24FJ64GA110__)

#include "devices/pic24fj64ga110_pyports.h"

#elif defined(__PIC24FJ64GB106__)

#include "devices/pic24fj64gb106_pyports.h"

#elif defined(__PIC24FJ64GB108__)

#include "devices/pic24fj64gb108_pyports.h"

#elif defined(__PIC24FJ64GB110__)

#include "devices/pic24fj64gb110_pyports.h"

#elif defined(__PIC24FJ96GA006__)

#include "devices/pic24fj96ga006_pyports.h"

#elif defined(__PIC24FJ96GA008__)

#include "devices/pic24fj96ga008_pyports.h"

#elif defined(__PIC24FJ96GA010__)

#include "devices/pic24fj96ga010_pyports.h"

#elif defined(__dsPIC33FJ06GS101__)

#include "devices/dspic33fj06gs101_pyports.h"

#elif defined(__dsPIC33FJ06GS102__)

#include "devices/dspic33fj06gs102_pyports.h"

#elif defined(__dsPIC33FJ06GS202__)

#include "devices/dspic33fj06gs202_pyports.h"

#elif defined(__dsPIC33FJ128GP202__)

#include "devices/dspic33fj128gp202_pyports.h"

#elif defined(__dsPIC33FJ128GP204__)

#include "devices/dspic33fj128gp204_pyports.h"

#elif defined(__dsPIC33FJ128GP206A__)

#include "devices/dspic33fj128gp206a_pyports.h"

#elif defined(__dsPIC33FJ128GP206__)

#include "devices/dspic33fj128gp206_pyports.h"

#elif defined(__dsPIC33FJ128GP306A__)

#include "devices/dspic33fj128gp306a_pyports.h"

#elif defined(__dsPIC33FJ128GP306__)

#include "devices/dspic33fj128gp306_pyports.h"

#elif defined(__dsPIC33FJ128GP310A__)

#include "devices/dspic33fj128gp310a_pyports.h"

#elif defined(__dsPIC33FJ128GP310__)

#include "devices/dspic33fj128gp310_pyports.h"

#elif defined(__dsPIC33FJ128GP706A__)

#include "devices/dspic33fj128gp706a_pyports.h"

#elif defined(__dsPIC33FJ128GP706__)

#include "devices/dspic33fj128gp706_pyports.h"

#elif defined(__dsPIC33FJ128GP708A__)

#include "devices/dspic33fj128gp708a_pyports.h"

#elif defined(__dsPIC33FJ128GP708__)

#include "devices/dspic33fj128gp708_pyports.h"

#elif defined(__dsPIC33FJ128GP710A__)

#include "devices/dspic33fj128gp710a_pyports.h"

#elif defined(__dsPIC33FJ128GP710__)

#include "devices/dspic33fj128gp710_pyports.h"

#elif defined(__dsPIC33FJ128GP802__)

#include "devices/dspic33fj128gp802_pyports.h"

#elif defined(__dsPIC33FJ128GP804__)

#include "devices/dspic33fj128gp804_pyports.h"

#elif defined(__dsPIC33FJ128MC202__)

#include "devices/dspic33fj128mc202_pyports.h"

#elif defined(__dsPIC33FJ128MC204__)

#include "devices/dspic33fj128mc204_pyports.h"

#elif defined(__dsPIC33FJ128MC506A__)

#include "devices/dspic33fj128mc506a_pyports.h"

#elif defined(__dsPIC33FJ128MC506__)

#include "devices/dspic33fj128mc506_pyports.h"

#elif defined(__dsPIC33FJ128MC510A__)

#include "devices/dspic33fj128mc510a_pyports.h"

#elif defined(__dsPIC33FJ128MC510__)

#include "devices/dspic33fj128mc510_pyports.h"

#elif defined(__dsPIC33FJ128MC706A__)

#include "devices/dspic33fj128mc706a_pyports.h"

#elif defined(__dsPIC33FJ128MC706__)

#include "devices/dspic33fj128mc706_pyports.h"

#elif defined(__dsPIC33FJ128MC708A__)

#include "devices/dspic33fj128mc708a_pyports.h"

#elif defined(__dsPIC33FJ128MC708__)

#include "devices/dspic33fj128mc708_pyports.h"

#elif defined(__dsPIC33FJ128MC710A__)

#include "devices/dspic33fj128mc710a_pyports.h"

#elif defined(__dsPIC33FJ128MC710__)

#include "devices/dspic33fj128mc710_pyports.h"

#elif defined(__dsPIC33FJ128MC802__)

#include "devices/dspic33fj128mc802_pyports.h"

#elif defined(__dsPIC33FJ128MC804__)

#include "devices/dspic33fj128mc804_pyports.h"

#elif defined(__dsPIC33FJ12GP201__)

#include "devices/dspic33fj12gp201_pyports.h"

#elif defined(__dsPIC33FJ12GP202__)

#include "devices/dspic33fj12gp202_pyports.h"

#elif defined(__dsPIC33FJ12MC201__)

#include "devices/dspic33fj12mc201_pyports.h"

#elif defined(__dsPIC33FJ12MC202__)

#include "devices/dspic33fj12mc202_pyports.h"

#elif defined(__dsPIC33FJ16GP304__)

#include "devices/dspic33fj16gp304_pyports.h"

#elif defined(__dsPIC33FJ16GS402__)

#include "devices/dspic33fj16gs402_pyports.h"

#elif defined(__dsPIC33FJ16GS404__)

#include "devices/dspic33fj16gs404_pyports.h"

#elif defined(__dsPIC33FJ16GS502__)

#include "devices/dspic33fj16gs502_pyports.h"

#elif defined(__dsPIC33FJ16GS504__)

#include "devices/dspic33fj16gs504_pyports.h"

#elif defined(__dsPIC33FJ16MC304__)

#include "devices/dspic33fj16mc304_pyports.h"

#elif defined(__dsPIC33FJ256GP506A__)

#include "devices/dspic33fj256gp506a_pyports.h"

#elif defined(__dsPIC33FJ256GP506__)

#include "devices/dspic33fj256gp506_pyports.h"

#elif defined(__dsPIC33FJ256GP510A__)

#include "devices/dspic33fj256gp510a_pyports.h"

#elif defined(__dsPIC33FJ256GP510__)

#include "devices/dspic33fj256gp510_pyports.h"

#elif defined(__dsPIC33FJ256GP710A__)

#include "devices/dspic33fj256gp710a_pyports.h"

#elif defined(__dsPIC33FJ256GP710__)

#include "devices/dspic33fj256gp710_pyports.h"

#elif defined(__dsPIC33FJ256MC510A__)

#include "devices/dspic33fj256mc510a_pyports.h"

#elif defined(__dsPIC33FJ256MC510__)

#include "devices/dspic33fj256mc510_pyports.h"

#elif defined(__dsPIC33FJ256MC710A__)

#include "devices/dspic33fj256mc710a_pyports.h"

#elif defined(__dsPIC33FJ256MC710__)

#include "devices/dspic33fj256mc710_pyports.h"

#elif defined(__dsPIC33FJ32GP202__)

#include "devices/dspic33fj32gp202_pyports.h"

#elif defined(__dsPIC33FJ32GP204__)

#include "devices/dspic33fj32gp204_pyports.h"

#elif defined(__dsPIC33FJ32GP302__)

#include "devices/dspic33fj32gp302_pyports.h"

#elif defined(__dsPIC33FJ32GP304__)

#include "devices/dspic33fj32gp304_pyports.h"

#elif defined(__dsPIC33FJ32GS406__)

#include "devices/dspic33fj32gs406_pyports.h"

#elif defined(__dsPIC33FJ32GS606__)

#include "devices/dspic33fj32gs606_pyports.h"

#elif defined(__dsPIC33FJ32GS608__)

#include "devices/dspic33fj32gs608_pyports.h"

#elif defined(__dsPIC33FJ32GS610__)

#include "devices/dspic33fj32gs610_pyports.h"

#elif defined(__dsPIC33FJ32MC202__)

#include "devices/dspic33fj32mc202_pyports.h"

#elif defined(__dsPIC33FJ32MC204__)

#include "devices/dspic33fj32mc204_pyports.h"

#elif defined(__dsPIC33FJ32MC302__)

#include "devices/dspic33fj32mc302_pyports.h"

#elif defined(__dsPIC33FJ32MC304__)

#include "devices/dspic33fj32mc304_pyports.h"

#elif defined(__dsPIC33FJ64GP202__)

#include "devices/dspic33fj64gp202_pyports.h"

#elif defined(__dsPIC33FJ64GP204__)

#include "devices/dspic33fj64gp204_pyports.h"

#elif defined(__dsPIC33FJ64GP206A__)

#include "devices/dspic33fj64gp206a_pyports.h"

#elif defined(__dsPIC33FJ64GP206__)

#include "devices/dspic33fj64gp206_pyports.h"

#elif defined(__dsPIC33FJ64GP306A__)

#include "devices/dspic33fj64gp306a_pyports.h"

#elif defined(__dsPIC33FJ64GP306__)

#include "devices/dspic33fj64gp306_pyports.h"

#elif defined(__dsPIC33FJ64GP310A__)

#include "devices/dspic33fj64gp310a_pyports.h"

#elif defined(__dsPIC33FJ64GP310__)

#include "devices/dspic33fj64gp310_pyports.h"

#elif defined(__dsPIC33FJ64GP706A__)

#include "devices/dspic33fj64gp706a_pyports.h"

#elif defined(__dsPIC33FJ64GP706__)

#include "devices/dspic33fj64gp706_pyports.h"

#elif defined(__dsPIC33FJ64GP708A__)

#include "devices/dspic33fj64gp708a_pyports.h"

#elif defined(__dsPIC33FJ64GP708__)

#include "devices/dspic33fj64gp708_pyports.h"

#elif defined(__dsPIC33FJ64GP710A__)

#include "devices/dspic33fj64gp710a_pyports.h"

#elif defined(__dsPIC33FJ64GP710__)

#include "devices/dspic33fj64gp710_pyports.h"

#elif defined(__dsPIC33FJ64GP802__)

#include "devices/dspic33fj64gp802_pyports.h"

#elif defined(__dsPIC33FJ64GP804__)

#include "devices/dspic33fj64gp804_pyports.h"

#elif defined(__dsPIC33FJ64GS406__)

#include "devices/dspic33fj64gs406_pyports.h"

#elif defined(__dsPIC33FJ64GS606__)

#include "devices/dspic33fj64gs606_pyports.h"

#elif defined(__dsPIC33FJ64GS608__)

#include "devices/dspic33fj64gs608_pyports.h"

#elif defined(__dsPIC33FJ64GS610__)

#include "devices/dspic33fj64gs610_pyports.h"

#elif defined(__dsPIC33FJ64MC202__)

#include "devices/dspic33fj64mc202_pyports.h"

#elif defined(__dsPIC33FJ64MC204__)

#include "devices/dspic33fj64mc204_pyports.h"

#elif defined(__dsPIC33FJ64MC506A__)

#include "devices/dspic33fj64mc506a_pyports.h"

#elif defined(__dsPIC33FJ64MC506__)

#include "devices/dspic33fj64mc506_pyports.h"

#elif defined(__dsPIC33FJ64MC508A__)

#include "devices/dspic33fj64mc508a_pyports.h"

#elif defined(__dsPIC33FJ64MC508__)

#include "devices/dspic33fj64mc508_pyports.h"

#elif defined(__dsPIC33FJ64MC510A__)

#include "devices/dspic33fj64mc510a_pyports.h"

#elif defined(__dsPIC33FJ64MC510__)

#include "devices/dspic33fj64mc510_pyports.h"

#elif defined(__dsPIC33FJ64MC706A__)

#include "devices/dspic33fj64mc706a_pyports.h"

#elif defined(__dsPIC33FJ64MC706__)

#include "devices/dspic33fj64mc706_pyports.h"

#elif defined(__dsPIC33FJ64MC710A__)

#include "devices/dspic33fj64mc710a_pyports.h"

#elif defined(__dsPIC33FJ64MC710__)

#include "devices/dspic33fj64mc710_pyports.h"

#elif defined(__dsPIC33FJ64MC802__)

#include "devices/dspic33fj64mc802_pyports.h"

#elif defined(__dsPIC33FJ64MC804__)

#include "devices/dspic33fj64mc804_pyports.h"

#else

#error -- processor ID not specified in pic24_pyports.h

#endif


#ifndef _PIC24_PYDIGIO_DEFINED
#warning Digital IO macros not defined for this device!
#warning Edit common\pic24_pyports.h file!
#endif

#endif  // #define _PIC24_PYPORTS_H_
