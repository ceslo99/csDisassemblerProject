/****************************************************************************
Team:
Samantha Quiroz - CSSC0416
Jacqueline Gronotte - CSSC0415
Cesar Lopez - CSSC0552
Class: CS530, Spring 2019
Project: Assignment 2
File: symtabreader.cpp
Note: cpp file for symTabReader; Reads the .sym file and stores all the
      symbols and literals from the symbol table and literal table
****************************************************************************/
#include "symtabreader.h"

SYMTABReader::symbols::symbols(string n, int a, string f) {
    symbolName = n;
    addr = a;
    flag = f;
}

SYMTABReader::literals::literals(string s, int l, int a) {
    literal = s;
    length = l;
    addr = a;
}

void SYMTABReader::symTabRead(string filename) {
    ifstream file(filename.c_str()); //copy in the .sym file name
    file.ignore(50,'\n');  //skips the table headers
    file.ignore(50,'\n'); //skips until we reach the first symbol

    string a, b, c;
    int valueLength, addr;

    if(file.is_open()) { //reading symTab
        while(file >> a >> b >> c) {//reading literal table
            if(a == "Name") {
                file.ignore(50,'\n'); //skips the table headers
                file.ignore(50,'\n'); //skips until we reach the first literal
                while(file >> a >> b >> c) {
                    string literal = a;
                    istringstream(b) >> std::hex >> valueLength;
                    istringstream(c) >> std::hex >> addr;
                    LITTAB.push_back(literals(literal,valueLength,addr));
                }
            break;
            }
            string symbol = a;
            string flags = c;
            istringstream(b) >> std::hex >> valueLength;
            SYMTAB.push_back(symbols(symbol,valueLength,flags));
        }
    }
    file.close();
}

vector<SYMTABReader::symbols> SYMTABReader::getSymbolVector(){
    return SYMTAB;
}

vector<SYMTABReader::literals> SYMTABReader::getLiteralVector(){
    return LITTAB;
}
