#!/bin/sh

g++ -ggdb -std=c++1z -Wall -Wextra -fmax-errors=1 hdl_parser.cpp scanner.cpp -o parser_test
