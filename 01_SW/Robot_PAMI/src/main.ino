// main.ino
#include <Wire.h>
#include <VL53L0X.h>
#include <AccelStepper.h>
#include <TMCStepper.h>
#include <Thread.h>
#include <ThreadController.h>
#include "Differentiel.h"
#include "UART_TMC.h"
#include "pinout.h"

#define R_SENSE 3.7f
#define DELAY_START 500

Uart_TMC driverG(TMC_UART_RX, TMC_UART_TX, R_SENSE, 0b00);
Uart_TMC driverD(TMC_UART_RX, TMC_UART_TX, R_SENSE, 0b01);

AccelStepper stepperG(AccelStepper::DRIVER, STEP_G, DIR_G);
AccelStepper stepperD(AccelStepper::DRIVER, STEP_D, DIR_D);
differentiel RobotDiff(&stepperG, &stepperD);

VL53L0X sensor1, sensor2;
ThreadController controller;
Thread ThreadSensors; 
Thread ThreadDrive; 
Thread ThreadShowPos;

int state;
bool StopMove = false;
bool StartSequence = false;
bool tiretteRemoved = false;
bool sequenceReady = false;
unsigned long tiretteRemovedTime = 0;

void checkSensors() {
  int dist1 = sensor1.readRangeContinuousMillimeters();
  int dist2 = sensor2.readRangeContinuousMillimeters();
  StopMove = (dist1 < 200 || dist2 < 200);
  digitalWrite(LED_R, StopMove);
}

void showPosition() {

  RobotDiff.updatePosition();
  // Serial.print("[Position] X = ");
  // Serial.print(RobotDiff.getPositionX(), 2);
  // Serial.print(" mm | Y = ");
  // Serial.print(RobotDiff.getPositionY(), 2);
  // Serial.print(" mm | Angle = ");
  // Serial.print(RobotDiff.getAlpha(), 2);
  // Serial.println("°");

  // Serial.print(RobotDiff.getPositionX(), 2);
  // Serial.print(";");
  // Serial.print(RobotDiff.getPositionY(), 2);
  // Serial.print(";");
  // Serial.print(RobotDiff.getAlpha(), 2);
  // Serial.print("\r\n");
}



void taskDrive() {
  static bool movementDone = false;

  if (!tiretteRemoved && digitalRead(TIRETTE) == HIGH) {
    tiretteRemoved = true;
    tiretteRemovedTime = millis();
  }
  if (tiretteRemoved && !sequenceReady && (millis() - tiretteRemovedTime >= DELAY_START)) {
    sequenceReady = true;
  }
  switch (state) {
    case 0:
      if (sequenceReady){
        RobotDiff.setPosition(70,1900,90);
        state++;
      }
      break;
    case 1:
      movementDone = Robotgoto(RobotDiff,1275,1900,180,StopMove);
      if(movementDone) state++;
      break;
    case 2:
      movementDone = Robotgoto(RobotDiff,1275,1600,-90,StopMove);
      if(movementDone) state++;
      break;
    case 3:
    break;
  }
}

void setupSensors() {
  pinMode(XSHUT_1, OUTPUT);
  pinMode(XSHUT_2, OUTPUT);
  digitalWrite(XSHUT_1, HIGH);
  digitalWrite(XSHUT_2, LOW);

  Wire.begin();
  sensor1.setTimeout(500);
  sensor1.init();
  sensor1.setAddress(1);

  digitalWrite(XSHUT_2, HIGH);
  sensor2.setTimeout(500);
  sensor2.init();
  sensor2.setAddress(2);

  sensor1.startContinuous();
  sensor2.startContinuous();
}

void setup() {
  //Serial.begin(230400);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(SW_BAU, INPUT_PULLUP);
  pinMode(TIRETTE, INPUT_PULLUP);

  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, LOW);

  setupSensors();

  driverG.setup_stepper();
  driverD.setup_stepper();

  ThreadSensors.onRun(checkSensors);
  ThreadSensors.setInterval(300);

  ThreadDrive.onRun(taskDrive);
  ThreadDrive.setInterval(1);

  ThreadShowPos.onRun(showPosition);
  ThreadShowPos.setInterval(1);

  controller.add(&ThreadSensors);
  controller.add(&ThreadDrive);
  controller.add(&ThreadShowPos);
}

void loop() {
  controller.run();
}
