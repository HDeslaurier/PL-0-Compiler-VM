#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

#define MAX_CODE_LENGTH 32768

//function params
void program(void) ;
void block(int) ;
void statement(void) ;
void expression(void) ;
void factor(void) ;
void term(void) ;
void condition(void) ;
void getNextToken(void) ;
void compileError(int) ;
void emit(int, int, int) ;
void enter(int, char [10], int,int) ;
void outputParserCode(int) ;
int searchTable(const char *) ;
char* trim(char [10]) ;

//token variables
char currentToken[11]  ;
char tokenList [MAX_CODE_LENGTH] ;
int tokenIndex ;

//lex level variable
int lexLevel ;

//initialize struct for instructions
struct generatedInstruction{

    int op ;
    int l ;
    int m ;
};

struct generatedInstruction newCode[500] ;

//initialize struct for symbol table
struct
{
    int kind ; //const = 1 var = 2
    char name[10] ; //name up to 11 chars
    int val ; //number (ascii value)
    int level ; //L level
    int addr ; //M address
}symbol_table[1000] ;

//holds the current index of the table
int tableIndex ;

int varIndex ;

int procIndex ;

int codeIndex ;

//starting function for the parser
int parser(int a)
{
    //set values for indexes
    procIndex = 0 ;
    varIndex = 4 ;
    codeIndex = 0 ;
    tokenIndex = 0 ;
    tableIndex = 1 ;
    lexLevel = 0 ;

    //start parsing
    program() ;

    outputParserCode(a) ;
}

//function to parse a program
void program()
{
    //get token
    getNextToken();

    //call block
    block(0) ;

    //check for period
    if(currentToken[0]!= '1' || currentToken[1]!= '9')
    {
        compileError(9);
    }

    //compiled exit program
    emit(9,0,3) ;
}

//function to parse a block
void block(int type)
{
    varIndex = 4 ;

    //saves the current code index for later
    int tempCodeIndex = codeIndex ;

    //jump instruction
    emit(7,0,0) ;

    int constantCount = 0 ;

    //check for constant symbol
    if(currentToken[0] == '2' && currentToken[1] == '8')
    {
        while(1)
        {
            getNextToken() ;

            //check for identifier
            if(currentToken[0]!='2')
            {
                compileError(4) ;
            }

            getNextToken() ;

            char variableName[10] ;
            strcpy(variableName,trim(currentToken)) ;


            getNextToken() ;

            //check for equal symbol
            if(currentToken[0]!=57)
            {
                compileError(3) ;
            }

            getNextToken()  ;

            //check for number
            if(currentToken[0]!=51)
            {
                compileError(2) ;
            }

            getNextToken()  ;

            enter(1,variableName,atoi(currentToken),lexLevel) ;

            getNextToken()  ;

            constantCount ++ ;

            //check for comma
            if(currentToken[0]!=49 || currentToken[1]!=55)
            {
                break ;
            }
        }

        //check for semicolon
        if(currentToken[0]!=49 || currentToken[1]!=56)
        {
            compileError(5) ;
        }

        getNextToken() ;
    }

    int varCount = 0 ;

    //check for var symbol
    if(currentToken[0] == '2' && currentToken[1] == '9')
    {
        while(1)
        {
            getNextToken() ;

            //check for identifier
            if(currentToken[0]!='2')
            {
                compileError(4) ;
            }

            getNextToken() ;

            char variableName[10] ;
            strcpy(variableName,trim(currentToken)) ;

            getNextToken() ;

            enter(2,variableName,0,lexLevel) ;

            varCount ++ ;

            //check for comma
            if(currentToken[0] != '1' || currentToken[1] != '7')
            {
                break ;
            }
        }

        //check for semicolon
        if(currentToken[0]!= '1' || currentToken[1]!= '8')
        {
            compileError(5) ;
        }

        getNextToken() ;

    }

    int procCount = 0;

    //check for procedure symbol
    if(currentToken[0] == '3' && currentToken[1] == '2')
    {
        while(1)
        {
            if(procCount == 0)
            {
                getNextToken() ;
            }

            //check for identifier
            if(currentToken[0] != '2')
            {
                compileError(4) ;
            }

            getNextToken() ;

            char procName[10] ;
            strcpy(procName,trim(currentToken)) ;

            getNextToken() ;

            enter(3, procName, 0,lexLevel) ;

            //check for semi colon
            if(currentToken[0] != '1' && currentToken[1] != '8')
            {
                compileError(5) ;
            }

            getNextToken() ;

            lexLevel ++ ;

            block(1) ;

            lexLevel -- ;

            //check for semi colon
            if(currentToken[0] != '1' && currentToken[1] != '8')
            {
                compileError(5) ;
            }

            procCount ++ ;

            getNextToken() ;

            if(currentToken[0] != '3' && currentToken[1] != '2')
            {
                break ;
            }
        }

    }

    newCode[tempCodeIndex].m = codeIndex ;

    //inc instruction
    emit(6,0,4+varCount) ;

    //go into statement
    statement() ;

    tableIndex -= (constantCount+varCount+procCount) ;

    if(type != 0)
    {
        //return
        emit(2,0,0);
    }
}

