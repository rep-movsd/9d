#pragma once 

using std::vector;

// We will use 1 and 0 for booleans rather than -1 and 0
using Bit = int;


// Bus is an abstraction of a unidirectional signal carrier
// It carries an input signal to one or more end points
// Endpoints (of type Pin) are actually callbacks that are exposed by a gate or other component
// The input signal is set by a function set_signal()
// The number of bits carried is the dependent on the Signal template param
// Strictly speaking carrying more than one bit doesn't make much sense, but it may be useful
template<typename Signal>
class Bus
{
public: 
  
  // Pin is a function that's called when the input signal changes
  using Pin = std::function<void(Signal)>;
  
  // Current signal
  Signal signal() const { return m_signal; }
  
  // Update the signal value of this wire and feed the value to each connected pin
  void set_signal(Signal v)
  {
    m_signal = v;
    feed_pins();
  }
  
  // Use operator >> to connect things
  // Add a new pin that will be called whenever the input signal is updated with set_signal().
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
  void feed_pins() const
  {
    for(auto &p: m_arrPins)
    {
      p(m_signal);
    }
  }
};

// Simplest Bus is a wire that carries 1 bit of info
using Wire = Bus<Bit>;
