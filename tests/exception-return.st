"This test shows that it is possible to end the exception handler block prematurely."

^ [ Error signal: 'Something is wrong' ]
	on: Error
	do: [
		:error |
		error return: 42.
		Transcript show: 'This line will not be printed.'
	]
