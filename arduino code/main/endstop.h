#ifndef endstop_h
#define endstop_h

class endstop
{
  private:
   // d
  uint8_t m_signalPin;
  void (*m_function)();
  
  public:
  bool m_hited;
  // ham khoi tao cong tac hanh trinh
  endstop(uint8_t signalPin)
  {
    m_signalPin = signalPin;
    Serial.println(m_signalPin);
    pinMode(m_signalPin, INPUT_PULLUP);
    m_hited = false; 
  }
  ~endstop()
  {

  }
  
  // cap nhat trang thai cua ctht
  void update()
  {
    if(digitalRead(m_signalPin) == LOW)
    {
     
      m_hited = true;
      //Serial.println(m_hited);  
      //m_function;
    }
    else{
      m_hited = false;
    }
    
  }

  // cai dat cong tac hanh trinh
  void addHandler(void(*function)())
  {
    m_function = function;
  }
};


#endif 
