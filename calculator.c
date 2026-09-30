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

    //For Storing the previous operation +,-,*,/
    char operation = '+';

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

        // Handling unary '-'
        // For e.g. -3 , -3*2, 4*-2 etc
        if(expression[i]=='-' && expectingNumber){
            negative = !negative;
            i++;
            continue;
        }

        // Handling unary '+'
        // Since +2 == 2
        if(expression[i]=='+' && expectingNumber){
            // Do nothing and just move the iterator ahead
            i++;
            continue;
        }

        // Reading a number
        if(isDigit(expression[i])){
            number =0;

            while(isDigit(expression[i])){
                number = number * 10 + (expression[i]-'0');
                i++;
            }

            if(negative){
                number = -number;
                negative =0;
            }

            // Handling the previous Operations
            if(operation=='+'){
                result += currentTerm;
                currentTerm = number;
            }
            else if(operation=='-'){
                result += currentTerm;
                currentTerm = -number;
            }
            else if (operation == '*')
            {
                currentTerm *= number;
            }
            else if (operation == '/')
            {
                // Checking Division by 0 error
                if(number == 0){
                    printf("Error, Division by Zero\n");
                    return 1;
                }
                currentTerm /= number;
            }
            
            //Now that we have got the number reset the flags;
            expectingNumber =0;
            foundNumber = 1;  

            continue;
        }

        //Reading an operator
        if(isOperator(expression[i])){

            if(expectingNumber){
                printf("Error, Invaild Expression\n");
                return 1;
            }

            operation = expression[i];  // Storing the the previous expression
            expectingNumber =1;
            i++;
            continue;
        }

        // Apart from number, operators and whitespaces all are invalid
        printf("Error, Invaild Expression\n");
        return 1;
    }

    // Expression cannot end with an operator and atleast one number should be entered
    if(!foundNumber || expectingNumber){
        printf("Error, Invaild Expression\n");
        return 1;
    }

    // Adding the final term

    result += currentTerm;

    printf("%d\n",result);

    return 0;
}