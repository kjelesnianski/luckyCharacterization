# Makefile
# Author:	K Jelesnianski
# Date:		5/11/2017
#
# Makefile for Scheduler test suite 

######### Definitions #########################################################
CC=gcc
CFLAGS=-c -Wall
######### Recipes #############################################################
all: system kris_task

kris_system_fair.o: kris_system_fair.c
	$(CC) $(CFLAGS) kris_system_fair.c
#gcc $(CFLAGS) kris_system_fair.c

kris_task.o: kris_task.c
	gcc $(CFLAGS) kris_task.c

system: kris_system_fair.o
	gcc kris_system_fair.o -o system

kris_task: kris_task.o
	gcc kris_task.o -o kris_task

######### Automated Runs ######################################################
#test4:
#	sudo ./system simple_4bias
#
#test6:
#	sudo ./system simple_6bias
#
######### Clearn ##############################################################
clean:
	rm -rf *.o system kris_task
