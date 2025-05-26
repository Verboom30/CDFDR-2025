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

//***********************************/************************************
//                              UART_TMC                                //
//***********************************/************************************
Uart_TMC TMCSerial(TMC_UART_TX, TMC_UART_RX, SEL_UART_0, SEL_UART_1, SEL_UART_2, R_SENSE);
BufferedSerial pc(USBTX, USBRX, 230400);
//***********************************/************************************
//                                 MOVE                                 //
//***********************************/************************************
bool StopLidar = false;
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
int Couleur_Team = 0; // 0 bleu 1 jaune
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
 
  // printf pour processing
  // printf("%f;%f;%f\r\n",
  //       RobotDiff.getPositionX(),
  //       RobotDiff.getPositionY(),
  //       RobotDiff.getAlpha());
  
  printf("%d;%d;%d;%f;%f;%f;%f;%f;%f\r\n",
        LidaRayzer.isObstacleDetected(),
        int(RobotDiff.getPositionX()),
        int(RobotDiff.getPositionY()),
        RobotDiff.getAlpha(),
        LidaRayzer.getObstacleX(),
        LidaRayzer.getObstacleY(),
        RobotDiff.getPosCibleX(),
        RobotDiff.getPosCibleY(),
        LidaRayzer.getObstacleAngleCible());

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
  Hook_G.pulsewidth_us(theta2pluse(Hook[0].hook_up));
  Hook_D.pulsewidth_us(theta2pluse(Hook[1].hook_up));
  ThisThread::sleep_for(200ms);
  StepperRG->move(600);
  StepperRD->move(600);
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
  StepperRG->move(-2300);
  StepperRD->move(-2300);
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
  Suck_Valve.pulsewidth_us(theta2pluse(180));
  Suck_Pump.pulsewidth_us(theta2pluse(0));
  ThisThread::sleep_for(1000ms);
  Suck_Valve.pulsewidth_us(theta2pluse(0));
  StepperRM->goUp();
  while (!(StepperRM->goUp()));
  StepperRG->move(-200);
  StepperRD->move(-200);
  while (!(StepperRG->StepperAct->stopped() and StepperRD->StepperAct->stopped()));
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
}

void print_lcd(void)
{

  while (1)
  {
    if (!end_match)
    {
      lcd.locate(0, 1);
      lcd.printf("Time :%d\n", int(endMatch.remaining_time().count()) / 1000000);
      lcd.locate(7, 0);
      lcd.printf("Score :%d\n", score);
    }
  }
}

