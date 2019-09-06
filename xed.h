/****************************************************************************
Team:
Samantha Quiroz - CSSC0416
Jacqueline Gronotte - CSSC0415
Cesar Lopez - CSSC0552
Class: CS530, Spring 2019
Project: Assignment 2
File: xed.h
Note: header file for the main.cpp program
****************************************************************************/
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>
#include <math.h>
#include <ctype.h>
#include "symtabreader.h"

using namespace std;
//////////////////GLOBAL VARIABLES://////////////////
//NIXBPE Flags
bool flagN = false;
bool flagI = false;
bool flagX = false;
bool flagB = false;
bool flagP = false;
bool flagE = false;

//addressing modes
bool indexAddressing = false;
bool immediateAddressing = false;
bool indirectAddressing = false;
bool simpleAddressing = false;
bool directAddressing = false;
bool PCRelativeAddressing = false;
bool BaseRelativeAdressing = false;
bool extendedAddressing = false;
bool checkLiteral = false;

int locationCounter = 0;
int addrDisplacement;
int format; //what format the op code is
int targetAddress;
int baseCounter;
int displacement = 0;
int lengthOfProg = 0;
string registers; //stores which registers are used in format 2
string foundLiteral; //holds the literal
int addrOfFirstExec;
string symbol; //stores the symbol that will be printed out
string literal; //stores the literal that will be printed out
bool base = false; //indicates when we need to print out BASE or not
string LDBOperand; //stores the operand used with LDB in order to add it after BASE
bool checkLastLiteral = false; //says if we hit the last literal in order to know when to print LTORG
int incrLC; //adds to the locationCounter how many bytes the literal was
int TA;
string objectCode; //stores the object code
bool checkLTORG = false;
int literalCounter = 1; //counts number of literals; used to check when we hit the last literal
stringstream stringTemp; //used for formatting
vector<SYMTABReader::symbols> symTab; //vector that stores the symbol table
vector<SYMTABReader::literals> litTab; //vector that stores the literal table

//Needed to save variables for outFile
vector<string> lcColumn;
vector<string> symbolColumn;
vector<string> mnemonicColumn;
vector<string> operandColumn;
vector<string> objectColumn;

/**
* Function: takes in a string (in hex) and converts the string to an integer
* Parameters: string that holds the starting address of prog or the object prog length
* Return Values: the starting address of prog or the object prog length as an int
*/
int hexToInt(string);

/**
* Function: changes the character sent in to an integer
* Parameters: a character
* Return Value: the character as an integer
*/
int charToInt(char);

/**
* function: converts a byte (written in hex) to a decimal
* Parameters: two integers, one that has the first digit of a byte and the second that has the
* second digit of a byte
* Return Value: an integer that stores the byte's decimal value
*/
int IntToDecimal(int,int);

/**
* function: gets the instruction based on the given OP Code
* Parameters: a string that holds the OP Code
* Return Value: a string that holds the instruction correlating to the given OP Code
*/
string getOpValue(string);

/**
* Function: set the NIXBPE flags and sets the addressing modes
* Parameters: a string that holds the digits that indicates which flags are set
* Return Value: N/A
*/
void setNIXBPEFlags(string);

/**
* Function: figures out which registers are being used for the format 2 object code
* Parameters: OpCode and the portion of the object code that holds the registers mnemonic
* Return Value: a string that holds the registers being used; the string will later be used in the print out
*/
string format2(string,string);

/**
* Function: finds the address displacement
* Parameters: the portion of the object that is the address displacement
* Return Value: returns that address displacement
*/
int findAddrDisp(string);

/**
* Function: finds the target address dependent on the format of the object code
* Parameters: the format of the object code and the address displacement
* Return Value: returns the target address
*/
int findTargetAddr(int, int);

/**
* Function: looks through the symbol vector to see if there is a symbol at a given address
* Parameters: the symbol vector that holds the symbol table and the location counter
* Return Value: string of the symbol
*/
string findSymbolFromTab( vector<SYMTABReader::symbols>, int);

/**
* Function: looks to see if there is a literal at the given address displacement.
* Also keeps track of when we find our last literal
* Parameters: the vector that holds the literal table, and the address displacement
* Return Value: string of the literal
*/
string searchLitTable( vector<SYMTABReader::literals>, int);

/**
* Function: looks for a symbol at the given target address
* Parameters: integer of the target address
* Return Value: true if there is a symbol at the target address; false otherwise
*/
bool searchSymTable(int);

/**
* Function: looks for a literal at the given address displacement.
* Also keeps track of when we find our last literal
* Parameters: integer that has the address displacement
* Return Value: true if literal found; false if no literal found
*/
bool searchLitTable(int);

/**
* function: gets the symbol's name at a target address
* Parameters: integer of the target address
* Return Value: the symbol
*/
string getSymbolName(int);

/**
* Function: gets the literal's name at an address displacement
* Parameters: integer of the address displacement
* Return Value: the literal
*/
string getLiteralName(int);

/**
* Function: outputs the information to the .lis and .sic files
* Parameters: string that holds the <file name>
* Return Value: N/A
*/
void printLis(string);
