#ifndef HOLONOME_H
#define HOLONOME_H

#include "mbed.h"
#include "pinout.hpp"
#include "../TMC2209/TMC2209.hpp"
#include <string.h>

#define PI      3.14159265
#define RADIUS  121.0 // robot wheel-base radius
#define MSTEP   8
#define RSTEP   198.5
#define RWHEEL  27.5 
#define REDUC   0.5
#define KSTP     ((PI*2.0*RWHEEL/(RSTEP*MSTEP))*REDUC)
#define SPEED   800.0 // max 50000 Mstepper 16 3200Ma
#define THETA   60.0 // trop angle sur le A

#define ACC    4.0
#define DEC    4.0

#define R_SENSE 0.11f        // R-Sense in OHM. Match to your driver
#define RMSCURRENT 1500      // RMS current of Stepper Coil in mA
#define MICROSTEPS 8         // # of microsteps
#define TOFF 5               // Enables driver in software - 3, 5
#define EN_SPREADCYCLE false // Toggle spreadCycle on TMC2208/2209/2224: default false, true: much faster!!!!
#define PWM_AUTOSCALE true   // Needed for stealthChop

class Holonome 
{
    public:
        Holonome(PinName Uart_TX_pin, PinName Uart_RX_pin, 
            PinName A_step_pin, PinName A_dir_pin, uint8_t A_Slave_Addr,
            PinName B_step_pin, PinName B_dir_pin, uint8_t B_Slave_Addr,
            PinName C_step_pin, PinName C_dir_pin, uint8_t C_Slave_Addr,
            float RS);

        TMC2209Stepper* StepperA = nullptr;
        TMC2209Stepper* StepperB = nullptr;
        TMC2209Stepper* StepperC = nullptr;

        bool    setupSteppers(void);
        void    getPosition(void);
        float   getPositionX(void);
        float   getPositionY(void);
        float   getPosCibleX(void);
        float   getPosCibleY(void);
        float   getAlpha(void);
        float   getSpeedX(void);
        float   getSpeedY(void);
        float   getSpeedAlpha(void);
        
    
        void  setPosition(int positionX, int positionY, int Alpha);
        void  setPositionZero(void);
        bool  stopped(void);
        bool  waitAck(void);
        bool  PosCibleDone(void);

        void  HolonomeSpeed(int SpeedX, int SpeedY, int W);
        void  goesTo(int positionX, int positionY, int Alpha); 
        void  move(int positionX, int positionY, int Alpha); 
        void  stop(void);
        void  run(void);
        float getSpeedA(void);
        float getSpeedB(void);
        float getSpeedC(void);
        int getPosA(void);
        int getPosB(void);
        int getPosC(void);
        int getStepA(void);
        int getStepB(void);
        int getStepC(void);
        
    private : 
        float _acc;                             //Acceleration [step/s²]
        float _dec;                             //Decceleration [step/s²]
        float _spd;                             //Speed [step/s]
        float _Alpha;
        float _positionX;
        float _positionY;
        float _cibleposX;
        float _cibleposY;
        float _positionX_Save;
        float _positionY_Save;
        float _Alpha_Save;
        float _MoveAlpha; 
        float _MovepositionX;
        float _MovepositionY;
        float _SpeedX;
        float _SpeedY;
        float _SpeedAlpha;
        
        string _Cmd;
        bool _AckStpA;
        bool _AckStpB;
        bool _AckStpC;

        Thread StepperA_thread;
        Thread StepperB_thread;
        Thread StepperC_thread;
        Thread routine;
        
        // void routine_stepperA(void);
        // void routine_stepperB(void);
        // void routine_stepperC(void); 
        void routine_holonome(void);
    protected:
        //***********************************/************************************
        //                          Protected Methods                           //
        //***********************************/************************************
         SerialTMC * SWSerial = nullptr;
        
        
};
#endif


