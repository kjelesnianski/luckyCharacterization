#!/usr/bin/env python3

#get all CSV files
import glob
#pandas 
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from sklearn import datasets, linear_model

#Only Care about LLCMS & MIPS (elements 7,8 starting from 0)

#--------- Format to Panda input ----------------#
#Grab All CSV files
csv_files = sorted(glob.glob('../results/*.csv'))
print(csv_files)
#Create Series and append all CSV files to the one series
header = ['Benchmark','NumThreads','Run', 'LLC-load-miss','LLC-store-miss','LLC-prefetch-miss','L2D-Refill','instructions','time','LLCMS','MIPS']

#iterate and dump contents into series
curr_series = None
target_series = None
#total_series = pd.DataFrame(None, columns=header)
total_series = pd.DataFrame(columns=header)
#total_series.columns = header

for f in csv_files:
	#create current series
	curr_series = pd.read_csv(f, header=None)
	curr_series.columns = header
	#append
	total_series = total_series.append(curr_series, ignore_index=True)
	#print(total_series)

#should now have total Series
#print(curr_series)

print(total_series)

#Get MIPS+LLCMS FEATURE (x86)
feature_X = total_series.loc[:,['MIPS','LLCMS']]
print(feature_X)
#Get LLCMS FEATURE (x86)
#feature = curr_series['LLCMS']

#Get TARGET FEATURE (ARM MIPS)
#feature_target = target_series['MIPS']

#Split DATA into training/test sets
# .sample returns a random sample of items from an axis of object.
feature_X_train = feature_X.sample(frac=.75)
feature_X_test = feature_X.sample(frac=.25)

print(feature_X_train)
print(feature_X_test)

#Split TARGET into training/test sets
feature_target_mips_train = None
feature_target_mips_test = None

#Create Multiple Linear Regression Object
#m_regr = linear_model.LinearRegression()

#Train using training Sets
# We are doing MULTIPLE LINEAR REGRESSION!!
# Equation is: MIPS(j,k) = A(j) * MIPS(i,k) + B(j) * LLCMS(i,k) + Y(j)
#m_regr.fit(feature_X_train, feature_target_mips_train)

#Coefficients
#print('Coefficients: \n', m_regr.coef_)
#Mean Squared Error
#print("Mean Squared Error: %.2f" % np.mean((m_regr.predict(feature_X_test) - feature_target_mips_test) ** 2))
#Explained Variance score: 1 is perfect prediction
#print('Variance score: %.2f' % m_regr.score(feature_X_test, feature_target_mips_score))

#Plot (But need X11 so do on local computer)
#
#plt.scatter(feature_X_test, feature_target_mips_test, color='black')
#plt.plot(feature_X_test, m_regr.predict(feature_X_test), color='blue', linewidth=3)
#plt.xticks(())
#plt.yticks(())
#plt.show()

