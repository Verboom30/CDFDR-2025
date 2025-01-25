#include "mbed.h"
#include "pinout.hpp"
#include "TMC2209.hpp"
#include <inttypes.h>

#define R_SENSE 0.11f   // R-Sense in OHM. Match to your driver
#define RMSCURRENT 1500  // RMS current of Stepper Coil in mA
#define MICROSTEPS 8  // # of microsteps

TMC2209Stepper StepperA(PIN_STEP,PIN_DIR,TMC_UART_TX, TMC_UART_RX, R_SENSE, 0x00);

int main()
{
  printf("connecting to TMC-Module...\r\n");
  StepperA.begin();  
  //read and check version - must be 0x21
  uint8_t tmc_version = StepperA.version();
  printf("TMC-Version: %02X\r\n",tmc_version);
   if (tmc_version != 0x21) {
           printf("Wrong TMC-Version(not 0x21) or communication error!! STOPPING!!!\r\n");
           if (StepperA.CRCerror) {
               printf("CRC-Error!!!\r\n");
           }
   }

  StepperA.toff(5);                   // Enables driver in software - 3, 5 ????
  StepperA.rms_current(RMSCURRENT);   // Set motor RMS current in mA / min 500 for 24V/speed:3000
                                      // 1110, 800
                                      // working: 800 12V/0,6Amax,  Speed up to 5200=4U/min
  StepperA.microsteps(MICROSTEPS);    // Set microsteps to 1:Fullstep ... 256: 1/256th
  StepperA.en_spreadCycle(false);      // Toggle spreadCycle on TMC2208/2209/2224: default false, true: much faster!!!!
  StepperA.pwm_autoscale(true);       // Needed for stealthChop

  StepperA.setSpeed(10000);
  StepperA.setAcceleration(4000);
  StepperA.setDeceleration(4000);
  StepperA.stop();
  StepperA.setPositionZero();

  //printf("Read Uart 0x%08"PRIx32"\n",StepperA.IOIN());
  StepperA.move(10000);
  while(!StepperA.stopped());
  // while (1)
  // {
  // /* code */
  // }
}