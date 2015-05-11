"This test shows the hierarchy of classes starting with String and the parallel hierarchy of the corresponding metaclasses. While the class hierarchy terminates at ProtoObject, metaclasses are classes as well so their hierarchy has to wrap around to Class."

Transcript show: 'Class hierarchy:'; cr; cr.

Transcript show: 'test' class; cr.
Transcript show: 'test' class superclass; cr.
Transcript show: 'test' class superclass superclass; cr.
Transcript show: 'test' class superclass superclass superclass; cr.
Transcript show: 'test' class superclass superclass superclass superclass; cr.
Transcript show: 'test' class superclass superclass superclass superclass superclass; cr.
Transcript show: 'test' class superclass superclass superclass superclass superclass superclass; cr; cr.

Transcript show: 'Metaclass hierarchy:'; cr; cr.

Transcript show: 'test' class class; cr.
Transcript show: 'test' class class superclass; cr.
Transcript show: 'test' class class superclass superclass; cr.
Transcript show: 'test' class class superclass superclass superclass; cr.
Transcript show: 'test' class class superclass superclass superclass superclass; cr.
Transcript show: 'test' class class superclass superclass superclass superclass superclass; cr.
Transcript show: 'test' class class superclass superclass superclass superclass superclass superclass; cr; cr.
