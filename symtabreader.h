/****************************************************************************
Team:
Samantha Quiroz - CSSC0416
Jacqueline Gronotte - CSSC0415
Cesar Lopez - CSSC0552
Class: CS530, Spring 2019
Project: Assignment 2
File: symtabreader.h
Note: Header file for the symTabReader
****************************************************************************/
#ifndef SYMTABREADER_H
#define SYMTABREADER_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

class SYMTABReader {
    public:
        //symbol vector class
        class symbols {
            public:
                string symbolName;
                int addr;
                string flag;
                symbols(string n, int a, string f);
        };
        //literal vector class
        class literals {
            public:
                string literal;
                int length;
                int addr;
                literals(string li, int le, int a);
        };

    vector<symbols> SYMTAB; //stores contents of SYMTAB
    vector<literals> LITTAB; //stores contents of LITTAB

    /**
     * Function: Reads and stores the SYMTAB and LITTAB.
     * Parameters: string of the .sym file name
     * Return Values: N/A
     */
    void symTabRead(string filename);

    /**
     * Function: gets a symbol and its parameters
     * Parameters: index of the symbol
     * Return Values: symbol name, address, or flag
     */
    vector<symbols> getSymbolVector();

    /**
     * Function: gets a literal and its parameters
     * Parameters: index of the literal
     * Return Values: literal, length, or address
     */
    vector<literals> getLiteralVector();
};
#endif
