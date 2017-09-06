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
 
 
### Collaboration model

Until I am aware of any collaborators competence, all architectural and platform considerations are solely decided by me.
There are some collaborators whose judgement I trust enough to let them architect things.

My job will be to create task based issues and let people pick what they feel confident of doing. If any task takes too long, someone else can step in.

There is bound to be a lot of code churn and rewriting involved in such a large project. Until specified, treat your code as likely to be re-architected (or rewritten).

After some consideration, it is decided that the project will be multi-lingual - C++ will be used for the engine and GUI will be built in Lazarus/FreePascal which is a free/libre development environment closely matching Delphi

A web UI may be provided for some tools.

Scripting and codegen tools can be in python or JS etc.

### Values

Only code matters - code that helps the end user get their stuff done.
How they ought to get the stuff done is a subjective decision which I will take, as creator of the project.

Egos, genders, colors, pleb-ness, pru-ness - all of these are irrelevant.

If the code is good, I let it be, until such time as it is ready to be rewritten in a better way.
This includes my own code.

Any issues not related to code and end user experience will be deleted promptly.


