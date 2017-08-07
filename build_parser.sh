#!/bin/sh

g++ -ggdb -std=c++1z -fmax-errors=1 hdl_parser.cpp scanner.cpp -o parser_test
