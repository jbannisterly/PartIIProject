import subprocess
import numpy as np
from scipy.optimize import minimize
import cv2

def MakeGuess(bounds):
  bound_str = [str(x * 2) for x in bounds]
  error_count = str(subprocess.check_output(['./output/bin/Lib_Count'] + bound_str + ['200', '200'])).split('\\n')
  n_errors = error_count[-2]
  print(bounds)
  print(n_errors)
  return float(n_errors)

initial_guess = str(subprocess.check_output(['./output/bin/Lib_Bounds'])).split("\\n")
initial_bounds = initial_guess[-11:-3]

print(initial_bounds)

input = cv2.imread('./output/img/output_distorted.png')
output = cv2.resize(input, None, None, 2, 2)
cv2.imwrite('output/img/output_distorted2.png', output)

methods = ['Nelder-Mead', 'Powell', 'COBYLA', 'COBYQA']

minimize(MakeGuess, initial_bounds, method=methods[0])