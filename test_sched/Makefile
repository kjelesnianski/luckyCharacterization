# Makefile
# Author:	K Jelesnianski
# Date:		5/11/2017
#
# Makefile for Scheduler test suite 

######### Definitions #########################################################
CC=gcc

CLFAGS= -c -Wall
######### Recipes #############################################################
all: system kris_task

system: kris_system_fair.o
	$(CC) kris_system_fair.o -o system

kris_system_fair.o: kris_system_fair.c
	$(CC) $(CFLAGS) kris_system_fair.c

kris_task: kris_task.o

kris_task.o: kris_task.c
	$(CC) $(CFLAGS) kris_task.c

######### Automated Runs ######################################################
test4:
	sudo ./system simple_4bias

test6:
	sudo ./system simple_6bias

######### Clearn ##############################################################
clean:
	rm system kris_task
