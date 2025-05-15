#ifndef HOLONOME_H
#define HOLONOME_H

#include "mbed.h"
#include "pinout.hpp"
#include "Stepper.hpp"
#include "main_pck.hpp"
#include <string.h>
#include <cmath>



#define RADIUS  66.5f // robot wheel-base radius
#define RSTEP   200
#define RWHEEL  35.0f
#define REDUC   0.5f
#define KSTP    float((M_PI*2.0f*RWHEEL/(RSTEP*MSTEP))*REDUC)
#define SPEED   300.0f // max 50000 Mstepper 16 3200Ma

#define ACC    2.0f
#define DEC    2.0f

class diffrentiel
{
    public:
    diffrentiel(Stepper* moteurGauche, Stepper* moteurDroit);

    void run();
    void stop();
    
    void move(int distance, int Alpha); 
    void setPosition(int positionX, int positionY, int Alpha);
    void setPositionZero();
    void resetPosition(); 

    float getSpeedG();
    float getSpeedD();
    int getPosG();
    int getPosD();
    int getStepG();
    int getStepD();
    int getDeltaG();
    int getDeltaD();

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
    Thread threadOdometrie;
    EventFlags flags;
    Mutex mutexData;
  
    int readyCount;
    Semaphore semG, semD;
    Mutex syncMutex;
   

    // Données de mouvement
    float _positionX, _positionY;
    float _cibleposX, _cibleposY;
    float _Alpha;
    float _Speed, _SpeedAlpha;
    float _Move, _MoveAlpha;
    int _deltaG, _deltaD;

    int lastPosG = 0;
    int lastPosD = 0;

    // Routines moteurs
    void routine_gauche();
    void routine_droite();
    void synchroniser();

    void routine_odometrie();
    void updatePosition();

};

#endif // DIFFERENTIEL_HPP