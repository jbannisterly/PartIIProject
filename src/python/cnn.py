import torch
import cv2
import numpy as np
import noise
import subprocess
import os
import json

PATH_IMG_TRAIN = "output/cnn/training_data/img/"
PATH_IMG_TEST = "output/cnn/testing_data/img/"
PATH_IMG_TEMP = "output/cnn/training_data/img_temp"
PATH_TRAINING_DATA = "output/cnn/training_data/targets"
PATH_TESTING_DATA = "output/cnn/testing_data/targets"
PATH_MODEL_SAVE = "output/cnn/models/"

class Distorter():

    def __init__(self):
        self.background = self.getBackground()
        self.overlay = self.getOverlay()
        

    def getBackground(self):
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
        final_position = cv2.perspectiveTransform(np.float32([[[35.5, 35.5]]]), transform_matrix)[0][0]
        barcode_proj = np.float32(cv2.warpPerspective(barcode, transform_matrix, (1024, 1024))) / 256

        mask = np.zeros((1024, 1024, 3))
        mask = cv2.fillPoly(mask, pts=[np.int32(proj_to[[0,1,3,2]])], color=(1,1,1))

        if (np.random.uniform() > 0.97):
            self.background = self.getBackground()
        background = self.background
        overlay = self.overlay

        barcode_background = barcode_proj * mask + background * (1-mask) + overlay

        distorted = np.uint8(np.clip(barcode_background, 0, 255/256) * 256)

        return final_position, distorted


class CNN(torch.nn.Module):
    def __init__(self, modules: torch.nn.Module):
        super().__init__()
        self.net = torch.nn.Sequential(
            *modules
        )

    def forward(self, x):
        # print("forward begins")
        # print(x.shape)
        x = self.net(x)
        return x
    

def GetModules():
    return [
        torch.nn.Conv2d(3,16,3, padding='same'),
        torch.nn.MaxPool2d((4,4)),
        torch.nn.Conv2d(16,32,3, padding='same'),
        torch.nn.MaxPool2d((4,4)),
        torch.nn.Flatten(),
        torch.nn.Linear(32*32*32, 16),
        torch.nn.ReLU(),
        torch.nn.Linear(16, 16),
        torch.nn.ReLU(),
        torch.nn.Linear(16, 2)
    ]

def CreateModules(convSizes, kernelSizes, poolSizes, linearSizes):
    module = []
    imageChannels = 3
    imageWidth = 512

    for (conv, kern, pool) in zip(convSizes, kernelSizes, poolSizes):
        module += [torch.nn.Conv2d(imageChannels, conv, kern, padding='same')]
        module += [torch.nn.MaxPool2d((pool, pool))]
        imageChannels = conv
        imageWidth /= pool

    networkWidth = imageWidth * imageWidth * imageChannels
    module += [torch.nn.Flatten()]

    for linear in linearSizes:
        module += [torch.nn.Linear(int(networkWidth), int(linear))]
        module += [torch.nn.BatchNorm1d(int(linear))]
        module += [torch.nn.ReLU()]
        networkWidth = linear

    module += [torch.nn.Linear(networkWidth, 2)]
    
    return module



def PixelError(lossResult):
    return np.sqrt(lossResult) * 512


def GenImage(ids, savePath):
    for id in ids:
        randomData = np.random.random(10000)
        subprocess.Popen(['./output/bin/Encoder', "' ".join(str(x) for x in randomData[:1200]) + "'", savePath + str(id) + '.png'])

def GetDistorted(id, distorter: Distorter):
    path = 'output/img/cnn/img_raw_' + str(id) + '.png'
    theta = np.random.uniform(size=1) * 0.5 - 0.25
    magnitude = np.random.uniform(size=1) * 100 + 600
    delta = [np.cos(theta[0]) * magnitude[0], np.sin(theta[0]) * magnitude[0]]
    target = np.array([[0,0], [delta[0], delta[1]], [-delta[1], delta[0]], [delta[0]-delta[1], delta[1]+delta[0]]])
    offset = np.array([delta[0]-delta[1], delta[1]+delta[0]]) / 2
    target = target - offset + 512 + np.random.uniform(size=(4,2)) * 50
    # print(target)
    finder_centre, distorted = distorter.distort(path, target)
    if np.random.uniform(size=1) > 0.9:
        cv2.imwrite('output/img/temp/img_' + str(np.floor(np.random.uniform(size=1) * 100)) + '.png', distorted)
    return distorted,np.float32(finder_centre / 1024)


def PrepareImage(path):
    data = cv2.imread(path)
    data = cv2.resize(data, (512, 512))
    data = cv2.cvtColor(data, cv2.COLOR_RGB2HSV) / 255
    data = np.swapaxes(data, 0, 2)
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
    return loss.item()

