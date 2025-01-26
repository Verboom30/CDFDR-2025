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

int main()
{
   RobotHolonome.setupSteppers();
  // RobotHolonome.stop();
  // while(!RobotHolonome.waitAck());
  // RobotHolonome.setPositionZero();
  // while(!RobotHolonome.waitAck());

 

  // while (1)
  // {
  // /* code */
  // }
}