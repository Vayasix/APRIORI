TOPDIR:=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))/
CC = g++
$(info $(TOPDIR))
$(info )

CFLAGS = -O2 -std=c++11 
#CFLAGS+= -Wall -I/usr/local/include
DEBUG = -g 
LDLIBS = -L/usr/local/lib -L/usr/local/include -lntl -lm -lrt -lstdc++ -lboost_filesystem -lboost_system
CFLAGS+= -I/usr/local/include -I.

.SUFFIXES: .cpp

./%: %.cpp
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $< 

./%_x: %.cpp
	$(CC) $(CFLAGS) -o $@ $< 
	@echo  $< can be compiled!! great!!
	@echo 

