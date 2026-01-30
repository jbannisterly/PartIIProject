import torch
import cv2
import numpy as np
import noise
import subprocess

class Distorter():
    def distort(path, target):
        barcode = cv2.imread("output/img/output.png")
        y,x,c = barcode.shape
        barcode = cv2.resize(barcode, (x * 8, y * 8), interpolation=cv2.INTER_NEAREST)
        y,x,c = barcode.shape

        proj_from = np.float32([[0,0],[x,0],[0,y],[x,y]])
        proj_to = np.float32(target)

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

        distorted = np.uint8(np.clip(barcode_background, 0, 255/256) * 256)
        return distorted


class CNN(torch.nn.Module):
    def __init__(self):
        super().__init__()
        self.net = torch.nn.Sequential(
            torch.nn.Conv2d(3,6,7, padding='same'),
            torch.nn.MaxPool2d((4,4)),
            torch.nn.Conv2d(6,16,5, padding='same'),
            torch.nn.MaxPool2d((4,4)),
            torch.nn.Conv2d(16,16,5, padding='same'),
            torch.nn.MaxPool2d((4,4)),
            torch.nn.Flatten(),
            torch.nn.Linear(16*16*16, 64),
            torch.nn.ReLU(),
            torch.nn.Linear(64, 64),
            torch.nn.ReLU(),
            torch.nn.Linear(64, 8)
        )

    def forward(self, x):
        x = self.net(x)
        print(x)
        return x
    
def GenImage(ids):
    for id in ids:
        randomData = str(np.random.random(1000))
        subprocess.Popen(['./output/bin/Encoder', randomData[:9000], 'output/img/cnn/img_raw_' + str(id) + '.png'])

def GetImage(id):
    data = cv2.imread('output/img/cnn/img_' + str(id) + '.png')
    data = cv2.resize(data, (1024, 1024))
    data = np.swapaxes(data, 2, 0)
    data = np.float32(data)
    tensorData = torch.tensor(data)
    return tensorData

def train(model, optimiser, lossFn):
    model.train()

    data = torch.stack([GetImage(0)])
    target = torch.tensor([1.1] * 8)

    pred = model(data)
    loss = lossFn(pred, target)

    loss.backward()
    optimiser.step()
    optimiser.zero_grad()
    print(pred)


model = CNN().to('cpu')
optimiser = torch.optim.SGD(model.parameters(), 0.0001)
lossFn = torch.nn.MSELoss()

GenImage(np.arange(10))

for i in range(100):
    train(model, optimiser, lossFn)
