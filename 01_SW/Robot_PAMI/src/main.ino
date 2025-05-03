#include "TMCStepper.h"
#include "BasicStepperDriver.h"
#include "pinout.h"
#include "UART_TMC.h"

#define R_SENSE 3.70f // Match to your driver
                      // SilentStepStick series use 0.11
                      // UltiMachine Einsy and Archim2 boards use 0.2
                      // Panucatt BSD2660 uses 0.1
                      // Watterott TMC5160 uses 0.075

Uart_TMC driverG(TMC_UART_RX, TMC_UART_TX, R_SENSE, 0b00);
Uart_TMC driverD(TMC_UART_RX, TMC_UART_TX, R_SENSE, 0b01);

// 2-wire basic config, microstepping is hardwired on the driver
BasicStepperDriver stepperG(MOTOR_STEPS, DIR_G, STEP_G);
BasicStepperDriver stepperD(MOTOR_STEPS, DIR_D, STEP_D);



void setup() {

 
  Serial.begin(115200);
  driverG.setup_stepper();
  driverD.setup_stepper();
  stepperG.begin(RPM, MSTEP);
  stepperD.begin(RPM, MSTEP);
 
  
 
  
  
}

void loop() {
  stepperG.move(-MOTOR_STEPS*MSTEP);
  stepperD.move(-MOTOR_STEPS*MSTEP);
  delay(1500);
}
