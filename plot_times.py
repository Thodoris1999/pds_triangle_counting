#!/usr/bin/python
import matplotlib.pyplot as plt
import sys

assert len(sys.argv) == 2
name = sys.argv[1] # can be v3 or v4 + "_" + omp or pthreads or cilk

with open(name+"_times.txt") as f:
    xs = []
    ys = []
    arr =  next(f).split() #first line
    xs.append(int(arr[0]))
    ys.append(float(arr[1]))
    for line in f: # read rest of lines
        arr = line.split()
        xs.append(int(arr[0]))
        ys.append(float(arr[1]))

    plt.plot(xs, ys, label=name)    


plt.legend()
plt.show()
