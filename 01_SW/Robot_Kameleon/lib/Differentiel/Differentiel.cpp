#include "differentiel.hpp"
//***********************************/************************************
//                         Constructors                                 //
//***********************************/************************************
diffrentiel::diffrentiel()
{
    routine.start(callback(this, &diffrentiel::routine_diffrentiel));

    _AckStpG =false;
    _AckStpD =false;
    _Cmd ="";
    _Alpha = 0;
    _positionY  =0;
    _positionX  =0;
    _positionY_Save  =0;
    _positionX_Save  =0;
    _cibleposX       =0;
    _cibleposY       =0;
    _Alpha_Save      =0;
    _Speed     =0; 
    _SpeedAlpha =0;
    _Move=0;
    _MoveAlpha=0;

   
}

//***********************************/************************************
//                                Get Set                               //
//***********************************/************************************

float diffrentiel::getSpeedG(void)
{
    return StepperG->getSpeed();
}

float diffrentiel::getSpeedD(void)
{
    return StepperD->getSpeed();
}

int diffrentiel::getPosG(void)
{
    return StepperG->getPosition();
}

int diffrentiel::getPosD(void)
{
    return StepperD->getPosition();
}

int diffrentiel::getStepG(void)
{
    return StepperG->getStep();
}

int diffrentiel::getStepD(void)
{
    return StepperD->getStep();
}

void diffrentiel::setPosition(int positionX, int positionY, int Alpha)
{
    _positionX = positionX;
    _positionY = positionY;
    _MoveAlpha = Alpha;
    _Alpha_Save = 0;
    _positionX_Save = 0;
    _positionY_Save = 0;
   
}

void diffrentiel::setPositionZero(void)
{
    _Cmd ="RST";
    while(!waitAck());
}

float diffrentiel::getPositionX(void)
{
    return -_positionX;
}

float diffrentiel::getPositionY(void)
{
    return _positionY;
}

float diffrentiel::getPosCibleX(void)
{
    return _cibleposX;
}

float diffrentiel::getPosCibleY(void)
{
    return _cibleposY;
}

float diffrentiel::getAlpha(void)
{
    return _Alpha;
}

float diffrentiel::getSpeed(void)
{
    return _Speed;
}

float diffrentiel::getSpeedAlpha(void)
{
    return _SpeedAlpha;
}

bool diffrentiel::stopped(void)
{
    return (StepperG->stopped() == true and StepperD->stopped()== true) ? true : false;
}

bool diffrentiel::PosCibleDone(void)
{
    return (StepperG->getPosCibleDone() == true and StepperD->getPosCibleDone()== true) ? true : false;
}

bool diffrentiel::waitAck(void)
{
    if(_AckStpG ==true and _AckStpD ==true){
      
        _Cmd ="ACK";
        return 0;
    }else if(_AckStpG ==false and _AckStpD ==false and _Cmd =="ACK"){
        return 1;
    }else{
        return 0;
    }
}

//***********************************/************************************
//                             Public Methods                           //
//***********************************/************************************

void diffrentiel::run(void)
{
     StepperG->run();    
     StepperD->run();    
}

void diffrentiel::stop(void)
{   
    _Cmd = "STOP";
     
}

void diffrentiel::goesTo(int positionX, int positionY, int Alpha)
{
    _cibleposX      = positionX;
    _cibleposY      = positionY;
    _Move = sqrt(pow(float(_cibleposX-_positionX),2.0)+pow(float(_cibleposY-_positionY),2.0));
    _MoveAlpha      = float(Alpha -_Alpha)          ; 
    _positionX_Save = _positionX;
    _positionY_Save = _positionY;
    _Alpha_Save     = _Alpha;
     setPositionZero();
    _Speed =  (float(abs(_Move))/(abs(_Move)+abs(_MoveAlpha)))*SPEED;
    _SpeedAlpha =  (float(abs(_MoveAlpha))/(abs(_Move)+abs(_MoveAlpha)))*(SPEED/4.0);
    _Cmd = "GOTO";
}

