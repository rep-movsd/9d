# 9d
Complete software suite for the [nand2tetris](http://nand2tetris.org/) [MOOC](https://www.coursera.org/learn/build-a-computer) course


### Intro
The nand2tetris MOOC is a challenging course that teaches how to build a computer from ground up, using logic gates all the way upto the software stack, including a compiler and a rudimentary OS.

The course is quite old, and the software tools that they provide feel a bit dated. We aim to build an alternative suite that will help students of this course.

Beyond the course itself, it can serve as a playground to test various ideas for programming languages, CPU architecture etc. within the limits of the course tooling specifications.

### Features

nand2tetris has several tools:

 * A digital logic designer based on a hardware description language (called HDL) that lets you wire together gates and simulate them
 * A cpu simulator for the HACK CPU that runs machine code
 * A VM emulator that executes the JACK VM bytecode
 * A HACK platform simulator, that simulates an entire computer including a keyboard and a display
 
We will attempt to build a complete IDE/Debugger/runtime that wraps all of these 

Several ideas to implement:
 * Graphical display of HDL circuits, and ability to visually design them (two way visual design)
 * HACK assembler/emulator with reversible debugging, breakpoints, watches etc.
 * JACK language IDE, with syntactic/semantic highlighting, plugin architecture for new language features
 * JACK VM emulator with reversible debugging etc.
 * HACK machine emulation with plugin architecture for custom devices
 * Generic compiled plugins for handling other experimental CPU architectures and machine code
 * JACK VM to native x86 compilation
 
 

