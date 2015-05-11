"This test shows the order of executing exception handler blocks and #ensure: blocks."

[
	[
		[
			Error signal: 'Something is wrong'.
			Transcript show: 'This will not be displayed'
			
		] ensure: [ Transcript show: 'Cleaning up in inner #ensure:'; cr ]
		
	] ensure: [ Transcript show: 'Cleaning up in outer #ensure:'; cr ]
]
	on: Error
	do: [ Transcript show: 'An error has occurred!'; cr ]
