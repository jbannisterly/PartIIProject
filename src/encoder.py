import bitarray
import cv2
import numpy as np

f = open("testData")
data = f.read()
f.close()

ba = bitarray.bitarray()
byteData = data.encode("utf-8")
ba.frombytes(byteData)

bits = np.unpackbits(ba)
image = np.resize(np.pad(bits, (0,25 * 25 - len(bits))), (25, 25))

cv2.imwrite('../output/output.png', image * 255)