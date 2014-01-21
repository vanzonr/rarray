# Compiler settings for Makefile for rarray.h
#
# g++ 4.8 settings
#
CXX?=g++
CXXFLAGS?=-g -std=c++11 
CPPFLAGS=-Wall
CPPFLAGSOPT?=-DNDEBUG
CXXFLAGSOPT?=-O3 -fstrict-aliasing -ffast-math -march=native
