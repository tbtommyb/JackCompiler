# JackCompiler

An assembler, bytecode VM and compiler written in C++. Based on the exercises from [nand2tetris](nand2tetris.org).

## Build instructions

Make a build directory for the component you want:

```
$ mkdir assemblr/build && cd assemblr/build
$ cmake ..
$ cmake --build .
```

## Future

The compiler mixes parsing with codegen. It would be good to parse to an AST and separate the two steps.
