#include "mbed.h"
#include "pinout.hpp"
#include "Holonome.hpp"
#include "Stepper.hpp"
#include "UART_TMC.hpp"
#include "lcd.hpp"
#include "main_pck.hpp"
#include <inttypes.h>

//***********************************/************************************
//                              UART_TMC                                //
//***********************************/************************************
Uart_TMC TMCSerial(TMC_UART_TX, TMC_UART_RX, SEL_UART_0, SEL_UART_1, SEL_UART_2, R_SENSE);
// //***********************************/************************************
// //                                 MOVE                                 //
// //***********************************/************************************
Stepper *StepperA = new Stepper(STEP_A,DIR_A);
Stepper *StepperB = new Stepper(STEP_B,DIR_B);
Stepper *StepperC = new Stepper(STEP_C,DIR_C);
DigitalOut En_drive_N(ENABLE_DRIVE_N);
DigitalOut En_step_N(ENABLE_STEP_N);
Holonome RobotHolonome(StepperA, StepperB, StepperC);
//***********************************/************************************
//                              LIMIT SWITCH                            //
//***********************************/************************************
DigitalIn  SW_r1_up(R1_SW_UP);
DigitalIn  SW_r2_up(R2_SW_UP);
DigitalIn  SW_r3_up(R3_SW_UP);
DigitalIn  SW_r4_up(R4_SW_UP);

DigitalIn  SW_r1_down(R1_SW_DOWN);
DigitalIn  SW_r2_down(R2_SW_DOWN);
DigitalIn  SW_r3_down(R3_SW_DOWN);
DigitalIn  SW_r4_down(R4_SW_DOWN);

DigitalIn  SW_fork_up(FORK_SW_UP);
DigitalIn  SW_fork_mid(FORK_SW_MID);
DigitalIn  SW_fork_down(FORK_SW_DOWN);

//***********************************/************************************
//                                 SWITCH                               //
//***********************************/************************************
DigitalIn  SW_init(SW_INIT);
DigitalIn  SW_team(SW_TEAM);
DigitalIn  SW_bau(SW_BAU);
DigitalIn  SW_Tirette(TIRETTE);
//***********************************/************************************
//                                 SERVO                                //
//***********************************/************************************
PwmOut Pince_r1(PINCE_R1);
PwmOut Pince_r2(PINCE_R2);
PwmOut Pince_r3(PINCE_R3);
PwmOut Pince_r4(PINCE_R4);
PwmOut Fork(FORK);
PwmOut Sucket_pump(SUCKER_PUMP);
PwmOut Sucket_valve(SUCKER_VALVE);

//***********************************/************************************
//                                 LIDAR                                //
//***********************************/************************************
DigitalOut led_lidar(LIDAR_LED);
//***********************************/************************************
//                                 LCD                                  //
//***********************************/************************************
LCD lcd(LCD_RS,LCD_EN,LCD_D4,LCD_D5,LCD_D6,LCD_D7, LCD16x2); // rs, e, d4-d7

Thread show_pos_thread;

void showPostion(void)
{
  while (1)
  {
    // printf("PosX:%f PosY:%f Alpha:%f  SpeedX:%f SpeedY:%f SpeedAlpha:%f SpeedA:%f SpeedB:%f SpeedC:%f \n"
    // ,RobotMove->getPositionX(),RobotMove->getPositionY(),RobotMove->getAlpha(),RobotMove->getSpeedX(),RobotMove->getSpeedY(),
    // RobotMove->getSpeedAlpha(),RobotMove->getSpeedA(),RobotMove->getSpeedB(),RobotMove->getSpeedC()
    // );

    printf("PosX:%f PosY:%f Alpha:%f  SpeedX:%f SpeedY:%f SpeedAlpha:%f PosA:%d PosB:%d PosC:%d \n"
    ,RobotHolonome.getPositionX(),RobotHolonome.getPositionY(),RobotHolonome.getAlpha(),RobotHolonome.getSpeedX(),RobotHolonome.getSpeedY(),
    RobotHolonome.getSpeedAlpha(),RobotHolonome.getPosA(),RobotHolonome.getPosB(),RobotHolonome.getPosC()
    );

    // printf("PosX:%f PosY:%f Alpha:%f  SpeedX:%f SpeedY:%f SpeedAlpha:%f StepA:%d StepB:%d StepC:%d SpeedA:%f SpeedB:%f SpeedC:%f\n"
    // ,RobotMove->getPositionX(),RobotMove->getPositionY(),RobotMove->getAlpha(),RobotMove->getSpeedX(),RobotMove->getSpeedY(),
    // RobotMove->getSpeedAlpha(),RobotMove->getStepA(),RobotMove->getStepB(),RobotMove->getStepC(),RobotMove->getSpeedA(),RobotMove->getSpeedB(),RobotMove->getSpeedC()
    // );

    //printf("%f;%f;%f\r\n",RobotMove->getPositionX(),RobotMove->getPositionY(),RobotMove->getAlpha());
 
  }
  
}
float theta2pluse(int theta)
{
  return 500.0+(100.0/9.0)*float(theta);
}

