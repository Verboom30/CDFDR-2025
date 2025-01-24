#ifndef __TMC2209_H_
#define __TMC2209_H_
#include "mbed.h"
#include "TMC2209_bitfields.hpp"
class TMC2209Stepper 
{
  public:  
    //***********************************/************************************
    //                         Constructors                                 //
    //***********************************/************************************    
    TMC2209Stepper (PinName Uart_TX_pin, PinName Uart_RX_pin, uint8_t Slave_Addr);
    //***********************************/************************************
    //                             Public Methods                           //
    //***********************************/************************************
    void begin();
    void beginSerial(uint32_t baudrate);
   
    void write(uint8_t addr, uint32_t dataWrite);
    uint32_t read(uint8_t addr);

  protected:
    //***********************************/************************************
    //                          Protected Methods                           //
    //***********************************/************************************
    BufferedSerial * SWSerial = nullptr;
    
    int16_t serial_single_read();
    uint8_t serial_single_write(const uint8_t data);
    uint8_t calcCRC(uint8_t datagram[], uint8_t len);
    uint64_t ReadResquest(uint8_t datagram[], const uint8_t len, uint16_t timeout);

    static constexpr uint8_t TMC_READ = 0x00, TMC_WRITE = 0x80;
    static constexpr uint8_t  TMC2209_SYNC = 0x05, TMC2209_SLAVE_ADDR = 0x00;
    static constexpr uint8_t replyDelay = 2;  //ms
    static constexpr uint8_t max_retries = 2;
    static constexpr uint8_t abort_window = 5; //5ms timeout
    bool CRCerror = false;

  private : 
    //***********************************/************************************
    //                            Private Methods                           //
    //***********************************/************************************
};


#endif // __TMC2209_H_