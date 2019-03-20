import numpy as np
from matplotlib import pyplot as plt

dat = np.loadtxt('readings.out')
plt.scatter(np.arange(len(dat)), dat)
plt.ylim(0, 5)
plt.show()

plt.hist(dat, bins=500, range=(0, 5))
plt.show()
