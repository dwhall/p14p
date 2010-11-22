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
#include "pic24_all.h"
#include "pyToC.h"
#include <stdio.h>

#undef __FILE_ID__
#define __FILE_ID__ 0x72


/** \file
Demonstrates pulse width modulation using four digital outputs
and the OC1 module to create four PWM outputs for hobby servos.
A table is used to control the pulse widths of the four servos.
This projects uses an external crystal for accuracy.
CLOCK_CONFIG=PRIPLL_8MHzCrystal_40MHzFCY is defined in the MPLAB project.
Remove this macro if you wish to use the internal oscillator.
*/

#define PWM_PERIOD 20000   //in microseconds  

static void configTimer2(void) {
  T2CON = T2_OFF | T2_IDLE_CON | T2_GATE_OFF
          | T2_32BIT_MODE_OFF
          | T2_SOURCE_INT
          | T2_PS_1_256 ;  //1 tick = 1.6 us at FCY=40 MHz
  PR2 = usToU16Ticks(PWM_PERIOD, getTimerPrescale(T2CONbits)) - 1;
  TMR2  = 0;       //clear timer2 value
}

//just pick four digital outputs
#define NUM_SERVOS 3
#define SERVO0  _LATB4
#define SERVO1  _LATA4
#define SERVO2  _LATB15

#define MIN_PW  600            //minimum pulse width, in us
#define MAX_PW 2400            //minimum pulse width, in us
#define SLOT_WIDTH 2800        //slot width, in us

static volatile uint16_t au16_servoPWidths[NUM_SERVOS];
static volatile uint16_t u16_currentServo =0;
static volatile uint16_t u16_servoEdge = 1;  //1 = RISING, 0 = FALLING
static volatile uint16_t u16_slotWidthTicks = 0;

static void initServos(void) {
  uint8 u8_i;
  uint16 u16_initPW;

  u16_currentServo = 0;
  CONFIG_RB4_AS_DIG_OUTPUT();
  CONFIG_RA4_AS_DIG_OUTPUT();
  CONFIG_RB15_AS_DIG_OUTPUT();
  u16_initPW = usToU16Ticks(MIN_PW + (MAX_PW-MIN_PW)/2, getTimerPrescale(T2CONbits));

  //config all servos for half maximum pulse width
  for (u8_i=0; u8_i<NUM_SERVOS; u8_i++) au16_servoPWidths[u8_i]=u16_initPW;
  SERVO0 = 0; //all servo outputs low initially
  SERVO1 = 0;
  SERVO2 = 0;
  u16_slotWidthTicks = usToU16Ticks(SLOT_WIDTH, getTimerPrescale(T2CONbits));
}

static void
setServoOutput(uint16_t u16_servo, uint16_t u16_val) {
  switch (u16_servo) {
    case 0:
      SERVO0 = u16_val;
      break;
    case 1:
      SERVO1 = u16_val;
      break;
    case 2:
      SERVO2 = u16_val;
      break;
    default:
      break;
  }
}

void _ISR
_OC1Interrupt(void) {
  _OC1IF = 0;
//change the servo's value
  setServoOutput(u16_currentServo, u16_servoEdge);
//schedule next interrupt
  if (u16_servoEdge == 1) {  //rising edge
    //next interrupt occurs after pulse width has elapsed
    OC1R = OC1R + au16_servoPWidths[u16_currentServo];
    u16_servoEdge = 0;     //change to falling edge
  } else { //falling edge
    //next interrupt occurs at beginning of next slot
    if (u16_currentServo != NUM_SERVOS - 1)
      OC1R = u16_slotWidthTicks*(u16_currentServo + 1);
    else //last servo!
      OC1R = 0;
    u16_servoEdge = 1;     //change to rising edge
    u16_currentServo++;
    if (u16_currentServo == NUM_SERVOS) u16_currentServo = 0;
  }
}

static void configOutputCapture1(void) {
  T2CONbits.TON = 0;       //disable Timer when configuring Output compare
  OC1R  =  0;  //initialize to 0
//turn on the compare toggle mode using Timer2
  OC1CON = OC_TIMER2_SRC |     //Timer2 source
           OC_TOGGLE_PULSE;    //use toggle mode, just care about compare event
  _OC1IF = 0;
  _OC1IP = 1;
  _OC1IE = 1;    //enable the OC1 interrupt
}


PmReturn_t
setServoPulseWidth(uint16_t u16_servo, uint16_t u16_pwUs)
{
  PmReturn_t retval = PM_RET_OK;

  EXCEPTION_UNLESS(u16_servo < NUM_SERVOS, PM_RET_EX_VAL,
    "Invalid servo %u.", u16_servo);
  EXCEPTION_UNLESS( (u16_pwUs >= MIN_PW) && (u16_pwUs <= MAX_PW),
    PM_RET_EX_VAL, "Invalid pulse width %u.", u16_pwUs);
  au16_servoPWidths[u16_servo] = usToU16Ticks(u16_pwUs, getTimerPrescale(T2CONbits));

  return retval;
}

/** Configure an output compare module for servo operation. */
/*  @param b_isTimer2 True to use timer2, false to use timer3 for
 *      PWM.
 *  @param u16_oc Output compare module to use.
 */
PmReturn_t
initMultiServo()
{
  PmReturn_t retval = PM_RET_OK;

  configTimer2();
  initServos();
  configOutputCapture1();
  T2CONbits.TON = 1;       //turn on Timer2 to start PWM

  return retval;
}
