#pragma once

// Gate is an abstraction of a 2->1 boolean gate
// Specify 4 template params as the truth table.

// It has two input Pins and one output Wire (Bus<Bit>)
// Gates can be connected together by connecting the "out"
// wire of one gate to the Pin input of another gate

template<Bit o00, Bit o01, Bit o10, Bit o11> struct gate
{
  using Pin = Wire::Pin;
  
private: 
  Bit inA = 0, inB = 0;
  const Bit arrTruthTable[2][2] = {{o00, o01}, {o10, o11}};
  
  // Helper function to define each of the pins
  // Each Pin is a function that gets called when the signal its being fed has to change
  Pin make_pin(Bit *pBit)
  {
    return [this, pBit](Bit bit){ *pBit = bit; this->tick(); };
  }
  
  // This computes the result of the inputs and sends it to the output wire
  // Todo - make happen with a fixed delay 
  void tick()
  {
    out.set_signal(arrTruthTable[inA][inB]);
  }
  
public: 
  Wire out;
  Pin a, b;
  gate(): a(make_pin(&inA)), b(make_pin(&inB)) {}
};