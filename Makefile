TOPDIR:=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))/
CC = g++
$(info $(TOPDIR))
$(info )

CFLAGS = -O2 -std=c++11 
DEBUG = -g 
LDLIBS = -L/usr/local/lib -L/usr/local/include -lntl -lm -lrt -lstdc++

.SUFFIXES: .cpp

./%: %.cpp
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $< 

./%_x: %.cpp
	$(CC) $(CFLAGS) -o $@ $< 
	@echo  $< can be compiled!! great!!
	@echo 

