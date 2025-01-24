#ifndef __TMC2009_BITFIELDS_H_
#define __TMC2009_BITFIELDS_H_
#include "mbed.h"

//###############################################
//#      GENERAL CONFIGURATION REGISTERS        #
//###############################################
//***********************************/************************************
// R/W Register GCONF: Addr x00      /                                   /
//***********************************/************************************
namespace TMC2209_n {
  struct GCONF_t {
    constexpr static uint8_t address = 0x00;
    union {
      uint16_t sr : 10; // size 10
      struct {
        bool  i_scale_analog    : 1, //bit(0) //size
              internal_rsense   : 1, //bit(1) //size
              en_spreadcycle    : 1, //bit(2) //size
              shaft             : 1, //bit(3) //size 
              index_otpw        : 1, //bit(4) //size
              index_step        : 1, //bit(5) //size
              pdn_disable       : 1, //bit(6) //size
              mstep_reg_select  : 1, //bit(4) //size
              multistep_filt    : 1, //bit(8) //size
              test_mode         : 1; //bit(9) //size
      };
    };
  };
}
//***********************************/************************************
// R+WC Register GCONF: Addr x01     /                                   /
//***********************************/************************************
namespace TMC2209_n {
  struct GSTAT_t {
    constexpr static uint8_t address = 0x01;
    union {
      uint16_t sr : 3; // size 3
      struct {
        bool  reset   : 1, //bit(0) //size
              drv_err : 1, //bit(1) //size
              uc_cp   : 1; //bit(2) //size
      };
    };
  };
}
//***********************************/************************************
// R  Register IFCNT: Addr x02       /                                   /
//***********************************/************************************
struct IFCNT_t { constexpr static uint8_t address = 0x02; };

//***********************************/************************************
// W Register SLAVECONF: Addr x03        /                               /
//***********************************/************************************
namespace TMC2209_n {
  struct SLAVECONF_t {
    constexpr static uint8_t address = 0x03;
    union {
      uint16_t sr : 4; // size 4
      struct {
         uint8_t senddelay : 4; //size
      }; 
    };
  };
}
//***********************************/************************************
// W Register OTP_PROG: Addr x04     /                                   /
//***********************************/************************************
namespace TMC2209_n {
  struct OTP_PROG_t {
    constexpr static uint8_t address = 0x04;
    union {
      uint16_t sr : 16; // size 16
      struct {
         uint8_t otpbit   : 3, //size
                 otpbyte  : 2, //size
                 otpmagic : 8; //size
      };
    };
  };
}
//***********************************/************************************
// R Register OTP_READ: Addr x05     /                                   /
//***********************************/************************************
struct OTP_READ_t   { constexpr static uint8_t address = 0x05; };
//***********************************/************************************
// R Register IOIN: Addr x06         /                                   /
//***********************************/************************************
namespace TMC2209_n {
  struct IOIN_t {
    constexpr static uint8_t address = 0x06;
    union {
      uint32_t sr;
      struct {
        bool  enn       : 1,
              : 1,
              ms1       : 1,
              ms2       : 1,
              diag      : 1,
              : 1,
              pdn_uart  : 1,
              step      : 1,
              sel_a     : 1,
              dir       : 1;
        uint16_t: 14;
        uint8_t version : 8;
      };
    };
  };
}
//***********************************/************************************
// RW Register FACTORY_CONF: Addr x07                                   /
//***********************************/************************************
namespace TMC2209_n {
  struct FACTORY_CONF_t {
    constexpr static uint8_t address = 0x07;
    union {
      uint32_t sr;
      struct {
        uint8_t fclktrim  : 5,
                : 3,
                ottrim    : 2;
      };
    };
  };
}
//######################################################################
//#      VELOCITY DEPENDENT DRIVER FEATURE CONTROL REGISTER SET        #
//######################################################################
//***********************************/************************************
// W Register IHOLD_IRUN: Addr x10                                      /
//***********************************/************************************
struct IHOLD_IRUN_t {
  constexpr static uint8_t address = 0x10;
  union {
    uint32_t sr : 20;
    struct {
      uint8_t ihold       : 5,
              : 3,
              irun        : 5,
              : 3,
              iholddelay  : 4;
    };
  };
};
//***********************************/************************************
// W Register TPOWER DOWN: Addr x11                                     /
//***********************************/************************************
struct TPOWER_DOWN_t   { constexpr static uint8_t address = 0x11; };
//***********************************/************************************
// R Register TSTEP: Addr x12                                            /
//***********************************/************************************
struct TSTEP_t { 
  constexpr static uint8_t address = 0x12; 
  uint32_t sr : 20;
};
//***********************************/************************************
// W Register TPWMTHRS: Addr x13                                         /
//***********************************/************************************
struct TPWMTHRS_t {
  constexpr static uint8_t address = 0x13;
  uint32_t sr : 20;
};
//***********************************/************************************
// W Register VACTUAL: Addr x22                                          /
//***********************************/************************************
namespace TMC2209_n {
  struct VACTUAL_t {
    constexpr static uint8_t address = 0x22;
    uint32_t sr;
  };
}
#endif // __TMC2009_BITFIELDS_H_