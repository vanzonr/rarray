# Compiler settings for Makefile for rarray.h
#
# intel compiler settings
#
CXX?=icpc
CXXFLAGS?=-g -std=c++11 
CPPFLAGS=-Wall
CPPFLAGSOPT?=-DNDEBUG
CXXFLAGSOPT?=-O3 -xhost -ansi-alias -no-prec-div -fp-model fast=2 -funroll-loops
