import matplotlib.pyplot as plt

filenames = ['stats_' + str(i) + '.txt' for i in range(0, 4)]
d = [25, 50, 75, 100]

xVal = [2 ** i for i in range(1, 16)]

i = 0

plt.xlabel('Graph size (number of vertices')
plt.ylabel('Runtime (seconds)')

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
    plt.subplot(2, 2, i+1)
    plt.plot(xVal, yVal1)
    plt.plot(xVal, yVal2)
    plt.legend(['DFS', 'Kahn'])
    plt.title('Runtime vs Graph Size at ' + str(d[i]) + '% edge density')
    i += 1

plt.show()
