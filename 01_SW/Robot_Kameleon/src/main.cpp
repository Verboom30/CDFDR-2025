#include "mbed.h"
#include "pinout.hpp"
#include "Stepper.hpp"
#include "UART_TMC.hpp"
#include "lcd.hpp"
#include "main_pck.hpp"
#include "LinearActuator.hpp"
#include <inttypes.h>
#include "Differentiel.hpp"

//***********************************/************************************
//                              UART_TMC                                //
//***********************************/************************************
Uart_TMC TMCSerial(TMC_UART_TX, TMC_UART_RX, SEL_UART_0, SEL_UART_1, SEL_UART_2, R_SENSE);
//***********************************/************************************
//                                 MOVE                                 //
//***********************************/************************************
// Stepper *StepperA = new Stepper(STEP_A,DIR_A);
// Stepper *StepperB = new Stepper(STEP_B,DIR_B);
// Stepper *StepperC = new Stepper(STEP_C,DIR_C);
Stepper StepperG(STEP_G,DIR_G);
Stepper StepperD(STEP_D,DIR_D);
diffrentiel RobotDiff(&StepperG,&StepperD);
DigitalOut En_drive_N(ENABLE_DRIVE_N);
DigitalOut En_step_N(ENABLE_STEP_N);


//***********************************/************************************
//                                 STEPPER                              //

//***********************************/************************************
LinearActuator *StepperRG = new LinearActuator(STEP_RG,DIR_RG,RG_SW_UP,RG_SW_DOWN,false);
LinearActuator *StepperRD = new LinearActuator(STEP_RD,DIR_RD,RD_SW_UP,RD_SW_DOWN,true);
LinearActuator *StepperRM = new LinearActuator(STEP_RM,DIR_RM,RM_SW_UP,RM_SW_DOWN,true);

//***********************************/************************************
//                                 SWITCH                               //
//***********************************/************************************
DigitalIn  SW_init(SW_INIT);
DigitalIn  SW_team(SW_TEAM);
DigitalIn  SW_bau(SW_BAU);
DigitalIn  SW_Tirette(TIRETTE);
DigitalIn  SW_Drive(SW_SPARE_1);
DigitalIn  SW_Stepper(SW_SPARE_2);
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
LCD lcd(LCD_RS,LCD_EN,LCD_D4,LCD_D5,LCD_D6,LCD_D7, LCD16x2); // rs, e, d4-d7

//Thread show_pos_thread;

int state= 0;

