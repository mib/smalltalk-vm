#!/bin/bash

# Launches an interactive version (REPL).
# Expects an image file called 'Smalltalk.image' in the current working directory.

"$(dirname "$0")/vm" -s Smalltalk.image -i
