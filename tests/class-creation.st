"This test shows basic class creation. An ExampleClass is created with one field (content) and a getter+setter for it."

| exampleObject |

Object
	subclass: #ExampleClass
	instanceVariableNames: #(content).


"add a getter and a setter for the field"

ExampleClass compile: '
	content
		^ content
'.

ExampleClass compile: '
	content: aString
		content := aString
'.


exampleObject := ExampleClass new.

exampleObject content: 'It works!'.

^ exampleObject content

"this should return 'It works!' "
