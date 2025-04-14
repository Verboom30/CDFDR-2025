#ifndef __MAIN_PKG_H_
#define __MAIN_PKG_H_

#define RED     "\033[0;31m"
#define CYAN    "\033[0;36m"
#define WHITE   "\033[0;37m"
#define GREEN   "\033[0;32m"
#define YELLOW  "\033[0;33m"

#define R_SENSE 0.11f        // R-Sense in OHM. Match to your driver
#define RMSCURRENT 5000       // RMS current of Stepper Coil in mA
#define MSTEP   16
#define TOFF 5               // Enables driver in software - 3, 5
#define EN_SPREADCYCLE false // Toggle spreadCycle on TMC2208/2209/2224: default false, true: much faster!!!!
#define PWM_AUTOSCALE true   // Needed for stealthChop

struct pos_pince {
    int pince_open;
    int pince_close;
};

const struct pos_pince Pince[]{
    {170, 140}, //Pince R1
    {70, 50},  //Pince R2
    {90, 60}, //Pince R3
    {180, 150},   //Pince R4
};
#endif // __MAIN_PKG_H_