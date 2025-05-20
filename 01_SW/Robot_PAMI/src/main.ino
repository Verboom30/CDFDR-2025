#include "TMCStepper.h"
#include <AccelStepper.h>
#include "pinout.h"
#include "UART_TMC.h"
#include <Wire.h>
#include <VL53L0X.h>
#include <Thread.h>
#include <ThreadController.h>
#include "Differentiel.h"

ThreadController controller = ThreadController();
Thread ThreadSensors = Thread();
Thread ThreadDrive = Thread();


#define R_SENSE 3.70f // Match to your driver
                      // SilentStepStick series use 0.11
                      // UltiMachine Einsy and Archim2 boards use 0.2
                      // Panucatt BSD2660 uses 0.1
                      // Watterott TMC5160 uses 0.075

Uart_TMC driverG(TMC_UART_RX, TMC_UART_TX, R_SENSE, 0b00);
Uart_TMC driverD(TMC_UART_RX, TMC_UART_TX, R_SENSE, 0b01);


// 2-wire basic config, microstepping is hardwired on the driver
//BasicStepperDriver stepper(MOTOR_STEPS, DIR_G, STEP_G, DIR_D, STEP_D);
AccelStepper stepperG(AccelStepper::DRIVER, STEP_G, DIR_G);
AccelStepper stepperD(AccelStepper::DRIVER, STEP_D, DIR_D);
differentiel RobotDiff(&stepperG,&stepperD);
VL53L0X sensor1;
VL53L0X sensor2;
int state =0;
bool StartMove = false;
bool StopMove = false;


void taskSensors(){
  // Serial.print("sensor1: " );
  // Serial.print(sensor1.readRangeContinuousMillimeters());
  // if (sensor1.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
  // Serial.print("  ");
  // Serial.print("sensor2: " );
  // Serial.print(sensor2.readRangeContinuousMillimeters());
  // if (sensor1.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
  // Serial.println();
  StopMove = (sensor1.readRangeContinuousMillimeters() < 200 or sensor2.readRangeContinuousMillimeters() < 200);
  

}
// bool MovetoPoint(int stepG, int StepD, bool stop){
//   if (SetupSpeed == false) {
//     stepperG.move(stepG);stepperD.move(StepD);
//     SetupSpeed = true;
//   }
//   if (!stop) stepperG.run(), stepperD.run(); 
//   if (!(stepperG.isRunning()) and !(stepperD.isRunning())) {
//     SetupSpeed = false; 
//     return true;
//   } 
//   return false;
// }



bool Robotmoveto(differentiel& robot, int distance, int alpha, bool stop) {
  if(!StartMove){
    StartMove = true;
    robot.move(distance, alpha);
    robot.handleRoutineGauche();         // gestion moteur gauche
    robot.handleRoutineDroite();         // gestion moteur droite
  }
  if(!stop)robot.run();                         // déclenche moteurs
  if(!robot.Running()){
    StartMove = false;
    return true;
  } 
  return false;
}
void Robotgoto(differentiel& robot, int positionX, int positionY, int alpha, bool stop ) {
  float dx = positionX - robot.getPositionX();
  float dy = positionY - robot.getPositionY();

  if (dx < 0.1f && dx > -0.1f) dx = 0.0f;
  if (dy < 0.1f && dy > -0.1f) dy = 0.0f;

  int move = (int)sqrt(dx * dx + dy * dy);

  float targetAlpha = (180.0f / M_PI) * atan2(dx, dy);
  if (targetAlpha < 0.01f && targetAlpha > -0.01f) targetAlpha = 0.0f;

  float moveAlpha = targetAlpha - robot.getAlpha();
  float finalAlpha = alpha - targetAlpha;

  // Normalisation des angles
  if (moveAlpha > 180) moveAlpha -= 360;
  if (moveAlpha < -180) moveAlpha += 360;

  if (finalAlpha > 180) finalAlpha -= 360;
  if (finalAlpha < -180) finalAlpha += 360;

  // Inversion du mouvement si plus efficace
  if (abs(moveAlpha) > 90) {
      moveAlpha -= 180;
      finalAlpha -= 180;
      move = -move;

      if (moveAlpha > 180) moveAlpha -= 360;
      if (moveAlpha < -180) moveAlpha += 360;

      if (finalAlpha > 180) finalAlpha -= 360;
      if (finalAlpha < -180) finalAlpha += 360;
  }

  // Étapes de mouvement
  Robotmoveto(robot, 0, moveAlpha,stop);
  Robotmoveto(robot, move, 0,stop);
  Robotmoveto(robot, 0, finalAlpha,stop);
}
void TaskDrive(){

  switch (state)
  {
  case 0 :
   //while(!MovetoPoint(int(RADIUS*(PI/180.0)*95.0/KSTP),int(RADIUS*(PI/180.0)*95.0/KSTP),false));
   if(digitalRead(SW_BAU) == LOW){
    while(!Robotmoveto(RobotDiff,0,360,false));
   }
   
   //StartMove++;
  break;
  case 1 :
   
  break;
  
  default:
    break;
  }
    
}

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

  if(!(driverG.setup_stepper())) { Serial.println("TMC communication error StepG !"); }
  if(!(driverD.setup_stepper())) { Serial.println("TMC communication error StepD !\n"); }

  ThreadSensors.onRun(taskSensors);
  ThreadDrive.onRun(TaskDrive);
  controller.add(&ThreadSensors);
  controller.add(&ThreadDrive);
  ThreadSensors.setInterval(300);
  ThreadDrive.setInterval(1);
}

void loop() {
  controller.run();
  digitalWrite(LED_R,StopMove);
  
  
  
 
  
}
