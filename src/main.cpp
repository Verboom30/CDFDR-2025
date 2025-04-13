#include "mbed.h"
#include "pinout.hpp"
#include "TMC2209.hpp"
#include <inttypes.h>

#define R_SENSE 3.7f   // R-Sense in OHM. Match to your driver
#define RMSCURRENT 200  // RMS current of Stepper Coil in mA
#define MICROSTEPS 16  // # of microsteps

TMC2209Stepper StepperA(STEP_R1,DIR_R1,TMC_UART_TX, TMC_UART_RX, R_SENSE, 0x00);

DigitalOut En_drive_N(ENABLE_DRIVE_N);
DigitalOut En_step_N(ENABLE_STEP_N);
DigitalOut sel_0(SEL_UART_0);
DigitalOut sel_1(SEL_UART_1);
DigitalOut sel_2(SEL_UART_2);
int main()
{
  sel_0 =0;
  sel_1 =0;
  sel_2 =1;
  En_drive_N =0;
  En_step_N =0;
  HAL_Delay (500);
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
  StepperA.setAcceleration(0);
  StepperA.setDeceleration(0);
  StepperA.stop();
  StepperA.setPositionZero();

  //printf("Read Uart 0x%08"PRIx32"\n",StepperA.IOIN());
  StepperA.move(5000);
  while(!StepperA.stopped());
  // while (1)
  // {
  // /* code */
  // }
}