import subprocess
import time
import numpy as np
import matplotlib.pyplot as plot

testfile = "lab2/tests/test3.txt"
out = open("lab2/tests/output.txt", 'wb')

figure = plot.figure()

ax1 = figure.add_subplot(1,1,1)

N = np.arange(1, 10)
T = np.zeros_like(N)

for i in N:
    file = open(testfile, 'r')
    t = np.zeros(1)
    for _ in range(1):
        t0 = time.perf_counter()
        # print(["./build/main2", f"{i}"])
        subprocess.call(["./build/main2", f"{i}"], stdin=file, stdout=out, shell=False)
        t1 = time.perf_counter()
        t[_] = (t1 - t0) * 10000
    T[i - 1] = min(t)

ax1.plot(N, T)

plot.show()