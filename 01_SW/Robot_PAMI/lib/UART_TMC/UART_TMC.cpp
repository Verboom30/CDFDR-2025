#include "UART_TMC.h"
#include "Differentiel.h"

bool Uart_TMC::setup_stepper(void)
{
    beginSerial(11520);
    delay(100);
    pdn_disable(true);
    mstep_reg_select(true);
    int tmc_version = version();
    //Serial.print("TMC-Version :");
    //Serial.println(tmc_version,HEX);
     if (tmc_version != 0x21) {
             //Serial.println("Wrong TMC-Version(not 0x21) or communication error!! STOPPING!!!");
             if (CRCerror) {
                 //Serial.println("CRC-Error!!!");
                 //printf("Read Uart %d\n",slave_address);
             }
        return false;
     } else {
        delay(100);
        toff(TOFF);                 // Enables driver in software
        rms_current(RMSCURRENT);   // Set motor RMS current in mA / min 500 for 24V/speed:3000
        // 1110, 800
        // working: 800 12V/0,6Amax,  Speed up to 5200=4U/min
        microsteps(MSTEP);    // Set microsteps to 1:Fullstep ... 256: 1/256th
        en_spreadCycle(EN_SPREADCYCLE);     // Toggle spreadCycle on TMC2208/2209/2224: default false, true: much faster!!!!
        pwm_autoscale(PWM_AUTOSCALE);       // Needed for stealthChop
        return true;
     }  
     
    
}