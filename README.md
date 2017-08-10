# PL-0-Compiler-VM
A compiler and virtual machine for the PL/o language 
--------------------------------------------------
Necessary Files to run the compiler 
	1. main.c
	2. parser.h
	3. machine.h
	4.  lexicalAnalyzer.h 
	5. input program 
The compiler takes in a  tiny PL/0 program and produces 3 output files. These files are lexerOut.txt, output of the lexeme tokens from the scanner, parserOut.txt, output of the intermediate assembly instructions from the parser, and vmOut.txt, output of the stack trace and execution of the program. 

To Compile and Run 
	1. Make sure main and the header files and the input are all in the same location  
	2. Compile with the command: gcc -o <runfile> main.c
	3. Run with: ./runfile <inputFile.txt> <directive> <directive> <directive> 
	The compiler takes up to 3 directives which are "-l", which prints to the consol the output of the scanner, "-a", which prints to the consol the parser out, and "-v", which prints the stack trace of the virtual machine. All the directives are optional and no directives with only print the execution of the program. The can be in any order, below are examples of run commands. 
	./run input.txt
	./run intput.txt -a -v -l 
	./run intput.txt -l -v
	./run intput.txt -a
