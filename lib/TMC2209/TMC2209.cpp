#include "TMC2209.hpp"

//***********************************/************************************
//                         Constructors                                 //
//***********************************/************************************

TMC2209Stepper::TMC2209Stepper(PinName step_pin, PinName dir_pin, PinName Uart_TX_pin, PinName Uart_RX_pin, float RS, uint8_t Slave_Addr) :
  Stepper(step_pin,dir_pin),
  Rsense(RS)
{
  BufferedSerial *SWSerialObj = new BufferedSerial(Uart_TX_pin, Uart_RX_pin);
  SWSerial = SWSerialObj;
  
}
//***********************************/************************************
//                                Get Set                               //
//***********************************/************************************


//***********************************/************************************
//                             Public Methods                           //
//***********************************/************************************
void TMC2209Stepper::begin() {
   
    beginSerial(115200);
    //beginSerial(9600);
    pdn_disable(true);
    mstep_reg_select(true);
    //Wait to initialize
    wait_us(replyDelay*1000);

};
void TMC2209Stepper::beginSerial(uint32_t baudrate) {
if (SWSerial != nullptr)
      {
          //SWSerial->begin(baudSrate);
          //SWSerial->end();
          SWSerial->set_baud(baudrate);
          SWSerial->set_format(
                      /* bits */ 8,
                      /* parity */ BufferedSerial::None,
                      /* stop bit */ 1
                  );
          SWSerial->set_blocking(false);  //set to non-blocking read
      }
}

void TMC2209Stepper::write(uint8_t addr, uint32_t dataWrite) {
  uint16_t bytesWritten = 0;
  uint8_t len = 7;
  addr |= TMC_WRITE;
  uint8_t datagram[] = {TMC2209_SYNC, TMC2209_SLAVE_ADDR, addr, (uint8_t)(dataWrite>>24), (uint8_t)(dataWrite>>16), (uint8_t)(dataWrite>>8), (uint8_t)(dataWrite>>0), 0x00};
  datagram[len] = calcCRC(datagram, len);
  for(uint8_t i=0; i<=len; i++) {
    bytesWritten += serial_single_write(datagram[i]);
  }
  //wait_us(replyDelay*1000);
}

uint32_t TMC2209Stepper::read(uint8_t addr){
  constexpr uint8_t len = 3;
  addr |= TMC_READ;
  uint8_t datagram[] = {TMC2209_SYNC, TMC2209_SLAVE_ADDR, addr, 0x00};
  datagram[len] = calcCRC(datagram, len);
  uint64_t out = 0x00000000UL;
  for (uint8_t i = 0; i < max_retries; i++) {
    out = ReadResquest(datagram, len, abort_window);
    CRCerror = false;
    uint8_t out_datagram[] = {
            static_cast<uint8_t>(out>>56),
            static_cast<uint8_t>(out>>48),
            static_cast<uint8_t>(out>>40),
            static_cast<uint8_t>(out>>32),
            static_cast<uint8_t>(out>>24),
            static_cast<uint8_t>(out>>16),
            static_cast<uint8_t>(out>> 8),
            static_cast<uint8_t>(out>> 0)
        };
        uint8_t crc = calcCRC(out_datagram, 7);
        if ((crc != static_cast<uint8_t>(out)) || crc == 0 ) {
            CRCerror = true;
            out = 0;
        } else {
            break;
        }
  }
  return out>>8;
}

uint32_t TMC2209Stepper::IOIN() {
    return read(TMC2209_n::IOIN_t::address);
}
bool TMC2209Stepper::enn()          { TMC2209_n::IOIN_t r{0}; r.sr = IOIN(); return r.enn;      }
bool TMC2209Stepper::ms1()          { TMC2209_n::IOIN_t r{0}; r.sr = IOIN(); return r.ms1;      }
bool TMC2209Stepper::ms2()          { TMC2209_n::IOIN_t r{0}; r.sr = IOIN(); return r.ms2;      }
bool TMC2209Stepper::diag()         { TMC2209_n::IOIN_t r{0}; r.sr = IOIN(); return r.diag;     }
bool TMC2209Stepper::pdn_uart()     { TMC2209_n::IOIN_t r{0}; r.sr = IOIN(); return r.pdn_uart; }
bool TMC2209Stepper::step()         { TMC2209_n::IOIN_t r{0}; r.sr = IOIN(); return r.step;     }
bool TMC2209Stepper::spread_en()    { TMC2209_n::IOIN_t r{0}; r.sr = IOIN(); return r.spread_en;}
bool TMC2209Stepper::dir()          { TMC2209_n::IOIN_t r{0}; r.sr = IOIN(); return r.dir;      }
uint8_t TMC2209Stepper::version()   { TMC2209_n::IOIN_t r{0}; r.sr = IOIN(); return r.version;  }


