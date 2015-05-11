"This test shows that it is possible to inject a value back into the context when resuming execution."

^ [
	| value |
	
	value := Exception signal: 'Something is wrong'.
	
	value * 2
]
	on: Exception
	do: [ :exception | exception resume: 12 ]
