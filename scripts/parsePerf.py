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
#print(sys.argv[1])
#print f
#f.readline()
LLC_LM=0
LLC_SM=0
LLC_PM=0
LLC_CM=0

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
	#ARM CacheMiss
	elif "cache-misses" in line:
		t = line.split()
		LLC_CM = int(t[0].replace(',',''))
		print('CACHEMISS (ARM) count:'+str(INSTR_CNT))
		
#Format to CSV
MIPS = INSTR_CNT/TIME

LLCMS = (LLC_LM+LLC_SM+LLC_PM)/TIME
#ARM
#LLCMS = LLC_CM/TIME

print("LLCMS = "+str(LLCMS)+"  MIPS ="+str(MIPS))

filename = '../results/'+bench+'_perfstats.csv'
fields = [bench,run,LLC_LM,LLC_SM,LLC_PM,LLC_CM,INSTR_CNT,TIME,LLCMS,MIPS]
with open(filename,'a') as csvv:
	writer = csv.writer(csvv)
	writer.writerow(fields)
#Do I need to close file and writer?????