def LoadImages(solutionPath: str, imagePath: str):
    with open(solutionPath, 'r') as solutionFile:
        trainingDataInfo = json.load(solutionFile)

    datas = []
    solutions = []

    for imgName in trainingDataInfo:

        data = PrepareImage(PATH_IMG_TRAIN + imgName + '.png')
        datas += [data]

        solutionLocation = trainingDataInfo[imgName]
        solutions += [[float(solutionLocation['x']), float(solutionLocation['y'])]]

    print('loaded images')

    solutions = np.float32(solutions) * 2
    return datas, solutions




def ModelTrain(iterations: int, modelPath: str, model: CNN):
    optimiser = torch.optim.SGD(model.parameters(), 0.01)
    lossFn = torch.nn.MSELoss()
    solutions = []
    losses = []

    datas, solutions = LoadImages(PATH_TRAINING_DATA, PATH_IMG_TRAIN)

    for i in range(iterations):
        loss = train(model, optimiser, lossFn, torch.tensor(solutions), np.array(datas))
        # for param in model.parameters():
        #     print(param)
        # print(str(loss) + ' ' + str(PixelError(loss)))
        losses.append(loss)

    if not os.path.exists(os.path.dirname(modelPath)):
        os.mkdir(os.path.dirname(modelPath))
    torch.save(model.state_dict(), modelPath)

    return losses
            

def ModelTest(modelPath: str):
    if os.path.exists(modelPath):
        model.load_state_dict(torch.load(modelPath))

        # print('------')
        # for param in model.parameters():
        #     print(param.shape)
        #     print(param)
        # print('------')


        datas, solutions = LoadImages(PATH_TESTING_DATA, PATH_IMG_TEST)

        error : torch.Tensor = 0

        for data,solution in zip(datas,solutions):
            # print(data.shape)
            prediction = ModelPredict(model, data)
            # print(str(prediction) + ' ' + str(solution))
            error += (prediction[0] - solution[0]) ** 2 + (prediction[1] - solution[1]) ** 2
        error /= len(datas)
        

        # print('final error ' + str(error))
        # print('final error pixels ' + str(PixelError(error)))
        print(error)
    return float(error.detach())


def ModelPredict(model: CNN, data: torch.Tensor):
    model.eval()
    
    with torch.no_grad():
        output: torch.Tensor = model(data[None, :, :, :])

    return output[0]

def InitialiseData(n0: int, n1: int, imgPath: str, solPath: str):
    solutions = {}
    distorter = Distorter()

    GenImage(np.arange(n0), PATH_IMG_TEMP)
    
    for i in range(n0):
        for j in range(n1):
            data,solution = GetDistorted(j, distorter)
            (sizeX, sizeY, sizeC) = data.shape
            data = data[:int(sizeX/2), :int(sizeY/2), :]
            print("writing")
            id = str(i) + '_' + str(j)
            cv2.imwrite(imgPath + id + '.png', data)
            solutions[id] = {}
            solutions[id]['x'] = str(solution[0])
            solutions[id]['y'] = str(solution[1])

    with open(solPath, 'w') as solutionFile:
        print(solutions)
        json.dump(solutions, solutionFile)





# InitialiseData(10, 10, PATH_IMG_TRAIN, PATH_TRAINING_DATA)
# InitialiseData(10, 10, PATH_IMG_TEST, PATH_TESTING_DATA)


convolve0 = [4, 16, 32]
kernel0 = [3]
kernel1 = [3, 5]
convolve1 = [4, 16, 32]
linear0 = [8, 16, 32, 64]
linear1 = [8, 16, 32, 64]
pool0 = [4, 8]
pool1 = [8]

for c0 in convolve0:
    for k0 in kernel0:
        for c1 in convolve1:
            for k1 in kernel1:
                for l0 in linear0:
                    for l1 in linear1:
                        for p0 in pool0:
                            for p1 in pool1:
                                model = CNN(CreateModules([c0, c1], [k0, k1], [p0, p1], [l0, l1])).to('cpu')
                                model_id = '_'.join([str(c0), str(c1), str(k0), str(k1), str(p0), str(p1), str(l0), str(l1)])  
                                train_error = ModelTrain(50, PATH_MODEL_SAVE + 'model_' + model_id, model)
                                test_error = ModelTest(PATH_MODEL_SAVE + 'model_' + model_id)
                                results = {}
                                results['train'] = train_error
                                results['test'] = test_error

                                with open(PATH_MODEL_SAVE + 'results_' + model_id, 'w') as resultsFile:
                                    json.dump(results, resultsFile)
