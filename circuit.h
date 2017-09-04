#pragma once
#include <string>
#include <unordered_set>
#include <unordered_map>
using std::string;
using std::unordered_map;
using std::unordered_set;
using std::hash;


// represents a connection between two components
// source.output connects to sink.input
struct connection
{
  string source;
  string sink;
  string input;
  string output;
  
  bool operator==(const connection &c) const
  {
    return 
      source == c.source  &&
      source == c.sink    &&
      source == c.input   &&
      source == c.output;
  }
};

struct connection_hash
{
  std::size_t operator()(const connection &c) const 
  {
    return 
      hash<string>{}(c.source)  ^
      hash<string>{}(c.sink)    ^
      hash<string>{}(c.input)   ^
      hash<string>{}(c.output);
  }
};



// An abstraction for a circuit diagram - essentially a graph with named typed nodes
class circuit
{
public:
  using node_kind = string;
  using node_name = string;
  using cstrref = const string&;
  
  // description of node topology - names of I/O pins
  struct node_config
  {
    unordered_set<string> inputs;
    unordered_set<string> outputs;
  };
  
private:
  
  
  // A singleton map to component types and their topology
  static unordered_map<node_kind, node_config> &library()
  {
    static unordered_map<node_kind, node_config> lib;
    return lib;
  }
  
  // Components and wires
  unordered_map<node_name, node_kind> elems;
  unordered_set<connection, connection_hash> wires;
  
  // simple assert
  static void check(bool bCond, string err)
  {
    if(!bCond)
    {
      throw err;
    }
  }
  
public:
  
  // Add a component to this circuit
  void add_elem(cstrref kind, cstrref name)
  {
    auto &lib = library();
    
    // Throw if unknown kind or already existing
    check(lib.count(kind), string("Unknown component: ") + kind);
    check(!elems.count(name), string("Component of this name already exists: ") + name);
    elems[name] = kind;
  }
  
  // Add a connection between two components
  void connect_elems(cstrref source, cstrref sink, cstrref pin1, cstrref pin2)
  {
    auto &lib = library();
    
    // Ensure components exist
    check(elems.count(source), string("Component not found: ") + source);
    check(elems.count(sink), string("Component not found: ") + sink);

    // Get the topology of the two gates
    node_config &nc1 = lib[elems[source]];
    node_config &nc2 = lib[elems[sink]];
    
    // Make sure pin1 is an output of name1 and pin2 is an input of name2
    check(nc1.outputs.count(pin1), "Not a valid output pin: " + pin1);
    check(nc2.inputs.count(pin2), "Not a valid input pin: " + pin2);
    
    // Doesnt matter if wire is duplicated
    connection c{source, sink, pin1, pin2};
    wires.insert(c);
  }
  
  // add a component to lib
  static void register_component(const node_kind &kind, const node_config &config)
  {
    auto &lib = library();
    
    // Ensure its not already extant then add this component
    check(!lib.count(kind), string("Component already registered: " + kind));
    lib[kind] = config;
  }
};



