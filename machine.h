#ifndef MACHINE_H_INCLUDED
#define MACHINE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

//initialize struct for instructions
struct Instruction{
    int op ;
    int l ;
    int m ;
}code[500];

//function signatures
void runProgram(int PC, int BP, int SP, int stack[], FILE * outputFile, int) ;
//void outputAssembly(FILE * outputFile) ;
int base(int l, int base, int stack[]) ;

int vmPrint = 0 ;
int vmPrinting[500] ;


int machine(int v)
{
    //setup initial values for the stack and registers
    int stack [2000] ;
    int SP = 0 ;
    int BP = 1 ;
    int PC = 0 ;

    //open the files for input/output
    FILE * inputFile = fopen("parserOut.txt", "r");
    FILE * outputFile = fopen("vmOut.txt", "w");

    //read in the file and assign the instructions to code
    int counter = 0;
    while(1)
    {
        int number;
            if (fscanf(inputFile, "%d", &number) != 1)
                break;        // file finished or there was an error
        code[counter].op = number ;

        fscanf(inputFile, "%d", &number) ;
        code[counter].l = number ;

        fscanf(inputFile, "%d", &number) ;
        code[counter].m = number ;

        counter ++  ;
    }

    //output the code in assembly
    //outputAssembly(outputFile) ;

    //execute the program
    runProgram(PC, BP, SP,stack, outputFile,v) ;

    //close output
    fclose(inputFile) ;
    fclose(outputFile)  ;

    printf("\n") ;
    printf("Virtual Machine Output: \n") ;
    for(int i=0; i<vmPrint;i++)
    {
        printf(" %d ",vmPrinting[i]) ;
    }

    //exit the program
    return 0;
}

