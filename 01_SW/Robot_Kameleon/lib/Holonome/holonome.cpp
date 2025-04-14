#include "Holonome.hpp"
//***********************************/************************************
//                         Constructors                                 //
//***********************************/************************************
Holonome::Holonome()
{

    //SWSerialHolonome =new SerialTMC(Uart_TX_pin, Uart_RX_pin);
    // StepperA_thread.start(callback(this, &Holonome::routine_stepperA));
    // StepperB_thread.start(callback(this, &Holonome::routine_stepperB));
    // StepperC_thread.start(callback(this, &Holonome::routine_stepperC));
    // StepperA = new TMC2209Stepper(A_step_pin, A_dir_pin, SEL_UART_0, SWSerial, RS, A_Slave_Addr);
    // StepperB = new TMC2209Stepper(B_step_pin, B_dir_pin, SEL_UART_0, SWSerial, RS, B_Slave_Addr);
    // StepperC = new TMC2209Stepper(C_step_pin, C_dir_pin, SEL_UART_0, SWSerial, RS, C_Slave_Addr);
    // this->StepperA =StepperA;
    // this->StepperB =StepperB;
    // this->StepperC =StepperC;
  
    routine.start(callback(this, &Holonome::routine_holonome));

    _AckStpA =false;
    _AckStpB =false;
    _AckStpC =false;
    _Cmd ="";
    _Alpha = 0;
    _positionY  =0;
    _positionX  =0;
    _positionY_Save  =0;
    _positionX_Save  =0;
    _cibleposX       =0;
    _cibleposY       =0;
    _Alpha_Save      =0;
    _SpeedX     =0; 
    _SpeedY     =0;
    _SpeedAlpha =0;
    _MovepositionY =0;
    _MovepositionX =0;
    _MoveAlpha=0;

   
}

//***********************************/************************************
//                                Get Set                               //
//***********************************/************************************

float Holonome::getSpeedA(void)
{
    return StepperA->getSpeed();
}

float Holonome::getSpeedB(void)
{
    return StepperB->getSpeed();
}

float Holonome::getSpeedC(void)
{
    return StepperC->getSpeed();
}


int Holonome::getPosA(void)
{
    return StepperA->getPosition();
}

int Holonome::getPosB(void)
{
    return StepperB->getPosition();
}

int Holonome::getPosC(void)
{
    return StepperC->getPosition();
}

int Holonome::getStepA(void)
{
    return StepperA->getStep();
}

int Holonome::getStepB(void)
{
    return StepperB->getStep();
}

int Holonome::getStepC(void)
{
    return StepperC->getStep();
}

void Holonome::setPosition(int positionX, int positionY, int Alpha)
{
    _MovepositionX =-positionX;
    _MovepositionY =positionY;
    _MoveAlpha =Alpha;
    _Alpha_Save =0;
    _positionX_Save =0;
    _positionY_Save =0;
    _Cmd ="SET";
   
}

void Holonome::setPositionZero(void)
{
    _Cmd ="RST";
    while(!waitAck());
}

float Holonome::getPositionX(void)
{
    return -_positionX;
}

float Holonome::getPositionY(void)
{
    return _positionY;
}

float Holonome::getPosCibleX(void)
{
    return _cibleposX;
}

float Holonome::getPosCibleY(void)
{
    return _cibleposY;
}

float Holonome::getAlpha(void)
{
    return _Alpha;
}

float Holonome::getSpeedX(void)
{
    return _SpeedX;
}

float Holonome::getSpeedY(void)
{
    return _SpeedY;
}
float Holonome::getSpeedAlpha(void)
{
    return _SpeedAlpha;
}

bool Holonome::stopped(void)
{
    return (StepperA->stopped() == true and StepperB->stopped()== true and StepperC->stopped() == true) ? true : false;
}

bool Holonome::PosCibleDone(void)
{
    return (StepperA->getPosCibleDone() == true and StepperB->getPosCibleDone()== true and StepperC->getPosCibleDone() == true) ? true : false;
}

bool Holonome::waitAck(void)
{
    if(_AckStpA ==true and _AckStpB ==true and _AckStpC ==true ){
      
        _Cmd ="ACK";
        return 0;
    }else if(_AckStpA ==false and _AckStpB ==false and _AckStpC ==false and _Cmd =="ACK"){
        return 1;
    }else{
        return 0;
    }
}

