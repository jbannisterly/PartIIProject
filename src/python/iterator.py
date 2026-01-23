import subprocess

initial_guess = str(subprocess.check_output(['./output/bin/Lib_Bounds'])).split("\\n")
initial_bounds = initial_guess[-11:-3]

print(initial_bounds)