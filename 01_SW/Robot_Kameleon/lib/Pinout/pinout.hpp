#ifndef __PINOUT_H_
#define __PINOUT_H_

//***********************************/************************************
//                              LIMIT SWITCH                            //
//***********************************/************************************
//RAIL GAUCHE 
#define RG_SW_UP     PB_3  //SW6
#define RG_SW_DOWN   PB_13 //SW3

//RAIL DROIT
#define RD_SW_UP     PA_15 //SW4
#define RD_SW_DOWN   PC_6  //SW1

//RAIL MID
#define RM_SW_UP     PC_7  //SW5
#define RM_SW_DOWN   PB_15 //SW2

//***********************************/************************************
//                                 SWITCH                               //
//***********************************/************************************
#define TIRETTE    PA_5 
#define SW_TEAM    PA_7 
#define SW_BAU     PF_12 
#define SW_INIT    PE_0
#define SW_SPARE_1 PE_2
#define SW_SPARE_2 PA_0

//***********************************/************************************
//                                 SERVO                                //
//***********************************/************************************
#define PINCE_R1       PB_5  //SERVO1
#define PINCE_R2       PC_8 //SERVO2
#define PINCE_R3       PC_9//SERVO3
#define PINCE_R4       PB_6  //SERVO4
#define PINCE_MOVE_R1  PD_13 //SERVO5
#define PINCE_MOVE_R4  PD_14 //SERVO6
#define SERVO_HOOKG    PD_15 //SERVO7
#define SERVO_HOOKD    PE_5  //SERVO8
#define SUCKER_PUMP    PF_8 
#define SUCKER_VALVE   PF_7 
//***********************************/************************************
//                                 STEPPER                              //
//***********************************/************************************
// STEPPER_A
#define STEP_G PE_9
#define DIR_G  PF_13

// STEPPER_B
#define STEP_D PE_11
#define DIR_D  PF_14

// STEPPER_R1
#define STEP_RG PE_14
#define DIR_RG  PB_1

// STEPPER_R2
#define STEP_RD PB_8
#define DIR_RD  PC_2

// STEPPER_R3
#define STEP_RM PB_9
#define DIR_RM PF_4

#define ENABLE_DRIVE_N PE_10
#define ENABLE_STEP_N PE_12

//***********************************/************************************
//                               STEPPER UART                           //
//***********************************/************************************
#define SEL_UART_0 PD_7 //STEPPER A, B, C.
#define SEL_UART_1 PD_4 //STEPPER 1, 2, 3, 4.
#define SEL_UART_2 PD_3 //STEPPER 5, 6.

//UART TMC2209
#define TMC_UART_TX PE_8
#define TMC_UART_RX PE_7

//***********************************/************************************
//                               UART BTH                               //
//***********************************/************************************
// Bluetooth HC-06
#define BTH_UART_TX PG_9
#define BTH_UART_RX PG_14
//***********************************/************************************
//                               SERIAL                                 //
//***********************************/************************************
//Serial port
#define SERIAL_TX USBTX
#define SERIAL_RX USBRX
//***********************************/************************************
//                                 LIDAR                                //
//***********************************/************************************
#define LIDAR_TX  PD_5  
#define LIDAR_RX  PD_6
#define LIDAR_LED PE_15
//***********************************/************************************
//                                 LCD                                  //
//***********************************/************************************

//Ldc Screen
#define LCD_RS PC_10
#define LCD_EN PC_11
#define LCD_D4 PC_12
#define LCD_D5 PD_2
#define LCD_D6 PG_2
#define LCD_D7 PG_3

#endif // __PINOUT_H_