//***********************************/************************************
//                             Public Methods                           //
//***********************************/************************************

void Holonome::run(void)
{
     StepperA->run();    
     StepperB->run();    
     StepperC->run();       
}

void Holonome::stop(void)
{   
    _Cmd = "STOP";
     
}

void Holonome::goesTo(int positionX, int positionY, int Alpha)
{
    _cibleposX      = -positionX;
    _cibleposY      = positionY;
    _MovepositionX  = float(-positionX -_positionX)        ; 
    _MovepositionY  = float(positionY -_positionY)        ; 
    _MoveAlpha      = float(Alpha -_Alpha)          ; 
    _positionX_Save = _positionX;
    _positionY_Save = _positionY;
    _Alpha_Save     = _Alpha;
     setPositionZero();
    _SpeedX = (float((_MovepositionX))/(abs(_MovepositionX)+abs(_MovepositionY)+abs(_MoveAlpha)))*SPEED;
    _SpeedY = (float((_MovepositionY))/(abs(_MovepositionX)+abs(_MovepositionY)+abs(_MoveAlpha)))*SPEED;
    _SpeedAlpha = (float((_MoveAlpha))/(abs(_MovepositionX)+abs(_MovepositionY)+abs(_MoveAlpha)))*float(2*SPEED/RADIUS);
    _Cmd = "GOTO";
}

void Holonome::move(int positionX, int positionY, int Alpha)
{
    _cibleposX      = -positionX;
    _cibleposY      = positionY;
    _MovepositionX  = -positionX         ; 
    _MovepositionY  = positionY         ; 
    _MoveAlpha      = Alpha; 
    _positionX_Save = _positionX;
    _positionY_Save = _positionY;
    _Alpha_Save     = _Alpha;
    setPositionZero();
    _SpeedX = (float((_MovepositionX))/(abs(_MovepositionX)+abs(_MovepositionY)+abs(_MoveAlpha)))*SPEED;
    _SpeedY = (float((_MovepositionY))/(abs(_MovepositionX)+abs(_MovepositionY)+abs(_MoveAlpha)))*SPEED;
    _SpeedAlpha = (float(_MoveAlpha)/(abs(_MovepositionX)+abs(_MovepositionY)+abs(_MoveAlpha)))*float(2*SPEED/RADIUS);
    _Cmd = "MOVE";
   
}


//***********************************/************************************
//                          Protected Methods                           //
//***********************************/************************************
// void Holonome::routine_stepperA(void)
// {
//     while (1)
//     {   
//         //printf("%s _AckStpA =%d\n",_Cmd.data(),_AckStpA);
//         if ((_Cmd == "GOTO" or _Cmd == "MOVE") and _AckStpA ==false){    
//             StepperA->setSpeed(((-RADIUS*_SpeedAlpha) - cos((PI/180.0)*_Alpha)*_SpeedX + sin((PI/180.0)*_Alpha)*_SpeedY)/KSTP);
//             StepperA->setAcceleration(getSpeedA()/ACC);
//             StepperA->setDeceleration(getSpeedA()/DEC); 
//             StepperA->move(int(((-RADIUS*_MoveAlpha*(PI/180.0)) - cos((PI/180.0)*_Alpha)*_MovepositionX + sin((PI/180.0)*_Alpha)*_MovepositionY)/KSTP));
//             _AckStpA = true;

//         }else if (_Cmd == "STOP" and _AckStpA ==false){
//             StepperA->stop();
//             _AckStpA = true;

//         }else if (_Cmd == "SET" and _AckStpA ==false){
//              StepperA->setPosition(int(((-RADIUS*_MoveAlpha*(PI/180.0)) - cos((PI/180.0)*_MoveAlpha)*_MovepositionX + sin((PI/180.0)*_MoveAlpha)*_MovepositionY)/KSTP));
//              _AckStpA = true;

//         }else if (_Cmd == "RST" and _AckStpA ==false){
//             StepperA->setPositionZero();
//             _AckStpA = true;
//         }else if (_Cmd == "ACK"){
//             _AckStpA = false;
//         }
//     }
// }

