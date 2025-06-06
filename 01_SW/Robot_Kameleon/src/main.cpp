#include "mbed.h"
#include "pinout.hpp"
#include "Stepper.hpp"
#include "UART_TMC.hpp"
#include "lcd.hpp"
#include "main_pck.hpp"
#include "LinearActuator.hpp"
#include <inttypes.h>
#include "Differentiel.hpp"
#include "lidar.hpp"
#include "LidarAnalyzer.hpp"
#include <cmath>

//#define DEBUG
#define LIDAR

#define NORMALSPEED  1.0f
#define SLOWSPEED    0.4f

//***********************************/************************************
//                              UART_TMC                                //
//***********************************/************************************
Uart_TMC TMCSerial(TMC_UART_TX, TMC_UART_RX, SEL_UART_0, SEL_UART_1, SEL_UART_2, R_SENSE);
BufferedSerial pc(USBTX, USBRX, 230400);
//***********************************/************************************
//                                 MOVE                                 //
//***********************************/************************************
bool StopLidar = false;
bool Fin_de_match = false;
bool   Couleur_Team = 0; // false bleu true jaune
Stepper StepperG(STEP_G, DIR_G);
Stepper StepperD(STEP_D, DIR_D);
differentiel RobotDiff(&StepperG, &StepperD, &StopLidar);
DigitalOut En_drive_N(ENABLE_DRIVE_N);
DigitalOut En_step_N(ENABLE_STEP_N);
//***********************************/************************************
//                                 STEPPER                              //
//***********************************/************************************
LinearActuator *StepperRG = new LinearActuator(STEP_RG, DIR_RG, RG_SW_UP, RG_SW_DOWN, false);
LinearActuator *StepperRD = new LinearActuator(STEP_RD, DIR_RD, RD_SW_UP, RD_SW_DOWN, true);
LinearActuator *StepperRM = new LinearActuator(STEP_RM, DIR_RM, RM_SW_UP, RM_SW_DOWN, true);

//***********************************/************************************
//                                 SWITCH                               //
//***********************************/************************************
DigitalIn SW_init(SW_INIT);
DigitalIn SW_team(SW_TEAM);
DigitalIn SW_bau(SW_BAU);
DigitalIn SW_Tirette(TIRETTE);
DigitalIn SW_Drive(SW_SPARE_1);
DigitalIn SW_Stepper(SW_SPARE_2);
//***********************************/************************************
//                                 SERVO                                //
//***********************************/************************************
PwmOut Pince_r1(PINCE_R1);
PwmOut Pince_r2(PINCE_R2);
PwmOut Pince_r3(PINCE_R3);
PwmOut Pince_r4(PINCE_R4);
PwmOut Mover_rg(PINCE_MOVE_R1);
PwmOut Mover_rd(PINCE_MOVE_R4);
PwmOut Hook_G(SERVO_HOOKG);
PwmOut Hook_D(SERVO_HOOKD);
PwmOut Suck_Pump(SUCKER_PUMP);
PwmOut Suck_Valve(SUCKER_VALVE);

//***********************************/************************************
//                                 LIDAR                                //
//***********************************/************************************
DigitalOut led_lidar(LIDAR_LED);
//***********************************/************************************
//                                 LCD                                  //
//***********************************/************************************
LCD lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7, LCD16x2); // rs, e, d4-d7

int state = 0;

volatile bool end_match = false;
Timeout endMatch;
Thread lcd_thread;
Thread game_thread;

int score = 0;
int offset_posX = 0;
int offset_Alpha = 1;


Lidar*    LidarLD19 = new Lidar(LIDAR_TX, LIDAR_RX,230400);
LidarAnalyzer LidaRayzer(LidarLD19, &RobotDiff, &led_lidar);


void endMatchProcess()
{
  end_match = true;
}

