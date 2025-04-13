#ifndef __UART_TMC_H_
#define __UART_TMC_H_

#include "mbed.h"
#include "pinout.hpp"
#include "TMC2209/TMC2209.hpp"
#include "LinearActuator.hpp"

class Uart_TMC
{
    public:
    Uart_TMC(PinName Uart_TX_pin, PinName Uart_RX_pin, PinName Sel_uart_0, PinName Sel_uart_1, PinName Sel_uart_2,float RS);

    SerialTMC * SWSerialHolonome;
    TMC2209Stepper* UART_StepperA;
    TMC2209Stepper* UART_StepperB;
    TMC2209Stepper* UART_StepperC;
    TMC2209Stepper* UART_StepperR1;
    TMC2209Stepper* UART_StepperR2;
    TMC2209Stepper* UART_StepperR3;
    TMC2209Stepper* UART_StepperR4;
    TMC2209Stepper* UART_StepperFork;
    TMC2209Stepper* UART_StepperSucker;
    bool setup_all_stepper();
   

    private : 
    DigitalOut _sel_0;                        
    DigitalOut _sel_1;  
    DigitalOut _sel_2; 

     
    protected:
};

#endif // __TMC2209_H_