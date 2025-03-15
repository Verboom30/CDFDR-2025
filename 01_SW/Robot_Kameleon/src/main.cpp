#include "mbed.h"
#include "pinout.hpp"
#include "TMC2209.hpp"
#include "Holonome.hpp"
#include <inttypes.h>



Holonome RobotHolonome(TMC_UART_TX, TMC_UART_RX, 
            STEP_A, DIR_A, 0x00,
            STEP_B, DIR_B, 0x01,
            STEP_C, DIR_C, 0x02,
            R_SENSE);


// SerialTMC *SWSerial = new SerialTMC(TMC_UART_TX, TMC_UART_RX);
// TMC2209Stepper StepperA(STEP_A, DIR_A, SWSerial, R_SENSE, 0x01);

int main()
{
  // SWSerial->beginSerial(155200);
  // wait_us(10*1000);
  // StepperA.begin();
  // StepperA.toff(TOFF);                // Enables driver in software - 3, 5 ????
  // StepperA.rms_current(RMSCURRENT);   // Set motor RMS current in mA / min 500 for 24V/speed:3000
  //                                      // 1110, 800
  //                                      // working: 800 12V/0,6Amax,  Speed up to 5200=4U/min
  // StepperA.microsteps(MICROSTEPS);    // Set microsteps to 1:Fullstep ... 256: 1/256th
  // StepperA.en_spreadCycle(EN_SPREADCYCLE);     // Toggle spreadCycle on TMC2208/2209/2224: default false, true: much faster!!!!
  // StepperA.pwm_autoscale(PWM_AUTOSCALE);       // Needed for stealthChop

  // StepperA.stop();
  // StepperA.setPositionZero();
  // StepperA.setSpeed(5000);
  // StepperA.setAcceleration(5000/ACC);
  // StepperA.setDeceleration(5000/DEC);
  // StepperA.move(20000);
  // while(!StepperA.stopped());

   RobotHolonome.setupSteppers();
  RobotHolonome.stop();
  while(!RobotHolonome.waitAck());
  RobotHolonome.setPositionZero();
  while(!RobotHolonome.waitAck());
  

  RobotHolonome.goesTo(0,0,90);
  while(!RobotHolonome.waitAck());
  while(!RobotHolonome.stopped());
 

  // while (1)
  // {
  // /* code */
  // }
}