float theta2pluse(int theta)
{
  return 500.0 + (100.0 / 9.0) * float(theta);
}
void printPosition()
{
 
  //printf pour processing
  printf("%f;%f;%f\r\n",
        RobotDiff.getPositionX(),
        RobotDiff.getPositionY(),
        RobotDiff.getAlpha());
  
  // printf("%d;%d;%d;%f;%f;%f;%f;%f;%f\r\n",
  //       LidaRayzer.isObstacleDetected(),
  //       int(RobotDiff.getPositionX()),
  //       int(RobotDiff.getPositionY()),
  //       RobotDiff.getAlpha(),
  //       LidaRayzer.getObstacleX(),
  //       LidaRayzer.getObstacleY(),
  //       RobotDiff.getPosCibleX(),
  //       RobotDiff.getPosCibleY(),
  //       LidaRayzer.getObstacleAngleCible());

  // printf("[Position] X = %.2f mm | Y = %.2f mm | Angle = %.2f°\n",
  //        RobotDiff.getPositionX(),
  //        RobotDiff.getPositionY(),
  //        RobotDiff.getAlpha());

  // printf("[Position] X = %.2f mm | Y = %.2f mm | Angle = %.2f° | posG = %d stp | posD = %d stp | StepG = %d stp | StepD = %d stp | SpeedG = %f stp/sec² | SpeedD = %f stp/sec² | DeltaG = %d | DeltaD = %d\n",
  //        RobotDiff.getPositionX(),
  //        RobotDiff.getPositionY(),
  //        RobotDiff.getAlpha(),
  //        RobotDiff.getPosG(),
  //        RobotDiff.getPosD(),
  //        RobotDiff.getStepG(),
  //        RobotDiff.getStepD(),
  //        RobotDiff.getSpeedG(),
  //        RobotDiff.getSpeedD(),
  //        RobotDiff.getDeltaG(),
  //        RobotDiff.getDeltaD()
  //       );
}

// --- Thread d'affichage de position ---
void routineAffichage()
{
  while (true)
  {
    printPosition();
    ThisThread::sleep_for(100ms);
  }
}

void construction_gradin_niveau_2()
{
  Suck_Valve.pulsewidth_us(theta2pluse(0));
  Hook_G.pulsewidth_us(theta2pluse(Hook[0].hook_up));
  Hook_D.pulsewidth_us(theta2pluse(Hook[1].hook_up));
  ThisThread::sleep_for(200ms);
  StepperRG->move(300);
  StepperRD->move(300);
  while (!(StepperRG->StepperAct->stopped() and StepperRD->StepperAct->stopped()));
  ThisThread::sleep_for(500ms);
  Mover_rg.pulsewidth_us(theta2pluse(Bras[0].bras_side));
  Mover_rd.pulsewidth_us(theta2pluse(Bras[1].bras_side));
  StepperRM->move(-11500);
  while (!StepperRM->StepperAct->stopped());
  ThisThread::sleep_for(200ms);
  StepperRM->move(-200);
  while (!StepperRM->StepperAct->stopped());
  Suck_Pump.pulsewidth_us(theta2pluse(180));
  StepperRM->move(-200);
  while (!StepperRM->StepperAct->stopped());
  ThisThread::sleep_for(1000ms);
  StepperRM->goUp();
  StepperRG->goUp();
  StepperRD->goUp();
  while (!(StepperRG->goUp() and StepperRD->goUp() and StepperRM->goUp()));
  ThisThread::sleep_for(200ms);
  StepperRG->move(-2000);
  StepperRD->move(-2000);
  while (!(StepperRG->StepperAct->stopped() and StepperRD->StepperAct->stopped()));
  ThisThread::sleep_for(200ms);
  Mover_rg.pulsewidth_us(theta2pluse(Bras[0].bras_take));
  Mover_rd.pulsewidth_us(theta2pluse(Bras[1].bras_take));
  ThisThread::sleep_for(200ms);
  StepperRM->move(-2500);
  while (!StepperRM->StepperAct->stopped());
  ThisThread::sleep_for(200ms);
  Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_open));
  Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_open));
  Pince_r2.pulsewidth_us(theta2pluse(Pince[1].pince_open));
  Pince_r3.pulsewidth_us(theta2pluse(Pince[2].pince_open));
  Suck_Pump.pulsewidth_us(theta2pluse(0));
  Suck_Valve.pulsewidth_us(theta2pluse(180));
  ThisThread::sleep_for(1000ms);
  StepperRM->goUp();
  while (!(StepperRM->goUp()));
  StepperRG->move(-500);
  StepperRD->move(-500);
  while (!(StepperRG->StepperAct->stopped() and StepperRD->StepperAct->stopped()));
}

