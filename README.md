Smalltalk VM written in C
=========================

This is a very simple Smalltalk VM, written in pure C. It runs images encoded in a custom format (see the [minimal kernel image](https://github.com/mib/smalltalk-vm-image)).

It was implemented as an assignment for a university course on runtime systems. It had to be done within ~2 months by myself, all the while being enrolled to several other courses with their own assignments. I did not have any prior knowledge of VMs when I started this project.


Limitations
-----------

The VM cannot do much, there are a lot of limitations:

*	Blocks/closures are [BlueBook](http://sdmeta.gforge.inria.fr/FreeBooks/BlueBook/)-style: they are not reentrant.
*	There are only small integers. No large integers, no floats, no fractions.
*	Heap size is specified at compile-time, there is no dynamic growing/shrinking.
*	The only way of communicating with the external world is through files.
*	There is no support for weak references. Symbols will never get garbage-collected.
*	Hashing was not implemented. Dictionaries and the symbol table have linear access time.
*	And many more…


Building
--------

Makefile is present for standard GNU `make`, the VM is compiled by executing

    make

in the root directory. An executable binary called `vm` will be created.


Executing
---------

A simple Bash wrapper to launch the VM with the default image is called `run.sh`. It expects an image file called Smalltalk.image in the current directory.

There are three possible execution modes:

1) Run a script from a file

    ./run.sh tests/hello-world.st

When a value is returned (^) from the script, it will be printed on stdout.

2) Run a script passed as an argument

    ./run.sh -r 'Transcript show: #hello; cr'
    ./run.sh -r '^ 10 factorial'
    ./run.sh -r 'Smalltalk keysDo: [ :each | Transcript show: each; cr ]'

Again, a value returned (^) from the script will be printed on stdout.

3) Launch an interactive mode (REPL)

    ./run.sh -i
    # or a shortcut
    ./repl.sh

In this mode, each expression result is printed automatically on stdout.


A sample program: `grep`
------------------------

A simple implementation of a Unix `grep` utility was written to demostrate the capabilities of the VM and the kernel. It only provides fixed pattern matching, no support for regular expressions so it acts like GNU `grep` with the `-F` option.

The program is embedded directly in the [minimal kernel image](https://github.com/mib/smalltalk-vm-image) to avoid its compilation on each run.

A Bash wrapper to launch it is provided in file `grep.sh`. Basic usage:

    ./grep.sh PATTERN INPUT-FILE OUTPUT-FILE

If any of the file is `-` or the argument is omitted, stdin or stdout is used. There are several options implemented: `-i`, `-v`, `-c`, `-x`, `-e`, `-f`; their function is the same as in GNU `grep`. Running:

    ./grep.sh -h

provides an overview of these options.


License
-------

The MIT License, see file `LICENSE` for more information.
