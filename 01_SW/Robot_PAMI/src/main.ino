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
#define DELAY_START 0//85000
#define DELAY_END 15000//100000
#define PAMI_3
#define DEBUG

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
const long interval = 500; // intervalle en millisecondes
bool ServoState = false;


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
  StopMove = (dist1 < 150 || dist2 < 150) or LatchBAU or EndMatch;
  digitalWrite(LED_R, (StopMove or LatchBAU) and !EndMatch ); //BAU LED Rouge
  digitalWrite(LED_B, StopMove and !LatchBAU);//Detection obstacle LED Violette
}

void showPosition() {

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
int converalphaTeam(int angle, bool team){
  return (team) ? -angle : angle;
}

void movementPAMI1(){
  bool movementDone = false;
  switch (state) {
    case 0:
      if (sequenceReady){
        RobotDiff.setPosition(2930,1900,-90,CouleurTeam);
        state++;
      }
      break;
    case 1:
      if (sequenceReady){
        movementDone = Robotmoveto(RobotDiff,1275, converalphaTeam(0, CouleurTeam),StopMove);
        if(movementDone) state++;
      }
      break;
    case 2:
        movementDone = Robotmoveto(RobotDiff,0, converalphaTeam(-90, CouleurTeam),StopMove);
        if(movementDone) state++;
          
         
      break;
    case 3:
      movementDone = Robotmoveto(RobotDiff,260, converalphaTeam(0, CouleurTeam),StopMove);
      if(movementDone) state++;
       
      break;
    case 4:
      movementDone = Robotmoveto(RobotDiff,0, converalphaTeam(-90, CouleurTeam),StopMove);
      if(movementDone) state++;
      break;
    case 5:
    break;
  }
  
}

void movementPAMI2(){
  bool movementDone = false;
  switch (state) {
    case 0:
      if (sequenceReady){
        RobotDiff.setPosition(2930,1775,-90,CouleurTeam);
        delay(1000);
        state++;
      }
      break;
    case 1:
      movementDone = Robotmoveto(RobotDiff,300, converalphaTeam(0, CouleurTeam),StopMove);
      if(movementDone) state++;
      break;
    case 2:
       movementDone = Robotmoveto(RobotDiff,0, converalphaTeam(-45, CouleurTeam),StopMove);
      if(movementDone) state++;
      break;
    case 3:
      movementDone = Robotmoveto(RobotDiff,500, converalphaTeam(0, CouleurTeam),StopMove);
      if(movementDone) state++;
      break;
    case 4:
      movementDone = Robotmoveto(RobotDiff,0, converalphaTeam(45, CouleurTeam),StopMove);
      if(movementDone) state++;
      break;
    case 5:
      movementDone = Robotmoveto(RobotDiff,360, converalphaTeam(0, CouleurTeam),StopMove);
      if(movementDone) state++;
       break;
    break;
  }
}

void movementPAMI3(){
  bool movementDone = false;
  switch (state) {
    case 0:
      if (sequenceReady){
        RobotDiff.setPosition(2930,1625,-90,CouleurTeam);
        state++;
      }
      break;
    case 1:
      movementDone = Robotmoveto(RobotDiff,300, converalphaTeam(0, CouleurTeam),StopMove);
      if(movementDone) state++;
      break;
    case 2:
       movementDone = Robotmoveto(RobotDiff,0, converalphaTeam(-45, CouleurTeam),StopMove);
      if(movementDone) state++;
      break;
    case 3:
      movementDone = Robotmoveto(RobotDiff,250, converalphaTeam(0, CouleurTeam),StopMove);
      if(movementDone) state++;
      break;
    case 4:
      movementDone = Robotmoveto(RobotDiff,0, converalphaTeam(45, CouleurTeam),StopMove);
      if(movementDone) state++;
      break;
    case 5:
      movementDone = Robotmoveto(RobotDiff,950, converalphaTeam(0, CouleurTeam),StopMove);
      if(movementDone) state++;
       break;
    break;
  }
}

void PAMIAction(){
  unsigned long currentMillis = millis();
  if (currentMillis - servoStartTime >= interval) {
    servoStartTime = currentMillis;  
    if(ServoState == true){
      ServoState = false;
    }else{
      ServoState = true;
    }
    if(!LatchBAU){
      if(ServoState == true){
        myservo.write(7);
      
      }else{
        myservo.write(60);
      }
    }  
  }
}

void taskDrive() {
  
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
    PAMIAction();
  }

#ifdef PAMI_1
  movementPAMI1();
#endif

#ifdef PAMI_2
  movementPAMI2();
#endif

#ifdef PAMI_3
  movementPAMI3();
#endif

  
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
  #ifdef DEBUG
  Serial.begin(230400);
  #endif
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
  ThreadSensors.setInterval(300);

  ThreadDrive.onRun(taskDrive);
  ThreadDrive.setInterval(1);
  #ifdef DEBUG
  ThreadShowPos.onRun(showPosition);
  ThreadShowPos.setInterval(1);
  #endif

  controller.add(&ThreadSensors);
  controller.add(&ThreadDrive);
  #ifdef DEBUG
  controller.add(&ThreadShowPos);
  #endif

}

void loop() {
  controller.run();
}