// void Holonome::routine_stepperB(void)
// {
//     while (1)
//     {
//        if ((_Cmd == "GOTO" or _Cmd == "MOVE") and _AckStpB ==false){   
//             StepperB->setSpeed(((-RADIUS*_SpeedAlpha) + cos((PI/180.0)*(THETA+_Alpha))*_SpeedX - sin((PI/180.0)*(THETA+_Alpha))*_SpeedY)/KSTP); 
//             StepperB->setAcceleration(getSpeedB()/ACC);
//             StepperB->setDeceleration(getSpeedB()/DEC); 
//             StepperB->move(int(((-RADIUS*_MoveAlpha*(PI/180.0)) + cos((PI/180.0)*(THETA+_Alpha))*_MovepositionX - sin((PI/180.0)*(THETA+_Alpha))*_MovepositionY)/KSTP)); 
//             _AckStpB = true;

//         }else if (_Cmd == "STOP" and _AckStpB ==false){
//             StepperB->stop();
//             _AckStpB = true;
       
//          }else if (_Cmd == "SET" and _AckStpB ==false){
            
//             StepperB->setPosition(int(((-RADIUS*_MoveAlpha*(PI/180.0)) + cos((PI/180.0)*(THETA+_MoveAlpha))*_MovepositionX - sin((PI/180.0)*(THETA+_MoveAlpha))*_MovepositionY)/KSTP));
//             _AckStpB = true;  

//          }else if (_Cmd == "RST" and _AckStpB ==false){
//             StepperB->setPositionZero();
//             _AckStpB = true;
//         }else if (_Cmd == "ACK"){
//             _AckStpB = false;
//         }  
//     }
// }

// void Holonome::routine_stepperC(void)
// {
//     while (1)
//     {
//         if ((_Cmd == "GOTO" or _Cmd == "MOVE")  and _AckStpC == false){  
//             StepperC->setSpeed(((-RADIUS*_SpeedAlpha) + cos((PI/180.0)*(THETA-_Alpha))*_SpeedX + sin((PI/180.0)*(THETA-_Alpha))*_SpeedY)/KSTP); 
//             StepperC->setAcceleration(getSpeedC()/ACC);
//             StepperC->setDeceleration(getSpeedC()/DEC); 
//             StepperC->move(int(((-RADIUS*_MoveAlpha*(PI/180.0)) + cos((PI/180.0)*(THETA-_Alpha))*_MovepositionX + sin((PI/180.0)*(THETA-_Alpha))*_MovepositionY)/KSTP)); 
//             _AckStpC = true;

//         }else if (_Cmd == "STOP" and _AckStpC ==false){
//             StepperC->stop();
//             _AckStpC = true;

//         }else if (_Cmd == "SET" and _AckStpC ==false){
//                 StepperC->setPosition(int(((-RADIUS*_MoveAlpha*(PI/180.0)) + cos((PI/180.0)*(THETA-_MoveAlpha))*_MovepositionX + sin((PI/180.0)*(THETA-_MoveAlpha))*_MovepositionY)/KSTP));
//             _AckStpC = true; 

//             }else if (_Cmd == "RST" and _AckStpC ==false){
//             StepperC->setPositionZero();
//             _AckStpC = true;

//         }else if (_Cmd == "ACK"){
//             _AckStpC = false;
//         }
//     }   
// }