//function to parse a statement
void statement()
{
    if(currentToken[0] == '2' && currentToken[1] == '1') //check for begin
    {
        getNextToken() ;

        //go into statement
        statement() ;

        //check for semicolon symbol
        while(currentToken[0] == '1' && currentToken[1] == '8')
        {
            getNextToken() ;

            //go into statement
            statement() ;
        }

        //check for end symbol
        if(currentToken[0]!= '2' || currentToken[1]!= '2')
        {
            compileError(26) ;
        }

        getNextToken() ;

    }else if(currentToken[0] == '2' && currentToken[1] == '7') //check for call symbol
    {
        getNextToken() ;

        //check for identifier
        if(currentToken[0] != '2')
        {
            compileError(4) ;
        }

        getNextToken() ;

        char procName[10] ;
        strcpy(procName,trim(currentToken)) ;

        int index = searchTable(procName) ;

        //name not declared
        if(index == -1)
        {
            compileError(11) ;
        }

        if(symbol_table[index].kind != 3)
        {
            compileError(23333) ;
        }

        //generate call instruction
        emit(5,lexLevel-symbol_table[index].level,symbol_table[index].addr+1) ;

        getNextToken() ;

    }else if(currentToken[0] == '2' && currentToken[1] == '3') //check for if symbol
    {
        getNextToken() ;

        condition() ;

        if(currentToken[0]!= '2' || currentToken[1]!= '4') //check for then symbol
        {
            compileError(16) ;
        }

        getNextToken()  ;

        int cTemp = codeIndex ;

        //jump on condition
        emit(8,0,0) ;

        statement() ;

        //check for else
        if(currentToken[0]=='3' && currentToken[1]=='3')
        {
            int cTemp2 = codeIndex ;

            emit(7,0,0) ;

            newCode[cTemp].m = codeIndex ;

            getNextToken() ;

            statement() ;

            newCode[cTemp2].m = codeIndex ;
        }else
        {
            newCode[cTemp].m = codeIndex ;
        }

    }else if(currentToken[0] == '2' && currentToken[1] == '5') //check for while symbol
    {
        int cx1 = codeIndex ;

        getNextToken() ;

        condition() ;

        int cx2 = codeIndex ;

        //jump on condition
        emit(8,0,0) ;

        if(currentToken[0]!= '2' || currentToken[1]!= '6')
        {
            compileError(18) ;
        }

        getNextToken() ;

        statement() ;

        //jump
        emit(7,0,cx1)  ;
        newCode[cx2].m = codeIndex ;
    }else if(currentToken[0] == '3' && currentToken[1] == '1') //check for read symbol
    {
        getNextToken() ;

        if(currentToken[0] != '2')
        {
            compileError(4) ;
        }

        getNextToken() ;

        char variableName[10] ;
        strcpy(variableName,trim(currentToken)) ;

        //read input
        emit(9,0,2) ;

        int index = searchTable(variableName) ;

        if(index == -1)
        {
            compileError(11) ;
        }

        if(symbol_table[index].kind==1)
        {
            compileError(28) ;
        }else if(symbol_table[index].kind==2)
        {
            emit(4,lexLevel - symbol_table[index].level,symbol_table[index].addr) ;
        }else
        {
            compileError(28) ;
        }

        getNextToken() ;

    }else if(currentToken[0] == '3' && currentToken[1] == '0') //check for write symbol
    {
        getNextToken() ; //2

        if(currentToken[0] != '2')
        {
            compileError(4) ;
        }

        getNextToken() ;

        char variableName[10] ;
        strcpy(variableName,trim(currentToken)) ;

        int index = searchTable(variableName) ;

        if(index == -1)
        {
            compileError(11) ;
        }

        if(symbol_table[index].kind==1)
        {
            //lit operation
            emit(1,0,symbol_table[index].val) ;
        }else if(symbol_table[index].kind==2)
        {
            //load operation
            emit(3,lexLevel - symbol_table[index].level,symbol_table[index].addr) ;
        }else
        {
            compileError(27) ;
        }


        //write the top of the stack to the screen
        emit(9,0,1) ;

        getNextToken() ;
    }else if(currentToken[0] == '2' && currentToken[1]==' ') //check for identifier
    {
        getNextToken() ;

        char variableName[10] ;
        strcpy(variableName,trim(currentToken)) ;

        int index = searchTable(variableName) ;

        //variable not declared
        if(index == -1)
        {
            compileError(11) ;
        }

        //trying to assign values to a constant
        if(symbol_table[index].kind!=2)
        {
            compileError(12) ;
        }


        getNextToken() ;

        //check for become symbol
        if(currentToken[0]!= '2' || currentToken[1]!= '0')
        {
            compileError(13) ;
        }

        getNextToken() ;

        expression()  ;

        //store operation
        emit(4,lexLevel - symbol_table[index].level,symbol_table[index].addr) ;
    }
}