void TMC2209Stepper::rms_current(uint16_t mA) {
  uint8_t CS = 32.0*1.41421*mA/1000.0*(Rsense+0.02)/0.325 - 1;
  // If Current Scale is too low, turn on high sensitivity R_sense and calculate again
  if (CS < 16) {
    vsense(true);
    CS = 32.0*1.41421*mA/1000.0*(Rsense+0.02)/0.180 - 1;
  } else { // If CS >= 16, turn off high_sense_r
    vsense(false);
  }

  if (CS > 31)
    CS = 31;

  irun(CS);
  ihold(CS*holdMultiplier);
  //val_mA = mA;
}

void TMC2209Stepper::microsteps(uint16_t ms) {
  switch(ms) {
    case 256: mres(0); break;
    case 128: mres(1); break;
    case  64: mres(2); break;
    case  32: mres(3); break;
    case  16: mres(4); break;
    case   8: mres(5); break;
    case   4: mres(6); break;
    case   2: mres(7); break;
    case   1: mres(8); break;
    default: break;
  }
}

//***********************************/************************************
//                          Protected Methods                           //
//***********************************/************************************
int16_t TMC2209Stepper::serial_single_read(){
  int16_t out = 0;
  int16_t count = 0;
  if (&SWSerial != nullptr) {
     count = SWSerial->read(&out, 1); // read one character
  } 
  if (count >= 1) {
    //printf("<%02X|",out);
    return out;
  } else {
    return -1;
  }
}

uint8_t TMC2209Stepper::serial_single_write(const uint8_t data){
  int out = 0;
  if (SWSerial != nullptr) {
    return SWSerial->write(&data,1);
  }
  return out;
}

uint8_t TMC2209Stepper::calcCRC(uint8_t datagram[], uint8_t len) {
    uint8_t crc = 0;
    for (uint8_t i = 0; i < len; i++) {
        uint8_t currentByte = datagram[i];
        for (uint8_t j = 0; j < 8; j++) {
            if ((crc >> 7) ^ (currentByte & 0x01)) {
                crc = (crc << 1) ^ 0x07;
            } else {
                crc = (crc << 1);
            }
            crc &= 0xff;
            currentByte = currentByte >> 1;
        }
    }
    return crc;
}

uint64_t TMC2209Stepper::ReadResquest(uint8_t datagram[], const uint8_t len, uint16_t timeout){
  SWSerial->sync(); //Fulsh buffers
  for(int i=0; i<=len; i++) serial_single_write(datagram[i]);
  wait_us(replyDelay*1000);

  uint32_t sync_target = (static_cast<uint32_t>(datagram[0])<<16) | 0xFF00 | datagram[2];
  uint32_t sync = 0;
  uint64_t start_ms, now;
  start_ms = Kernel::get_ms_count();
  do {
    now = Kernel::get_ms_count();
    if (now - start_ms > timeout) return 0;
    int16_t res = serial_single_read();
    if (res < 0) continue;
    sync <<= 8;
    sync |= res & 0xFF;
    sync &= 0xFFFFFF;
  } while (sync != sync_target);

  uint64_t out = sync;
  start_ms = Kernel::get_ms_count();
  for(uint8_t i=0; i<5;) {
    now = Kernel::get_ms_count();
    if (now - start_ms > timeout) return 0;

    int16_t res = serial_single_read();
    if (res < 0) continue;

    out <<= 8;
    out |= res & 0xFF;
    i++;
  }
  SWSerial->sync();
  return out;
}

