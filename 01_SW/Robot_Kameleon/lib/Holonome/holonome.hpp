#ifndef HOLONOME_H
#define HOLONOME_H

#include "mbed.h"
#include "pinout.hpp"
#include "Stepper.hpp"
#include "main_pck.hpp"
#include <string.h>

#define PI      3.14159265
#define RADIUS  121.0 // robot wheel-base radius
#define RSTEP   198.5
#define RWHEEL  27.5 
#define REDUC   0.5
#define KSTP     ((PI*2.0*RWHEEL/(RSTEP*MSTEP))*REDUC)
#define SPEED   200.0 // max 50000 Mstepper 16 3200Ma
#define THETA   60.0 // trop angle sur le A

#define ACC    4.0
#define DEC    4.0


class Holonome 
{
    public:
        Holonome();
        Stepper *StepperA = new Stepper(STEP_A,DIR_A);
        Stepper *StepperB = new Stepper(STEP_B,DIR_B);
        Stepper *StepperC = new Stepper(STEP_C,DIR_C);
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
        Thread routine;
        void routine_holonome(void);
        
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

        // Thread StepperA_thread;
        // Thread StepperB_thread;
        // Thread StepperC_thread;
      
        
        // // void routine_stepperA(void);
        // // void routine_stepperB(void);
        // // void routine_stepperC(void); 
         
    protected:
        //***********************************/************************************
        //                          Protected Methods                           //
        //***********************************/************************************
        
     
        
        
};
#endif