void diffrentiel::move(int positionX, int positionY, int Alpha)
{
    _cibleposX      = positionX;
    _cibleposY      = positionY;
    _Move = positionY;
    _MoveAlpha      = float(Alpha); 
    _positionX_Save = _positionX;
    _positionY_Save = _positionY;
    _Alpha_Save     = _Alpha;
     setPositionZero();
    _Speed =  (float(abs(_Move))/(abs(_Move)+abs(_MoveAlpha)))*SPEED;
    _SpeedAlpha =  (float(abs(_MoveAlpha))/(abs(_Move)+abs(_MoveAlpha)))*(SPEED/4.0);
    _Cmd = "MOVE";
   
}

void diffrentiel::routine_diffrentiel(void)
{
    
    while (1)
    {
        if ((_Cmd == "GOTO" or _Cmd == "MOVE") and _AckStpG ==false){    
            StepperG->setSpeed((_Speed - (RADIUS*_SpeedAlpha))/KSTP);
            StepperG->setAcceleration(float(getSpeedG()/ACC));
            StepperG->setDeceleration(float(getSpeedG()/DEC)); 
            StepperG->move(int((-_Move -(RADIUS*_SpeedAlpha))/KSTP));
            _AckStpG = true;


        }else if (_Cmd == "STOP"){
            StepperG->stop();
            _AckStpG = true;

        }else if (_Cmd == "RST" and _AckStpG ==false){
            StepperG->setPositionZero();
            _AckStpG = true;
        }else if (_Cmd == "ACK"){
            _AckStpG = false;
        }
        
        if ((_Cmd == "GOTO" or _Cmd == "MOVE") and _AckStpD ==false){   
            StepperD->setSpeed((_Speed + (RADIUS*_SpeedAlpha))/KSTP);
            StepperD->setAcceleration(float(getSpeedD()/ACC));
            StepperD->setDeceleration(float(getSpeedD()/DEC)); 
            StepperD->move(int((_Move + (RADIUS*_SpeedAlpha))/KSTP));
            //StepperD->move(int(((-RADIUS*_MoveAlpha*(PI/180.0)) + cos((PI/180.0)*(THETA+_Alpha))*_MovepositionX - sin((PI/180.0)*(THETA+_Alpha))*_MovepositionY)/KSTP)); 
            _AckStpD = true;

        }else if (_Cmd == "STOP"){
            StepperD->stop();
            _AckStpD = true; 

         }else if (_Cmd == "RST" and _AckStpD ==false){
            StepperD->setPositionZero();
            _AckStpD = true;
        }else if (_Cmd == "ACK"){
            _AckStpD = false;
        } 
            //_Alpha     =  _Alpha_Save+ (((-1.0/(3.0*RADIUS))*StepperD->getPosition()) + ((-1.0/(3.0*RADIUS))*StepperG->getPosition()) + ((-1.0/(3.0*RADIUS))*StepperC->getPosition()))*KSTP/(PI/180.0);
            //_positionX = _positionX_Save+ ((-cos((PI/180.0)*_Alpha)/6.0)*StepperG->getPosition() + ((cos((PI/180.0)*_Alpha)-sqrt(3)*sin((PI/180.0)*_Alpha))/12.0)*StepperD->getPosition() + ((sqrt(3)*sin((PI/180.0)*_Alpha)+cos((PI/180.0)*_Alpha))/12.0)*StepperC->getPosition())*KSTP*4;
            //_positionY = _positionY_Save+ ((sin((PI/180.0)*_Alpha)/6.0)*StepperG->getPosition() - ((sin((PI/180.0)*_Alpha)+sqrt(3)*cos((PI/180.0)*_Alpha))/12.0)*StepperD->getPosition() + ((sqrt(3)*cos((PI/180.0)*_Alpha)-sin((PI/180.0)*_Alpha))/12.0)*StepperC->getPosition())*KSTP*4;

    }
}