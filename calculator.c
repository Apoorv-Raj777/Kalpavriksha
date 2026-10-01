#include<stdio.h>
#include<stdbool.h>

#define MAX_EXPRESSION_LENGTH 1000

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

bool isWhitespace(char c){
    if(c==' ' || c == '\t' || c=='\n' || c =='\r')
      return true;
    return false;
}

int main(){

    int index = 0;
    long long currentNumber;
    long long result = 0;
    long long currentTerm = 0;

    //For Storing the previous operation +,-,*,/
    char pendingOperation = '+';

    //Flags 
    int expectingNumber = 1; // For checking if operator is present where operand should be
    int isNegative = 0; // For handling cases like -2*3
    int foundNumber = 0;
    
    char expression[MAX_EXPRESSION_LENGTH];

    printf("Enter the expression.\n");
    if(fgets(expression, MAX_EXPRESSION_LENGTH,stdin) == NULL){
        printf("Invalid, Could not read Expression.\n");
        return 1;
    }

    while(expression[index]!='\0'){

        //Handling spaces
        if(isWhitespace(expression[index])){
            index++;
            continue;
        }

        // Handling unary '-'
        // For e.g. -3 , -3*2, 4*-2 etc
        if(expression[index]=='-' && expectingNumber){
            isNegative = !isNegative;
            index++;
            continue;
        }

        // Handling unary '+'
        // Since +2 == 2
        if(expression[index]=='+' && expectingNumber){
            // Do nothing and just move the iterator ahead
            index++;
            continue;
        }

        // Reading a number
        if(isDigit(expression[index])){

            // A number cannot come immediately after another number
            if(!expectingNumber){
                printf("Error, Invalid Expression\n");
                return 1;
            }

            currentNumber = 0;

            while(isDigit(expression[index])){
                currentNumber = currentNumber * 10 +
                                 (expression[index]-'0');
                index++;
            }

            if(isNegative){
                currentNumber = -currentNumber;
                isNegative = 0;
            }

            // Handling the previous Operations
            if(pendingOperation=='+'){
                result += currentTerm;
                currentTerm = currentNumber;
            }
            else if(pendingOperation=='-'){
                result += currentTerm;
                currentTerm = -currentNumber;
            }
            else if (pendingOperation == '*')
            {
                currentTerm *= currentNumber;
            }
            else if (pendingOperation == '/')
            {
                // Checking Division by 0 error
                if(currentNumber == 0){
                    printf("Error, Division by Zero\n");
                    return 1;
                }
                currentTerm /= currentNumber;
            }
            
            //Now that we have got the number reset the flags;
            expectingNumber = 0;
            foundNumber = 1;  

            continue;
        }

        //Reading an operator
        if(isOperator(expression[index])){

            if(expectingNumber){
                printf("Error, Invalid Expression\n");
                return 1;
            }

            pendingOperation = expression[index];
            expectingNumber = 1;
            index++;
            continue;
        }

        // Apart from number, operators and whitespaces all are invalid
        printf("Error, Invalid Expression\n");
        return 1;
    }

    // Expression cannot end with an operator and atleast one number should be entered
    if(!foundNumber || expectingNumber){
        printf("Error, Invalid Expression\n");
        return 1;
    }

    // Adding the final term
    result += currentTerm;

    printf("%lld\n",result);

    return 0;
}