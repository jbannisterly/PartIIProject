import bitarray as ba
import cv2
import numpy as np

image = cv2.imread("output/img/output.png")
pixels = image.reshape(25 * 25, 3)
bitArray = [1 if pixel[0] > 127 else 0 for pixel in pixels]
lengthArray = np.packbits(bitArray[0:16])
length = int(lengthArray[0]) + int(lengthArray[1]) * 256  
bytes = bytearray(np.packbits(bitArray[16:length + 16]))

f = open("output/img/output.txt", "w+")
f.write(bytes.decode())
f.close()