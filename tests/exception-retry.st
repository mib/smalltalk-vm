"This test uses an exception handler to retry a block that has failed."

| tries |

tries := 0.

[
	tries := tries + 1.
	Transcript show: 'Attempt number #'; show: tries; cr.
	
	Error signal: 'Something went wrong'
]
	on: Error
	do: [
		:error |
		Transcript show: 'Failed: ' , error messageText; cr.
		
		tries < 5 ifTrue: [
			Transcript show: 'Retrying...'; cr.
			error retry
		].
		
		Transcript show: 'Giving up after 5 unsuccessful tries.'; cr
	]
