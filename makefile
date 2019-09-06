###########################################################
#Makefile for CS 530 Assignment #2
#Jacqueline Gronotte, Cesar Lopez, Samantha Quiroz
########################################################### 

CC = g++

FILE1 = xed.cpp
FILE2 = symtabreader.cpp

OUT = "xed"

all: 
        $(CC) $(FILE1) $(FILE2) -w -o $(OUT)
#######################[ EOF: Makefile ]###################