void Holonome::routine_holonome(void)
{
    
    while (1)
    {
        if ((_Cmd == "GOTO" or _Cmd == "MOVE") and _AckStpA ==false){    
            StepperA->setSpeed(((-RADIUS*_SpeedAlpha) - cos((PI/180.0)*_Alpha)*_SpeedX + sin((PI/180.0)*_Alpha)*_SpeedY)/KSTP);
            StepperA->setAcceleration(getSpeedA()/ACC);
            StepperA->setDeceleration(getSpeedA()/DEC); 
            StepperA->move(int(((-RADIUS*_MoveAlpha*(PI/180.0)) - cos((PI/180.0)*_Alpha)*_MovepositionX + sin((PI/180.0)*_Alpha)*_MovepositionY)/KSTP));
            _AckStpA = true;

        }else if (_Cmd == "STOP"){
            StepperA->stop();
            _AckStpA = true;

        }else if (_Cmd == "SET" and _AckStpA ==false){
             StepperA->setPosition(int(((-RADIUS*_MoveAlpha*(PI/180.0)) - cos((PI/180.0)*_MoveAlpha)*_MovepositionX + sin((PI/180.0)*_MoveAlpha)*_MovepositionY)/KSTP));
             _AckStpA = true;

        }else if (_Cmd == "RST" and _AckStpA ==false){
            StepperA->setPositionZero();
            _AckStpA = true;
        }else if (_Cmd == "ACK"){
            _AckStpA = false;
        }
       
        if ((_Cmd == "GOTO" or _Cmd == "MOVE") and _AckStpB ==false){   
            StepperB->setSpeed(((-RADIUS*_SpeedAlpha) + cos((PI/180.0)*(THETA+_Alpha))*_SpeedX - sin((PI/180.0)*(THETA+_Alpha))*_SpeedY)/KSTP); 
            StepperB->setAcceleration(getSpeedB()/ACC);
            StepperB->setDeceleration(getSpeedB()/DEC); 
            StepperB->move(int(((-RADIUS*_MoveAlpha*(PI/180.0)) + cos((PI/180.0)*(THETA+_Alpha))*_MovepositionX - sin((PI/180.0)*(THETA+_Alpha))*_MovepositionY)/KSTP)); 
            _AckStpB = true;

        }else if (_Cmd == "STOP"){
            StepperB->stop();
            _AckStpB = true;
       
         }else if (_Cmd == "SET" and _AckStpB ==false){
            
            StepperB->setPosition(int(((-RADIUS*_MoveAlpha*(PI/180.0)) + cos((PI/180.0)*(THETA+_MoveAlpha))*_MovepositionX - sin((PI/180.0)*(THETA+_MoveAlpha))*_MovepositionY)/KSTP));
            _AckStpB = true;  

         }else if (_Cmd == "RST" and _AckStpB ==false){
            StepperB->setPositionZero();
            _AckStpB = true;
        }else if (_Cmd == "ACK"){
            _AckStpB = false;
        } 

        if ((_Cmd == "GOTO" or _Cmd == "MOVE")  and _AckStpC == false){  
            StepperC->setSpeed(((-RADIUS*_SpeedAlpha) + cos((PI/180.0)*(THETA-_Alpha))*_SpeedX + sin((PI/180.0)*(THETA-_Alpha))*_SpeedY)/KSTP); 
            StepperC->setAcceleration(getSpeedC()/ACC);
            StepperC->setDeceleration(getSpeedC()/DEC); 
            StepperC->move(int(((-RADIUS*_MoveAlpha*(PI/180.0)) + cos((PI/180.0)*(THETA-_Alpha))*_MovepositionX + sin((PI/180.0)*(THETA-_Alpha))*_MovepositionY)/KSTP)); 
            _AckStpC = true;

        }else if (_Cmd == "STOP"){
            StepperC->stop();
            _AckStpC = true;

        }else if (_Cmd == "SET" and _AckStpC ==false){
             StepperC->setPosition(int(((-RADIUS*_MoveAlpha*(PI/180.0)) + cos((PI/180.0)*(THETA-_MoveAlpha))*_MovepositionX + sin((PI/180.0)*(THETA-_MoveAlpha))*_MovepositionY)/KSTP));
            _AckStpC = true; 

         }else if (_Cmd == "RST" and _AckStpC ==false){
            StepperC->setPositionZero();
            _AckStpC = true;

        }else if (_Cmd == "ACK"){
            _AckStpC = false;
        }
            _Alpha     =  _Alpha_Save+ (((-1.0/(3.0*RADIUS))*StepperB->getPosition()) + ((-1.0/(3.0*RADIUS))*StepperA->getPosition()) + ((-1.0/(3.0*RADIUS))*StepperC->getPosition()))*KSTP/(PI/180.0);
            _positionX = _positionX_Save+ ((-cos((PI/180.0)*_Alpha)/6.0)*StepperA->getPosition() + ((cos((PI/180.0)*_Alpha)-sqrt(3)*sin((PI/180.0)*_Alpha))/12.0)*StepperB->getPosition() + ((sqrt(3)*sin((PI/180.0)*_Alpha)+cos((PI/180.0)*_Alpha))/12.0)*StepperC->getPosition())*KSTP*4;
            _positionY = _positionY_Save+ ((sin((PI/180.0)*_Alpha)/6.0)*StepperA->getPosition() - ((sin((PI/180.0)*_Alpha)+sqrt(3)*cos((PI/180.0)*_Alpha))/12.0)*StepperB->getPosition() + ((sqrt(3)*cos((PI/180.0)*_Alpha)-sin((PI/180.0)*_Alpha))/12.0)*StepperC->getPosition())*KSTP*4;

    }
}