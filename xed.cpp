/****************************************************************************
Team:
Samantha Quiroz - CSSC0416
Jacqueline Gronotte - CSSC0415
Cesar Lopez - CSSC0465
Class: CS530, Spring 2019
Project: Assignment 2
File: xed.cpp
Note: Handles reading, processing, and output of the data from the .obj file
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
#include "xed.h"

int main(int argc, char* argv[]){
    //checks if too little or too many argument. Just need obj file name
    if(argc <= 1 || argc >2) {
        cout << "Need obj file: xeDisassembler <filename>.obj" << endl;
        exit(1);
    }
    //reads obj file and sets it to variable
    string objFile = argv[1];

    //double checks if .obj file is given in argument
    int extPosition = objFile.find(".obj");
    if(extPosition < 1) {
        cout << "Wrong file extension!" << endl;
        exit(1);
    }

    //if file not found
    ifstream objectFile(argv[1]);
    if(!objectFile) {
        cout << "File not found!" << endl;
        exit(1);
    }

    //retrieving <filename>.symbol
    string fileName = objFile.substr(0,extPosition);
    ifstream symbolFile( (fileName + ".sym").c_str() );
    if(!symbolFile) {
        cout << "File not found!" << endl;
        exit(1);
    }

    //check if able to open both OBJ and SYM
    if( !symbolFile.is_open() || !objectFile.is_open() ){
            cout<<" <filename>.sym and <filename>.obj not present\n";
            exit(1);
    }

    //Generating the respective .sym file to open in the symTabReader
    string symFile = objFile.substr(0,extPosition);
    symFile.append(".sym");

    //call symtabreader
    SYMTABReader symTR;
    symTR.symTabRead(symFile);
    symTab = symTR.getSymbolVector();
    litTab = symTR.getLiteralVector();

    string line;
    string programName; //holds the project name
    string startingAddress; //holds the starting address
    string objProgLength; //holds length of program
    string objCodeLength; //length of object code in that text record
    string locationCounterString; //location counter as a string
    string targetAddressString; //holds the target address as a string
    stringstream result;
    int index = 0; //keeps track of what digit in the text record we are at

    //grabbing one line at a time from the .obj file
    while(getline(objectFile,line)){
        int i = 0;

        //read Header Record
        if(line.at(i) == 'H' ){
            programName = line.substr(i+1,6) ;
            i = 6;

            startingAddress = line.substr(i+1,6);
            i = i+6;

            objProgLength = line.substr(i+1,6);
            lengthOfProg = hexToInt(objProgLength);

            //if starting address is 0, simply display 0
            if( startingAddress == "000000" ){
                startingAddress = "0";
            }

            locationCounter = hexToInt(startingAddress);

            stringTemp.str("");
            stringTemp << uppercase << setw(6) << setfill('0') << hex << locationCounter ;
            lcColumn.push_back(stringTemp.str());
            symbolColumn.push_back(programName);
            mnemonicColumn.push_back("START");
            operandColumn.push_back(startingAddress);
            objectColumn.push_back("");
        }

        //read Text(s) Record
        else if(line.at(i) == 'T'){
            int lineLength = (line.size()-9)/2;
            if(i == 0){
                index = 9; //where the object codes start at in the text record; index is what digit we are on
            }

            string objCodeLength = line.substr(7,2); //length of object code in this record in bytes (hex)

            //convert objCodeLength to decimal and store in totalBytes; half to convert each digit
            int opHalf1 = charToInt(objCodeLength[0]);
            int opHalf2 = charToInt(objCodeLength[1]);
            int totalBytes = IntToDecimal(opHalf1, opHalf2);

            int byteIndex = 1;//keeps track of what byte we are on
            while (byteIndex <= totalBytes && byteIndex < lineLength){
                string opCode = line.substr(index,2); //first byte of the object Code
                char opCodeHalfByte = opCode[1];
                //prints location counter in .lis
                stringTemp.str("");
                stringTemp << uppercase << setw(6) << setfill('0') << hex << locationCounter;

                lcColumn.push_back(stringTemp.str());

                stringTemp.str("");
                stringTemp<< findSymbolFromTab(symTab, locationCounter);
                symbolColumn.push_back(stringTemp.str());
                stringTemp.str("");

                //finding original OPCode before n and i bits were modified;line will still hold original opCode
                if(opCodeHalfByte != '0' ||opCodeHalfByte != '4' || opCodeHalfByte != '8' || opCodeHalfByte != 'C'){ //in binary: xx00
                    if(opCodeHalfByte == '1' || opCodeHalfByte == '2' || opCodeHalfByte == '3'){ //switch to 0000
                        opCodeHalfByte = '0';
                    }
                    else if(opCodeHalfByte == '5' || opCodeHalfByte == '6' || opCodeHalfByte == '7'){ //switch to 0100
                        opCodeHalfByte = '4';
                    }
                    else if(opCodeHalfByte == '9' || opCodeHalfByte == 'A' || opCodeHalfByte == 'B'){ //switch to 1000
                        opCodeHalfByte = '8';
                    }
                    else if(opCodeHalfByte == 'D' || opCodeHalfByte == 'E' || opCodeHalfByte == 'F'){ //switch 1100
                        opCodeHalfByte = 'C';
                    }
                }

                //puts corrected opCode into the string opCode in order to do comparisons
                stringstream temp;
                temp << opCode[0];
                temp << opCodeHalfByte;
                opCode = temp.str();

                //set NIXPE flags
                setNIXBPEFlags(line.substr(index,3));

                //DETERMINE FORMAT
                if (opCode == "C4"|| opCode == "C0"|| opCode == "F4" || opCode == "C8" || opCode == "F0"|| opCode == "F8"){ //format 1 OPCodes
                    format =1;
                    objectCode = line.substr(index,2);
                    byteIndex +=1; //move over one byte because format 1 object code is 1 byte
                    index += 2; //move over two digits
                    locationCounter +=1;

                }
                else if (opCode == "90"|| opCode == "B4"|| opCode == "A0" || opCode == "9C" || opCode == "98" || opCode == "AC"|| opCode == "A4" || opCode == "A8" || opCode == "94" ||opCode == "B0" ||opCode == "B8"){
                    //format 2 Op Codes
                    format = 2;
                    objectCode = line.substr(index,4);
                    string registers = format2(opCode, line.substr(index+2,2)); //determine which registers are used in the object code
                    byteIndex +=2; //move over two bytes because format 2 object code is 2 bytes
                    index += 4; //move over four digits
                    locationCounter +=2;
                }

                else if(flagE == true){ //format 4
                    format = 4;
                    addrDisplacement = findAddrDisp(line.substr(index+3, 5)); //find the address displacement
                    targetAddress = findTargetAddr(format,addrDisplacement); //find the target address
                    objectCode = line.substr(index,8);
                    byteIndex +=4; //move over 4 bytes because format 4 object code is 4 bytes
                    index +=8; //move over 8 digits
                    locationCounter+=4;
                    //print out the + in front of the format 4 instruction
                    stringTemp<<"+";
                }

                else{ //format 3
                    format = 3;
                    addrDisplacement = findAddrDisp(line.substr(index+3, 3)); //find the address displacement
                    targetAddress = findTargetAddr(format,addrDisplacement); //find the target address
                    objectCode = line.substr(index,6);
                    byteIndex +=3;
                    index +=6;
                    locationCounter +=3;
                }

                if(opCode == "68"){ //"LDB" means we need a base counter
                    baseCounter = targetAddress;
                }

                stringTemp << getOpValue(opCode);
                mnemonicColumn.push_back(stringTemp.str());
                stringTemp.str("");

                if(getOpValue(opCode) == "LDB"){
                    LDBOperand = getSymbolName(targetAddress);
                    base = true;
                }

                if (format == 1){
                    operandColumn.push_back("");
                    objectColumn.push_back(objectCode);
                }
                else if(format == 2){
                    operandColumn.push_back(registers);
                    objectColumn.push_back(objectCode);
                }
                else if(format ==3 || format ==4){
                    bool symbFound = searchSymTable(targetAddress);
                    bool litFound = searchLitTable(addrDisplacement);

                    if(symbFound && (flagB || flagP || flagE)){ //WE HAVE A SYMBOL
                        symbol = getSymbolName(targetAddress);
                        if(indirectAddressing){ //indirect
                            stringTemp.str("");
                            stringTemp << "@" << symbol;
                            operandColumn.push_back(stringTemp.str());
                        }
                        if((flagB || flagP || flagE) &&immediateAddressing){ //immediate
                            stringTemp.str("");
                            stringTemp << "#" << symbol;
                            operandColumn.push_back(stringTemp.str());
                        }

                        if(base == true){
                            objectColumn.push_back(objectCode);
                            stringTemp.str("");
                            lcColumn.push_back("");
                            symbolColumn.push_back("");
                            mnemonicColumn.push_back("BASE");
                            operandColumn.push_back(LDBOperand);
                            objectColumn.push_back("");
                        }
                    }

                    else{ // WE HAVE NO SYMBOL
                        if(immediateAddressing){
                            stringTemp.str("");
                            stringTemp << "#" << targetAddress;
                            operandColumn.push_back(stringTemp.str());
                            objectColumn.push_back(objectCode);
                        }
                    }

                    if(litFound){
                        literal = getLiteralName(addrDisplacement);
                        foundLiteral= searchLitTable(litTab, addrDisplacement);
                        locationCounter += incrLC;
                        operandColumn.push_back(literal);
                        objectColumn.push_back(objectCode);
                        index +=2;
                        litFound = false;
                        if(checkLastLiteral == true){
                            lcColumn.push_back("");
                            symbolColumn.push_back("");
                            mnemonicColumn.push_back("LTORG");
                            operandColumn.push_back("");
                            objectColumn.push_back("");
                            checkLastLiteral = false;
                            checkLTORG = true;
                        }
                    }

                    if(getOpValue(opCode) == "RSUB"){
                        operandColumn.push_back("");
                        objectColumn.push_back(objectCode);
                    }

                    if(format ==3 && flagP){
                        TA = (locationCounter) + addrDisplacement; //locationCounter +3 = PC Counter, but LC is one ahead
                        symbFound = searchSymTable(targetAddress);
                        if(symbFound){
                            symbol = getSymbolName(TA);
                        }
                        if(indexAddressing){ //index
                            stringTemp.str("");
                            stringTemp<< symbol << ",X";
                            operandColumn.push_back(stringTemp.str());
                            objectColumn.push_back(objectCode);
                        }
                        else{
                            if(!checkLTORG){
                                operandColumn.push_back(symbol);
                                objectColumn.push_back(objectCode);
                            }
                        }
                        checkLTORG = false;
                    }

                    if(format == 3 && flagB){
                        TA = (baseCounter) + addrDisplacement;
                        symbFound = searchSymTable(targetAddress);
                        if(symbFound){
                            symbol = getSymbolName(TA);
                        }
                        if(indexAddressing){ //index
                            stringTemp.str("");
                            stringTemp<< symbol << ",X";
                            operandColumn.push_back(stringTemp.str());
                            objectColumn.push_back(objectCode);
                        }
                        else{
                            operandColumn.push_back(symbol);
                            objectColumn.push_back(objectCode);
                        }
                    }

                    if(format == 4 && !base){
                        symbFound = searchSymTable(targetAddress);
                        symbol = getSymbolName(targetAddress);
                        operandColumn.push_back(symbol);
                        objectColumn.push_back(objectCode);
                    }
                    base = false;
                }
            }
            int vectorSize = 0;
            if(symTab.size() < 1){
                vectorSize = 1;
            }
            else{
                vectorSize = symTab.size();
            }

            //do RESW and RESB
            for(int i = 0; i < vectorSize -1; i++){ //go up to last symbol definition
                if(locationCounter == symTab[i].addr){
                    stringTemp.str("");
                    stringTemp << uppercase << setw(6) << setfill('0') << hex << locationCounter;
                    lcColumn.push_back(stringTemp.str());
                    displacement = symTab[i+1].addr - symTab[i].addr;
                    if (displacement % 3 == 0){// RESW
                        int numOfWords = displacement /3;
                        symbolColumn.push_back(symTab[i].symbolName);
                        mnemonicColumn.push_back("RESW");
                        stringTemp.str("");
                        stringTemp << dec << numOfWords;
                        operandColumn.push_back(stringTemp.str());
                        objectColumn.push_back("");
                    }
                    else{ //RESB
                        symbolColumn.push_back(symTab[i].symbolName);
                        mnemonicColumn.push_back("RESB");
                        stringTemp.str("");
                        stringTemp << dec << displacement;
                        operandColumn.push_back(stringTemp.str());
                        objectColumn.push_back("");
                    }
                    locationCounter += displacement;
                }
            }

            //last symbol definition print out
            if(searchSymTable(locationCounter)){
                stringTemp.str("");
                stringTemp << uppercase << setw(6) << setfill('0') << hex << locationCounter;
                lcColumn.push_back(stringTemp.str());
                displacement = lengthOfProg - symTab[symTab.size() - 1].addr;
                if (displacement % 3 == 0){// RESW
                    int numOfWords = displacement /3;
                    symbolColumn.push_back(symTab[symTab.size() - 1].symbolName);
                    mnemonicColumn.push_back("RESW");
                    stringTemp.str("");
                    stringTemp << dec << numOfWords;
                    operandColumn.push_back(stringTemp.str());
                    objectColumn.push_back("");
                }
                else{ //RESB
                    symbolColumn.push_back(symTab[symTab.size() - 1].symbolName);
                    mnemonicColumn.push_back("RESB");
                    stringTemp.str("");
                    stringTemp << dec << displacement;
                    operandColumn.push_back(stringTemp.str());
                    objectColumn.push_back("");
                }
                locationCounter += displacement;
            }
        }

        //read Modification Record
        else if(line.at(i) == 'M'){
            int modStartingAddress;
            int modLength;
            string modAddressStr = line.substr(1,6);
            string modLengthStr = line.substr(7,2);
            istringstream(modAddressStr) >> hex >> modStartingAddress;
            istringstream(modLengthStr) >> hex >> modLength;
        }

        //read End Record
        else if (line.at(i) == 'E'){
            istringstream(line.substr(1,6)) >> addrOfFirstExec;
            lcColumn.push_back("");
            symbolColumn.push_back("");
            mnemonicColumn.push_back("END");
            if(symTab.size() < 1){
                operandColumn.push_back("FIRST");
            }
            else{
                operandColumn.push_back(symTab[0].symbolName);
            }
            objectColumn.push_back("");
        }
    }
    printLis(fileName);
    return 0;
}


int hexToInt(string address){
    int length = address.length();
    int value = 0 ;

    for(int i = 0; i < length; i++){
        int temp = charToInt(address[i]);
        int exponent = length - (1+i);
        temp = temp * (pow(16,exponent));
        value = value + temp;
    }
    return value;
}

int charToInt(char val){
    if(val == '1')
        return 1;
    else if(val == '0')
        return 0;
    else if( val == '2')
        return 2;
    else if( val == '3')
        return 3;
    else if( val == '4')
        return 4;
    else if( val == '5')
        return 5;
    else if( val == '6')
        return 6;
    else if( val == '7')
        return 7;
    else if( val == '8')
        return 8;
    else if( val == '9')
        return 9;
    else if( val == 'A')
        return 10;
    else if( val == 'B')
        return 11;
    else if( val == 'C')
        return 12;
    else if( val == 'D')
        return 13;
    else if( val == 'E')
        return 14;
    else if( val == 'F')
        return 15;
}

int IntToDecimal(int HalfByteOne, int HalfByteTwo){
    return ((HalfByteOne * 16) + HalfByteTwo);
}

string getOpValue(string key){
    if(key == "18")
        return "ADD";
    else if(key == "58")
        return "ADDF";
    else if(key == "90")
        return "ADDR";
    else if( key == "40")
        return "AND";
    else if( key == "B4")
        return "CLEAR";
    else if( key == "28")
        return "COMP";
    else if( key == "88")
        return "COMPF";
    else if( key == "A0")
        return "COMPR";
    else if( key == "24")
        return "DIV";
    else if( key == "64")
        return "DIVF";
    else if( key == "9C")
        return "DIVR";
    else if( key == "C4")
        return "FIX";
    else if( key == "C0")
        return "FLOAT";
    else if( key == "F4")
        return "HIO";
    else if( key == "3C")
        return "J";
    else if( key == "30")
        return "JEQ";
    else if( key == "34")
        return "JGT";
    else if( key == "38")
        return "JLT";
    else if( key == "48")
        return "JSUB";
    else if( key == "00")
        return "LDA";
    else if( key == "68")
        return "LDB";
    else if( key == "50")
        return "LDCH";
    else if( key == "70")
        return "LDF";
    else if( key == "08")
        return "LDL";
    else if( key == "6C")
        return "LDS";
    else if( key == "74")
        return "LDT";
    else if( key == "04")
        return "LDX";
    else if( key == "D0")
        return "LPS";
    else if( key == "20")
        return "MUL";
    else if( key == "60")
        return "MULF";
    else if( key == "98")
        return "MULR";
    else if( key == "C8")
        return "NORM";
    else if( key == "44")
        return "OR";
    else if( key == "D8")
        return "RD";
    else if( key == "AC")
        return "RMO";
    else if( key == "4C")
        return "RSUB";
    else if( key == "A4")
        return "SHIFTL";
    else if( key == "A8")
        return "SHIFTR";
    else if( key == "F0")
        return "SIO";
    else if( key == "EC")
        return "SSK";
    else if( key == "0C")
        return "STA";
    else if( key == "78")
        return "STB";
    else if( key == "54")
        return "STCH";
    else if( key == "80")
        return "STF";
    else if( key == "D4")
        return "STI";
    else if( key == "14")
        return "STL";
    else if( key == "7C")
        return "STS";
    else if( key == "E8")
        return "STSW";
    else if( key == "84")
        return "STT";
    else if( key == "10")
        return "STX";
    else if( key == "1C")
        return "SUB";
    else if( key == "5C")
        return "SUBF";
    else if( key == "94")
        return "SUBR";
    else if( key == "B0")
        return "SVC";
    else if( key == "E0")
        return "TD";
    else if( key == "F8")
        return "TIO";
    else if( key == "2C")
        return "TIX";
    else if( key == "B8")
        return "TIXR";
    else
        return "WD";
}


void setNIXBPEFlags(string OPnixbpe){
    char niFlag = OPnixbpe[1]; //2nd halfbyte in the object code; holds ni flags
    char xbpeFlag = OPnixbpe[2]; //3rd halfbyte in the object code; hold xbpe flags

    flagN = false;
    flagI = false;
    flagX = false;
    flagB = false;
    flagP = false;
    flagE = false;

    indexAddressing = false;
    immediateAddressing = false;
    indirectAddressing = false;
    simpleAddressing = false;
    directAddressing = false;
    PCRelativeAddressing = false;
    BaseRelativeAdressing = false;
    extendedAddressing = false;

    //SET N and I Flags
    if (niFlag == '0'  || niFlag == '3' || niFlag == '4' || niFlag == '7' || niFlag == '8' || niFlag == 'B' || niFlag == 'C' || niFlag == 'F'){ //in binary: xx00 or xx11
        simpleAddressing = true;
    }

    else if(niFlag == '1' || niFlag == '5' || niFlag == '9' || niFlag == 'D'){ //binary xx01
        flagI = true;
        immediateAddressing = true;
    }

    else {//if(niFlag == '2' || niFlag == '6' || niFlag == 'A', || niFlag == 'E'){ //binary is 10
        flagN = true;
        indirectAddressing = true;
    }

    //SET X, B, and P Flags
    if(immediateAddressing == false || indirectAddressing == false){
        if (xbpeFlag == '2' ||xbpeFlag == '3' || xbpeFlag == 'A' || xbpeFlag == 'B'){ //in binary: x01x so p needs to be set
            flagP = true;
            PCRelativeAddressing = true;
        }
        else if(xbpeFlag == '4' ||xbpeFlag == '5' || xbpeFlag == 'C' ||xbpeFlag == 'D'){ //in binary: x10x so B needs to be set
            flagB = true;
            BaseRelativeAdressing = true;
        }
        else if(xbpeFlag == '0' || xbpeFlag == '1' || xbpeFlag == '8' || xbpeFlag == '9'){ //in binary: x00x so direct addr
            directAddressing = true;
        }
        if(xbpeFlag == '8' || xbpeFlag == '9' || xbpeFlag == 'A' || xbpeFlag == 'B' || xbpeFlag == 'C' || xbpeFlag == 'D'){ //in binary: xxx1
            flagX = true;
            indexAddressing = true;
        }
    }

    //check for E bit
    if(xbpeFlag == '1'|| xbpeFlag == '3'|| xbpeFlag == '5'||  xbpeFlag == '9'|| xbpeFlag == 'B'|| xbpeFlag == 'D'){ //binary where the last bit is 1
        flagE = true;
        extendedAddressing = true;
    }
}
string format2(string opCode, string registersmnemonic){
    string opCodeInstruction = opCode;
    string reg1 = registersmnemonic.substr(0,1);
    string reg2 = registersmnemonic.substr(1);
    //first register
    if(reg1.compare("0") == 0){
        registers = "A";
    }
    else if (reg1.compare("1") == 0){
        registers = "X";
    }
    else if (reg1.compare("2") == 0){
        registers = "L";
    }
    else if (reg1.compare("3") == 0){
        registers = "B";
    }
    else if (reg1.compare("4") == 0){
        registers = "S";
    }
    else if (reg1.compare("5") == 0){
        registers = "T";
    }
    else if (reg1.compare("6") == 0){
        registers = "F";
    }

    if(opCodeInstruction == "B4" || opCodeInstruction == "B0" || opCodeInstruction == "B8"){
        return registers;
    }
    else{
        //second register
        if(reg2.compare("0") == 0) {
            registers.append(",A");
        }
        else if(reg2.compare("1") == 0) {
            registers.append(",X");
        }
        else if(reg2.compare("2") == 0) {
            registers.append(",L");
        }
        else if(reg2.compare("3") == 0) {
            registers.append(",B");
        }
        else if(reg2.compare("4") == 0) {
            registers.append(",S");
        }
        else if(reg2.compare("5") == 0) {
            registers.append(",T");
        }
        else if(reg2.compare("6") == 0) {
            registers.append(",F");
        }
        return registers;
    }
}

int findAddrDisp(string dispObjCode){
    unsigned int retAD;

    if(dispObjCode.substr(0, 1) == "F"){
        for(int i = 0; dispObjCode.length() < 8; i++){
            dispObjCode = "F" + dispObjCode;
        }
    }

    std::stringstream strStream;
    strStream << std::hex << dispObjCode;
    strStream >> retAD;
    retAD = static_cast<int>(retAD);
    return retAD;
}

int findTargetAddr(int format, int addrDispCode){
    int addrDisp = addrDispCode;
    int returnTA; //returnable TA

    if(format == 3){
        if(flagB == true){ //if Base
            returnTA = baseCounter + addrDisp;
        }
        else if (flagP == true){ //if PC
            returnTA  = locationCounter + 3 + addrDisp;  //locationCounter + 3 is the (PC)
        }
        else { //not Base or PC
            returnTA = addrDisp;
        }
    }
    else if (format == 4){
        returnTA = addrDisp;
    }
    return returnTA;
}

string findSymbolFromTab( vector<SYMTABReader::symbols> sym, int lc){
    for(int i = 0; i < sym.size();i++){
        if(sym[i].addr == lc){
            return sym[i].symbolName;
        }
    }
    return "";
}

string searchLitTable( vector<SYMTABReader::literals> litTab, int AD){
    if(literalCounter == litTab.size()){
        checkLiteral = true;
    }
    literalCounter++;
    for(int i = 0; i < litTab.size(); i ++){
        if(litTab[i].addr == AD){
            incrLC = litTab[i].length -1;
            return  litTab[i].literal ;
        }
    }
    return "";
}
bool searchSymTable(int TA){
    int vectorSize = 0;
    if(symTab.size() < 1){
        vectorSize = 0;
    }

    else{
        vectorSize = symTab.size();
    }

    for(int i = 0; i < vectorSize; i ++){
        if(symTab[i].addr == TA){
            return true;
        }
    }
    return false;
}

bool searchLitTable(int AD){
    if(literalCounter == litTab.size()){
        checkLastLiteral = true;
    }
    literalCounter++;
    for(int i = 0; i < litTab.size(); i ++){
        if(litTab[i].addr == AD){

            return true;
        }
    }
    return false;
}

string getSymbolName(int addrDisp){
    for(int i = 0; i < symTab.size(); i++){
        if(symTab[i].addr == addrDisp){
            return symTab[i].symbolName;
        }
    }
    return "";
}

string getLiteralName(int addrDisp){
    for (int i = 0; i < litTab.size(); i++){
        if (litTab[i].addr == addrDisp){
            return litTab[i].literal;
        }
    }
    return "";
}

void printLis(string fileName){
    //creating out file called <filename>.sic and <filename>.lis
    ofstream outFile((fileName + ".sic").c_str());
    ofstream outFileLIS((fileName + ".lis").c_str());
    //gets symbol column then prints all

    for(int i = 0; i < symbolColumn.size(); i++){
        outFile<<setw(10)<< left<< setfill(' ')<< symbolColumn[i];
        outFile<<setw(10)<< left<< setfill(' ')<< mnemonicColumn[i];
        outFile<<setw(10)<< left<< setfill(' ')<< operandColumn[i] <<endl;
    }

    //gets mnemonic columns and prints all
    for(int i = 0; i < mnemonicColumn.size(); i++){
        outFileLIS<<setw(10)<< left<< setfill(' ')<< lcColumn[i];
        outFileLIS<<setw(10)<< left<< setfill(' ')<< symbolColumn[i];
        outFileLIS<<setw(10)<< left<< setfill(' ')<< mnemonicColumn[i];
        outFileLIS<<setw(10)<< left<< setfill(' ')<< operandColumn[i];
        outFileLIS<<setw(10)<< left<< setfill(' ')<< objectColumn[i] << endl;
    }

}
