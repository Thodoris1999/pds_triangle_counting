
import matplotlib.pyplot as plt


with open('timesv1.txt') as f:
    xs = []
    ys = []
    arr =  next(f).split() #first line
    xs.append(int(arr[0]))
    ys.append(float(arr[1]))
    for line in f: # read rest of lines
        arr = line.split()
        xs.append(int(arr[0]))
        ys.append(float(arr[1]))

    plt.plot(xs, ys, label="v1")    

with open('timesv2.txt') as f:
    xs = []
    ys = []
    arr =  next(f).split() #first line
    xs.append(int(arr[0]))
    ys.append(float(arr[1]))
    for line in f: # read rest of lines
        arr = line.split()
        xs.append(int(arr[0]))
        ys.append(float(arr[1]))

    plt.plot(xs, ys, label="v2")

plt.legend()
plt.show()
