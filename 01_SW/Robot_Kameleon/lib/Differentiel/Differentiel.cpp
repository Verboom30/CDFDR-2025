#include "Differentiel.hpp"

differentiel::differentiel(Stepper* moteurGauche, Stepper* moteurDroit)
    : StepperG(moteurGauche), StepperD(moteurDroit), readyCount(0), semG(0), semD(0) 
{
    _positionX = _positionY = 0;
    _cibleposX = _cibleposY = 0;
    _Alpha = 0;
    _Speed = _SpeedAlpha = 0;
    _Move = _MoveAlpha = 0;
    _deltaG = _deltaD = 0;

    routineG.start(callback(this, &differentiel::routine_gauche));
    routineD.start(callback(this, &differentiel::routine_droite));
    threadOdometrie.start(callback(this, &differentiel::routine_odometrie));
    setPositionZero();
    resetPosition();
}

void differentiel::run()
{
    StepperG->run();
    StepperD->run();
}

void differentiel::stop()
{
    StepperG->stop();
    StepperD->stop();
}

void differentiel::setPosition(int positionX, int positionY, int Alpha)
{
     ScopedLock<Mutex> lock(mutexData);

    _positionX = positionX;
    _positionY = positionY;
    _Alpha = Alpha;

    lastPosG = -StepperG->getPosition();
    lastPosD = StepperD->getPosition();
}

void differentiel::setPositionZero()
{
    StepperG->setPositionZero();
    StepperD->setPositionZero();

}

void differentiel::resetPosition()
{
    ScopedLock<Mutex> lock(mutexData);
    _positionX = 0;
    _positionY = 0;
    _Alpha = 0;

    lastPosG = -StepperG->getPosition();
    lastPosD = StepperD->getPosition();
}

void differentiel::move(int Distance, int Alpha)
{
    float move = Distance;
    float moveAlpha = float(Alpha);

    {
        ScopedLock<Mutex> lock(mutexData);
        _Move = move;
        _MoveAlpha = moveAlpha;
        _Speed = (abs(_Move) / (abs(_Move) + abs(_MoveAlpha))) * SPEED;
        _SpeedAlpha = (abs(_MoveAlpha) / (abs(_Move) + abs(_MoveAlpha))) * (SPEED * 0.5f);
    }

    flags.set(0x1 | 0x2);
}

void differentiel::synchroniser() {
    ScopedLock<Mutex> lock(syncMutex);
    readyCount++;
    if (readyCount == 2) {  
        semG.release();
        semD.release();
        readyCount = 0;
    }
}

// ======================== Routines moteurs ========================= //

void differentiel::routine_gauche()
{
    while (true) {
        flags.wait_any(0x1); // attend ordre moteur gauche
        synchroniser();
        ScopedLock<Mutex> lock(mutexData);

        float speed = (_Speed + RADIUS * _SpeedAlpha * (M_PI/180.0f)) / KSTP;
        StepperG->setSpeed(speed);
        StepperG->setAcceleration(speed / ACC);
        StepperG->setDeceleration(speed / DEC);
        StepperG->move(-int((_Move + RADIUS * _MoveAlpha * (M_PI/180.0f)) / KSTP));
    }
}

void differentiel::routine_droite()
{
    while (true) {
        flags.wait_any(0x2); // attend ordre moteur droit
        synchroniser();
        ScopedLock<Mutex> lock(mutexData);

        float speed = (_Speed - RADIUS * _SpeedAlpha * (M_PI/180.0f)) / KSTP;
        StepperD->setSpeed(speed);
        StepperD->setAcceleration(speed / ACC);
        StepperD->setDeceleration(speed / DEC);
        StepperD->move(int((_Move - RADIUS * _MoveAlpha * (M_PI/180.0f)) / KSTP));
    }
}

void differentiel::routine_odometrie()
{
    while (true) {
        updatePosition();
        ThisThread::sleep_for(10ms);
    }
}

void differentiel::updatePosition()
{
    ScopedLock<Mutex> lock(mutexData);

    int posG = -StepperG->getPosition();
    int posD = StepperD->getPosition();

    _deltaG = posG - lastPosG;
    _deltaD = posD - lastPosD;

    lastPosG = posG;
    lastPosD = posD;

    float dG = _deltaG * KSTP;
    float dD = _deltaD * KSTP;

    float dC = (dG + dD) / 2.0f;
    float dAlpha = (dG - dD) / (2.0f * RADIUS); // radians

    float alpha_rad = _Alpha * (M_PI / 180.0f);
    _positionX += dC * sin(alpha_rad + dAlpha / 2.0f);
    _positionY += dC * cos(alpha_rad + dAlpha / 2.0f);

    alpha_rad += dAlpha;
    _Alpha = alpha_rad * (180.0f / M_PI);

    if (_Alpha > 180.0f) _Alpha -= 360.0f;
    if (_Alpha < -180.0f) _Alpha += 360.0f;
}

// ======================== Getters ========================= //

float differentiel::getSpeedG()      { return StepperG->getSpeed(); }
float differentiel::getSpeedD()      { return StepperD->getSpeed(); }
int   differentiel::getPosG()        { return -StepperG->getPosition(); }
int   differentiel::getPosD()        { return StepperD->getPosition(); }
int   differentiel::getStepG()       { return StepperG->getStep(); }
int   differentiel::getStepD()       { return StepperD->getStep(); }

float differentiel::getPositionX()   { ScopedLock<Mutex> lock(mutexData); return _positionX; }
float differentiel::getPositionY()   { ScopedLock<Mutex> lock(mutexData); return _positionY; }
float differentiel::getPosCibleX()   { ScopedLock<Mutex> lock(mutexData); return _cibleposX; }
float differentiel::getPosCibleY()   { ScopedLock<Mutex> lock(mutexData); return _cibleposY; }
float differentiel::getAlpha()       { ScopedLock<Mutex> lock(mutexData); return _Alpha; }
float differentiel::getSpeed()       { ScopedLock<Mutex> lock(mutexData); return _Speed; }
float differentiel::getSpeedAlpha()  { ScopedLock<Mutex> lock(mutexData); return _SpeedAlpha; }
int   differentiel::getDeltaG()        { ScopedLock<Mutex> lock(mutexData); return _deltaG; }
int   differentiel::getDeltaD()        { ScopedLock<Mutex> lock(mutexData); return _deltaD; }


bool differentiel::stopped()
{
    return StepperG->stopped() && StepperD->stopped();
}

bool differentiel::PosCibleDone()
{
    return StepperG->getPosCibleDone() && StepperD->getPosCibleDone();
}
