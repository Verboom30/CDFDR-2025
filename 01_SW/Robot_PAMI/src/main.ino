#include "TMCStepper.h"
#include "BasicStepperDriver.h"
#include "pinout.h"

#define R_SENSE 0.11f // Match to your driver
                      // SilentStepStick series use 0.11
                      // UltiMachine Einsy and Archim2 boards use 0.2
                      // Panucatt BSD2660 uses 0.1
                      // Watterott TMC5160 uses 0.075

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200
#define RPM 50

#define RMSCURRENT 3000       // RMS current of Stepper Coil in mA
#define MSTEP   16
#define TOFF 5               // Enables driver in software - 3, 5
#define EN_SPREADCYCLE false // Toggle spreadCycle on TMC2208/2209/2224: default false, true: much faster!!!!
#define PWM_AUTOSCALE true   // Needed for stealthChop

TMC2209Stepper driver(TMC_UART_RX, TMC_UART_TX, R_SENSE, 0b00);

// 2-wire basic config, microstepping is hardwired on the driver
BasicStepperDriver stepper(MOTOR_STEPS, DIR_G, STEP_G);


void setup() {

  driver.beginSerial(115200);     // SW UART drivers
  Serial.begin(115200);
  stepper.begin(RPM, MSTEP);
  delay(1000);
  
  Serial.println("connecting to TMC-Module...");
  
  int tmc_version = driver.version();
  Serial.print("TMC-Version :");
  Serial.println(tmc_version,HEX);
   if (tmc_version != 0x21) {
           Serial.println("Wrong TMC-Version(not 0x21) or communication error!! STOPPING!!!");
           if (driver.CRCerror) {
               Serial.println("CRC-Error!!!");
           }
   }
  driver.begin();                // Initialize driver
  driver.toff(TOFF);                 // Enables driver in software
  driver.rms_current(RMSCURRENT);   // Set motor RMS current in mA / min 500 for 24V/speed:3000
  // 1110, 800
  // working: 800 12V/0,6Amax,  Speed up to 5200=4U/min
  driver.microsteps(MSTEP);    // Set microsteps to 1:Fullstep ... 256: 1/256th
  driver.en_spreadCycle(EN_SPREADCYCLE);     // Toggle spreadCycle on TMC2208/2209/2224: default false, true: much faster!!!!
  driver.pwm_autoscale(PWM_AUTOSCALE);       // Needed for stealthChop
  
  
}

bool shaft = false;

void loop() {
  stepper.move(-MOTOR_STEPS*MSTEP);
  delay(1500);
}
