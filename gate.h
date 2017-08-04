#pragma once

#define LUT(m, n) arrTruthTable[m][n] = o ## m ## n;
    
// Gate is an abstraction of a 2->1 boolean gate
// Specify 4 template params as the truth table.
template<Bit o00, Bit o01, Bit o10, Bit o11> struct gate
{
  using Pin = Wire::Pin;
  
private: 
  Bit inA = 0, inB = 0;
  Bit arrTruthTable[2][2];
  
  Pin make_pin(Bit *pBit)
  {
    return [this, pBit](Bit bit){ *pBit = bit; this->tick(); };
  }
  
  // This computes the result of the inputs and sends it to the output wire
  // Todo - make this async and happen with a fixed delay 
  void tick()
  {
    out.set_signal(arrTruthTable[inA][inB]);
  }
  
public: 
  Wire out;
  Pin a;
  Pin b;
  
  gate(): a(make_pin(&inA)), b(make_pin(&inB))
  {
    LUT(0, 0);
    LUT(0, 1);
    LUT(1, 0);
    LUT(1, 1);
  }
};