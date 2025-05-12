#include "kameleon.hpp"

// Initialize all structures
Kameleon::Kameleon(Kameleon_mode mode) {
    // Pinces
    _ExtLeftPince = new PwmOut(pinout::ExtLeftPince);
    _CenLeftPince = new PwmOut(pinout::CenLeftPince);
    _CenRightPince = new PwmOut(pinout::CenRightPince);
    _ExtRightPince = new PwmOut(pinout::ExtRightPince);
    // Pinces Rotation
    _LeftPinceRotation = new PwmOut(pinout::LeftPinceRotation);
    _RightPinceRotation = new PwmOut(pinout::RightPinceRotation);
    // Hooks
    _LeftHooker = new PwmOut(pinout::LeftHooker);
    _RightHooker = new PwmOut(pinout::RightHooker);
}

// Pinces Actions
void Kameleon::moveLeftExteriorPince(bool move) {
    _ExtLeftPince->pulsewidth_us(constraints::PinceOpenClose[0][move ? 1 : 0]);
}
void Kameleon::moveRightExteriorPince(bool move) {
    _ExtRightPince->pulsewidth_us(constraints::PinceOpenClose[3][move ? 1 : 0]);
}
void Kameleon::moveLeftCenterPince(bool move) {
    _CenLeftPince->pulsewidth_us(constraints::PinceOpenClose[1][move ? 1 : 0]);
}
void Kameleon::moveRightCenterPince(bool move) {
    _CenRightPince->pulsewidth_us(constraints::PinceOpenClose[2][move ? 1 : 0]);
}
void Kameleon::moveAllPinces(bool move) {
    moveLeftExteriorPince(move);
    moveRightExteriorPince(move);
    moveLeftCenterPince(move);
    moveRightCenterPince(move);
}