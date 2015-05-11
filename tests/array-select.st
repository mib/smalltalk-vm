"This test shows one of the basic collection enumerations: selecting only positive numbers from an array."

^ #(1 -3 8 47 -25) select: [ :number | number positive ]

"this should return #(1 8 47)"
