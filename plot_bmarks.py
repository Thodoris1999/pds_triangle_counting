#!/usr/bin/python
import matplotlib.pyplot as plt
import sys

assert len(sys.argv) == 2
name = sys.argv[1] # can be v3 or v4
methods = ["cilk","omp"]
if name == "v4":
    methods.append("pthreads")


for method in methods:
    filename = name+"_"+method+"_bmark.txt"
    with open(filename) as f:
        ys = []
        lines = f.readlines()
        for line in lines:
            ys.append(float(line))

        plt.plot(ys, label=method)


plt.title(name+" times with respect to the number of threads")
plt.legend()
plt.show()
