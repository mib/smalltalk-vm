"This test shows basic usage of file streams."

| filename writeStream readStream |

filename := './testFile.txt'.


writeStream := FileWriteStream on: filename.

writeStream nextPutAll: 'Hello, World'.
writeStream nextPut: $!.

writeStream close.


readStream := FileReadStream on: filename.

Transcript show: readStream contents; cr.

readStream close
