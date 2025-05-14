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
    threadOdometrie.start(callback(this, &diffrentiel::routine_odometrie));
    setPositionZero();
    resetPosition();
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

    _positionX_Save = positionX;
    _positionY_Save = positionY;
    _Alpha_Save = Alpha;

    lastPosG = -StepperG->getPosition();
    lastPosD = StepperD->getPosition();
}

void diffrentiel::setPositionZero()
{
    StepperG->setPositionZero();
    StepperD->setPositionZero();

}

void diffrentiel::resetPosition()
{
    ScopedLock<Mutex> lock(mutexData);
    _positionX = 0;
    _positionY = 0;
    _Alpha = 0;
    _positionX_Save = 0;
    _positionY_Save = 0;
    _Alpha_Save = 0;
    lastPosG = -StepperG->getPosition();
    lastPosD = StepperD->getPosition();
}

void diffrentiel::goesTo(int positionX, int positionY, int Alpha)
{
    updatePosition();

    float dx = positionX - _positionX;
    float dy = positionY - _positionY;
    float move = sqrt(dx * dx + dy * dy);

    float targetAlpha = ((180.0f / M_PI) * atan2(dx, dy)); 
    float moveAlpha = targetAlpha - _Alpha;

    // Gérer la différence d'angle pour éviter les rotations inutiles
    // L'angle est borné entre -180° et 180°
    if (moveAlpha > 180) moveAlpha -= 360;
    if (moveAlpha < -180) moveAlpha += 360;

    // Si la différence d'angle est proche de 180° ou -180°, on inverse le mouvement
    if (std::abs(moveAlpha) > 90) {
        moveAlpha -= 180;
        move = -move; 
        if (moveAlpha > 180) moveAlpha -= 360;
        if (moveAlpha < -180) moveAlpha += 360;
    }
   
   
    
    // 1. Rotation vers direction
    diffrentiel::move(0,moveAlpha);
    do
    {
      ThisThread::sleep_for(10ms);
    } while (!diffrentiel::stopped());
    
    // 2. Translation
    diffrentiel::move(move,0);
    do
    {
      ThisThread::sleep_for(10ms);
    } while (!diffrentiel::stopped());

    // 3. Rotation finale vers Alpha
    float finalAlpha = Alpha - targetAlpha;  
    if (finalAlpha > 180) finalAlpha -= 360;
    if (finalAlpha < -180) finalAlpha += 360;

    if (std::abs(finalAlpha) > 90) {
        finalAlpha -= 180;
        if (finalAlpha > 180) finalAlpha -= 360;
        if (finalAlpha < -180) finalAlpha += 360;
    }

    diffrentiel::move(0, finalAlpha);
    do
    {
        ThisThread::sleep_for(10ms);
    } while (!diffrentiel::stopped());

    setPosition(positionX,positionY,Alpha);

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
        _SpeedAlpha = (abs(_MoveAlpha) / (abs(_Move) + abs(_MoveAlpha))) * (SPEED * 2.0f);
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

        float speed = (_Speed + RADIUS * _SpeedAlpha * (M_PI/180.0f)) / KSTP;
        StepperG->setSpeed(speed);
        StepperG->setAcceleration(speed / ACC);
        StepperG->setDeceleration(speed / DEC);
        StepperG->move(-int((_Move + RADIUS * _MoveAlpha * (M_PI/180.0f)) / KSTP));
    }
}

void diffrentiel::routine_droite()
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

void diffrentiel::routine_odometrie()
{
    while (true) {
        updatePosition();
        ThisThread::sleep_for(1ms);
    }
}

void diffrentiel::updatePosition()
{
    ScopedLock<Mutex> lock(mutexData);

    int posG = -StepperG->getPosition();
    int posD = StepperD->getPosition();

    int deltaG = posG - lastPosG;
    int deltaD = posD - lastPosD;

    lastPosG = posG;
    lastPosD = posD;

    float dG = deltaG * KSTP;
    float dD = deltaD * KSTP;

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

float diffrentiel::getSpeedG()      { return StepperG->getSpeed(); }
float diffrentiel::getSpeedD()      { return StepperD->getSpeed(); }
int   diffrentiel::getPosG()        { return -StepperG->getPosition(); }
int   diffrentiel::getPosD()        { return StepperD->getPosition(); }
int   diffrentiel::getStepG()       { return StepperG->getStep(); }
int   diffrentiel::getStepD()       { return StepperD->getStep(); }

float diffrentiel::getPositionX()   { ScopedLock<Mutex> lock(mutexData); return _positionX; }
float diffrentiel::getPositionY()   { ScopedLock<Mutex> lock(mutexData); return _positionY; }
float diffrentiel::getPosCibleX()   { ScopedLock<Mutex> lock(mutexData); return _cibleposX; }
float diffrentiel::getPosCibleY()   { ScopedLock<Mutex> lock(mutexData); return _cibleposY; }
float diffrentiel::getAlpha()       { ScopedLock<Mutex> lock(mutexData); return _Alpha; }
float diffrentiel::getSpeed()       { ScopedLock<Mutex> lock(mutexData); return _Speed; }
float diffrentiel::getSpeedAlpha()  { ScopedLock<Mutex> lock(mutexData); return _SpeedAlpha; }


bool diffrentiel::stopped()
{
    return StepperG->stopped() && StepperD->stopped();
}

bool diffrentiel::PosCibleDone()
{
    return StepperG->getPosCibleDone() && StepperD->getPosCibleDone();
}
