#include "Differentiel.h"

differentiel::differentiel(AccelStepper* moteurGauche, AccelStepper* moteurDroit)
    : StepperG(moteurGauche), StepperD(moteurDroit)
{
    _positionX = _positionY = _Alpha = 0;
    _Speed = _SpeedAlpha = _Move = _MoveAlpha = 0;
    _deltaG = _deltaD = 0;
    lastPosG = lastPosD = 0;
    flagGauche = flagDroite = false;
    setPositionZero();
    resetPosition();
}

void differentiel::run() {
    StepperG->run();
    StepperD->run();
}

void differentiel::stop() {
    StepperG->stop();
    StepperD->stop();
}

void differentiel::move(int Distance, int Alpha) {
    _Move = Distance;
    _MoveAlpha = float(Alpha);

    float total = abs(_Move) + abs(_MoveAlpha);
    _Speed = (abs(_Move) / total) * SPEED;
    _SpeedAlpha = (abs(_MoveAlpha) / total) * (SPEED * 0.5f);

    flagGauche = true;
    flagDroite = true;
}

void differentiel::handleRoutineGauche() {
    if (!flagGauche) return;
    flagGauche = false;

    float speed = (_Speed + RADIUS * _SpeedAlpha * (M_PI / 180.0f)) / KSTP;
    StepperG->setMaxSpeed(MAX_SPEED);       
    StepperG->setSpeed(speed);
    StepperG->setAcceleration(speed / ACC);
    StepperG->move(int((_Move + RADIUS * _MoveAlpha * (M_PI / 180.0f)) / KSTP));
}

void differentiel::handleRoutineDroite() {
    if (!flagDroite) return;
    flagDroite = false;

    float speed = (_Speed - RADIUS * _SpeedAlpha * (M_PI / 180.0f)) / KSTP;
    StepperD->setMaxSpeed(MAX_SPEED);   
    StepperD->setSpeed(speed);
    StepperD->setAcceleration(speed / ACC);
    StepperD->move(int((_Move - RADIUS * _MoveAlpha * (M_PI / 180.0f)) / KSTP));
}

void differentiel::updatePosition() {
    int posG = StepperG->currentPosition();
    int posD = StepperD->currentPosition();

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

void differentiel::setPosition(int x, int y, int Alpha) {
    _positionX = x;
    _positionY = y;
    _Alpha = Alpha;
    lastPosG = StepperG->currentPosition();
    lastPosD = StepperD->currentPosition();
}

void differentiel::setPositionZero() {
    StepperG->currentPosition();
    StepperD->currentPosition();
}

void differentiel::resetPosition() {
    _positionX = _positionY = _Alpha = 0;
    lastPosD = StepperD->currentPosition();
}

float differentiel::getPositionX() { return _positionX; }
float differentiel::getPositionY() { return _positionY; }
float differentiel::getAlpha()     { return _Alpha; }
int   differentiel::getDeltaG()    { return _deltaG; }
int   differentiel::getDeltaD()    { return _deltaD; }
bool  differentiel::Running() {
    return StepperG->isRunning() && StepperD->isRunning();
}