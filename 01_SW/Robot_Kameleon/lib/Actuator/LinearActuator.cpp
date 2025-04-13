#include "LinearActuator.hpp"
//***********************************/************************************
//                         Constructors                                 //
//***********************************/************************************
LinearActuator::LinearActuator(PinName step, PinName dir,PinName SW_up, PinName SW_down)
{
    
    routine.start(callback(this, &LinearActuator::routine_Actuator));
    _Cmd ="";
    _position =0;
    Stepper* StepperAct = new Stepper(step, dir);
  
    
}

//***********************************/************************************
//                                Get Set                               //
//***********************************/************************************

float LinearActuator::getSpeed(void)
{
    return StepperAct->getSpeed();
}

int LinearActuator::getPos(void)
{
    return StepperAct->getPosition();
}

int LinearActuator::getStep(void)
{
    return StepperAct->getStep();
}

void LinearActuator::setPosition(int position)
{
    _position =position;
    _Cmd ="SET";
   
}
void LinearActuator::setPositionZero(void)
{
    _Cmd ="RST";
}

bool LinearActuator::stopped(void)
{
    return StepperAct->stopped();
}

void LinearActuator::run(void)
{
    StepperAct->run();    
}
void LinearActuator::stop(void)
{   
    _Cmd = "STOP";
     
}
void LinearActuator::goesTo(int position)
{
    _position = position;
    _Cmd = "GOTO";
}
void LinearActuator::move(int position)
{
    _position = position;
    _Cmd = "MOVE";
}


//***********************************/************************************
//                             Public Methods                           //
//***********************************/************************************

bool InitLinearActuator(void){


}
void LinearActuator::routine_Actuator(void)
{
    while(1)
    {
        if((_Cmd == "MOVE") and StepperAct->stopped()){
            StepperAct->setSpeed(SPEED_ACT);
            StepperAct->setAcceleration(ACC_ACT);
            StepperAct->setDeceleration(DEC_ACT);
            StepperAct->move(_position);
           
        }else if ((_Cmd == "GOTO")and StepperAct->stopped()){
            StepperAct->setSpeed(SPEED_ACT);
            StepperAct->setAcceleration(ACC_ACT);
            StepperAct->setDeceleration(DEC_ACT);
            StepperAct->goesTo(_position);
          
        }else if (_Cmd == "STOP"){
            StepperAct->stop();
           
        }else if (_Cmd == "SET" and StepperAct->stopped()){
            StepperAct->setPosition(_position);
            
        }else if (_Cmd == "RST" and StepperAct->stopped()){
            StepperAct->setPositionZero();
           
        }

        

    }
        
}