#!/bin/sh

g++ -ggdb -fmax-errors=1 hdl_parser.cpp scanner.cpp -o parser_test -lstdc++fs
