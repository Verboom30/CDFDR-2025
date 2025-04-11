#include "UART_TMC.hpp"
//***********************************/************************************
//                         Constructors                                 //
//***********************************/************************************
Uart_TMC::Uart_TMC(PinName Uart_TX_pin, PinName Uart_RX_pin, PinName Sel_uart_0, PinName Sel_uart_1, PinName Sel_uart_2,float RS): 
_sel_0(Sel_uart_0),
_sel_1(Sel_uart_1),
_sel_2(Sel_uart_2)
{
    SWSerialHolonome =new SerialTMC(Uart_TX_pin, Uart_RX_pin);

    UART_StepperA = new TMC2209Stepper(SWSerialHolonome, RS, 0x00);
    UART_StepperB = new TMC2209Stepper(SWSerialHolonome, RS, 0x01);
    UART_StepperC = new TMC2209Stepper(SWSerialHolonome, RS, 0x02);

    UART_StepperR1 = new TMC2209Stepper(SWSerialHolonome, RS, 0x00);
    UART_StepperR2 = new TMC2209Stepper(SWSerialHolonome, RS, 0x01);
    UART_StepperR3 = new TMC2209Stepper(SWSerialHolonome, RS, 0x02);
    UART_StepperR4 = new TMC2209Stepper(SWSerialHolonome, RS, 0x03);

    UART_StepperFork = new TMC2209Stepper(SWSerialHolonome, RS, 0x01);
    UART_StepperSucker = new TMC2209Stepper(SWSerialHolonome, RS, 0x00);
    _sel_0 =0;
    _sel_1 =0;
    _sel_2 =0;

}

