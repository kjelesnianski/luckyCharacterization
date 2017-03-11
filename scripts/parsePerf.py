#!/usr/bin/env python3
import argparse
import re
import glob
import sys
import tokenize
import csv

########## Begin
f = open(sys.argv[1],'r')
bench = sys.argv[2]
run = sys.argv[3]
n_thread = sys.argv[4]
arch = sys.argv[5]
#print(sys.argv[1])
#print f
#f.readline()
LLC_LM=0
LLC_SM=0
LLC_PM=0

LLC_CM=0
LLC_L1LM=0
LLC_L1SM=0

filename=None
field=None

if arch == "x86_64":
	for line in f:
		#Grab LLC-LM
		if "LLC-load-misses" in line:
			#print line
			t = line.split()
			LLC_LM = int(t[0].replace(',',''))
			print('llc_lm:'+str(LLC_LM))
		#Grab LLC-SM
		elif "LLC-store-misses" in line:
			t = line.split()
			LLC_SM = int(t[0].replace(',',''))
			print('llc_sm:'+str(LLC_SM))
		#Grab LLC-PM
		elif "LLC-prefetch-misses" in line:
			t = line.split()
			LLC_PM = int(t[0].replace(',',''))
			print('llc_pm:'+str(LLC_PM))
		#Grab Time
		elif "time elapsed" in line:
			t = line.split()
			TIME = float(t[0])
			print('time:'+str(TIME))
		#Grab Instruction Count
		elif "instructions" in line:
			t = line.split()
			INSTR_CNT = int(t[0].replace(',',''))
			print('instruction count:'+str(INSTR_CNT))
	#LLC-Calculation
	LLCMS = (LLC_LM+LLC_SM+LLC_PM)/TIME
	
elif arch == "aarch64":
	print('Im on aRM!')
	for line in f:
	#ARM CacheMiss
	#	if "cache-misses" in line:
	#		t = line.split()
	#		LLC_CM = int(t[0].replace(',',''))
	#		print('CACHEMISS (ARM) count:'+str(LLC_CM))
	#	elif "L1-dcache-load-misses" in line:
	#		t = line.split()
	#		LLC_L1LM = int(t[0].replace(',',''))
	#		print('L1 load-miss count:'+str(LLC_L1LM))
	#	elif "L1-dcache-store-misses" in line:
	#		t = line.split()
	#		LLC_L1SM = int(t[0].replace(',',''))
	#		print('L1 store-miss count:'+str(LLC_L1SM))
		if "r017" in line:
			t = line.split()
			L2D_REFILL = int(t[0].replace(',',''))
			print('L2 DCache Refill (miss):'+str(L2D_REFILL))
		#Grab Time
		elif "time elapsed" in line:
			t = line.split()
			TIME = float(t[0])
			print('time:'+str(TIME))
		elif "instructions" in line:
			t = line.split()
			INSTR_CNT = int(t[0].replace(',',''))
			print('instruction count:'+str(INSTR_CNT))
	#LLC Calculation ARM		
	LLCMS = L2D_REFILL/TIME
#end if

#Format to CSV
MIPS = INSTR_CNT/TIME
print("LLCMS = "+str(LLCMS)+"  MIPS ="+str(MIPS))

if arch == "x86_64":
	filename = '../results/'+bench+'_perfstats.csv'
elif arch == "aarch64":
	filename = '../results/'+bench+'_perfstats_ARM.csv'
	
fields = [bench,n_thread,run,LLC_LM,LLC_SM,LLC_PM,L2D_REFILL,INSTR_CNT,TIME,LLCMS,MIPS]
with open(filename,'a') as csvv:
	writer = csv.writer(csvv)
	writer.writerow(fields)
#Do I need to close file and writer?????
