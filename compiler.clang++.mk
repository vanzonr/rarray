# Compiler settings for Makefile for rarray.h
#
# clang++ 3.5
#
CXX?=clang++
FC?=gfortran
CXXFLAGS?=-g -std=c++11 
CPPFLAGS=-Wall
CPPFLAGSOPT?=-DNDEBUG
CXXFLAGSOPT?=-O3 -fstrict-aliasing -ffast-math -march=native
FFLAGSOPT?=-O3 -fstrict-aliasing -ffast-math -march=native
