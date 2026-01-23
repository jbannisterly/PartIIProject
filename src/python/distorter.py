import bitarray as ba
import cv2
import numpy as np
import noise

barcode = cv2.imread("output/img/output.png")
y,x,c = barcode.shape
barcode = cv2.resize(barcode, (x * 8, y * 8), interpolation=cv2.INTER_NEAREST)
y,x,c = barcode.shape

proj_from = np.float32([[0,0],[x,0],[0,y],[x,y]])
proj_to = np.float32([[40,205],[940, 256],[100,1520],[1028,1488]])

transform_matrix = cv2.getPerspectiveTransform(proj_from, proj_to)
barcode_proj = np.float32(cv2.warpPerspective(barcode, transform_matrix, (1080, 1920))) / 256

mask = np.zeros((1920, 1080, 3))
mask = cv2.fillPoly(mask, pts=[np.int32(proj_to[[0,1,3,2]])], color=(1,1,1))

background = np.ones((1920, 1080, 3)) * 0.5
background[:][:][2] = 0.5
background[:][:][1] = 0.5
for i in range(len(background)):
    for j in range(len(background[i])):
        background[i][j][0] = (noise.snoise2(i / 200, j / 200) * 0.5 + 0.5) * 256


background = np.float32(cv2.cvtColor(np.float32(background), cv2.COLOR_HSV2RGB))

barcode_background = barcode_proj * mask + background * (1-mask)

for i in range(len(barcode_background)):
    for j in range(len(barcode_background[i])):
        barcode_background[i][j][0] += (noise.snoise3(i * 2, j * 2, 0) * 0.1)
        barcode_background[i][j][1] += (noise.snoise3(i * 2, j * 2, 1) * 0.1)
        barcode_background[i][j][2] += (noise.snoise3(i * 2, j * 2, 2) * 0.1)

for i in range(70):
    for j in range(70):
        barcode_background[i + len(barcode_background) // 2][j + len(barcode_background[i]) // 2][0] = 1
        barcode_background[i + len(barcode_background) // 2][j + len(barcode_background[i]) // 2][1] = 1
        barcode_background[i + len(barcode_background) // 2][j + len(barcode_background[i]) // 2][2] = 0





cv2.imwrite("output/img/output_distorted.png", np.uint8(np.clip(barcode_background, 0, 255/256) * 256))