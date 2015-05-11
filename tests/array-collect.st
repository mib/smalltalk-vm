"This test shows one of the basic collection enumerations: transforming positive numbers to negative and vice versa."

^ #(1 -3 8 47 -25) collect: [ :number | number negated ]

"this should return #(-1 3 -8 -47 25)"
