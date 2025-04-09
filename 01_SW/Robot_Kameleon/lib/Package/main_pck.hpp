#ifndef __MAIN_PKG_H_
#define __MAIN_PKG_H_

#define RED     "\033[0;31m"
#define CYAN    "\033[0;36m"
#define WHITE   "\033[0;37m"
#define GREEN   "\033[0;32m"
#define YELLOW  "\033[0;33m"



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