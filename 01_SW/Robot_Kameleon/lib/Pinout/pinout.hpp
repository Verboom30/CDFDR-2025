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
#define TIRETTE    D13  
#define SW_TEAM    D11 
#define SW_BAU     D8 
#define SW_INIT    PE_0
#define SW_SPARE_1 PE_2
#define SW_SPARE_2 PA_0

//***********************************/************************************
//                                 SERVO                                //
//***********************************/************************************

#define PINCE_R1     PB_6  //SERVO4
#define PINCE_R2     PC_9  //SERVO3
#define PINCE_R3     PC_8  //SERVO2
#define PINCE_R4     PB_5  //SERVO1
#define FORK         PD_13 //SERVO5
#define SUCKER_PUMP  D10   //SERVO6
#define SUCKER_VALVE D9    //SERVO7
#define SERVO_8      PE_5  //SERVO8
//***********************************/************************************
//                                 STEPPER                              //
//***********************************/************************************
// STEPPER_A
#define STEP_A D6
#define DIR_A  D7

// STEPPER_B
#define STEP_B D5
#define DIR_B  D4

// STEPPER_C
#define STEP_C D3 
#define DIR_C  D2

// STEPPER_R1
#define STEP_R1 PE_14
#define DIR_R1  PB_1

// STEPPER_R2
#define STEP_R2 D15
#define DIR_R2  PC_2

// STEPPER_R3
#define STEP_R3 D14
#define DIR_R3  PF_4

// STEPPER_R4
#define STEP_R4 PB_10
#define DIR_R4  PB_2

// STEPPER_FORK STEPPER5
#define STEP_FORK D12
#define DIR_FORK  PD_11

// STEPPER_SUCKER STEPPER6
#define STEP_SUCKER PB_11
#define DIR_SUCKER  PD_12

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
#define BTH_UART_TX D1
#define BTH_UART_RX D0
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
#define LCD_RS PE_2
#define LCD_EN PE_4
#define LCD_D4 PC_12
#define LCD_D5 PD_2
#define LCD_D6 PG_2
#define LCD_D7 PG_3


#endif // __PINOUT_H_
