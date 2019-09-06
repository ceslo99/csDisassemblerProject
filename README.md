Jacqueline Gronotte, Cesar Lopez, Samantha Quiroz
cssc0415, cssc 0465, cssc0416
CS 530, Spring 2019
Assignment #2
README.txt

Program Files: (in directory 'a2') xed.cpp, xed.h, symtabreader.cpp, 
symtabreader.h, README.txt, Makefile.txt, sample.obj, sample.sym, sample3.obj, sample3.sym, sample2.obj, sample2.sym

Compile Instructions: call 'make'

Operating instructions: ./xed sample.obj
	
List/description of novel/significant design decisions: 
	We first took into consideration the NIXBPE bits and how each flag requires 
different formatting for disassembling. From there, we wrote an algorithm for 
each format instruction and planned for the addressing modes to determine the 
displacement of each instruction. We used two files, a main file "xed.cpp", 
which contains all of the necessary functions of our disassembled and a "
symTabReader.cpp" that reads the sym file and extracts the literals and symbols 
and places them into a vector. We decided to primarily use strings to store all 
of our values; we later realized that our code could potentially have been 
cleaner with vectors, but we continued with strings because that is what we 
felt more confident implementing correctly. Through our program, we read in each
object code and print out our program line by line, rather than storing the 
final result and outputting it at the end. 

	
List/description of any extra features/algorithms/functionality you included 
which were not required: N/A
	
List/description of all known deficiencies or bugs: 
	Regarding literals, the name of the literal is not obtained in the vector, 
nor does it check if there is or isn't a name attached to it at all. In our 
assignment, we assumed that we would not be given a name with a literal 
based on the example object file. 

Lessons Learned: 
	Through this assignment, we gained stronger time management skills when 
dealing with an extensive project with numerous aspects to it. We learned to set
deadlines and to better work as a team with varying roles. It was interesting 
writing the program for a disassembled too, it allowed us to truly understand 
the ins and outs of the SIC/XE machine. 