void separation_gradin_niveau_2()
{
  Suck_Valve.pulsewidth_us(theta2pluse(0));
  Hook_G.pulsewidth_us(theta2pluse(Hook[0].hook_up));
  Hook_D.pulsewidth_us(theta2pluse(Hook[1].hook_up));
  ThisThread::sleep_for(200ms);
  StepperRG->move(300);
  StepperRD->move(300);
  while (!(StepperRG->StepperAct->stopped() and StepperRD->StepperAct->stopped()));
  ThisThread::sleep_for(500ms);
  Mover_rg.pulsewidth_us(theta2pluse(Bras[0].bras_side));
  Mover_rd.pulsewidth_us(theta2pluse(Bras[1].bras_side));
  StepperRM->move(-11500);
  while (!StepperRM->StepperAct->stopped());
  ThisThread::sleep_for(200ms);
  StepperRM->move(-200);
  while (!StepperRM->StepperAct->stopped());
  Suck_Pump.pulsewidth_us(theta2pluse(180));
  StepperRM->move(-200);
  while (!StepperRM->StepperAct->stopped());
  ThisThread::sleep_for(1000ms);
  StepperRM->goUp();
  StepperRG->goUp();
  StepperRD->goUp();
  while (!(StepperRG->goUp() and StepperRD->goUp() and StepperRM->goUp()));
  ThisThread::sleep_for(200ms);
  StepperRG->move(-2000);
  StepperRD->move(-2000);
  while (!(StepperRG->StepperAct->stopped() and StepperRD->StepperAct->stopped()));
  ThisThread::sleep_for(200ms);
  Mover_rg.pulsewidth_us(theta2pluse(Bras[0].bras_take));
  Mover_rd.pulsewidth_us(theta2pluse(Bras[1].bras_take));
  ThisThread::sleep_for(200ms);
  StepperRM->move(-1500);
  while (!StepperRM->StepperAct->stopped());
  StepperRG->move(-500);
  StepperRD->move(-500);
  StepperRM->move(-1800);
  while (!(StepperRG->StepperAct->stopped() and StepperRD->StepperAct->stopped() and StepperRM->StepperAct->stopped()));
  ThisThread::sleep_for(200ms);
  Pince_r3.pulsewidth_us(theta2pluse(Pince[2].pince_open));
  Pince_r2.pulsewidth_us(theta2pluse(Pince[1].pince_open));
  Suck_Pump.pulsewidth_us(theta2pluse(0));
  Suck_Valve.pulsewidth_us(theta2pluse(180));
  ThisThread::sleep_for(1000ms);
  StepperRM->goUp();
  while (!(StepperRM->goUp()));
  Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_open));
  Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_open));
  ThisThread::sleep_for(500ms);
  StepperRG->move(-800);
  StepperRD->move(-800);
  while (!(StepperRG->StepperAct->stopped() and StepperRD->StepperAct->stopped()));
  ThisThread::sleep_for(500ms);
  Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_close));
  Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_close));
  ThisThread::sleep_for(500ms);
  StepperRG->move(1600);
  StepperRD->move(1600);
  while (!(StepperRG->StepperAct->stopped() and StepperRD->StepperAct->stopped()));
  ThisThread::sleep_for(500ms);
}

void down_pince_take()
{
  Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_open));
  Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_open));
  Mover_rg.pulsewidth_us(theta2pluse(Bras[0].bras_take));
  Mover_rd.pulsewidth_us(theta2pluse(Bras[1].bras_take));
  StepperRG->goDown();
  StepperRD->goDown();
  while (!(StepperRG->goDown() and StepperRD->goDown()));
  ThisThread::sleep_for(200ms);
}

void print_lcd(void)
{

  while (1)
  {
    if (!end_match)
    {
      lcd.locate(0, 1);
      lcd.printf("Time :%d\n", int(endMatch.remaining_time().count()) / 1000000);
      lcd.locate(6, 0);
      lcd.printf("Score :%d\n", score);
    }
  }
}

