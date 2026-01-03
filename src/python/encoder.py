import bitarray as ba
import cv2
import numpy as np

f = open("testData")
data = f.read()
f.close()

byteData = data.encode("utf-8")
bitData = ba.bitarray()
bitData.frombytes(byteData)

length = np.array(len(bitData), dtype="uint16")
bitLength = ba.bitarray()
bitLength.frombytes(length) 

bits = np.concatenate([np.unpackbits(bitLength), np.unpackbits(bitData)])
image = np.resize(np.pad(bits, (0,25 * 25 - length)), (25, 25))

cv2.imwrite('output/img/output.png', image * 255)