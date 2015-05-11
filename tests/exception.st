"This test defines an exception handler and prints an error message when the exception occurs."

[ Error signal: 'Something is wrong' ]
	on: Error
	do: [ Transcript show: 'An error has occurred!'; cr ]
