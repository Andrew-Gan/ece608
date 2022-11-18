import matplotlib.pyplot as plt

density_filenames = ['stats_density_' + str(i) + '.txt' for i in range(0, 4)]
depth_filenames = ['stats_depth_' + str(i) + '.txt' for i in [100, 1000, 10000, 100000]]
filenames = density_filenames + depth_filenames

d = [25, 50, 75, 100]

xVal = [2 ** i for i in range(1, 16)]

i = 0

for filename in filenames:
    yVal1 = []
    yVal2 = []
    with open(filename, 'r') as f:
        newline = f.readline()
        while newline:
            leftVal = newline.split(' ')[0]
            rightVal = newline.split(' ')[1]
            yVal1.append(float(leftVal))
            yVal2.append(float(rightVal))
            newline = f.readline()
    plt.figure(figsize=[4, 4])
    plt.xlabel('Graph size (number of vertices)')
    plt.ylabel('Runtime (seconds)')
    plt.plot(xVal, yVal1)
    plt.plot(xVal, yVal2)
    plt.legend(['DFS', 'Kahn'])
    plt.tight_layout()
    plt.savefig('graphs/' + filename + '.png')
    plt.clf()
    i += 1
