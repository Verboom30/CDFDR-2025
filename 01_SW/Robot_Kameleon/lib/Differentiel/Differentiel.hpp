#ifndef HOLONOME_H
#define HOLONOME_H

#include "mbed.h"
#include "pinout.hpp"
#include "Stepper.hpp"
#include "main_pck.hpp"
#include <string.h>


#define PI      3.14159265
#define RADIUS  71.0 // robot wheel-base radius
#define RSTEP   200
#define RWHEEL  35.0 
#define REDUC   0.5
#define KSTP    ((PI*2.0*RWHEEL/(RSTEP*MSTEP))*REDUC)
#define SPEED   300.0 // max 50000 Mstepper 16 3200Ma

#define ACC    2.0
#define DEC    2.0

class diffrentiel
{
    public:
    diffrentiel();
    Stepper *StepperG = new Stepper(STEP_G,DIR_G);
    Stepper *StepperD = new Stepper(STEP_D,DIR_D);
   
    float   getPositionX(void);
    float   getPositionY(void);
    float   getPosCibleX(void);
    float   getPosCibleY(void);
    float   getAlpha(void);
    float   getSpeed(void);
    float   getSpeedAlpha(void);
    

    void  setPosition(int positionX, int positionY, int Alpha);
    void  setPositionZero(void);
    bool  stopped(void);
    bool  waitAck(void);
    bool  PosCibleDone(void);

    void  goesTo(int positionX, int positionY, int Alpha); 
    void  move(int positionX, int positionY, int Alpha); 
    void  stop(void);
    void  run(void);
    float getSpeedG(void);
    float getSpeedD(void);
    int getPosG(void);
    int getPosD(void);
   
    int getStepG(void);
    int getStepD(void);
 
    Thread routine;
    void routine_diffrentiel(void);
    
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
    float _Move;
    float _SpeedAlpha;
    float _Speed;
    
    string _Cmd;
    bool _AckStpG;
    bool _AckStpD;
   

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