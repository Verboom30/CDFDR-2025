#ifndef __MAIN_PKG_H_
#define __MAIN_PKG_H_

#define RED     "\033[0;31m"
#define CYAN    "\033[0;36m"
#define WHITE   "\033[0;37m"
#define GREEN   "\033[0;32m"
#define YELLOW  "\033[0;33m"

#define R_SENSE 0.11f        // R-Sense in OHM. Match to your driver
#define RMSCURRENT 3000       // RMS current of Stepper Coil in mA
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
    {180, 150},   //Pince R3
    {90, 60}, //Pince R4
   
};

struct pos_bras {
    int bras_home;
    int bras_side;
    int bars_take;
    int bras_banner;
};

const struct pos_bras Bras[]{
    {10,40,180,160}, //Bras G
    {185,160,20,40}, //Bras D
    
   
};

struct pos_hook {
    int hook_up;
    int hook_down;
};
const struct pos_hook Hook[]{
    {65,160}, //Hook G
    {145,50}, //Hook D
    
   
};
#endif // __MAIN_PKG_H_