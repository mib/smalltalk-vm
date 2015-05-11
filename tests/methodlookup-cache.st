"This test checks that compiling a new method updates the method lookup cache."

| b |

Object subclass: #ClassA.

ClassA compile: '
	print
		Transcript show: $A
'.

ClassA subclass: #ClassB.

b := ClassB new.

b print.

ClassB compile: '
	print
		Transcript show: $B
'.

b print

"'$A$B' should be printed"
