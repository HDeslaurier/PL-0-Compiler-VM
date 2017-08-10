#include <stdio.h>
#include <stdlib.h>
#include "lexicalAnalyzer.h"
#include "machine.h"
#include "parser.h"

int main(int argc, char ** argv)
{
    int l = 0, a = 0, v = 0, error = 0 ;

    char ** temp = argv  ;

    if(argc == 2) // ./compile [input]
    {
        printf("Reading program \n") ;
        lexer(0,argv);
        printf("Parsing program \n") ;
        parser(0) ;
        printf("Executing VM \n") ;
        machine(0) ;

        return 0 ;

    }else if(argc == 3) // ./compile [input] []
    {
        if(strcmp(temp[2],"-l")==0)
        {
            l = 1 ;
        }else if(strcmp(temp[2],"-a")==0)
        {
            a = 1 ;
        }else if(strcmp(temp[2],"-v")==0)
        {
            v = 1 ;
        }else
        {
            error = 1 ;
        }
    }else if(argc == 4) // ./compile [input] [] []
    {
        for(int i=2; i<=3;i++)
        {
            if(strcmp(temp[i],"-l")==0)
            {
                l = 1 ;
            }else if(strcmp(temp[i],"-a")==0)
            {
                a = 1 ;
            }else if(strcmp(temp[i],"-v")==0)
            {
                v = 1 ;
            }else
            {
                error = 1 ;
            }
        }
    }else if(argc == 5) // ./compile [input] [] [] []
    {
        for(int i=2; i<=4;i++)
        {
            if(strcmp(temp[i],"-l")==0)
            {
                l = 1 ;
            }else if(strcmp(temp[i],"-a")==0)
            {
                a = 1 ;
            }else if(strcmp(temp[i],"-v")==0)
            {
                v = 1 ;
            }else
            {
                error = 1 ;
            }
        }
    }else //error in input
    {
        error = 1 ;
    }

    if(error == 1)
    {
        printf("Incorrect use of the compiler! ./compiler [input] [directive] [directive] [directive] \n") ;
        return 1;
    }

    printf("\n") ;
    printf("Reading program \n") ;
    printf("\n") ;
    lexer(l,argv) ;

    printf("\n") ;
    printf("Parsing program \n") ;
    parser(a) ;

    printf("\n") ;
    printf("Executing VM \n") ;
    machine(v) ;



    return 0 ;
}
