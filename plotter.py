import matplotlib.pyplot as plt

xVal = [2 ** i for i in range(1, 16)]
yval1 = []
yval2 = []

with open('stats.txt', 'r') as f:
    newline = f.readline()
    while newline:
        leftVal = newline.split(' ')[0]
        rightVal = newline.split(' ')[1]
        yval1.append(float(leftVal))
        yval2.append(float(rightVal))
        newline = f.readline()

plt.plot(xVal, yval1)
plt.plot(xVal, yval2)
plt.legend(['DFS', 'Kahn'])
plt.show()