bool Uart_TMC::setup_all_stepper(void)
{
  SWSerialHolonome->beginSerial(155200);
  wait_us(10*1000);
  
  _sel_0 =1;
  _sel_1 =0;
  _sel_2 =0;
  UART_StepperA->begin();
  UART_StepperB->begin();
  UART_StepperC->begin();

  //***********************************/************************************
  // StepperA                                                              /
  //***********************************/************************************
  UART_StepperA->toff(TOFF);                // Enables driver in software - 3, 5 ????
  UART_StepperA->rms_current(RMSCURRENT);   // Set motor RMS current in mA / min 500 for 24V/speed:3000
                                       // 1110, 800
                                       // working: 800 12V/0,6Amax,  Speed up to 5200=4U/min
  UART_StepperA->microsteps(MSTEP);    // Set microsteps to 1:Fullstep ... 256: 1/256th
  UART_StepperA->en_spreadCycle(EN_SPREADCYCLE);     // Toggle spreadCycle on TMC2208/2209/2224: default false, true: much faster!!!!
  UART_StepperA->pwm_autoscale(PWM_AUTOSCALE);       // Needed for stealthChop

  //***********************************/************************************
  // StepperB                                                              /
  //***********************************/************************************
  UART_StepperB->toff(TOFF);                // Enables driver in software - 3, 5 ????
  UART_StepperB->rms_current(RMSCURRENT);   // Set motor RMS current in mA / min 500 for 24V/speed:3000
                                       // 1110, 800
                                       // working: 800 12V/0,6Amax,  Speed up to 5200=4U/min
  UART_StepperB->microsteps(MSTEP);    // Set microsteps to 1:Fullstep ... 256: 1/256th
  UART_StepperB->en_spreadCycle(EN_SPREADCYCLE);     // Toggle spreadCycle on TMC2208/2209/2224: default false, true: much faster!!!!
  UART_StepperB->pwm_autoscale(PWM_AUTOSCALE);       // Needed for stealthChop


  //***********************************/************************************
  // StepperC                                                              /
  //***********************************/************************************
  UART_StepperC->toff(TOFF);                 // Enables driver in software - 3, 5 ????
  UART_StepperC->rms_current(RMSCURRENT);   // Set motor RMS current in mA / min 500 for 24V/speed:3000
                                       // 1110, 800
                                       // working: 800 12V/0,6Amax,  Speed up to 5200=4U/min
  UART_StepperC->microsteps(MSTEP);    // Set microsteps to 1:Fullstep ... 256: 1/256th
  UART_StepperC->en_spreadCycle(EN_SPREADCYCLE);     // Toggle spreadCycle on TMC2208/2209/2224: default false, true: much faster!!!!
  UART_StepperC->pwm_autoscale(PWM_AUTOSCALE);       // Needed for stealthChop
  
  wait_us(10*1000);
  _sel_0 =0;
  _sel_1 =1;
  _sel_2 =0;

  UART_StepperR1->begin();
  UART_StepperR2->begin();
  UART_StepperR3->begin();
  UART_StepperR4->begin();
  //***********************************/************************************
  // UART_StepperR1                                                              /
  //***********************************/************************************
  UART_StepperR1->toff(TOFF);                // Enables driver in software - 3, 5 ????
  UART_StepperR1->rms_current(RMSCURRENT);   // Set motor RMS current in mA / min 500 for 24V/speed:3000
                                       // 1110, 800
                                       // working: 800 12V/0,6Amax,  Speed up to 5200=4U/min
  UART_StepperR1->microsteps(MSTEP);    // Set microsteps to 1:Fullstep ... 256: 1/256th
  UART_StepperR1->en_spreadCycle(EN_SPREADCYCLE);     // Toggle spreadCycle on TMC2208/2209/2224: default false, true: much faster!!!!
  UART_StepperR1->pwm_autoscale(PWM_AUTOSCALE);       // Needed for stealthChop

  //***********************************/************************************
  // UART_StepperR2                                                              /
  //***********************************/************************************
  UART_StepperR2->toff(TOFF);                // Enables driver in software - 3, 5 ????
  UART_StepperR2->rms_current(RMSCURRENT);   // Set motor RMS current in mA / min 500 for 24V/speed:3000
                                       // 1110, 800
                                       // working: 800 12V/0,6Amax,  Speed up to 5200=4U/min
  UART_StepperR2->microsteps(MSTEP);    // Set microsteps to 1:Fullstep ... 256: 1/256th
  UART_StepperR2->en_spreadCycle(EN_SPREADCYCLE);     // Toggle spreadCycle on TMC2208/2209/2224: default false, true: much faster!!!!
  UART_StepperR2->pwm_autoscale(PWM_AUTOSCALE);       // Needed for stealthChop

  //***********************************/************************************
  // UART_StepperR3                                                              /
  //***********************************/************************************
  UART_StepperR3->toff(TOFF);                // Enables driver in software - 3, 5 ????
  UART_StepperR3->rms_current(RMSCURRENT);   // Set motor RMS current in mA / min 500 for 24V/speed:3000
                                       // 1110, 800
                                       // working: 800 12V/0,6Amax,  Speed up to 5200=4U/min
  UART_StepperR3->microsteps(MSTEP);    // Set microsteps to 1:Fullstep ... 256: 1/256th
  UART_StepperR3->en_spreadCycle(EN_SPREADCYCLE);     // Toggle spreadCycle on TMC2208/2209/2224: default false, true: much faster!!!!
  UART_StepperR3->pwm_autoscale(PWM_AUTOSCALE);       // Needed for stealthChop

  //***********************************/************************************
  // UART_StepperR4                                                              /
  //***********************************/************************************
  UART_StepperR4->toff(TOFF);                // Enables driver in software - 3, 5 ????
  UART_StepperR4->rms_current(RMSCURRENT);   // Set motor RMS current in mA / min 500 for 24V/speed:3000
                                       // 1110, 800
                                       // working: 800 12V/0,6Amax,  Speed up to 5200=4U/min
  UART_StepperR4->microsteps(MSTEP);    // Set microsteps to 1:Fullstep ... 256: 1/256th
  UART_StepperR4->en_spreadCycle(EN_SPREADCYCLE);     // Toggle spreadCycle on TMC2208/2209/2224: default false, true: much faster!!!!
  UART_StepperR4->pwm_autoscale(PWM_AUTOSCALE);       // Needed for stealthChop
  
  wait_us(10*1000);
  _sel_0 =0;
  _sel_1 =0;
  _sel_2 =1;
  UART_StepperFork->begin();
  UART_StepperSucker->begin();

  //***********************************/************************************
  // UART_StepperFork                                                              /
  //***********************************/************************************
  UART_StepperFork->toff(TOFF);                // Enables driver in software - 3, 5 ????
  UART_StepperFork->rms_current(RMSCURRENT);   // Set motor RMS current in mA / min 500 for 24V/speed:3000
                                       // 1110, 800
                                       // working: 800 12V/0,6Amax,  Speed up to 5200=4U/min
  UART_StepperFork->microsteps(MSTEP);    // Set microsteps to 1:Fullstep ... 256: 1/256th
  UART_StepperFork->en_spreadCycle(EN_SPREADCYCLE);     // Toggle spreadCycle on TMC2208/2209/2224: default false, true: much faster!!!!
  UART_StepperFork->pwm_autoscale(PWM_AUTOSCALE);       // Needed for stealthChop
   
  //***********************************/************************************
  // UART_StepperSucker                                                              /
  //***********************************/************************************
  UART_StepperSucker->toff(TOFF);                // Enables driver in software - 3, 5 ????
  UART_StepperSucker->rms_current(RMSCURRENT);   // Set motor RMS current in mA / min 500 for 24V/speed:3000
                                       // 1110, 800
                                       // working: 800 12V/0,6Amax,  Speed up to 5200=4U/min
  UART_StepperSucker->microsteps(MSTEP);    // Set microsteps to 1:Fullstep ... 256: 1/256th
  UART_StepperSucker->en_spreadCycle(EN_SPREADCYCLE);     // Toggle spreadCycle on TMC2208/2209/2224: default false, true: much faster!!!!
  UART_StepperSucker->pwm_autoscale(PWM_AUTOSCALE);       // Needed for stealthChop

  wait_us(10*1000);
  _sel_0 =0;
  _sel_1 =0;
  _sel_2 =0;
  
  return true;
}