void thread_lidar() {
    while (true) {
        LidaRayzer.update();
        #ifdef LIDAR
         StopLidar = LidaRayzer.isObstacleDetected();
        #endif
        ThisThread::sleep_for(1ms);
    }
}
void main_thread(void) 
{
  FsmState = IDLE;
  lcd.printf("Wait Calibration\n");
  lcd.locate(0, 1);
  Couleur_Team = !SW_team;
  if (Couleur_Team == false)
  {
    lcd.printf("Team Blue\n");
    offset_posX = 0;
    offset_Alpha = 1;
  }
  else
  {
    lcd.printf("Team Yellow\n");
    offset_posX = 3000;
    offset_Alpha = -1;
  }

  while (1)
  {

    switch (FsmState)
    {
    case IDLE:
      if (SW_init != 1)
      { // Attente boutton d'init
        lcd.cls();
        lcd.printf("Start Up !\n");
        FsmState = START_UP; // Allumage actionneurs
      }
      break;

    case START_UP:
      //down_pince_take();
      // StepperRM->move(-10500);
      // while (!(StepperRM->StepperAct->stopped()));
      Mover_rg.pulsewidth_us(theta2pluse(Bras[0].bras_drop_banner));
      Mover_rd.pulsewidth_us(theta2pluse(Bras[1].bras_drop_banner));
      Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_open));
      Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_open));
      ThisThread::sleep_for(2500ms);
      Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_banner));
      Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_banner));
      ThisThread::sleep_for(1000ms);
      StepperRG->goUp();
      StepperRD->goUp();
      while (!(StepperRG->goUp() and StepperRD->goUp()));
      Mover_rg.pulsewidth_us(theta2pluse(Bras[0].bras_banner));
      Mover_rd.pulsewidth_us(theta2pluse(Bras[1].bras_banner));
      ThisThread::sleep_for(500ms);
      lcd.cls();
      lcd.printf("Calibration !\n");
      FsmState = CAL;
      break;

    case CAL:
      RobotDiff.setPosition(1775, 95, 180, Couleur_Team);
      FsmState = WAIT_MATCH;
      lcd.cls();
      lcd.printf("Wait Match !\n");
      break;

    case WAIT_MATCH:
      if (SW_Tirette == 1 and FsmState != END)
      {
        endMatch.attach(endMatchProcess, 100s);
        lcd.cls();
        lcd.printf("GAME!\n");
        lcd_thread.start(print_lcd);
        FsmState = GAME; // Lancement du match !
      }
      break;

    case GAME:

      //DROP BANNER
      RobotDiff.Robotmoveto(100, 0, false, NORMALSPEED);
      RobotDiff.setPosition(1775, 95, 180, Couleur_Team);
      RobotDiff.Robotgoto(1775, 105, 180, Couleur_Team, NORMALSPEED);
      Mover_rg.pulsewidth_us(theta2pluse(Bras[0].bras_drop_banner));
      Mover_rd.pulsewidth_us(theta2pluse(Bras[1].bras_drop_banner));
      ThisThread::sleep_for(500ms);
      StepperRG->goDown();
      StepperRD->goDown();
      while (!(StepperRG->goDown() and StepperRD->goDown()));
      ThisThread::sleep_for(500ms);
      Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_open));
      Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_open));
      score+=20;
      //Prise l'élement de jeu central
      RobotDiff.Robotgoto(1775, 450, 180, Couleur_Team, NORMALSPEED);
      RobotDiff.Robotgoto(1775, 600, 180, Couleur_Team, NORMALSPEED);
      RobotDiff.Robotgoto(1900, 600, 0, Couleur_Team, NORMALSPEED);
      down_pince_take();
      //construction gradin N°1
      //RobotDiff.Robotgoto(1900, 750, 0, Couleur_Team, NORMALSPEED);
      RobotDiff.Robotgoto(1900, 925, 0, Couleur_Team, SLOWSPEED);
      ThisThread::sleep_for(500ms);
      Hook_G.pulsewidth_us(theta2pluse(Hook[0].hook_down));
      Hook_D.pulsewidth_us(theta2pluse(Hook[1].hook_down));
      ThisThread::sleep_for(500ms);
      Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_close));
      Pince_r2.pulsewidth_us(theta2pluse(Pince[1].pince_close));
      Pince_r3.pulsewidth_us(theta2pluse(Pince[2].pince_close));
      Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_close));
      Hook_G.pulsewidth_us(theta2pluse(Hook[0].hook_take));
      Hook_D.pulsewidth_us(theta2pluse(Hook[1].hook_take));

      RobotDiff.Robotgoto(1900, 750, 0, Couleur_Team, NORMALSPEED);
      RobotDiff.Robotgoto(1725, 600, 180, Couleur_Team, NORMALSPEED);
      RobotDiff.Robotgoto(1725, 250, 180, Couleur_Team, NORMALSPEED);

      separation_gradin_niveau_2();
      RobotDiff.Robotgoto(1725, 400, 180, Couleur_Team, SLOWSPEED);
      StepperRG->goDown();
      StepperRD->goDown();
      while (!(StepperRG->goDown() and StepperRD->goDown()));
      Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_open));
      Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_open));
      StepperRG->move(-500);
      StepperRD->move(-500);
      while (!(StepperRG->StepperAct->stopped() and StepperRD->StepperAct->stopped()));
      score+=4;

      RobotDiff.Robotgoto(1725, 550, 180, Couleur_Team, SLOWSPEED);

      //construction gradin N°2

      RobotDiff.Robotgoto(1725, 700, 90, Couleur_Team, NORMALSPEED);
      RobotDiff.Robotgoto(2225, 700, 180, Couleur_Team, NORMALSPEED);
      down_pince_take();
      RobotDiff.Robotgoto(2225, 325, 180, Couleur_Team, SLOWSPEED);
      RobotDiff.Robotgoto(2225, 225, 180, Couleur_Team, SLOWSPEED);
      Hook_G.pulsewidth_us(theta2pluse(Hook[0].hook_down));
      Hook_D.pulsewidth_us(theta2pluse(Hook[1].hook_down));
      ThisThread::sleep_for(500ms);
      Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_close));
      Pince_r2.pulsewidth_us(theta2pluse(Pince[1].pince_close));
      Pince_r3.pulsewidth_us(theta2pluse(Pince[2].pince_close));
      Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_close));
      Hook_G.pulsewidth_us(theta2pluse(Hook[0].hook_take));
      Hook_D.pulsewidth_us(theta2pluse(Hook[1].hook_take));
      RobotDiff.Robotgoto(2225, 120, 180, Couleur_Team, NORMALSPEED);
      RobotDiff.setPosition(2225, 170, 180, Couleur_Team);
      RobotDiff.Robotgoto(2225, 225, 180, Couleur_Team, NORMALSPEED);
      separation_gradin_niveau_2();
      StepperRM->move(-1000);
      while (!(StepperRM->StepperAct->stopped()));
      ThisThread::sleep_for(200ms);
      
      RobotDiff.Robotgoto(2225, 700, -90, Couleur_Team, NORMALSPEED);
      RobotDiff.Robotgoto(1725, 700, 180, Couleur_Team, NORMALSPEED);
      RobotDiff.Robotgoto(1725, 400, 180, Couleur_Team, SLOWSPEED);
      StepperRG->move(-500);
      StepperRD->move(-500);
      while (!(StepperRG->StepperAct->stopped() and StepperRD->StepperAct->stopped()));
      ThisThread::sleep_for(500ms);
      Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_open));
      Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_open));
      ThisThread::sleep_for(200ms);
      StepperRG->move(-500);
      StepperRD->move(-500);
      while (!(StepperRG->StepperAct->stopped() and StepperRD->StepperAct->stopped()));
      ThisThread::sleep_for(500ms);
      RobotDiff.Robotgoto(1725, 550, 180, Couleur_Team, SLOWSPEED);
      StepperRG->goDown();
      StepperRD->goDown();
      while (!(StepperRG->goDown() and StepperRD->goDown()));
      StepperRM->move(-10500);
      while (!(StepperRM->StepperAct->stopped()));
      
      RobotDiff.Robotgoto(1725, 325, 180, Couleur_Team, SLOWSPEED);
      Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_close));
      Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_close));
      ThisThread::sleep_for(200ms);
      StepperRM->move(-2700);
      while (!(StepperRM->StepperAct->stopped()));
      Suck_Valve.pulsewidth_us(theta2pluse(0));
      Suck_Pump.pulsewidth_us(theta2pluse(180));
      ThisThread::sleep_for(1000ms);
      StepperRM->move(-50);
      while (!(StepperRM->StepperAct->stopped()));
      StepperRG->move(11000);
      StepperRD->move(11000);
      StepperRM->move(11000);
      while (!(StepperRG->StepperAct->stopped() and StepperRD->StepperAct->stopped() and StepperRM->StepperAct->stopped()));
      ThisThread::sleep_for(1000ms);
      RobotDiff.Robotgoto(1725, 225, 180, Couleur_Team, SLOWSPEED);
      Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_open));
      Pince_r2.pulsewidth_us(theta2pluse(Pince[1].pince_open));
      Pince_r3.pulsewidth_us(theta2pluse(Pince[2].pince_open));
      Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_open));
      ThisThread::sleep_for(200ms);
      Suck_Valve.pulsewidth_us(theta2pluse(180));
      Suck_Pump.pulsewidth_us(theta2pluse(0));
      StepperRM->move(500);
      StepperRG->move(-800);
      StepperRD->move(-800);
      while (!(StepperRG->StepperAct->stopped() and StepperRD->StepperAct->stopped() and StepperRM->StepperAct->stopped()));
      ThisThread::sleep_for(500ms);
      RobotDiff.Robotgoto(1725, 400, 180, Couleur_Team, SLOWSPEED);
      score+=28;
      RobotDiff.Robotgoto(1725, 800, 45, Couleur_Team, NORMALSPEED);
      while(int(endMatch.remaining_time().count()) / 1000000 > 10);
      RobotDiff.Robotgoto(2500, 1500, 0, Couleur_Team,NORMALSPEED);
      score+=10;
    
      lcd_thread.terminate();
      lcd.cls();
      lcd.printf("END !\n");
      lcd.locate(0, 1);
      lcd.printf("Time :%d\n", int(endMatch.remaining_time().count()) / 1000000);
      lcd.locate(6, 0);
      lcd.printf("Score :%d\n", score);
      FsmState = END;
      break;

    case END:

      break;
    }
  }
}

