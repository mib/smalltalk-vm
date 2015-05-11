#!/bin/bash

# Launches the VM and runs the provided script.
# Expects an image file called 'Smalltalk.image' in the current working directory.

if [[ "$1" ]]; then
	scriptFile="$1"
	shift
	
	cat "$scriptFile"
	echo
	echo --------------------------------
else
	echo "No script file specified."
	exit 1
fi

"$(dirname "$0")/vm" -s Smalltalk.image "$scriptFile" "$@" < /dev/stdin
