import numpy as np
import matplotlib.pyplot as plt

if __name__ == '__main__':
	plt.gca().set_aspect('equal', adjustable='box')
	cloud_1 = np.loadtxt('0.txt')
	c = ['r.', 'g.', 'b.', 'y.', 'm.']
	for i in range(cloud_1.shape[0]):
		plt.plot(cloud_1[i][0], cloud_1[i][1], c[int(cloud_1[i][2])])
	
	plt.show()