"This test shows that method lookup always starts in the nearest class of the object, even if the message is sent from a superclass."

| cc |

Object subclass: #ClassA.
ClassA subclass: #ClassB.
ClassB subclass: #ClassC.


ClassA compile: '
	identifier
		^ self className
'.

ClassA compile: '
	className
		^ $A
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

"this should return $C"