//function to parse a condition
void condition()
{
    if(currentToken[0] == '8') // check for odd symbol
    {
        getNextToken() ;

        expression() ;
    }else
    {
        expression() ;

        //check for relational operator
        if(currentToken[0]!= '9' && (currentToken[0]!= '1' || (currentToken[1]!= '0' && currentToken[1]!= '1' && currentToken[1]!= '2' && currentToken[1]!= '3' && currentToken[1]!= '4')))
        {
            compileError(20) ;
        }

        char operation[10] ;
        strcpy(operation,currentToken) ;


        getNextToken() ;

        expression() ;

         switch(operation[0])
        {
            case '9':
                //eql
                emit(2,0,8) ;
                break ;
            case '1':
                switch(operation[1])
                {
                    case '0':
                        //neq
                        emit(2,0,9) ;
                        break ;
                    case '1':
                        //lss
                        emit(2,0,10) ;
                        break ;
                    case '2' :
                        //leq
                        emit(2,0,11) ;
                        break ;
                    case '3':
                        //gtr
                        emit(2,0,12) ;
                        break  ;
                    case '4':
                        //geo
                        emit(2,0,13) ;
                        break ;
                }
                break ;
        }
    }
}

//function to parse an expression
void expression()
{
    char operation  ;

    if(currentToken[0] == '4'|| currentToken[0] == '5') //check for plus or minus
    {
        operation = currentToken[0] ;

        getNextToken() ;

        term() ;

        if(operation == '5')
        {
            //negate
            emit(2,0,1) ;
        }
    }else
    {
        term() ;
    }



    while(currentToken[0] == '4'|| currentToken[0] == '5') //check for plus or minus
    {
        operation = currentToken[0] ;

        getNextToken() ;

        term() ;

        if(operation == '4')
        {
            //add
            emit(2,0,2) ;
        }else
        {
            //sub
            emit(2,0,3) ;
        }
    }
}

//function to parse a term
void term()
{
    char operation ;

    factor() ;

    while(currentToken[0] == '6'|| currentToken[0] == '7')
    {
        operation = currentToken[0] ;

        getNextToken() ;

        factor() ;

        if(operation == '6')
        {
            //multiply
            emit(2,0,4) ;
        }else
        {
            //divide
            emit(2,0,5) ;
        }
    }

}

//function to parse a factor
void factor()
{
    if(currentToken[0] == '2') //check for identifier
    {
        getNextToken() ;

        char variableName[10]  ;
        strcpy(variableName,trim(currentToken)) ;

        int index = searchTable(variableName) ;

        //variable not declared
        if(index == -1)
        {
            compileError(11) ;
        }

        if(symbol_table[index].kind==1)
        {
            //lit operation
            emit(1,0,symbol_table[index].val) ;
        }else if(symbol_table[index].kind==2)
        {
            //load operation
            emit(3,lexLevel - symbol_table[index].level,symbol_table[index].addr) ;
        }else
        {
            compileError(27) ;
        }

        getNextToken() ;
    }else if(currentToken[0] == '3') //check for number
    {
        getNextToken() ;

        int num = atoi(currentToken) ;

        //lit operation
        emit(1,0,num) ;

        getNextToken() ;
    }else if(currentToken[0] == '1' && currentToken[1] == '5') // check for left paren
    {
        getNextToken() ;

        expression() ;

        if(currentToken[0]!= '1' || currentToken[1]!= '6') //check for right paren
        {
            compileError(22) ;
        }

        getNextToken() ;
    }else
    {
        compileError(29);
    }
}

//generates code for the virtual machine and stores it in an array
void emit(int op, int l, int m)
{
    if(codeIndex > MAX_CODE_LENGTH)
    {
        compileError(16) ;
    }else
    {
        newCode[codeIndex].op = op ; //op code
        newCode[codeIndex].l = l ; //lexicographical level
        newCode[codeIndex].m = m  ; //modifier
        codeIndex ++ ;
    }
}

