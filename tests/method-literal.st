"This test checks that mutable method literals are pushed to stack as copies and the original literal stored in CompiledMethod remains unchanged."

| obj |

Object subclass: #TestClass.

TestClass compile: 'changeLiteral | str | str := ''hello''. Transcript show: str; cr. str at: 2 put: $a'.

obj := TestClass new.

obj changeLiteral.
obj changeLiteral.
obj changeLiteral
