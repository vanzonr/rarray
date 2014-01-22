# Compiler settings for Makefile for rarray.h
#
# intel compiler settings
#
CXX?=xlc++
FC=xlf
CXXFLAGS?=-g -qlanglvl=extended0x
CPPFLAGS=-Wall
CPPFLAGSOPT?=-DNDEBUG
CXXFLAGSOPT?=-O3 -qarch=pwr6 -qtune=pwr6 -qinline=level=10
FFLAGSOPT?=-O3 -qarch=pwr6 -qtune=pwr6
