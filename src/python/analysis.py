import os
import json
from matplotlib import pyplot as plt
import numpy as np

PATH_MODEL = './output/cnn/models'
PATH_GRAPHS = './output/cnn/graphs/'

directory = os.listdir(PATH_MODEL)
results = {}

for file in directory:
    if file[0] == 'r':
        with open(PATH_MODEL + '/' + file, 'r') as result:
            results[file] = json.load(result)
            results[file]['name'] = file

results_list = list(results.values())

sorted_results = sorted(results_list, key=lambda res: res['test'])

rank = 0
for result in sorted_results:
    print(result['name'] + ' ' + str(result['test']))
    plt.plot(np.arange(0, 50), np.log2(result['train']))
    plt.savefig(PATH_GRAPHS + '_' + str(rank) + '_' + result['name'] + '.png')
    rank += 1