//function to run and execute the instructions
void runProgram(int PC, int BP, int SP, int stack[], FILE * outputFile, int v){

    //print the header for the output file's chart
    fprintf(outputFile, "\n");
    fprintf(outputFile, "PC   BP      SP       Stack \n");

    if(v==1)
    {
        printf("\n");
        printf("Virtual Machine Stack Trace \n") ;
        printf("PC   BP      SP       Stack \n");
    }

    struct Instruction IR ;

    int run = 1 ;
    //run until halt instruction is read
    while(run == 1){

        //print the stack
        int i ;
        for(i=1; i<=SP;i++){
            if(i==BP && BP!=1){
                fprintf(outputFile, "|") ;
                if(v==1)
                {
                    printf("|") ;
                }
            }
            fprintf(outputFile, " %d ", stack[i]);
            if(v==1)
            {
                printf(" %d ", stack[i]);
            }
        }
        if(BP>1 && BP>SP){
            fprintf(outputFile, "|") ;
            fprintf(outputFile, " %d ", stack[SP+1]);
            fprintf(outputFile, " %d ", stack[SP+2]);
            fprintf(outputFile, " %d ", stack[SP+3]);
            fprintf(outputFile, " %d ", stack[SP+4]);

            if(v==1)
            {
                printf("|") ;
                printf(" %d ", stack[SP+1]);
                printf(" %d ", stack[SP+2]);
                printf(" %d ", stack[SP+3]);
                printf(" %d ", stack[SP+4]);
            }
        }

        //fetch cycle
        IR = code[PC] ;
        PC = PC + 1 ;

        //execute cycle
        if(IR.op == 1){ //LIT
            SP = SP + 1 ;
            stack[SP] = IR.m ;
        }else if(IR.op == 2){ //OPR
            if(IR.m == 0){ //RET
                SP = BP - 1 ;
                PC = stack[SP + 4] ;
                BP = stack[SP + 3] ;
            }else if(IR.m == 1){ //NEG
                stack[SP] = stack[SP] * -1 ;
            }else if(IR.m == 2){ //ADD
                SP = SP - 1  ;
                stack[SP] = stack[SP] + stack[SP + 1] ;
            }else if(IR.m == 3){ //SUB
                SP = SP - 1  ;
                stack[SP] = stack[SP] - stack[SP + 1] ;
            }else if(IR.m == 4){ //MUL
                SP = SP - 1  ;
                stack[SP] = stack[SP] * stack[SP + 1] ;
            }else if(IR.m == 5){ //DIV
                SP = SP - 1  ;
                stack[SP] = stack[SP] / stack[SP + 1] ;
            }else if(IR.m == 6){ //ODD
                stack[SP] = stack[SP] % 2 ;
            }else if(IR.m == 7){ //MOD
                SP = SP - 1  ;
                stack[SP] = stack[SP] % stack[SP + 1] ;
            }else if(IR.m == 8){ //EQL
                SP = SP - 1  ;
                stack[SP] = stack[SP] == stack[SP + 1] ;
            }else if(IR.m == 9){ //NEQ
                SP = SP - 1  ;
                stack[SP] = stack[SP] != stack[SP + 1] ;
            }else if(IR.m == 10){ //LSS
                SP = SP - 1  ;
                stack[SP] = stack[SP] < stack[SP + 1] ;
            }else if(IR.m == 11){ //LEQ
                SP = SP - 1  ;
                stack[SP] = stack[SP] <= stack[SP + 1] ;
            }else if(IR.m == 12){ //GTR
                SP = SP - 1  ;
                stack[SP] = stack[SP] > stack[SP + 1] ;
            }else if(IR.m == 13){ //GEO
                SP = SP - 1  ;
                stack[SP] = stack[SP] >= stack[SP + 1] ;
            }
        }else if(IR.op == 3){ //LOD
            SP = SP + 1 ;
            stack[SP] = stack[base(IR.l,BP,stack)+IR.m] ;
        }else if(IR.op == 4){ //STO
            stack[base(IR.l,BP,stack)+IR.m] = stack[SP] ;
            SP = SP - 1 ;
        }else if(IR.op == 5){ //CAL
            stack[SP + 1] = 0 ;
            stack[SP + 2] = base(IR.l,BP,stack) ;
            stack[SP + 3] = BP ;
            stack[SP + 4] = PC ;
            BP = SP + 1 ;
            PC = IR.m ;
        }else if(IR.op == 6){ //INC
            SP = SP + IR.m ;
        }else if(IR.op == 7){ //JMP
            PC = IR.m ;
        }else if(IR.op == 8){ //JPC
            if(stack[SP]==0){
                PC = IR.m ;
            }
            SP = SP - 1 ;
        }else if(IR.op == 9){ //SIO
            if(IR.m == 1){ //print the top of the stack
                vmPrinting[vmPrint] = stack[SP] ;
                vmPrint ++ ;
                //printf("%d",stack[SP]) ;
                SP = SP - 1 ;
            }else if(IR.m == 2){ //have user enter a value
                SP = SP + 1 ;
                printf("Enter a value: ") ;
                fflush(stdout) ;
                int input ;
                scanf("\n%d", &input) ;
                stack[SP] = input;
            }else if(IR.m==3){ //Exit the program
                run = 2;
                PC = 0 ;
                BP = 0 ;
                SP = 0;
            }
        }

        //printing for the output chart
        fprintf(outputFile, "\n")  ;
        fprintf(outputFile, "%d    %d       %d        ", PC, BP, SP);

        if(v==1)
        {
            printf("\n")  ;
            printf("%d    %d       %d        ", PC, BP, SP);
        }


    }
}

//given function for finding the base record
int base(int l, int base, int stack[]) // l stand for L in the instruction format
{
  int b1; //find base L levels down
  b1 = base;
  while (l > 0)
  {
    b1 = stack[b1 + 1];
    l--;
  }
  return b1;
}

//function to output the code in assembly
/*
void outputAssembly(FILE * outputFile){
    int i = 0 ;
    fprintf(outputFile,"Line   OP      L       M \n");
    for(i=0; i<500; i++){
            if(code[i].op==0){
                break ;
            }

            fprintf(outputFile, "%d",i) ;
            if(newCode[i].op == 1){
                fprintf(outputFile, "     LIT") ;
            }else if(newCode[i].op == 2){
                fprintf(outputFile, "     OPR") ;
            }else if(newCode[i].op == 3){
                fprintf(outputFile, "     LOD") ;
            }else if(newCode[i].op == 4){
                fprintf(outputFile, "     STO") ;
            }else if(newCode[i].op == 5){
                fprintf(outputFile, "     CAL") ;
            }else if(newCode[i].op == 6){
                fprintf(outputFile, "     INC") ;
            }else if(newCode[i].op == 7){
                fprintf(outputFile, "     JMP") ;
            }else if(newCode[i].op == 8){
                fprintf(outputFile, "     JPC") ;
            }else if(newCode[i].op == 9){
                fprintf(outputFile, "     SIO") ;
            }

            fprintf(outputFile, "      %d",code[i].l) ;
            fprintf(outputFile, "       %d",code[i].m) ;
            fprintf(outputFile, "\n") ;
    }
}
*/


#endif // MACHINE_H_INCLUDED