void showPostion(void)
{
  while (1)
  {
    printf("PosX:%f PosY:%f Speed:%f SpeedAlpha:%f SpeedG:%f SpeedD:%f \n"
    ,RobotDiff.getPositionX(),RobotDiff.getPositionY(),RobotDiff.getSpeed(),RobotDiff.getSpeedAlpha()
    ,RobotDiff.getSpeedG(),RobotDiff.getSpeedD()
    );

    // printf("PosX:%f PosY:%f Alpha:%f  SpeedX:%f SpeedY:%f SpeedAlpha:%f PosA:%d PosB:%d PosC:%d \n"
    // ,RobotHolonome.getPositionX(),RobotHolonome.getPositionY(),RobotHolonome.getAlpha(),RobotHolonome.getSpeedX(),RobotHolonome.getSpeedY(),
    // RobotHolonome.getSpeedAlpha(),RobotHolonome.getPosA(),RobotHolonome.getPosB(),RobotHolonome.getPosC()
    // );

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

void Robotmoveto(diffrentiel& robot, int distance, int alpha = 0)
{
    robot.move(distance, alpha);
    do
    {
      ThisThread::sleep_for(100ms);
    } while (!robot.stopped());
    robot.stop();
}

void printPosition()
{
    printf("[Position] X = %.2f mm | Y = %.2f mm | Angle = %.2f°\n",
           RobotDiff.getPositionX(),
           RobotDiff.getPositionY(),
           RobotDiff.getAlpha());
}

// --- Thread d'affichage de position ---
void routineAffichage()
{
    while (true) {
        printPosition();
        ThisThread::sleep_for(100ms);
    }
}

int main()
{
  Thread threadAffichage;
  threadAffichage.start(routineAffichage);

  En_drive_N = SW_Drive;
  En_step_N =SW_Stepper; 

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
  // En_drive_N = 1;
  // En_step_N = 1;

  //show_pos_thread.start(showPostion);

  lcd.cls();
  lcd.printf("Kameleon\n");
  lcd.locate(0,1);
  
  HAL_Delay (500);
  TMCSerial.setup_all_stepper();
  
 
  StepperRG->InitLinearActuator();
  StepperRD->InitLinearActuator();
  StepperRM->InitLinearActuator();
  HAL_Delay (500);
 
  // StepperRG->goUp();
  // StepperRD->goUp();
  // StepperRM->goUp();
  // while(!(StepperRG->goUp() and StepperRD->goUp() and StepperRM->goUp()));

  // StepperRG->goDown();
  // StepperRD->goDown();
  // StepperRM->goDown();
  // while(!(StepperRG->goDown() and StepperRD->goDown() and StepperRM->goDown()));

  // RobotHolonome.stop();
  // while(!RobotHolonome.waitAck());
  // RobotHolonome.setPositionZero();
  Hook_G.pulsewidth_us(theta2pluse(Hook[0].hook_up));
  Hook_D.pulsewidth_us(theta2pluse(Hook[1].hook_up));
  Mover_rg.pulsewidth_us(theta2pluse(Bras[0].bras_side));
  Mover_rd.pulsewidth_us(theta2pluse(Bras[1].bras_side));
  HAL_Delay (500);
  Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_close)); 
  Pince_r2.pulsewidth_us(theta2pluse(Pince[1].pince_open));
  Pince_r3.pulsewidth_us(theta2pluse(Pince[2].pince_open));
  Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_close));
 
  StepperRG->goUp();
  StepperRD->goUp();
  StepperRM->goUp();
  while(!(StepperRG->goUp() and StepperRD->goUp() and StepperRM->goUp()));


  Mover_rg.pulsewidth_us(theta2pluse(Bras[0].bras_home));
  Mover_rd.pulsewidth_us(theta2pluse(Bras[1].bras_home));

  RobotDiff.setPosition(0, 0, 0);

  Robotmoveto(RobotDiff,100,0);

  Robotmoveto(RobotDiff,100,0);

  Robotmoveto(RobotDiff,0,45);

  Robotmoveto(RobotDiff,100,0);
  //Robotmoveto(RobotDiff,0,-90);
 
  

  



  // HAL_Delay (500);
  // Mover_rg.pulsewidth_us(theta2pluse(Bras[0].bars_take));
  // Mover_rd.pulsewidth_us(theta2pluse(Bras[1].bars_take));
  // HAL_Delay (500);
  // StepperRG->goDown();
  // StepperRD->goDown();

  // Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_open));
  // Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_open));

  // Hook_G.pulsewidth_us(theta2pluse(Hook[0].hook_down));
  // Hook_D.pulsewidth_us(theta2pluse(Hook[1].hook_down));
  


  
 
  // StepperRG->goUp();
  // StepperRD->goUp();
  // StepperRM->goUp();
  //  HAL_Delay (500);
  // StepperRG->goDown();
  // StepperRD->goDown();
  // StepperRM->goDown();
  
 
  //HAL_Delay (1500);
  //Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_close));
   //Pince_r2.pulsewidth_us(theta2pluse(Pince[1].pince_close));
   //Pince_r3.pulsewidth_us(theta2pluse(Pince[2].pince_close));
   //Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_open));
  // HAL_Delay (1500); // Attente de 2 secondes 
  // Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_open));
  // Pince_r2.pulsewidth_us(theta2pluse(Pince[1].pince_open));
  // Pince_r3.pulsewidth_us(theta2pluse(Pince[2].pince_open));
  // Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_open));
  // HAL_Delay (1500); // Attente de 2 secondes 
  // StepperR1->goDown();
  // StepperR2->goDown();
  // StepperR3->goDown();
  // StepperR4->goDown();
  
  
  
  // StepperR2.InitLinearActuator();
  // StepperR2.goUp();

  // StepperR3.InitLinearActuator();
  // StepperR3.goUp();

  // StepperR4.InitLinearActuator();
  // StepperR4.goUp();
  //StepperR1.StepperAct->move(1000);
  //while(!StepperR1.StepperAct->stopped());
  //while(!StepperR1.stopped());
  // RobotHolonome.move(50,0,0);
  // while(!RobotHolonome.waitAck());
  // while(!RobotHolonome.stopped());
  // HAL_Delay (2000);
  // RobotHolonome.move(50,0,0);
  // while(!RobotHolonome.waitAck());
  // while(!RobotHolonome.stopped());

  // HAL_Delay (2000);
  // RobotHolonome.goesTo(0,0,0);
  // while(!RobotHolonome.waitAck());
  // while(!RobotHolonome.stopped());

  // HAL_Delay (2000);
  // RobotHolonome.move(-50,0,0);
  // while(!RobotHolonome.waitAck());
  // while(!RobotHolonome.stopped());

  // HAL_Delay (2000);
  // RobotHolonome.move(150,0,0);
  // while(!RobotHolonome.waitAck());
  // while(!RobotHolonome.stopped());

  // HAL_Delay (2000);
  // RobotHolonome.goesTo(0,0,0);
  // while(!RobotHolonome.waitAck());
  // while(!RobotHolonome.stopped());
 

  while (true)
  {
    ThisThread::sleep_for(1s); 
    En_drive_N = SW_Drive;
    En_step_N =SW_Stepper; 


    // switch (state)
    // {
    //   case 0:
    //     if (SW_init != 1) {
    //       state++;
    //     }
    //     break;

    //   case 1:
    //       Mover_rg.pulsewidth_us(theta2pluse(Bras[0].bras_take));
    //       Mover_rd.pulsewidth_us(theta2pluse(Bras[1].bras_take));
    //       Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_open));
    //       Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_open));
    //       StepperRG->goDown();
    //       StepperRD->goDown();
    //       while(!(StepperRG->goDown() and StepperRD->goDown()));
    //       state++;
    //       break;
    //   case 2:
    //     if (SW_init != 1) {
    //       Hook_G.pulsewidth_us(theta2pluse(Hook[0].hook_down));
    //       Hook_D.pulsewidth_us(theta2pluse(Hook[1].hook_down));
    //       HAL_Delay (500);
    //       Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_close));
    //       Pince_r2.pulsewidth_us(theta2pluse(Pince[1].pince_close));
    //       Pince_r3.pulsewidth_us(theta2pluse(Pince[2].pince_close));
    //       Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_close));
    //       HAL_Delay (500);
    //       Mover_rg.pulsewidth_us(theta2pluse(Bras[0].bras_side));
    //       Mover_rd.pulsewidth_us(theta2pluse(Bras[1].bras_side));
    //       HAL_Delay (500);
    //       StepperRM->move(-11500);
    //       while(!StepperRM->StepperAct->stopped());
    //       Suck_Pump.pulsewidth_us(theta2pluse(180));
    //       HAL_Delay (500);
    //       StepperRM->move(-100);
    //       while(!StepperRM->StepperAct->stopped());
    //       HAL_Delay (500);
    //       Hook_G.pulsewidth_us(theta2pluse(Hook[0].hook_up));
    //       Hook_D.pulsewidth_us(theta2pluse(Hook[1].hook_up));
    //       HAL_Delay (500);
    //       StepperRM->goUp();
    //       StepperRG->goUp();
    //       StepperRD->goUp();
    //       while(!(StepperRG->goUp() and StepperRD->goUp() and StepperRM->goUp()));
    //       StepperRG->move(-2500);
    //       StepperRD->move(-2500);
    //       while(!(StepperRG->StepperAct->stopped() and StepperRD->StepperAct->stopped()));
    //       HAL_Delay (500);
    //       Mover_rg.pulsewidth_us(theta2pluse(Bras[0].bras_take));
    //       Mover_rd.pulsewidth_us(theta2pluse(Bras[1].bras_take));
    //       HAL_Delay (1500);
    //       StepperRM->move(-2500);
    //       while(!StepperRM->StepperAct->stopped());
    //       HAL_Delay (500);
    //       Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_open));
    //       Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_open));
    //       Suck_Pump.pulsewidth_us(theta2pluse(0));
    //       state = 7;
    //     }
    //     break;
    //   case 3:
    //     if (SW_init != 1) {
    //       Mover_rg.pulsewidth_us(theta2pluse(Bras[0].bras_drop_banner));
    //       Mover_rd.pulsewidth_us(theta2pluse(Bras[1].bras_drop_banner));
    //       Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_open));
    //       Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_open));
    //       HAL_Delay (2500);
    //       Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_banner));
    //       Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_banner));
    //       HAL_Delay (500);
    //       state++;
    //     }
    //     break;
    //   case 4:
    //     if (SW_init != 1) {
          
    //       StepperRG->goUp();
    //       StepperRD->goUp();
    //       while(!(StepperRG->goUp() and StepperRD->goUp()));
    //       Mover_rg.pulsewidth_us(theta2pluse(Bras[0].bras_banner));
    //       Mover_rd.pulsewidth_us(theta2pluse(Bras[1].bras_banner));
    //       HAL_Delay (500);
        
    //       state++;
    //     }
    //     break;

    //   case 5:
    //     if (SW_init != 1) {

    //       Mover_rg.pulsewidth_us(theta2pluse(Bras[0].bras_drop_banner));
    //       Mover_rd.pulsewidth_us(theta2pluse(Bras[1].bras_drop_banner));
    //       HAL_Delay (500);
    //       StepperRG->goDown();
    //       StepperRD->goDown();
    //       HAL_Delay (500);
    //       Pince_r1.pulsewidth_us(theta2pluse(Pince[0].pince_open));
    //       Pince_r4.pulsewidth_us(theta2pluse(Pince[3].pince_open));
    //       //Hook_G.pulsewidth_us(theta2pluse(Hook[0].hook_up));
    //       //Hook_D.pulsewidth_us(theta2pluse(Hook[1].hook_up));
    //       state++;
    //     }
    //     break;

    //   case 6:
    //     if (SW_init != 1){
          
    //       state++;
    //     }
    //     break;
    //   case 7:
    //         //Suck_Pump.pulsewidth_us(theta2pluse(180));
    //         //HAL_Delay (1000);
          
    //         //Suck_Pump.pulsewidth_us(theta2pluse(0));
    //         //HAL_Delay (1000);
    //         state++;
            
    //     break;
    //   case 8:
         
    //     break;
    //   /* code */
    
    //   default:
    //     break;
    // }
  
   
   
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