#!/usr/bin/python
# -*- coding: utf-8 -*-
from scipy import misc
# import cv2
import matplotlib.pyplot as plt

img1 = misc.imread('babel.png')
img2 = misc.imread('babel2.png')

# img1 = cv2.imread('babel.png', 0)
# img2 = cv2.imread('babel2.png', 0)

img1_x = img1.shape[0]
img1_y = img1.shape[1]
img2_x = img1.shape[0]
img2_y = img1.shape[1]

if(img1_x == img2_x and img1_y == img2_y):
    img3 = img2 - img1 
else:
    print ("nie sa rowne wymiary")
    
print (img1_x,' ', img1_y)

plt.imshow(img3, cmap=plt.cm.gray)
plt.show()







# https://brain.fuw.edu.pl/edu/index.php/Pracownia_EEG/analiza_obrazu