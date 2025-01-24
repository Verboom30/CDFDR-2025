#include "mbed.h"
#include "pinout.hpp"
#include "TMC2209.hpp"
#include <inttypes.h>

TMC2209Stepper StepperA(TMC_UART_TX, TMC_UART_RX,0x00);


int main()
{
  printf("\r\nConnected to mbed\r\n");
  StepperA.beginSerial(115200);
  wait_us(1000);


  //serial_write(0x0B);
  //write(0x20,0x11112222);
  printf("Read Uart 0x%08"PRIx32"\n",StepperA.read(0x06));
  while (1)
  {
  /* code */
  }
}