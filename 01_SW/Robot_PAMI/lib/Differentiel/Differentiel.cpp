#include "Differentiel.h"

MoveState moveState = IDLE;
GotoState gotoState = GOTO_IDLE;
static int Move = 0;
static float MoveAlpha = 0;
static float FinalAlpha = 0;



differentiel::differentiel(AccelStepper* moteurGauche, AccelStepper* moteurDroit)
    : StepperG(moteurGauche), StepperD(moteurDroit) {
    _positionX = _positionY = _Alpha = 0;
    _Speed = _SpeedAlpha = _Move = _MoveAlpha = 0;
    _deltaG = _deltaD = 0;
    lastPosG = lastPosD = 0;
    flagGauche = flagDroite = false;
 
    setPositionZero();
    resetPosition();
    // ThreadUpdatePosition.onRun(updateTask); // now using a valid static callback
    // ThreadUpdatePosition.setInterval(1);
    // controller->add(&ThreadUpdatePosition);
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
    _SpeedAlpha = (abs(_MoveAlpha) / total) * (SPEED * 0.01f);
    flagGauche = true;
    flagDroite = true;
}

void differentiel::handleRoutineGauche() {
    if (!flagGauche) return;
    flagGauche = false;
    float speed = (_Speed + RADIUS * _SpeedAlpha * (M_PI / 180.0f)) / KSTP;
    StepperG->setMaxSpeed(MAX_SPEED);
    StepperG->setSpeed(speed);
    StepperG->setAcceleration(ACC);
    StepperG->move(int((_Move + RADIUS * _MoveAlpha * (M_PI / 180.0f)) / KSTP));
}

void differentiel::handleRoutineDroite() {
    if (!flagDroite) return;
    flagDroite = false;
    float speed = (_Speed - RADIUS * _SpeedAlpha * (M_PI / 180.0f)) / KSTP;
    StepperD->setMaxSpeed(MAX_SPEED);
    StepperD->setSpeed(speed);
    StepperD->setAcceleration(ACC);
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
    float dAlpha = (dG - dD) / (2.0f * RADIUS);
    float alpha_rad = _Alpha * (M_PI / 180.0f);
    _positionX += dC * sin(alpha_rad + dAlpha / 2.0f);
    _positionY += dC * cos(alpha_rad + dAlpha / 2.0f);
    alpha_rad += dAlpha;
    _Alpha = alpha_rad * (180.0f / M_PI);
    if (_Alpha > 180.0f) _Alpha -= 360.0f;
    if (_Alpha < -180.0f) _Alpha += 360.0f;
}

void differentiel::setPosition(int x, int y, int Alpha) {
    updatePosition();
    _positionX = x;
    _positionY = y;
    _Alpha = Alpha;
    lastPosG = StepperG->currentPosition();
    lastPosD = StepperD->currentPosition();
}

void differentiel::setPositionZero() {
    StepperG->setCurrentPosition(0);
    StepperD->setCurrentPosition(0);
}

void differentiel::resetPosition() {
    updatePosition();
    _positionX = _positionY = _Alpha = 0;
    lastPosG = StepperG->currentPosition();
    lastPosD = StepperD->currentPosition();
}

float differentiel::getPositionX() { updatePosition(); return _positionX; }
float differentiel::getPositionY() { updatePosition(); return _positionY; }
float differentiel::getAlpha()     { updatePosition(); return _Alpha; }
int   differentiel::getDeltaG()    { return _deltaG; }
int   differentiel::getDeltaD()    { return _deltaD; }
bool  differentiel::Running()      { return StepperG->isRunning() || StepperD->isRunning(); }

bool Robotmoveto(differentiel& robot, int distance, int alpha, bool stop) {
    static bool setupDone = false;
    if (moveState == IDLE) {
        robot.move(distance, alpha);
        robot.handleRoutineGauche();
        robot.handleRoutineDroite();
        moveState = MOVING;
        setupDone = true;
    }
    if (!stop && moveState == MOVING) robot.run();
    if (!robot.Running()) {
        moveState = IDLE;
        setupDone = false;
        return true;
    }
    return false;
}

bool Robotgoto(differentiel& robot, int positionX, int positionY, int alpha, bool stop) {
    switch (gotoState) {
        case GOTO_IDLE: {
            //robot.updatePosition();
            float dx = positionX - robot.getPositionX();
            float dy = positionY - robot.getPositionY();
            if (abs(dx) < 0.1f) dx = 0.0f;
            if (abs(dy) < 0.1f) dy = 0.0f;
            Move = (int)sqrt(dx * dx + dy * dy);
            float targetAlpha = (180.0f / M_PI) * atan2(dy, dx);
            MoveAlpha = targetAlpha - robot.getAlpha();
            FinalAlpha = alpha - targetAlpha;
            if (MoveAlpha > 180) MoveAlpha -= 360;
            if (MoveAlpha < -180) MoveAlpha += 360;
            if (FinalAlpha > 180) FinalAlpha -= 360;
            if (FinalAlpha < -180) FinalAlpha += 360;
           
            gotoState = ROTATE_TO_TARGET;
            break;
        }
        case ROTATE_TO_TARGET:
            if (Robotmoveto(robot, 0, MoveAlpha, stop)) gotoState = FORWARD;
            break;
        case FORWARD:
            if (Robotmoveto(robot, Move, 0, stop)) gotoState = ROTATE_FINAL;
            break;
        case ROTATE_FINAL:
            if (Robotmoveto(robot, 0, FinalAlpha, stop)) {
                gotoState = GOTO_IDLE;
                return true;
            }
            break;
    }
    return false;
}
