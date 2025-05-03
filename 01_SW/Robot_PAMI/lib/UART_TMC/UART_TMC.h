#ifndef __UART_TMC_H_
#define __UART_TMC_H_
#include "TMCStepper.h"

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200
#define RPM 50

#define RMSCURRENT 3000       // RMS current of Stepper Coil in mA
#define MSTEP   16
#define TOFF 5               // Enables driver in software - 3, 5
#define EN_SPREADCYCLE false // Toggle spreadCycle on TMC2208/2209/2224: default false, true: much faster!!!!
#define PWM_AUTOSCALE true   // Needed for stealthChop


class Uart_TMC : public TMC2209Stepper
{
    public:
    Uart_TMC(uint16_t SW_RX_pin, uint16_t SW_TX_pin, float RS, uint8_t addr): TMC2209Stepper(SW_RX_pin, SW_TX_pin, RS, addr) {}
    bool setup_stepper();
};

#endif // __TMC2209_H_