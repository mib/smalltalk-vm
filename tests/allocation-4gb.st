"This test is used to measure the speed of allocation. It allocates 4 GB of memory, throwing it away so it can be collected immediately."

1 to: 10000 do: [
	Array new: 0 bytes: 400000
]
