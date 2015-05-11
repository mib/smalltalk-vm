#!/bin/bash

# Launches the example grep utility.
# Expects an image file called 'Smalltalk.image' in the current working directory.

"$(dirname "$0")/vm" -s -r "Grep run" Smalltalk.image -- "$@" < /dev/stdin
