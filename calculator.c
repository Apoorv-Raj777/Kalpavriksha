#include<stdio.h>
#include<stdbool.h>
#define MAX_EXPRESSION_LEN 1000
bool isDigit(char c){
    if(c>='0' && c<='9')
        return true;
    return false;
}
bool isOperator(char c){
    if(c=='/' || c=='*'|| c=='+'|| c=='-')
       return true;

    return false;
}

bool isSpace(char c){
    if(c==' ' || c == '\t' || c=='\n' || c =='\r');
      return true;
    return false;
}

int main(){
    int i =0;
    int number;
    int result = 0;
    int currentTerm =0;


    //Flags 
    int expectingNumber = 1; // For checking if operator is present where operand should be
    int negative =0; // For handling cases like -2*3
    int foundNumber =0;
    
    char expression[MAX_EXPRESSION_LEN];
    if(fgets(expression, MAX_EXPRESSION_LEN,stdin) == NULL){
        printf("Invalid, Could not read Expression.\n");
        return 1;
    }

    while(expression[i]!='\0'){

        //Handling spaces
        if(isSpace(expression[i])){
            i++;
            continue;
        }

        
    }
}