#ifndef __KAMELEON_H__
#define __KAMELEON_H__

#include <inttypes.h>
#include "mbed.h"

#define ANGLE_TO_PULSE(angle) (int)(500.0+(100.0/9.0)*angle)

// Enums
enum Kameleon_mode {
    RUNNING,
    DEBUG
};

// Typedefs
typedef const PinName PinStepper[2]; // Couple Step / Dir Pin for stepper control
typedef const PinName PinServo;
typedef const PinName PinSWEoT[2]; // Bas / Haut
typedef const PinName PinSW;
typedef const PinName PinAct;
typedef const PinName PinLCD[6]; // RS / EN / D4 / D5 / D6 / D7
typedef const PinName PinUART[2]; // Tx / Rx
typedef float ServoPosition;
typedef const ServoPosition CstServoPosition;

namespace pinout {
    // Servomotors Pin list
    PinServo ExtLeftPince  = PB_6;
    PinServo CenLeftPince  = PC_8;
    PinServo CenRightPince = PC_9;
    PinServo ExtRightPince = PB_5;
    // Exterior Pince Rotation
    PinServo LeftPinceRotation = PD_13;
    PinServo RightPinceRotation = PD_14;
    // Hookers
    PinServo LeftHooker = PD_15;
    PinServo RightHooker = PE_5;

    // Roadwheels
    PinStepper LeftWheel = {PE_9, PF_13};
    PinStepper RightWheel = {PE_11, PF_14};
    // Pince Elevator
    PinStepper LeftPinceElevator = {PE_14, PB_1};
    PinStepper RightPinceElevator = {PB_8, PC_2};
    // Sucker Elevator
    PinStepper SuckerElevator = {PB_9, PF_4};

    // Drive Enable
    PinAct EnableDrive = PE_10;
    PinAct EnableElevator = PE_12;

    // Sucker
    PinAct Sucker = PE_5; // TODO: True Pin

    // End Of Travel Switches
    PinSWEoT LeftPinceElevatorEoT = {PB_13, PB_3};
    PinSWEoT RightPinceElevatorEoT = {PC_6, PA_15};
    PinSWEoT SuckerElevatorEoT = {PB_15, PC_7};

    // Misc Switches
    PinSW Tirette = PA_5;
    PinSW EmergencyStop = PF_12;
    PinSW FrontPanelToggleSW0 = PE_2;
    PinSW FrontPanelToggleSW1 = PA_0;
    PinSW FrontPanelToggleSW2 = PA_7;
    PinSW FrontPanelPushSW = PE_0;

    // LCD
    PinLCD LcdDisplay = {PC_10, PC_11, PC_12, PD_2, PG_2, PG_3};

    // LIDAR
    PinUART LidarUart = {PD_5, PD_6};
    PinAct LidarLed = PE_15;

    // TMC UART
    PinUART TMCUart = {PE_8, PE_7};
    PinAct TMCSel[3] = {PD_7, PD_4, PD_3};

    // Serial USB
    PinUART SerialUSB = {USBTX, USBRX};

    // Bluetooth UART
    PinUART SerialBT = {PG_9, PG_14};
};

namespace constraints {
    // Servomotor Open / close constraints
    int PinceOpenClose[4][2] = {{ANGLE_TO_PULSE(170), ANGLE_TO_PULSE(140)},
                                             {ANGLE_TO_PULSE(70), ANGLE_TO_PULSE(50)},
                                             {ANGLE_TO_PULSE(180), ANGLE_TO_PULSE(150)},
                                             {ANGLE_TO_PULSE(90), ANGLE_TO_PULSE(60)}};
    int PinceRotationDeployement[2] = {ANGLE_TO_PULSE(10),
                                                    ANGLE_TO_PULSE(185)};
    int HookersUpDown[2][2] = {{ANGLE_TO_PULSE(65), ANGLE_TO_PULSE(160)},
                                            {ANGLE_TO_PULSE(45), ANGLE_TO_PULSE(50)}};                                        
    
};

class Kameleon {
public:
    // Constructor
    Kameleon(Kameleon_mode mode);

    // Pinces
    void moveLeftExteriorPince(bool move);
    void moveRightExteriorPince(bool move);
    void moveLeftCenterPince(bool move);
    void moveRightCenterPince(bool move);
    void moveAllPinces(bool move);

private:

    // Pinces
    PwmOut* _ExtLeftPince = nullptr;
    PwmOut* _CenLeftPince = nullptr;
    PwmOut* _CenRightPince = nullptr;
    PwmOut* _ExtRightPince = nullptr;
    // Pinces Rotation
    PwmOut* _LeftPinceRotation = nullptr;
    PwmOut* _RightPinceRotation = nullptr;
    // Hooks
    PwmOut* _LeftHooker = nullptr;
    PwmOut* _RightHooker = nullptr;
};

#endif