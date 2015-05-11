"This test shows that all collection classes have mostly unified protocol so it is possible to #select: from strings."

^ 'And Now For Something Completely Different'
	select: [ :character | character isUppercase ]
