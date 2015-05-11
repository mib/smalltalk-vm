"This test throws an exception and decides in the handler to ignore it and continue with the execution."

^ [
	Exception signal: 'Something is wrong'.
	42
]
	on: Exception
	do: [ :exception | exception resume ]