int main()
{
  
  Thread lidarAnalyzer_thread;
  #ifdef DEBUG
    Thread threadAffichage;
    threadAffichage.start(routineAffichage);
  #endif;
  
  
 
  En_drive_N = SW_Drive;
  En_step_N = SW_Stepper;

  SW_init.mode(PullUp);
  SW_team.mode(PullUp);
  SW_bau.mode(PullUp);
  SW_Tirette.mode(PullUp);
  SW_Drive.mode(PullUp);
  SW_Stepper.mode(PullUp);
  if (SW_bau ==0 ) {
    Pince_r1.period_ms(20);
    Pince_r2.period_ms(20);
    Pince_r3.period_ms(20);
    Pince_r4.period_ms(20);
    Mover_rg.period_ms(20);
    Mover_rd.period_ms(20);
    Hook_G.period_ms(20);
    Hook_D.period_ms(20);
    ThisThread::sleep_for(500ms);
    TMCSerial.setup_all_stepper();
    StepperRG->InitLinearActuator();
    StepperRD->InitLinearActuator();
    StepperRM->InitLinearActuator();
    ThisThread::sleep_for(500ms);
    Hook_G.pulsewidth_us(theta2pluse(Hook[0].hook_up));
    Hook_D.pulsewidth_us(theta2pluse(Hook[1].hook_up));
    Mover_rg.pulsewidth_us(theta2pluse(Bras[0].bras_side));
    Mover_rd.pulsewidth_us(theta2pluse(Bras[1].bras_side));
    ThisThread::sleep_for(500ms);
    Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_close));
    Pince_r2.pulsewidth_us(theta2pluse(Pince[1].pince_open));
    Pince_r3.pulsewidth_us(theta2pluse(Pince[2].pince_open));
    Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_close));
    StepperRG->goUp();
    StepperRD->goUp();
    StepperRM->goUp();
    while (!(StepperRG->goUp() and StepperRD->goUp() and StepperRM->goUp()));
    Mover_rg.pulsewidth_us(theta2pluse(Bras[0].bras_home));
    Mover_rd.pulsewidth_us(theta2pluse(Bras[1].bras_home));
  }

  game_thread.start(main_thread);
  lidarAnalyzer_thread.start(thread_lidar);

  while (1)
  {

    if (Fin_de_match == false) {
      En_drive_N = SW_Drive;
      En_step_N = SW_Stepper;
    }
    
    if ((end_match or SW_bau == 1) and Fin_de_match == false)
    {
      Fin_de_match = true;
      if (SW_bau == 1) {
        En_drive_N = 1;
        En_step_N = 1;
        Pince_r1 = 0.0;
        Pince_r2 = 0.0;
        Pince_r3 = 0.0;
        Pince_r4 = 0.0;
        Mover_rg = 0.0;
        Mover_rd = 0.0;
        Hook_G = 0.0;
        Hook_D = 0.0;
        Suck_Pump = 0.0;
        Suck_Valve = 0.0;
       
      }
      lcd_thread.terminate();
      game_thread.terminate();
      lcd.cls();
      if(SW_bau)  lcd.printf("Arret D'urgence !\n");
      if(end_match)  lcd.printf("END TIMOUT !\n");
      lcd.locate(0, 1);
      lcd.printf("Score :%d\n", score);
    }
  }
}
