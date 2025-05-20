#ifndef DIFFERENTIEL_H
#define DIFFERENTIEL_H

#include <Arduino.h>  // pour delay(), etc.
#include <math.h>
#include <AccelStepper.h>

#define RADIUS  84.00f // robot wheel-base radius
#define MOTOR_STEPS   198
#define RWHEEL  35.0f
#define REDUC   0.5f
#define KSTP    float((M_PI*2.0f*RWHEEL/(MOTOR_STEPS*MSTEP))*REDUC)
#define SPEED   10000000
#define MAX_SPEED   10000000
#define MSTEP 8
#define ACC    3.0f

class differentiel {
public:
    differentiel(AccelStepper* moteurGauche, AccelStepper* moteurDroit);

    void run();
    void stop();
    void move(int Distance, int Alpha);
    void updatePosition();
    void setPosition(int positionX, int positionY, int Alpha);
    void setPositionZero();
    void resetPosition();

    // Méthodes à appeler dans loop()
    void handleRoutineGauche();
    void handleRoutineDroite();

    // Getters
    float getPositionX();
    float getPositionY();
    float getAlpha();
    int getDeltaG();
    int getDeltaD();
    bool Running();

private:
    AccelStepper* StepperG;
    AccelStepper* StepperD;

    // Position et consignes
    float _positionX, _positionY, _Alpha;
    float _Speed, _SpeedAlpha;
    float _Move, _MoveAlpha;
    int _deltaG, _deltaD;
    int lastPosG, lastPosD;

    // Flags logiciels
    volatile bool flagGauche, flagDroite;
};

#endif