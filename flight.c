#include <stdio.h>
#include <string.h>

#define STANDARD_PRICE 150
#define PREMIUM_PRICE 250
#define RECLINER_PRICE 400
#define MIN_TICKETS 1
#define MAX_TICKETS 20
#define DISCOUNT_RATE 0.10


// Function to check whether the name contains only letters and spaces
int isValidName(char name[])
{
    int length = 0;

    while(name[length] != '\0')
    {
        if(!((name[length] >= 'A' && name[length] <= 'Z') ||
             (name[length] >= 'a' && name[length] <= 'z') ||
             name[length] == ' '))
        {
            return 0;
        }

        length++;
    }

    if(length == 0)
        return 0;

    // Checking for leading and trailing spaces
    if(name[0] == ' ' || name[length - 1] == ' ')
        return 0;

    return 1;
}


// Function to read a complete name and check for buffer overflow
int readName(char name[], int size)
{
    int ch;
    int length;

    if(fgets(name, size, stdin) == NULL)
        return 0;

    length = strlen(name);

    // Checking whether the newline was captured
    if(length > 0 && name[length - 1] == '\n')
    {
        name[length - 1] = '\0';
    }
    else
    {
        // Checking whether more characters remain in the input
        ch = getchar();

        if(ch != '\n' && ch != EOF)
        {
            // Clearing the remaining characters
            while((ch = getchar()) != '\n' && ch != EOF)
            {
                // Clearing input buffer
            }

            return 0;
        }

        // Rejecting a line that was not terminated by a newline
        if(ch == EOF && length == 0)
            return 0;
    }

    return isValidName(name);
}


// Function to check for extra input on an integer line
int hasExtraInput(void)
{
    int ch;
    int extra_input = 0;

    while((ch = getchar()) != '\n' && ch != EOF)
    {
        if(ch != ' ' && ch != '\t' && ch != '\r')
        {
            extra_input = 1;
        }
    }

    return extra_input;
}


// Function to read and validate an integer
int readInteger(int *value)
{
    int ch;

    if(scanf("%d", value) != 1)
    {
        // Clearing invalid input
        while((ch = getchar()) != '\n' && ch != EOF)
        {
            // Clearing input buffer
        }

        return 0;
    }

    if(hasExtraInput())
        return 0;

    return 1;
}


int main(void)
{
    char customer_name[50];

    int number_of_tickets;
    int category_choice;
    int ticket_price = 0;

    double subtotal;
    double discount;
    double final_bill;

    const char *ticket_category = NULL;


    // Reading the customer name
    if(!readName(customer_name, sizeof(customer_name)))
    {
        printf("Invalid customer name\n");
        return 1;
    }


    // Reading the number of tickets
    if(!readInteger(&number_of_tickets))
    {
        printf("Invalid ticket count\n");
        return 1;
    }

    // Checking the ticket count
    if(number_of_tickets < MIN_TICKETS ||
       number_of_tickets > MAX_TICKETS)
    {
        printf("Invalid ticket count\n");
        return 1;
    }


    // Reading the ticket category
    if(!readInteger(&category_choice))
    {
        printf("Invalid ticket category\n");
        return 1;
    }


    // Selecting the ticket price using switch-case
    switch(category_choice)
    {
        case 1:
            ticket_price = STANDARD_PRICE;
            ticket_category = "Standard";
            break;

        case 2:
            ticket_price = PREMIUM_PRICE;
            ticket_category = "Premium";
            break;

        case 3:
            ticket_price = RECLINER_PRICE;
            ticket_category = "Recliner";
            break;

        default:
            printf("Invalid ticket category\n");
            return 1;
    }


    // Calculating the subtotal
    subtotal = ticket_price * number_of_tickets;


    // Applying the discount
    if(number_of_tickets >= 5)
    {
        discount = subtotal * DISCOUNT_RATE;
    }
    else
    {
        discount = 0.0;
    }


    // Calculating the final bill
    final_bill = subtotal - discount;


    // Displaying the booking details
    printf("Customer: %s\n", customer_name);
    printf("Ticket Category: %s\n", ticket_category);
    printf("Tickets: %d\n", number_of_tickets);
    printf("Subtotal: %.2f\n", subtotal);
    printf("Discount: %.2f\n", discount);
    printf("Final Bill: %.2f\n", final_bill);

    return 0;
}