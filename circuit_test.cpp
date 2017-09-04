#include "circuit.h"

using namespace std::string_literals;

int main()
{
  // define a 2 input and 1 input gate config
  circuit::node_config basic_gate, not_gate;

  basic_gate.inputs.insert("a");
  basic_gate.inputs.insert("b");
  basic_gate.outputs.insert("y");

  not_gate.inputs.insert("a");
  not_gate.outputs.insert("y");
  
  // Register gates
  circuit::register_component("NAND"s, basic_gate);
  circuit::register_component("NOT"s, not_gate);
  
  // Wire up a NAND and 3 inverters to make an OR gate
  circuit c;
  c.add_elem("NAND"s, "nand1"s);
  c.add_elem("NOT"s, "not1"s);
  c.add_elem("NOT"s, "not2"s);
  c.add_elem("NOT"s, "not3"s);

  c.connect_elems("not1"s, "nand1"s, "y"s, "a"s);
  c.connect_elems("not2"s, "nand1"s, "y"s, "b"s);
  c.connect_elems("nand1"s, "not3"s, "y"s, "a"s);

  // TODO - dump as as .fig to verify connections are right
  
  return 0;
}

