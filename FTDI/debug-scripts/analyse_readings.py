import numpy as np
from matplotlib import pyplot as plt

dat = np.loadtxt('readings.out')
plt.scatter(np.arange(len(dat)), dat)
plt.show()

plt.hist(dat, bins=20)
plt.show()
