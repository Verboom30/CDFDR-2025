#include "TMCStepper.h"
#include "BasicStepperDriver.h"
#include "pinout.h"
#include "UART_TMC.h"
#include <Wire.h>
#include <VL53L0X.h>

#define R_SENSE 3.70f // Match to your driver
                      // SilentStepStick series use 0.11
                      // UltiMachine Einsy and Archim2 boards use 0.2
                      // Panucatt BSD2660 uses 0.1
                      // Watterott TMC5160 uses 0.075

Uart_TMC driverG(TMC_UART_RX, TMC_UART_TX, R_SENSE, 0b00);
Uart_TMC driverD(TMC_UART_RX, TMC_UART_TX, R_SENSE, 0b01);

// 2-wire basic config, microstepping is hardwired on the driver
BasicStepperDriver stepper(MOTOR_STEPS, DIR_G, STEP_G, DIR_D, STEP_D);


VL53L0X sensor1;
VL53L0X sensor2;

void setup() {
  Serial.begin(115200);
  pinMode(LED_R,OUTPUT);
  pinMode(LED_G,OUTPUT);
  pinMode(XSHUT_1,OUTPUT);
  pinMode(XSHUT_2,OUTPUT);
  pinMode(TIRETTE,INPUT_PULLUP);
  pinMode(SW_TEAM,INPUT_PULLUP);
  pinMode(SW_BAU,INPUT_PULLUP);

  digitalWrite(LED_R,LOW);
  digitalWrite(LED_G,LOW);
  digitalWrite(LED_B,LOW);

  digitalWrite(XSHUT_1,HIGH);
  digitalWrite(XSHUT_2,LOW);

  //Config First VL530X
  Wire.begin();
  sensor1.setTimeout(500);
  if (!sensor1.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    digitalWrite(LED_R,HIGH);
    while (1) {}
  }
  Serial.print("Addr first sensor this: ");
  Serial.println(sensor1.getAddress(),HEX);
  Serial.println("Set Addr to 1: ");
  sensor1.setAddress(1);
  Serial.print("Read Addr: ");
  Serial.println(sensor1.getAddress(),HEX);
  Serial.println();

  //Config Second VL530X
  digitalWrite(XSHUT_1,HIGH);
  digitalWrite(XSHUT_2,HIGH);
  
  sensor2.setTimeout(500);
  if (!sensor2.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    digitalWrite(LED_R,HIGH);
    while (1) {}
  }
  Serial.print("Addr second sensor this: ");
  Serial.println(sensor2.getAddress(),HEX);
  Serial.println("Set Addr to 2: ");
  sensor2.setAddress(2);
  Serial.print("Read Addr: ");
  Serial.println(sensor2.getAddress(),HEX);
  Serial.println();

  sensor1.startContinuous();
  sensor2.startContinuous();

  
  //driverG.setup_stepper();
  //driverD.setup_stepper();
  //stepper.begin(RPM, MSTEP);

}

void loop() {
  Serial.print("sensor1: " );
  Serial.print(sensor1.readRangeContinuousMillimeters());
  if (sensor1.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
  Serial.print("  ");
  Serial.print("sensor2: " );
  Serial.print(sensor2.readRangeContinuousMillimeters());
  if (sensor1.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
  Serial.println();
  //stepper.move(MOTOR_STEPS*MSTEP);
  //delay(500);
}
