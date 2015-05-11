"This test shows that even when an unhandled exception occurs, the #ensure: blocks still get executed."

[
	[
		Error signal: 'Something is wrong'.
		Transcript show: 'This will not be displayed'
		
	] ensure: [ Transcript show: 'Cleaning up in inner #ensure:'; cr ]
	
] ensure: [ Transcript show: 'Cleaning up in outer #ensure:'; cr ]
