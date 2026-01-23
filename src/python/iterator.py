import subprocess
import numpy as np
from scipy.optimize import minimize

def MakeGuess(bounds):
  bound_str = [str(x) for x in bounds]
  error_count = str(subprocess.check_output(['./output/bin/Lib_Count'] + bound_str + ['100', '100'])).split('\\n')
  n_errors = error_count[-2]
  print(bounds)
  print(n_errors)
  return float(n_errors)

initial_guess = str(subprocess.check_output(['./output/bin/Lib_Bounds'])).split("\\n")
initial_bounds = initial_guess[-11:-3]

print(initial_bounds)

methods = ['Nelder-Mead', 'Powell', 'COBYLA', 'COBYQA']

minimize(MakeGuess, initial_bounds, method=methods[0])