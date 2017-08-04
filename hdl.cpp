#include <functional>
#include <iomanip>
#include <vector>
#include <iostream>

#include "bus.h"
#include "gate.h"

using namespace std;

using nand_gate = gate<1, 1, 1, 0>;
using and_gate = gate<0, 0, 0, 1>;
using or_gate = gate<0, 1, 1, 1>;
using nor_gate = gate<1, 0, 0, 0>;
using xor_gate = gate<0, 1, 1, 0>;

// Extract the nth bit of a number as 1 or 0
#define EXTRACT_BIT(i, n) (i & (1 << n)) >> n

int main()
{
  // Build a cascaded 4 bit xor gate by combining
  xor_gate x, y, z;

  //Connect input a of y to output wire of x  
  x.out >> y.a;
  
  //Connect input a of z to output wire of y  
  y.out >> z.a;
  
  // this leaves x.a, x,b, y.b and z.b as inputs
  
  // test it
  for(unsigned int i = 0; i < 16; ++i)
  {
    Bit bits[4];
    for(int j = 0; j < 4; ++j)
    {
      bits[j] = EXTRACT_BIT(i, j);
    }
    
    x.a(bits[0]);
    x.b(bits[1]);
    y.b(bits[2]);
    z.b(bits[3]);
    
    for(int j = 0; j < 4; ++j)
      cout << bits[j];
  
    cout << " " << z.out.signal() << endl;
  }
}
