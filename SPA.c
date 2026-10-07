// Time Complexity O(N)
// Space Complexity O(N)

// Haven't added many validation checks yet

#include<stdio.h>
#include<stdbool.h>

#define MAX_STUDENTS 100

struct students
{
    int roll_no;
    char name[50];
    int marks1;
    int marks2;
    int marks3;
};

typedef struct students stud;


// function to calculate total marks
int calculateTotal(stud student)
{
    return student.marks1 + student.marks2 + student.marks3;
}


// To calculate average marks
float calculateAverage(int total)
{
    return total / 3.0;
}


// Calculating grade
char calculateGrade(float average)
{
    if(average >= 85)
        return 'A';
    else if(average >= 70)
        return 'B';
    else if(average >= 50)
        return 'C';
    else if(average >= 35)
        return 'D';
    else
        return 'F';
}


// For printing roll numbers using recursion
void printRollNumbers(stud Students[], int index, int number_of_stud)
{
    // base condition for recursion
    if(index == number_of_stud)
        return;

    printf("%d ", Students[index].roll_no);

    // calling the function for the next student
    printRollNumbers(Students, index + 1, number_of_stud);
}


int main()
{
    stud Students[MAX_STUDENTS];

    int number_of_stud;

    printf("Enter the number of students (MAX 100): ");
    scanf("%d", &number_of_stud);

    // checking the number of students
    if(number_of_stud < 1 || number_of_stud > MAX_STUDENTS)
    {
        printf("Invalid number of students\n");
        return 1;
    }

    printf("\nEnter the details of students:\n");
    printf("Rollno Name Marks1 Marks2 Marks3\n");

    // taking input for all students
    for(int i = 0; i < number_of_stud; i++)
    {
        scanf("%d %49s %d %d %d",
              &Students[i].roll_no,
              Students[i].name,
              &Students[i].marks1,
              &Students[i].marks2,
              &Students[i].marks3);

        // checking if marks are within the valid range
        if(Students[i].marks1 < 0 || Students[i].marks1 > 100 ||
           Students[i].marks2 < 0 || Students[i].marks2 > 100 ||
           Students[i].marks3 < 0 || Students[i].marks3 > 100)
        {
            printf("Invalid marks\n");
            return 1;
        }
    }


    // calculating and displaying the performance
    for(int i = 0; i < number_of_stud; i++)
    {
        int total;
        float average;
        char grade;

        // calculating total
        total = calculateTotal(Students[i]);

        // calculating average
        average = calculateAverage(total);

        // calculating grade
        grade = calculateGrade(average);

        printf("Roll: %d\n", Students[i].roll_no);
        printf("Name: %s\n", Students[i].name);
        printf("Total: %d\n", total);
        printf("Average: %.2f\n", average);
        printf("Grade: %c\n", grade);

        // F grade does not have a performance pattern
        if(grade == 'F')
            continue;

        printf("Performance: ");

        // printing performance according to grade
        switch(grade)
        {
            case 'A':
                printf("*****\n");
                break;

            case 'B':
                printf("****\n");
                break;

            case 'C':
                printf("***\n");
                break;

            case 'D':
                printf("**\n");
                break;
        }

        printf("\n");
    }


    // printing roll numbers using recursion
    printf("List of Roll Numbers (via recursion): ");

    printRollNumbers(Students, 0, number_of_stud);

    printf("\n");

    return 0;
}