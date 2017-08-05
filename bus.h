#pragma once 

using std::vector;

// We will use 1 and 0 for bools rather than -1 and 0
using Bit = int;


// Bus is an abstraction of a unidirectional signal carrier
// It carries a signal to one or more endpoints
// Endpoints (of type Pin) are actually callbacks that are registered by a gate
// The input signal is set by a funtion set_signal()
// TYhe number of bits carried is the dependent on the  Signal template param
template<typename Signal>
class Bus
{
public: 
  using Pin = std::function<void(Signal)>;
  
  // Current signal
  Signal signal() const { return m_signal; }
  
  // Update the signal value of this wire and feed the value to each pin
  void set_signal(Signal v)
  {
    m_signal = v;
    feed_pins();
  }
  
  // Use >> to connect things
  // Add a new pin that will be called whenever the signal is updated.
  Bus& operator>>(Pin &pin) 
  { 
    m_arrPins.push_back(pin); 
    return *this;
  }
  
private:
  
  // The current signal value of this wire
  Signal m_signal;
  
  // List of pins  associated with this wire
  vector<Pin> m_arrPins;
    
  // Feed the signal to each pin
  void feed_pins()
  {
    for(auto &p: m_arrPins)
    {
      p(m_signal);
    }
  }
};


using Wire = Bus<Bit>;