int main()
{
  SW_r1_up.mode(PullUp);
  SW_r2_up.mode(PullUp);
  SW_r3_up.mode(PullUp);
  SW_r4_up.mode(PullUp);
  SW_r1_down.mode(PullUp);
  SW_r2_down.mode(PullUp);
  SW_r3_down.mode(PullUp);
  SW_r4_down.mode(PullUp);
  SW_fork_up.mode(PullUp);
  SW_fork_mid.mode(PullUp);
  SW_fork_down.mode(PullUp);
  SW_init.mode(PullUp);
  SW_team.mode(PullUp);
  SW_bau.mode(PullUp);
  SW_Tirette.mode(PullUp);

  Pince_r1.period_ms(20);
  Pince_r2.period_ms(20);
  Pince_r3.period_ms(20);
  Pince_r4.period_ms(20);
  Fork.period_ms(20);
  Sucket_pump.period_ms(20);
  Sucket_valve.period_ms(20);
  En_drive_N = 0;
  En_step_N = 0;

  show_pos_thread.start(showPostion);

  lcd.cls();
  lcd.printf("Kameleon\n");
  lcd.locate(0,1);
  

  TMCSerial.setup_all_stepper();

  RobotHolonome.stop();
  while(!RobotHolonome.waitAck());
  RobotHolonome.setPositionZero();
  

  RobotHolonome.move(50,0,0);
  while(!RobotHolonome.waitAck());
  while(!RobotHolonome.stopped());
  HAL_Delay (2000);
  RobotHolonome.move(50,0,0);
  while(!RobotHolonome.waitAck());
  while(!RobotHolonome.stopped());

  HAL_Delay (2000);
  RobotHolonome.goesTo(0,0,0);
  while(!RobotHolonome.waitAck());
  while(!RobotHolonome.stopped());

  HAL_Delay (2000);
  RobotHolonome.move(-50,0,0);
  while(!RobotHolonome.waitAck());
  while(!RobotHolonome.stopped());

  HAL_Delay (2000);
  RobotHolonome.move(150,0,0);
  while(!RobotHolonome.waitAck());
  while(!RobotHolonome.stopped());

  HAL_Delay (2000);
  RobotHolonome.goesTo(0,0,0);
  while(!RobotHolonome.waitAck());
  while(!RobotHolonome.stopped());
 

  while (1)
  {
    // printf("SW_r1_up:%d SW_r2_up:%d SW_r3_up:%d SW_r4_up:%d SW_r1_dw:%d SW_r2_dw:%d SW_r3_dw:%d SW_r4_dw:%d SW_fork_up:%d SW_fork_mid:%d SW_fork_down:%d \n",
    // int(SW_r1_up), int(SW_r2_up), int(SW_r3_up), int(SW_r4_up),
    // int(SW_r1_down), int(SW_r2_down), int(SW_r3_down), int(SW_r4_down),
    // int(SW_fork_up), int(SW_fork_mid), int(SW_fork_down));

    // Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_close));
    // Pince_r2.pulsewidth_us(theta2pluse(Pince[1].pince_close));
    // Pince_r3.pulsewidth_us(theta2pluse(Pince[2].pince_close));
    // Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_close));
    // HAL_Delay (2000); // Attente de 2 secondes 
    // Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_open));
    // Pince_r2.pulsewidth_us(theta2pluse(Pince[1].pince_open));
    // Pince_r3.pulsewidth_us(theta2pluse(Pince[2].pince_open));
    // Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_open));
    // HAL_Delay (2000); // Attente de 2 secondes 
  
  }
}