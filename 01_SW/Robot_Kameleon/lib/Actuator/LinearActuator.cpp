#include "LinearActuator.hpp"
//***********************************/************************************
//                         Constructors                                 //
//***********************************/************************************
LinearActuator::LinearActuator(PinName step, PinName dir,PinName SW_up, PinName SW_down, bool reverve): _sw_up(SW_up), _sw_down(SW_down)
{
    StepperAct = new Stepper(step, dir);;
    _sw_up.mode(PullUp);
    _sw_down.mode(PullUp);
    if(reverve == true){
        _reverse =-1;
    }else{
        _reverse =1;
    }
   

}

//***********************************/************************************
//                                Get Set                               //
//***********************************/************************************



//***********************************/************************************
//                             Public Methods                           //
//***********************************/************************************
// bool LinearActuator::InitDir(void)
// {
//     do
//     {  
//         StepperAct->move(100);
//         while(!StepperAct->stopped());
//     } while (_sw_up !=0 and _sw_down !=0);
//     if(_sw_down !=1) reverse =-1;
//     else if (_sw_up !=1) reverse = 1;
//     return true;
// }

bool LinearActuator::goUp(void)
{
    do
    {  
        StepperAct->move(100*_reverse);
        while(!StepperAct->stopped());
    } while (_sw_up !=0);
    return true;
}

bool LinearActuator::goDown(void)
{
    do
    {  
        StepperAct->move(-100*_reverse);
        while(!StepperAct->stopped());
    } while (_sw_down !=0);
    return true;
}

void LinearActuator::InitLinearActuator(void){
    StepperAct->setSpeed(SPEED_ACT);
    StepperAct->setAcceleration(0);
    StepperAct->setDeceleration(0);
    //InitDir();
}

