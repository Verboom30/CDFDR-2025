#ifndef DIFFERENTIEL_H
#define DIFFERENTIEL_H

#include <AccelStepper.h>
#include <Thread.h>
#include <ThreadController.h>
#include <math.h>

#define RADIUS  37.35f // robot wheel-base radius
#define MOTOR_STEPS   201
#define RWHEEL  29.8f
#define REDUC   1.0f
#define KSTP    float((M_PI*2.0f*RWHEEL/(MOTOR_STEPS*MSTEP))*REDUC)
#define MAX_SPEED   SPEED*2
#define SPEED       50000
#define MSTEP 2
#define ACC    SPEED/4.f

class differentiel {
public:
    differentiel(AccelStepper* moteurGauche, AccelStepper* moteurDroit);
    void run();
    void stop();
    void move(int Distance, int Alpha);
    void handleRoutineGauche();
    void handleRoutineDroite();
    void updatePosition();
    void setPosition(int x, int y, int Alpha, bool team);
    void setPositionZero();
    void resetPosition();
    float getPositionX();
    float getPositionY();
    float getAlpha();
    int getDeltaG();
    int getDeltaD();
    bool Running();

private:
    AccelStepper* StepperG;
    AccelStepper* StepperD;
    float _positionX, _positionY, _Alpha;
    float _Speed, _SpeedAlpha;
    float _Move, _MoveAlpha;
    int _deltaG, _deltaD;
    int lastPosG, lastPosD;
    bool flagGauche, flagDroite;
};

// ====== Declarations only ======

enum MoveState { IDLE, MOVING };
enum GotoState { GOTO_IDLE, ROTATE_TO_TARGET, FORWARD, ROTATE_FINAL };

bool Robotmoveto(differentiel& robot, int distance, int alpha, bool stop);
bool Robotgoto(differentiel& robot, int positionX, int positionY, int alpha, bool stop, bool team);

#endif
