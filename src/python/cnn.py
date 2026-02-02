import torch
import cv2
import numpy as np
import noise
import subprocess
import os.path

class Distorter():

    def __init__(self):
        self.background = self.getBackground()
        self.overlay = self.getOverlay()
        

    def getBackground(self):
        print('new backgorund')
        background = np.ones((1024, 1024, 3)) * 0.5
        background[:][:][2] = 0.5
        background[:][:][1] = 0.5
        for i in range(len(background)):
            for j in range(len(background[i])):
                background[i][j][0] = (noise.snoise2(i / 200, j / 200) * 0.5 + 0.5) * 256

        background = np.float32(cv2.cvtColor(np.float32(background), cv2.COLOR_HSV2RGB))
        return background

    def getOverlay(self):
        overlay = np.zeros((1024, 1024, 3))
        for i in range(len(overlay)):
            for j in range(len(overlay[i])):
                overlay[i][j][0] += (noise.snoise3(i * 2, j * 2, 0) * 0.1)
                overlay[i][j][1] += (noise.snoise3(i * 2, j * 2, 1) * 0.1)
                overlay[i][j][2] += (noise.snoise3(i * 2, j * 2, 2) * 0.1)
        return overlay


    def distort(self, path, target):
        barcode = cv2.imread(path)
        y,x,c = barcode.shape
        barcode = cv2.resize(barcode, (x * 8, y * 8), interpolation=cv2.INTER_NEAREST)
        y,x,c = barcode.shape

        proj_from = np.float32([[0,0],[x,0],[0,y],[x,y]])
        proj_to = np.float32(target)

        transform_matrix = cv2.getPerspectiveTransform(proj_from, proj_to)
        barcode_proj = np.float32(cv2.warpPerspective(barcode, transform_matrix, (1024, 1024))) / 256

        mask = np.zeros((1024, 1024, 3))
        mask = cv2.fillPoly(mask, pts=[np.int32(proj_to[[0,1,3,2]])], color=(1,1,1))

        if (np.random.uniform() > 0.97):
            self.background = self.getBackground()
        background = self.background
        overlay = self.overlay

        barcode_background = barcode_proj * mask + background * (1-mask) + overlay

        distorted = np.uint8(np.clip(barcode_background, 0, 255/256) * 256)
        return distorted


class CNN(torch.nn.Module):
    def __init__(self):
        super().__init__()
        self.net = torch.nn.Sequential(
            torch.nn.Conv2d(3,6,3, padding='same'),
            torch.nn.MaxPool2d((4,4)),
            torch.nn.Conv2d(6,6,3, padding='same'),
            torch.nn.MaxPool2d((4,4)),
            torch.nn.Flatten(),
            torch.nn.Linear(6*32*32, 64),
            torch.nn.ReLU(),
            torch.nn.Linear(64, 64),
            torch.nn.ReLU(),
            torch.nn.Linear(64, 2)
        )

    def forward(self, x):
        # print(x.shape)
        x = self.net(x)
        return x
    
def GenImage(ids):
    for id in ids:
        randomData = str(np.random.random(10000))
        subprocess.Popen(['./output/bin/Encoder', randomData[:9000], 'output/img/cnn/img_raw_' + str(id) + '.png'])

def GetDistorted(id, distorter: Distorter):
    path = 'output/img/cnn/img_raw_' + str(id) + '.png'
    theta = np.random.uniform(size=1) * 0.5 - 0.25
    magnitude = np.random.uniform(size=1) * 100 + 600
    delta = [np.cos(theta[0]) * magnitude[0], np.sin(theta[0]) * magnitude[0]]
    target = np.array([[0,0], [delta[0], delta[1]], [-delta[1], delta[0]], [delta[0]-delta[1], delta[1]+delta[0]]])
    offset = np.array([delta[0]-delta[1], delta[1]+delta[0]]) / 2
    target = target - offset + 512 + np.random.uniform(size=(4,2)) * 50
    # print(target)
    distorted = distorter.distort(path, target)
    if np.random.uniform(size=1) > 0.9:
        cv2.imwrite('output/img/temp/img_' + str(np.floor(np.random.uniform(size=1) * 100)) + '.png', distorted)
    return distorted,np.float32(np.ndarray.flatten(np.array(target)) / 1024)


def GetImage(id):
    data = cv2.imread('output/img/cnn/img_' + str(id) + '.png')
    data = cv2.resize(data, (1024, 1024))
    data = np.swapaxes(data, 2, 0)
    data = np.float32(data)
    tensorData = torch.tensor(data)
    return tensorData

def train(model, optimiser, lossFn, target, data):
    model.train()

    data = torch.from_numpy(data).float()

    pred = model(data)
    loss = lossFn(pred, target)

    loss.backward()
    optimiser.step()
    optimiser.zero_grad()
    print('----------------------')
    print(loss)


def RunTraining():
    optimiser = torch.optim.SGD(model.parameters(), 0.00005)
    lossFn = torch.nn.MSELoss()

    while True:
        GenImage(np.arange(10))

        datas = []
        solutions = []
        distorter = Distorter()

        print('gen new images...')

        for i in range(70):
            for j in range(10):
                data,solution = GetDistorted(j, distorter)
                (sizeX, sizeY, sizeC) = data.shape
                data = data[:int(sizeX/2), :int(sizeY/2), :]
                data = cv2.cvtColor(data, cv2.COLOR_RGB2HSV)
                data = np.swapaxes(data, 0, 2)
                datas += [torch.tensor(data)]
                solutions += [solution[:2]]

        for i in range(20):
            if os.path.exists(modelSavePath):
                model.load_state_dict(torch.load(modelSavePath))
            print('training ' + str(i))
            train(model, optimiser, lossFn, torch.tensor(solutions), np.array(datas))
            torch.save(model.state_dict(), modelSavePath)

def RunModel(path, outputPath):
    data = cv2.imread(path)
    if os.path.exists(modelSavePath):
        model.load_state_dict(torch.load(modelSavePath))
    model.eval()

    data = data.swapaxes(0,2)
    dataTensor = torch.tensor(np.float32(data)).unsqueeze(0)
    with torch.no_grad():
        output = model(dataTensor)
    proj_from = np.float32(output[0]).reshape((4,2)) * 1024
    proj_to = np.float32([[0,0],[200, 0], [0, 200], [200, 200]])
    print(proj_from)
    transform_matrix = cv2.getPerspectiveTransform(proj_from, proj_to)
    proj = np.float32(cv2.warpPerspective(cv2.imread(path), transform_matrix, (200, 200)))

    cv2.imwrite(outputPath, proj)


model = CNN().to('cpu')
modelSavePath = './src/python/model_save'

RunTraining()
# for i in range(50):
#     RunModel('./output/img/temp/img_[' + str(i) + '.].png', './output/img/output_cnn_' + str(i) + '.png')

# make model simpler
# train to get [.] pattern (split into 4)
# generate all data into folder and batch train
# greyscale first, maybe with saturation
# dont rotate or skew too much at first