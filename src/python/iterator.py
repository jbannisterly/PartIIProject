import subprocess

initial_guess = str(subprocess.check_output(['./output/bin/Lib_Bounds'])).split("\\n")
initial_bounds = initial_guess[-11:-3]

print(initial_bounds)

error_count = str(subprocess.check_output(['./output/bin/Lib_Count'] + initial_bounds + ['100', '100'])).split('\\n')
n_errors = error_count[-2]

print(n_errors)