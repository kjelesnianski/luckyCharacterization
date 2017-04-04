#!/usr/bin/env python3

import sys
#get all CSV files
import glob
#pandas 
import pandas as pd
import numpy as np
#Linear Regression
from sklearn import datasets, linear_model
#Graphing
import matplotlib as mpl
import matplotlib.pyplot as plt
import pickle

#Care about LLCMS & MIPS (elements 7,8 starting from 0)

#Benchmark Class
b_class = sys.argv[1]


#--------- Format to Panda input ----------------#
#Grab All CSV files
#Per Class
csv_files = sorted(glob.glob('../results/*_'+b_class+'_perfstats.csv'))
csv_files_ARM = sorted(glob.glob('../results/*_'+b_class+'_perfstats_ARM.csv'))
#Combined
#csv_files = sorted(glob.glob('../results/*perfstats.csv'))
#csv_files_ARM = sorted(glob.glob('../results/*perfstats_ARM.csv'))

print('------ X86 Files ------')
print(csv_files)
print('------ ARM Files ------')
print(csv_files_ARM)

#Create Series and append all CSV files to the one series
header = ['Benchmark','Class','NumThreads','Run', 'LLC-load-miss','LLC-store-miss','LLC-prefetch-miss','L2D-Refill','instructions','time','LLCMS','MIPS']

#iterate and dump contents into series
curr_series = None
target_series = pd.DataFrame(columns=header)
total_series = pd.DataFrame(columns=header)

for f in csv_files:
	#create current series
	curr_series = pd.read_csv(f, header=None)
	curr_series.columns = header
	#append
	total_series = total_series.append(curr_series, ignore_index=True)
	#print(total_series)

print('--------------- Total X ---------------')
print(total_series)

for ff in csv_files_ARM:
  #create current series
  tmp_series = pd.read_csv(ff, header=None)
  tmp_series.columns = header
  #append
  target_series = target_series.append(tmp_series, ignore_index=True)
  #print(target_series)

#should now have total Series
print('--------------- Total Y ---------------')
print(target_series)

#Get MIPS+LLCMS FEATURE (x86)
feature_X = total_series.loc[:,['MIPS','LLCMS']]
print('--------------- FEATURE X ---------------')
print(feature_X)

#Split DATA into training/test sets
# .sample returns a random sample of items from an axis of object.
# DO NOT WANT SAMPLE NEED DATA TO MATCH  (X86 <> ARM)
feature_X_train = feature_X[::2]
feature_X_test = feature_X[1::2]

#reformated for LinearRegression...
#feature_X_train = feature_X_train_raw.reshape(320,1) # 50% of samples
#feature_X_test = feature_X_test_raw.reshape(320,1) # 50% of samples

print('--------------- FEATURE X TRAIN ---------------')
print(feature_X_train.values)
print('--------------- FEATURE X TEST ---------------')
print(feature_X_test.values)


#Split TARGET into training/test sets
print('------------------FEATURE Y----------------------------')
feature_Y = None
feature_Y_train = None
feature_Y_test = None

#Get TARGET FEATURE (ARM MIPS)
feature_Y = target_series.loc[:,'MIPS'].div(1000000)
#print(feature_Y)

feature_Y_train_raw = feature_Y[::2]
feature_Y_test_raw = feature_Y[1::2]

feature_Y_train = feature_Y_train_raw.reshape(320,1)
feature_Y_test = feature_Y_test_raw.reshape(320,1)
#print('------------------TARGET train----------------------------')
#print(feature_Y_train)
#print('------------------TARGET test----------------------------')
#print(feature_Y_test)


#Create Multiple Linear Regression Object
m_regr = linear_model.LinearRegression()

#Train using training Sets
# We are doing MULTIPLE LINEAR REGRESSION!!
# Equation is: MIPS(j,k) = A(j) * MIPS(i,k) + B(j) * LLCMS(i,k) + Y(j)
m_regr.fit(feature_X_train, feature_Y_train)

#Coefficients
print('Coefficients: \n', m_regr.coef_)
#Mean Squared Error
#print("Mean Squared Error: %.2f" % np.mean((m_regr.predict(feature_X_test) - feature_Y_test) ** 2))
#Explained Variance score: 1 is perfect prediction
print('Variance score: %.2f' % m_regr.score(feature_X_test, feature_Y_test))

###############################################################################
###############################################################################
# 3D Plot
print('--------------------- Type ----------------------------------')
print(feature_X_test['MIPS'])
#print(type(feature_X_test.iloc[:,'MIPS'].reshape(320,1)))
print(feature_Y_test)
from mpl_toolkits.mplot3d import Axes3D

print('--------------------- Ploting ----------------------------------')
#mpl.rcParams['legend.fontsize'] = 10
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
#ax.scatter(feature_X_test.iloc[:,'MIPS'], feature_X_test.iloc[:,'LLCMS'], feature_Y_test, marker='o')
ax.scatter(feature_X_test['MIPS'], feature_X_test['LLCMS'], zs=feature_Y_test, marker='o')

ax.set_xlabel('MIPS (x86_64)')
ax.set_ylabel('LLCMS (x86_64)')
ax.set_zlabel('MIPS (aarch64)')

#ax.plot(x, y, z, label='MIPS(arm) vs MIPS(x86) & LLCMS(x86)')
#ax.plot(feature_Y_test, feature_Y_test, feature_Y_test, label='MIPS(arm) vs MIPS(x86) & LLCMS(x86)')
#ax.legend()

plt.show()

#with open('Figure3D.fig.pickle','wb') as fp:
#	pickle.dump(fig, fp)

plt.savefig('3D_MIPS_LLCMS_'+b_class+'.png')
