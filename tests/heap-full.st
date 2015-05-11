"This test tries to allocate about 100 GB of memory which will result in an 'Out of space on the heap.' message."

| ar |

ar := Array new: 1000000.

1 to: 1000000 do: [
	:i |
	ar at: i put: (Array new: 0 bytes: 100000)
]
