#include "Differentiel.hpp"

diffrentiel::diffrentiel(Stepper* moteurGauche, Stepper* moteurDroit)
    : StepperG(moteurGauche), StepperD(moteurDroit), readyCount(0), semG(0), semD(0) 
{
    _positionX = _positionY = 0;
    _positionX_Save = _positionY_Save = 0;
    _cibleposX = _cibleposY = 0;
    _Alpha = _Alpha_Save = 0;
    _Speed = _SpeedAlpha = 0;
    _Move = _MoveAlpha = 0;

    routineG.start(callback(this, &diffrentiel::routine_gauche));
    routineD.start(callback(this, &diffrentiel::routine_droite));
}

void diffrentiel::run()
{
    StepperG->run();
    StepperD->run();
}

void diffrentiel::stop()
{
    StepperG->stop();
    StepperD->stop();
}

void diffrentiel::setPosition(int positionX, int positionY, int Alpha)
{
    ScopedLock<Mutex> lock(mutexData);
    _positionX = positionX;
    _positionY = positionY;
    _Alpha = Alpha;
}

void diffrentiel::setPositionZero()
{
    StepperG->setPositionZero();
    StepperD->setPositionZero();
}

void diffrentiel::goesTo(int positionX, int positionY, int Alpha)
{
    float dx = positionX - _positionX;
    float dy = positionY - _positionY;
    float move = sqrt(dx * dx + dy * dy);
    float moveAlpha = Alpha - _Alpha;

    {
        ScopedLock<Mutex> lock(mutexData);
        _cibleposX = positionX;
        _cibleposY = positionY;
        _Move = move;
        _MoveAlpha = moveAlpha;
        _positionX_Save = _positionX;
        _positionY_Save = _positionY;
        _Alpha_Save = _Alpha;
        _Speed = (abs(_Move) / (abs(_Move) + abs(_MoveAlpha))) * SPEED;
        _SpeedAlpha = (abs(_MoveAlpha) / (abs(_Move) + abs(_MoveAlpha))) * (SPEED / 4.0f);
    }

    flags.set(0x1 | 0x2);
}

void diffrentiel::move(int Distance, int Alpha)
{
    float move = Distance;
    float moveAlpha = float(Alpha);

    {
        ScopedLock<Mutex> lock(mutexData);
        _Move = move;
        _MoveAlpha = moveAlpha;
        _positionX_Save = _positionX;
        _positionY_Save = _positionY;
        _Alpha_Save = _Alpha;
        _Speed = (abs(_Move) / (abs(_Move) + abs(_MoveAlpha))) * SPEED;
        _SpeedAlpha = (abs(_MoveAlpha) / (abs(_Move) + abs(_MoveAlpha))) * (SPEED / 4.0f);
    }

    flags.set(0x1 | 0x2);
}

void diffrentiel::synchroniser() {
    ScopedLock<Mutex> lock(syncMutex);
    readyCount++;
    if (readyCount == 2) {
        semG.release();
        semD.release();
        readyCount = 0;
    }
}

// ======================== Routines moteurs ========================= //

void diffrentiel::routine_gauche()
{
    while (true) {
        flags.wait_any(0x1); // attend ordre moteur gauche
        synchroniser();
        ScopedLock<Mutex> lock(mutexData);

        float speed = (_Speed - RADIUS * _SpeedAlpha) / KSTP;
        StepperG->setSpeed(speed);
        StepperG->setAcceleration(speed / ACC);
        StepperG->setDeceleration(speed / DEC);
        StepperG->move(int((-_Move - RADIUS * _SpeedAlpha) / KSTP));
    }
}

void diffrentiel::routine_droite()
{
    while (true) {
        flags.wait_any(0x2); // attend ordre moteur droit
        synchroniser();
        ScopedLock<Mutex> lock(mutexData);

        float speed = (_Speed + RADIUS * _SpeedAlpha) / KSTP;
        StepperD->setSpeed(speed);
        StepperD->setAcceleration(speed / ACC);
        StepperD->setDeceleration(speed / DEC);
        StepperD->move(int((_Move + RADIUS * _SpeedAlpha) / KSTP));
    }
}

// ======================== Getters ========================= //

float diffrentiel::getSpeedG()      { return StepperG->getSpeed(); }
float diffrentiel::getSpeedD()      { return StepperD->getSpeed(); }
int   diffrentiel::getPosG()        { return StepperG->getPosition(); }
int   diffrentiel::getPosD()        { return StepperD->getPosition(); }
int   diffrentiel::getStepG()       { return StepperG->getStep(); }
int   diffrentiel::getStepD()       { return StepperD->getStep(); }

float diffrentiel::getPositionX()   { return _positionX; }
float diffrentiel::getPositionY()   { return _positionY; }
float diffrentiel::getPosCibleX()   { return _cibleposX; }
float diffrentiel::getPosCibleY()   { return _cibleposY; }
float diffrentiel::getAlpha()       { return _Alpha; }
float diffrentiel::getSpeed()       { return _Speed; }
float diffrentiel::getSpeedAlpha()  { return _SpeedAlpha; }

bool diffrentiel::stopped()
{
    return StepperG->stopped() && StepperD->stopped();
}

bool diffrentiel::PosCibleDone()
{
    return StepperG->getPosCibleDone() && StepperD->getPosCibleDone();
}
