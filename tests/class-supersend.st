"This test shows that method lookup for super-sends always starts in the superclass of the class containing the currently executed method."

| cc |

Object subclass: #ClassA.
ClassA subclass: #ClassB.
ClassB subclass: #ClassC.


ClassA compile: '
	className
		^ $A
'.

ClassB compile: '
	identifier
		^ super className
'.

ClassB compile: '
	className
		^ $B
'.

ClassC compile: '
	className
		^ $C
'.


cc := ClassC new.

^ cc identifier

"this should return $A"
