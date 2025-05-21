// === Includes ===
#include "TMCStepper.h"
#include <AccelStepper.h>
#include "pinout.h"
#include "UART_TMC.h"
#include <Wire.h>
#include <VL53L0X.h>
#include <Thread.h>
#include <ThreadController.h>
#include "Differentiel.h"
#include <math.h>

// === Threads ===
ThreadController controller = ThreadController();
//Thread ThreadSensors = Thread();
Thread ThreadDrive = Thread();
//Thread ThreadShowPos = Thread();

// === TMC Drivers ===
#define R_SENSE 3.70f
Uart_TMC driverG(TMC_UART_RX, TMC_UART_TX, R_SENSE, 0b00);
Uart_TMC driverD(TMC_UART_RX, TMC_UART_TX, R_SENSE, 0b01);

// === Motors and Robot ===
AccelStepper stepperG(AccelStepper::DRIVER, STEP_G, DIR_G);
AccelStepper stepperD(AccelStepper::DRIVER, STEP_D, DIR_D);
differentiel RobotDiff(&stepperG, &stepperD);

// === Sensors ===
VL53L0X sensor1;
VL53L0X sensor2;
bool StopMove = false;

// === Robot state ===
int state = 0;
bool StartMove = false;

// === Functions ===
void taskSensors() {
  StopMove = (sensor1.readRangeContinuousMillimeters() < 200 || sensor2.readRangeContinuousMillimeters() < 200);
  digitalWrite(LED_R, StopMove);
}


void TaskDrive() {
  static bool movementDone = false;
  switch (state) {
    case 0:
      if (digitalRead(SW_BAU) == LOW){
        state++;
      }
      break;
    case 1:
      movementDone = Robotgoto(RobotDiff,300,300, 90,false);
      if(movementDone) state++;
      break;
    case 2:
      movementDone = Robotgoto(RobotDiff,0,0, 0,false);
      if(movementDone) state++;
      break;
    case 3:
    break;
  }
}

void TaskShowPos() {
  //RobotDiff.updatePosition();
  // Serial.print("[Position] X = ");
  // Serial.print(RobotDiff.getPositionX(), 2);
  // Serial.print(" mm | Y = ");
  // Serial.print(RobotDiff.getPositionY(), 2);
  // Serial.print(" mm | Angle = ");
  // Serial.print(RobotDiff.getAlpha(), 2);
  // Serial.println("°");

  Serial.print(RobotDiff.getPositionX(), 2);
  Serial.print(";");
  Serial.print(RobotDiff.getPositionY(), 2);
  Serial.print(";");
  Serial.print(RobotDiff.getAlpha(), 2);
  Serial.print("\r\n");
}

// === Setup ===
void setup() {
  Serial.begin(230400);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(XSHUT_1, OUTPUT);
  pinMode(XSHUT_2, OUTPUT);
  pinMode(TIRETTE, INPUT_PULLUP);
  pinMode(SW_TEAM, INPUT_PULLUP);
  pinMode(SW_BAU, INPUT_PULLUP);

  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);

  digitalWrite(XSHUT_1, HIGH);
  digitalWrite(XSHUT_2, LOW);

  Wire.begin();
  sensor1.setTimeout(500);
  if (!sensor1.init()) {
    Serial.println("Failed to init sensor1");
    digitalWrite(LED_R, HIGH);
    while (1) {}
  }
  sensor1.setAddress(1);

  digitalWrite(XSHUT_2, HIGH);
  sensor2.setTimeout(500);
  if (!sensor2.init()) {
    Serial.println("Failed to init sensor2");
    digitalWrite(LED_R, HIGH);
    while (1) {}
  }
  sensor2.setAddress(2);

  sensor1.startContinuous();
  sensor2.startContinuous();

  if (!driverG.setup_stepper()) Serial.println("TMC error StepG");
  if (!driverD.setup_stepper()) Serial.println("TMC error StepD");

  //ThreadSensors.onRun(taskSensors);
  ThreadDrive.onRun(TaskDrive);
  //ThreadShowPos.onRun(TaskShowPos);

  //controller.add(&ThreadSensors);
  controller.add(&ThreadDrive);
  //controller.add(&ThreadShowPos);

  //ThreadSensors.setInterval(500);
  //ThreadShowPos.setInterval(1);
  ThreadDrive.setInterval(1);

  RobotDiff.setPosition(0, 0, 0);

 
}

// === Loop ===
void loop() {
  controller.run();
 
}
