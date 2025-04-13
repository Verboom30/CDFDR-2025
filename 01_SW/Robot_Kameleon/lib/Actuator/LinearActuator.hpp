#ifndef LINEARACTUATOR_H
#define LINEARACTUATOR_H

#include "mbed.h"
#include "pinout.hpp"
#include "Stepper.hpp"
#include "main_pck.hpp"
#include <string.h>

#define ACC_ACT   4.0
#define DEC_ACT   4.0
#define SPEED_ACT 200.0 // max 50000 Mstepper 16 3200Ma
#define MSTEP_ACT 16


class LinearActuator
{
  public:
    LinearActuator(PinName step, PinName dir,PinName SW_up, PinName SW_down);
    Stepper* StepperAct;
    Thread routine;
    void routine_Actuator(void);

    bool goUp(void);
    bool goDown(void);
    bool InitLinearActuator(void);

    void  setPosition(int position);
    void  setPositionZero(void);
    bool  stopped(void);
    //bool  PosCibleDone(void);

    void  move(int position); 
    void  goesTo(int position);
    void  stop(void);
    void  run(void);

    float getSpeed(void);
    int getPos(void);
    int getStep(void);

  
  private:
    string _Cmd;
    float _position;
  protected:
    
};

#endif