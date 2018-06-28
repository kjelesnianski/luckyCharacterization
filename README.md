#luckyCharacterization


edit edit edit

Desciption
----------------------------------------------------------------------

This is an initial implementation of a starting point for Popcorn
Linux's userspace (maybe inserted into kernel at later point) 
scheduler. 

Not really sure if this should be implemented in C or Python at this
point.

Goals:
----------------------------------------------------------------------
Minimize EDP (Energy Delay Product) (Watt/MIPS^2)
Maximize Inverse of EDP!

----------------------------------------------------------------------
Assumptions
----------------------------------------------------------------------

INPUTS
#################################################################
 > List of ISAs (or cores) on Het-System 
 > Desired reassignment "scheduling" Interval



OUTPUTS
 > Vector telling where to move each app or thread? (What Granularity do we want??)
 



##################################################################
PERSISTANT RULES
##################################################################

Have the following stats for the heterogeneous System
	> Peak Power (W)
	> Idle Power (W)
	> Avg. Power (W)
	> Capacity (MIPS - Millions of Instr. Per Second)

Measure (with perf)
	> retired_instructions
	> L3_cache_misses

Calculate
	> LLCMS (last level cache misses per Second)

	> Estimate Performance Behavior
		> k = current thread
		> i = core type k is currently running on
		> j = core type k could be assigned next sched interval
	MIPS(j,k) = alfa(j) * MIPS(i,k) + beta(j) * LLCMS(i,k) + yeta(j)
	> Regression co-efficients: derived from offline performance data collected
		> alfa
		> beta
		> yeta

	> Energy Efficiency Ratio (!!! MAIN ENGINE !!!)
		
To work within a Virtual Machine:
$ sudo apt-get install virtualbox

This shows you how to install Ubuntu on Oracle VirtualBox:
https://brb.nci.nih.gov/bdge/installUbuntu.html

NOTE: Will need an ISO file for Ubuntu 14.04.5 LTS (x86\_64)
http://releases.ubuntu.com/14.04/
