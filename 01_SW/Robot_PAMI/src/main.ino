/**
 * Author Teemu Mäntykallio
 * Initializes the library and runs the stepper
 * motor in alternating directions.
 */


#include "../lib/TMC2209/TMCStepper.h"
#include "BasicStepperDriver.h"


#define EN_PIN           38 // Enable
#define SW_RX            7 // TMC2208/TMC2224 SoftwareSerial receive pin
#define SW_TX            8 // TMC2208/TMC2224 SoftwareSerial transmit pin
#define SERIAL_PORT Serial // TMC2208/TMC2224 HardwareSerial port
#define DRIVER_ADDRESS 0b00 // TMC2209 Driver address according to MS1 and MS2

#define R_SENSE 0.11f // Match to your driver
                      // SilentStepStick series use 0.11
                      // UltiMachine Einsy and Archim2 boards use 0.2
                      // Panucatt BSD2660 uses 0.1
                      // Watterott TMC5160 uses 0.075

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200
#define RPM 150

// Since microstepping is set externally, make sure this matches the selected mode
// If it doesn't, the motor will move at a different RPM than chosen
// 1=full step, 2=half step etc.


// All the wires needed for full functionality
#define DIR 2
#define STEP 3
//Uncomment line to use enable/disable functionality
//#define SLEEP 13

#define RMSCURRENT 3000       // RMS current of Stepper Coil in mA
#define MSTEP   16
#define TOFF 5               // Enables driver in software - 3, 5
#define EN_SPREADCYCLE false // Toggle spreadCycle on TMC2208/2209/2224: default false, true: much faster!!!!
#define PWM_AUTOSCALE true   // Needed for stealthChop

TMC2209Stepper driver(SW_RX, SW_TX, R_SENSE, DRIVER_ADDRESS);

// 2-wire basic config, microstepping is hardwired on the driver
BasicStepperDriver stepper(MOTOR_STEPS, DIR, STEP);


void setup() {
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);   
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(0, HIGH); 
  digitalWrite(1, HIGH); 
  digitalWrite(EN_PIN, LOW);      // Enable driver in hardware

                                  // Enable one according to your setup
//SPI.begin();                    // SPI drivers
//SERIAL_PORT.begin(115200);      // HW UART drivers
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
  delay(300);
}
