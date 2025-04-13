#ifndef __PINOUT_H_
#define __PINOUT_H_

//***********************************/************************************
//                              LIMIT SWITCH                            //
//***********************************/************************************
//RAIL 1
#define R1_SW_UP     PE_3  //SW11
#define R1_SW_DOWN   PC_7  //SW5

//RAIL 2
#define R2_SW_UP     PG_1  //SW10
#define R2_SW_DOWN   PA_15 //SW4

//RAIL 3
#define R3_SW_UP     PB_4  //SW9
#define R3_SW_DOWN   PB_13 //SW3

//RAIL 4
#define R4_SW_UP     PG_0  //SW8
#define R4_SW_DOWN   PB_15 //SW2

//FORK
#define FORK_SW_UP   PA_4  //SW7
#define FORK_SW_MID  PB_3  //SW6
#define FORK_SW_DOWN PC_6  //SW1

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

#define PINCE_R1     PB_6  //SERVO4
#define PINCE_R2     PC_8  //SERVO3
#define PINCE_R3     PC_9  //SERVO2
#define PINCE_R4     PB_5  //SERVO1
#define FORK         PD_13 //SERVO5
#define SUCKER_PUMP  PD_14   //SERVO6
#define SUCKER_VALVE PD_15    //SERVO7
#define SERVO_8      PE_5  //SERVO8
//***********************************/************************************
//                                 STEPPER                              //
//***********************************/************************************
// STEPPER_A
#define STEP_A PE_9
#define DIR_A  PF_13

// STEPPER_B
#define STEP_B PE_11
#define DIR_B  PF_14

// STEPPER_C
#define STEP_C PE_13 
#define DIR_C  PF_15

// STEPPER_R1
#define STEP_R1 PE_14
#define DIR_R1  PB_1

// STEPPER_R2
#define STEP_R2 PB_8
#define DIR_R2  PC_2

// STEPPER_R3
#define STEP_R3 PB_9
#define DIR_R3  PF_4

// STEPPER_R4
#define STEP_R4 PB_10
#define DIR_R4  PB_2

// STEPPER_FORK STEPPER5
#define STEP_FORK PA_6
#define DIR_FORK  PD_11

// STEPPER_SUCKER STEPPER6
#define STEP_SUCKER PB_11
#define DIR_SUCKER  PD_12

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