void thread_lidar() {
    while (true) {
        LidaRayzer.update();
        //StopLidar = LidaRayzer.isObstacleDetected();
        StopLidar = !SW_init;
        ThisThread::sleep_for(1ms);
    }
}
void main_thread(void)
{
  FsmState = IDLE;
  lcd.printf("Wait Calibration\n");
  lcd.locate(0, 1);
  Couleur_Team = !SW_team;
  if (Couleur_Team == 0)
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
      RobotDiff.setPosition(1775, 170, 180);
      FsmState = WAIT_MATCH;
      lcd.cls();
      lcd.printf("Wait Match !\n");
      break;

    case WAIT_MATCH:
      if (SW_Tirette == 1 and FsmState != END)
      {
        endMatch.attach(endMatchProcess, 100s);
        lcd.cls();
        lcd.printf("GAME !\n");
        lcd_thread.start(print_lcd);
        FsmState = GAME; // Lancement du match !
      }
      break;

    case GAME:
      RobotDiff.Robotgoto(1775, 180, 180);
      Mover_rg.pulsewidth_us(theta2pluse(Bras[0].bras_drop_banner));
      Mover_rd.pulsewidth_us(theta2pluse(Bras[1].bras_drop_banner));
      ThisThread::sleep_for(500ms);
      StepperRG->goDown();
      StepperRD->goDown();
      while (!(StepperRG->goDown() and StepperRD->goDown()));
      ThisThread::sleep_for(500ms);
      Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_open));
      Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_open));
       RobotDiff.Robotgoto(1775, 450, 180);
      StepperRG->goUp();
      StepperRD->goUp();
      while (!(StepperRG->goUp() and StepperRD->goUp()));
      RobotDiff.Robotgoto(1775, 700, 90);
      RobotDiff.Robotgoto(2225, 700, 180);
      down_pince_take();
      RobotDiff.Robotgoto(2225, 325, 180);
      RobotDiff.Robotgoto(2225, 225, 180);
      Hook_G.pulsewidth_us(theta2pluse(Hook[0].hook_down));
      Hook_D.pulsewidth_us(theta2pluse(Hook[1].hook_down));
      ThisThread::sleep_for(500ms);
      Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_close));
      Pince_r2.pulsewidth_us(theta2pluse(Pince[1].pince_close));
      Pince_r3.pulsewidth_us(theta2pluse(Pince[2].pince_close));
      Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_close));
      RobotDiff.Robotgoto(2225, 120, 180);
      RobotDiff.setPosition(2225, 170, 180);
      RobotDiff.Robotgoto(2225, 225, 180);
      construction_gradin_niveau_2();
      RobotDiff.Robotgoto(2225, 325, 180);
      RobotDiff.Robotgoto(2225, 400, 90);
      down_pince_take();
      RobotDiff.Robotgoto(2700, 400, 90);
      RobotDiff.Robotgoto(2850, 400, 90);
      ThisThread::sleep_for(500ms);
      Hook_G.pulsewidth_us(theta2pluse(Hook[0].hook_down));
      Hook_D.pulsewidth_us(theta2pluse(Hook[1].hook_down));
      ThisThread::sleep_for(500ms);
      Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_close));
      Pince_r2.pulsewidth_us(theta2pluse(Pince[1].pince_close));
      Pince_r3.pulsewidth_us(theta2pluse(Pince[2].pince_close));
      Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_close));
      RobotDiff.Robotgoto(2700, 400, 0);
      RobotDiff.Robotgoto(2700, 400, -90);
      RobotDiff.Robotgoto(2900, 400, -90);
      RobotDiff.setPosition(2850, 400, -90);
      RobotDiff.Robotgoto(2700, 400, -90);
      RobotDiff.Robotgoto(1775, 600, 180);
      RobotDiff.Robotgoto(1775, 350, 180);
      construction_gradin_niveau_2();

      //===========================================//
      RobotDiff.Robotgoto(1775, 600, 180);
      RobotDiff.Robotgoto(1900, 600, 0);
      down_pince_take();
      RobotDiff.Robotgoto(1900, 875, 0);
      ThisThread::sleep_for(500ms);
      Hook_G.pulsewidth_us(theta2pluse(Hook[0].hook_down));
      Hook_D.pulsewidth_us(theta2pluse(Hook[1].hook_down));
      ThisThread::sleep_for(500ms);
      Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_close));
      Pince_r2.pulsewidth_us(theta2pluse(Pince[1].pince_close));
      Pince_r3.pulsewidth_us(theta2pluse(Pince[2].pince_close));
      Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_close));

      RobotDiff.Robotgoto(1900, 750, 0);
      RobotDiff.Robotgoto(1775, 600, 180);
      RobotDiff.Robotgoto(1775, 450, 180);
      construction_gradin_niveau_2();
      RobotDiff.Robotgoto(1775, 650, 180);
      RobotDiff.Robotgoto(2400, 600, 0);
      RobotDiff.Robotgoto(2500, 1500, 0);


      lcd_thread.terminate();
      lcd.cls();
      lcd.printf("END !\n");
      lcd.locate(0, 1);
      lcd.printf("Time :%d\n", int(endMatch.remaining_time().count()) / 1000000);
      lcd.locate(7, 0);
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
  Thread threadAffichage;
  Thread lidarAnalyzer_thread;
  threadAffichage.start(routineAffichage);
  

  En_drive_N = SW_Drive;
  En_step_N = SW_Stepper;

  SW_init.mode(PullUp);
  SW_team.mode(PullUp);
  SW_bau.mode(PullUp);
  SW_Tirette.mode(PullUp);
  SW_Drive.mode(PullUp);
  SW_Stepper.mode(PullUp);

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

  game_thread.start(main_thread);
  lidarAnalyzer_thread.start(thread_lidar);


  while (1)
  {

    En_drive_N = SW_Drive;
    En_step_N = SW_Stepper;
    if (end_match)
    {
      end_match = 0;
      lcd_thread.terminate();
      game_thread.terminate();
      lcd.cls();
      lcd.printf("END TIMOUT !\n");
      lcd.locate(0, 1);
      lcd.printf("Score :%d\n", score);
    }
  }
}
