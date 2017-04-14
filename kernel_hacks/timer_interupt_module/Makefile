# Makefile for Timer interrupt Module to keep track of perf counters,
# kernel side.
#
# Author:	K Jelesnianski
# Contact:	kjski@vt.edu
# Date:		04/13/17

obj-m		:= itrpt_module.o
KDIR		:= /home/user/linux

PWD			:= $(shell pwd)

all: itrpt_module.c
	make -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
make -C $(KDIR) SUBDIRS=$(PWD) clean