//updates currentToken to the next token in the token list
void getNextToken()
{
    int x = 0 ;
    while(strcmp(&lexemeList[tokenIndex+x+1],"")!=0)
    {
        if(lexemeList[tokenIndex+x+1] == 32)
        {
            break ;
        }
        x++ ;
    }

    if(tokenIndex==0)
    {
        strncpy(currentToken,&lexemeList[tokenIndex],x+1) ;
    }else
    {
        strncpy(currentToken,&lexemeList[tokenIndex+1],x+1) ;
    }


    tokenIndex = tokenIndex + x + 1  ;

}

//function to return the index of variable in the symbol table or -1 if it doesnt exist
int searchTable(const char *name)
{
    for(int i=tableIndex-1;i>0;i--)
    {
        //found the variable
        if(strcmp(symbol_table[i].name,name)==0)
        {
            return i ;
        }
    }

    return -1 ;
}

//function to eliminate extra chars from token strings
char* trim(char name[10])
{
    int gap = 0 ;
    for(int i=0;i<10;i++)
    {
        if(name[i] == ' ' && gap==0)
        {
            gap = i ;
        }
    }

    for(int i=0;i<11;i++)
    {
        if(i>gap)
        {
            name[i] = ' ' ;
        }
    }

    return name;
}

void enter(int type,char name [10],int value, int level)
{
    if(searchTable(name)!=-1)
    {
    }

    symbol_table[tableIndex].level = level ;

    symbol_table[tableIndex].kind = type ;
    strcpy(symbol_table[tableIndex].name, name) ;

    //constant
    if(type == 1)
    {
        symbol_table[tableIndex].val = value ;
    }

    //variable
    if(type == 2)
    {
        symbol_table[tableIndex].addr = varIndex ;
        varIndex ++ ;
    }

    //procedure
    if(type == 3)
    {
        symbol_table[tableIndex].addr = procIndex ;
        procIndex ++ ;
    }

    tableIndex ++ ;
}

//function to output the code in assembly
void outputParserCode(int a){
    FILE * outputFile = fopen("parserOut.txt", "w");

    if(a==1)
    {
        printf("\n") ;
        printf("Parser Assembly Output\n") ;
    }

    int i = 0 ;
    for(i=0; i<=codeIndex; i++){
            if(newCode[i].op==0){
                break ;
            }

            fprintf(outputFile, "%d",newCode[i].op) ;
            fprintf(outputFile, " %d",newCode[i].l) ;
            fprintf(outputFile, " %d",newCode[i].m) ;
            fprintf(outputFile, "\n") ;

            if(a==1)
            {
                printf("Line: %d",i) ;
                printf("    %d",newCode[i].op) ;
                printf(" %d",newCode[i].l) ;
                printf(" %d",newCode[i].m) ;
                printf("\n") ;
            }
    }

    fclose(outputFile) ;
}

//stops parsing and outputs the error found
void compileError(int error)
{
    printf("Compiler Error # %d, ",error) ;

    switch(error)
    {
        case 1:
            printf("Use = instead of := .\n") ;
            break ;
        case 2:
            printf("= must be followed by a number.\n") ;
            break ;
        case 3 :
            printf("Identifier must be followed by = . \n") ;
            break ;
        case 4:
            printf("const or var must be followed by an identifier. \n") ;
            break ;
        case 5:
            printf("Semicolon or comma missing. \n") ;
            break ;
        case 6:
            printf("Incorrect symbol after procedure declaration. \n") ;
            break ;
        case 7 :
            printf("Statement expected. \n") ;
            break ;
        case 8:
            printf("Incorrect symbol after statement part in block. \n") ;
            break ;
        case 9:
            printf("Period expected.  \n") ;
            break ;
        case 10:
            printf("Semicolon between statements expected. \n") ;
            break ;
        case 11:
            printf("Undeclared identifier \n") ;
            break ;
        case 12:
            printf("Assignment to constant or procedure is not allowed. \n") ;
            break ;
        case 13:
            printf("Assignment operator expected. \n") ;
            break ;
        case 14:
            printf("Call must be used on a procedure. ") ;
            break ;
        case 15:
            break ;
        case 16:
            printf("then expected. \n") ;
            break ;
        case 17:
            printf("Semicolon or } expected.  \n") ;
            break ;
        case 18:
            printf("do expected. \n") ;
            break ;
        case 19:
            break ;
        case 20:
            printf("Relational operator expected. \n") ;
            break ;
        case 21:
            break ;
        case 22:
            printf("Right parenthesis missing. \n") ;
            break ;
        case 23:
            break ;
        case 24:
            break ;
        case 25:
            break ;
        case 26:
            printf("end expected. \n") ;
            break ;
        case 27:
            printf("constant or variable expected. \n") ;
            break ;
        case 28:
            printf("variable expected. \n") ;
            break ;
        case 29:
            printf("facor expected. \n") ;
            break ;
    }

    exit(0) ;
}

#endif // PARSER_H_INCLUDED
