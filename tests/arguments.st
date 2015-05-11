"This test just prints all arguments passed on the command-line."

Transcript show: 'Arguments passed on the command line:'; cr.

1 to: ScriptRunner argumentCount do: [
	:index |
	Transcript
		show: index;
		show: ': ';
		show: (ScriptRunner argumentAt: index);
		cr
]
