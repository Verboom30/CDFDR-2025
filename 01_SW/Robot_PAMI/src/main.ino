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
#include <Servo.h>

Servo myservo;  // create servo object to control a servo

#define R_SENSE 3.7f
#define DELAY_START 500
#define DELAY_END 100000

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
bool LatchBAU = false;
bool EndMatch = false;
bool CouleurTeam = false; // False team Bleu, True team Jaune
bool StartSequence = false;
bool tiretteRemoved = false;
bool sequenceReady = false;
unsigned long tiretteRemovedTime = 0;
unsigned long servoStartTime = 0;


bool nonBlockingDelay(unsigned long duration, bool reset = false) {
  static unsigned long startTime = 0;
  static bool waiting = false;

  if (reset) {
    startTime = millis();
    waiting = true;
    return false;
  }

  if (waiting && (millis() - startTime >= duration)) {
    waiting = false;
    return true; // Delay terminé
  }

  return false; // Delay encore en cours
}

void checkSensors() {
  int dist1 = sensor1.readRangeContinuousMillimeters();
  int dist2 = sensor2.readRangeContinuousMillimeters();
  StopMove = (dist1 < 200 || dist2 < 200) or LatchBAU or EndMatch;
  digitalWrite(LED_R, (StopMove or LatchBAU) and !EndMatch ); //BAU LED Rouge
  digitalWrite(LED_B, StopMove and !LatchBAU);//Detection obstacle LED Violette
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

  Serial.print(RobotDiff.getPositionX(), 2);
  Serial.print(";");
  Serial.print(RobotDiff.getPositionY(), 2);
  Serial.print(";");
  Serial.print(RobotDiff.getAlpha(), 2);
  Serial.print("\r\n");
}



void taskDrive() {
  static bool movementDone = false;
  if(digitalRead(SW_BAU) == LOW and tiretteRemoved) LatchBAU = true;
 
  if (!tiretteRemoved) {
    if (digitalRead(SW_TEAM) == HIGH) {
        CouleurTeam = true;
        digitalWrite(LED_R, HIGH);
        digitalWrite(LED_G, HIGH);
        digitalWrite(LED_B, LOW);
    }else{
        CouleurTeam = false;
        digitalWrite(LED_R, LOW);
        digitalWrite(LED_G, LOW);
        digitalWrite(LED_B, HIGH);
    }
    myservo.write(60);
  }

  if (!tiretteRemoved && digitalRead(TIRETTE) == HIGH) {
    tiretteRemoved = true;
    tiretteRemovedTime = millis();
  }
  if (tiretteRemoved && !sequenceReady && (millis() - tiretteRemovedTime >= DELAY_START)) {
    sequenceReady = true;
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_B, LOW);
  }

  if (tiretteRemoved && sequenceReady && (millis() - tiretteRemovedTime >= DELAY_END)) {
    EndMatch = true;
  }
  switch (state) {
    case 0:
      if (sequenceReady){
        RobotDiff.setPosition(2930,1900,-90,CouleurTeam);
        state++;
      }
      break;
    case 1:
      movementDone = Robotgoto(RobotDiff,1725,1900,-180,StopMove, CouleurTeam);
      if(movementDone) state++;
      break;
    case 2:
        myservo.write(0);
        delay(500);
        myservo.write(60); 
        delay(500);
      // movementDone = Robotgoto(RobotDiff,1725,1600,90,StopMove, CouleurTeam);
      // if(movementDone) state++;
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
  pinMode(SW_TEAM, INPUT_PULLUP);

  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, LOW);

  setupSensors();

  driverG.setup_stepper();
  driverD.setup_stepper();
  delay(500);
  myservo.attach(SERVO_1);  // attaches the servo on pin 9 to the servo object

  ThreadSensors.onRun(checkSensors);
  ThreadSensors.setInterval(500);

  ThreadDrive.onRun(taskDrive);
  ThreadDrive.setInterval(1);

  //ThreadShowPos.onRun(showPosition);
  //ThreadShowPos.setInterval(1);

  controller.add(&ThreadSensors);
  controller.add(&ThreadDrive);
  //controller.add(&ThreadShowPos);
}

void loop() {
  controller.run();
}
