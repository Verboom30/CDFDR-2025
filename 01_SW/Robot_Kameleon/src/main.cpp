#include "mbed.h"
#include "pinout.hpp"
#include "Holonome.hpp"
#include "UART_TMC.hpp"
#include "lcd.hpp"
#include "main_pck.hpp"
#include <inttypes.h>

//SerialTMC *SWSerial = new SerialTMC(TMC_UART_TX, TMC_UART_RX);
//TMC2209Stepper StepperA(STEP_A, DIR_A, SWSerial, R_SENSE, 0x00);
// TMC2209Stepper StepperB(STEP_B, DIR_B, &SWSerial, R_SENSE, 0x01);
// TMC2209Stepper StepperC(STEP_C, DIR_C, &SWSerial, R_SENSE, 0x02);



Uart_TMC TMCSerial(TMC_UART_TX, TMC_UART_RX, SEL_UART_0, SEL_UART_1, SEL_UART_2, R_SENSE);
// //***********************************/************************************
// //                                 MOVE                                 //
// //***********************************/************************************
// Holonome RobotHolonome(StepperA, StepperB, StepperC);
//***********************************/************************************
//                                 MOVE                                  //
//***********************************/************************************


// TMC2209Stepper StepperA(STEP_A, DIR_A, SWSerial, R_SENSE, 0x01);
// DigitalOut sel_1(SEL_1);
// DigitalOut sel_2(SEL_2);
// DigitalOut sel_3(SEL_3);

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
//                               STEPPER UART                           //
//***********************************/************************************
// DigitalOut sel_1 (SEL_UART_0);
// DigitalOut sel_2 (SEL_UART_1);
// DigitalOut sel_3 (SEL_UART_2);

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


  lcd.cls();
  lcd.printf("Kameleon\n");
  lcd.locate(0,1);
  

  //SWSerial->beginSerial(155200);
 
  //StepperA.setup_stepper();
  // StepperB.setup_stepper();
  // StepperC.setup_stepper();
  TMCSerial.setup_all_stepper();
  Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_close));
 
 

  //RobotHolonome.setupSteppers();
  // RobotHolonome.stop();
  // while(!RobotHolonome.waitAck());
  // RobotHolonome.setPositionZero();
  // while(!RobotHolonome.waitAck());
  // SWSerial->beginSerial(155200);
  // wait_us(10*1000);
  // StepperA.begin();
  // StepperA.toff(TOFF);                // Enables driver in software - 3, 5 ????
  // StepperA.rms_current(RMSCURRENT);   // Set motor RMS current in mA / min 500 for 24V/speed:3000
  //                                      // 1110, 800
  //                                      // working: 800 12V/0,6Amax,  Speed up to 5200=4U/min
  // StepperA.microsteps(MICROSTEPS);    // Set microsteps to 1:Fullstep ... 256: 1/256th
  // StepperA.en_spreadCycle(EN_SPREADCYCLE);     // Toggle spreadCycle on TMC2208/2209/2224: default false, true: much faster!!!!
  // StepperA.pwm_autoscale(PWM_AUTOSCALE);       // Needed for stealthChop

  // StepperA.stop();
  // StepperA.setPositionZero();
  // StepperA.setSpeed(5000);
  // StepperA.setAcceleration(5000/ACC);
  // StepperA.setDeceleration(5000/DEC);
  // StepperA.move(20000);
  // while(!StepperA.stopped());
  // sel_1 = 0;
  // sel_2 = 0;
  // sel_3 = 0;
  // wait_us(10*2000);

  // RobotHolonome.SWSerialHolonome->beginSerial(115200);
  // wait_us(10*1000);
  // sel_1 = 1;
  // sel_2 = 0;
  // sel_3 = 0;
  // RobotHolonome.StepperA->begin();
  // wait_us(10*1000);
  // sel_1 = 0;
  // sel_2 = 0;
  // sel_3 = 1;
  // RobotHolonome.StepperB->begin();
  // wait_us(10*1000);
  // sel_1 = 0;
  // sel_2 = 0;
  // sel_3 = 1;
  // RobotHolonome.StepperC->begin();
  

  //RobotHolonome.setupSteppers();
  // RobotHolonome.stop();
  // while(!RobotHolonome.waitAck());
  // RobotHolonome.setPositionZero();
  // while(!RobotHolonome.waitAck());
  

  // RobotHolonome.goesTo(0,0,90);
  // while(!RobotHolonome.waitAck());
  // while(!RobotHolonome.stopped());
 

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