#ifndef HOLONOME_H
#define HOLONOME_H

#include "mbed.h"
#include "pinout.hpp"
#include "Stepper.hpp"
#include "main_pck.hpp"
#include <string.h>
#include <cmath>


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
    diffrentiel(Stepper* moteurGauche, Stepper* moteurDroit);

    void run();
    void stop();

    void goesTo(int positionX, int positionY, int Alpha);
    void move(int distance, int Alpha); 
    void setPosition(int positionX, int positionY, int Alpha);
    void setPositionZero();

    float getSpeedG();
    float getSpeedD();
    int getPosG();
    int getPosD();
    int getStepG();
    int getStepD();

    float getPositionX();
    float getPositionY();
    float getPosCibleX();
    float getPosCibleY();
    float getAlpha();
    float getSpeed();
    float getSpeedAlpha();

    bool stopped();
    bool PosCibleDone();
   

private : 
    // Moteurs
    Stepper* StepperG;
    Stepper* StepperD;

    // Threads et synchro
    Thread routineG;
    Thread routineD;
    EventFlags flags;
    Mutex mutexData;
  
    int readyCount;
    Semaphore semG, semD;
    Mutex syncMutex;
   

    // Données de mouvement
    float _positionX, _positionY;
    float _positionX_Save, _positionY_Save;
    float _cibleposX, _cibleposY;
    float _Alpha, _Alpha_Save;
    float _Speed, _SpeedAlpha;
    float _Move, _MoveAlpha;

    // Routines moteurs
    void routine_gauche();
    void routine_droite();
    void synchroniser();

};

#endif // DIFFERENTIEL_HPP