#include "Stepper.hpp"

//***********************************/************************************
//                         Constructors                                 //
//***********************************/************************************
Stepper::Stepper(PinName clk, PinName dir): _clk(clk) , _dir(dir)
{

    
    _clk = 1;
    _dir = 0;
    _state = STOP;
    _pos = 0;
    _steps = 0;
    _spd = 0;
    _dt0 = 0;
    _n=0;
    _nStartDec=0;
    _dtn=0;
    _dtmin=0;
    _dt0=0;
    _i=0;
    _acc =0;
    _dec =0;
    _Pos_Cible_Done = 1;
}

//***********************************/************************************
//                                Get Set                               //
//***********************************/************************************
void Stepper::setSpeed(float speed)
{
    //stop();   
    _spd = (speed<0) ? -speed : speed;  //speed must be unsigned
    if(_spd)_dtmin = 1000000/_spd;      //fin min delay (max spd)
    //handler();  
}

float Stepper::getSpeed(void)
{
    return _spd;
}

void Stepper::setAcceleration(float acc)
{
  
    _acc = (acc<0) ? -acc : acc;            //acceleration must be unsigned
    if(_acc)_dt0 = 676000 * sqrt(2.0/_acc); //Equation 15 [µs] instead Equation 7
}

float Stepper::getAcceleration(void)
{
    return _acc;
}

void Stepper::setDeceleration(float dec)
{
    _dec = (dec<0) ? -dec : dec;        //deceleration must be unsigned
}

float Stepper::getDeceleration(void)
{
    return _dec;
}

void Stepper::setPosition(int position)
{
    _pos = position;
}
void Stepper::setPositionZero(void)
{
    _state = STOP;
    _pos = 0;
    _steps = 0;
    _spd = 0;
    _dt0 = 0;
    _n=0;
    _nStartDec=0;
    _dtn=0;
    _dtmin=0;
    _dt0=0;
    _i=0;
    _acc =0;
    _dec =0;
}

int Stepper::getPosition(void)
{
    return _pos;
}

int Stepper::getStep(void)
{
    return _steps;
}

bool Stepper::stopped(void)
{
    return (_state == STOP) ? true : false;
}

bool Stepper::getPosCibleDone(void)
{
    return (_Pos_Cible_Done == 1) ? true : false;
}


//***********************************/************************************
//                             Public Methods                           //
//***********************************/************************************

void Stepper::run(void)
{
    handler();    
}

void Stepper::stop(void)
{   
    _clk = 1;
    remove();           //stop timer
    _state = STOP;      //update state machine 
    _steps = 0;         //reset total steps per move
   
    
}

void Stepper::rotate(bool direction)
{
    if(!_spd)return;    //spd must > 0
    _dir = direction;   //set output pin direction value
    _steps = 0;         //rotate until stop() by user
    handler();          //start thread
}

void Stepper::move(int steps)
{
    if(!steps || !_spd) return;
    if(steps<0) //fin direction
    {
        _dir = CCW;         //set output pin direction value
        _steps = -steps;    //total steps per move must be unsigned
    }
    else
    {
        _dir = CW;          //set output pin direction value
        _steps = steps;     //total steps per move
    }
    _Pos_Cible_Done =0;
    handler();              //start thread
}

void Stepper::goesTo(int position)
{
    move(position-_pos);    //absolute to relative transformation   
}

void Stepper::pause()
{
    if (_state != STOP && _state != PAUSE) {
        _prev_state = _state;
        _state = PAUSE;
        remove();  // arrête la minuterie
    }
}
void Stepper::resume()
{
    if (_state == PAUSE) {
        _state = _prev_state;
        insert(us_ticker_read() + _dtn);  // important : nouveau délai
    }
}

//***********************************/************************************
//                          Protected Methods                           //
//***********************************/************************************
void Stepper::handler(void)
{
    if (_isPaused) return;  // Ne rien faire si en pause
    
    switch(_state)
    {
        case STOP:
            _n = 0;                      //reset setp counter (motor stopped)
  
            if(_dt0 <= _dtmin || !_acc) //if first step faster than max speed step
            {
                _dtn = _dtmin;       //delay = delaymin
                _state = CRUISE;    //no acceleration needed
            }
            else
            {
                _dtn = _dt0;         //set first delay
                _state = ACCEL;     //acceleration phase
            }

            if(_steps)  //if finite mov required
            {
                unsigned int nToSpeed = nTo(_spd,_acc);      //Equation 16 How many steps to reach max speed 
                _nStartDec = (_steps * _dec) / (_dec + _acc);   //Equation 19 after how many step we must start decelerate  
                if(_nStartDec > nToSpeed)_nStartDec = _steps - ((nToSpeed*_acc)/_dec);  //if speed can be reach Equation 17                
            }
            _i = _dtn;
        break;
        
        case ACCEL:
            //_dtn -=  (_dtn*2.0) / ((_n<<2)+1);   //Equation 20 find next delay
            _i-= _i*2.0 / ((_n<<2)+1);
            _dtn = _i;
            
            if((unsigned int)_dtn <= _dtmin) //if max speed reached
            {
                 _dtn = _dtmin;
                 _i = _dtn;
                _state = CRUISE;    //constant phase
            }
            if(_steps && _dec && _n >= _nStartDec)_state = DECEL; //chech when must start decelerate
        break;
        
        case CRUISE:
            if(_steps && _dec && _n >= _nStartDec)_state = DECEL; //chech when must start decelerate
        break;
        
        case DECEL:
            //_dtn +=  (_dtn*2) / (((_steps-_n)<<2)+1);  //Equation 20 find next delay
            _i+= (_i*2.0) / (((_steps-_n)<<2)+1);
            _dtn = _i;
        break;    
    }
    
    _clk=0;
    
    if(!_n) insert(_dtn + us_ticker_read());   //start timer @ first delay
    else insert(event.timestamp+(unsigned int)_dtn);
    
    _n++;                                   //increment step counter
    _pos += (_dir<<1)-1;                    //set new position +1 if cw; -1 if ccw
    _clk = 1;                              //toggle step out pin

    if(_steps && _n >= _steps){
        _Pos_Cible_Done = 1;
        stop();       //check for motor stop
    }
}

unsigned int Stepper::nTo(float speed,float acc)
{
    if(speed<0)speed = -speed;
    if(acc<0)acc = -acc;
    
    return (!acc || !speed) ? 0 : (speed * speed) / (2 * acc); //Equation 16 step number n as a function of speed & acceleration
}


