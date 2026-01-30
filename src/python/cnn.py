import torch
import cv2
import numpy as np

class CNN(torch.nn.Module):
    def __init__(self):
        super().__init__()
        self.net = torch.nn.Sequential(
            torch.nn.Flatten(),
            torch.nn.Linear(64*64, 64),
            torch.nn.ReLU(),
            torch.nn.Linear(64, 64),
            torch.nn.ReLU(),
            torch.nn.Linear(64, 8)
        )

    def forward(self, x):
        x = torch.max_pool2d(x, (16,16))
        x =  x.mean(0, True)
        x = torch.nn.Flatten(1)(x)
        print(x.shape)
        x = self.net(x)
        return x

def GetImage(id):
    data = cv2.imread('output/img/cnn/img_' + str(id) + '.png')
    data = cv2.resize(data, (1024, 1024))
    data = np.swapaxes(data, 2, 0)
    data = np.float32(data)
    tensorData = torch.tensor(data)
    return tensorData

def train(model, optimiser, lossFn):
    model.train()

    data = GetImage(0)
    target = torch.tensor([1.1] * 8)

    pred = model(data)
    loss = lossFn(pred, target)

    loss.backward()
    optimiser.step()
    optimiser.zero_grad()
    print(loss)


model = CNN().to('cpu')
optimiser = torch.optim.SGD(model.parameters())
lossFn = torch.nn.MSELoss()

for i in range(100):
    train(model, optimiser, lossFn)
