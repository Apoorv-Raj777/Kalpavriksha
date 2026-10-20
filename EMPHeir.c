#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NAME_LENGTH 50
#define MAX_DESIGNATION_LENGTH 50

// Structure representing an employee
typedef struct Employee
{
    int employee_id;
    char name[MAX_NAME_LENGTH];
    char designation[MAX_DESIGNATION_LENGTH];

    struct Employee *manager;
    struct Employee *reports;
    struct Employee *next_report;
    struct Employee *next_employee;
} Employee;


// Master list containing all employees
Employee *employee_list = NULL;


// Function declarations
Employee *findEmployee(int employee_id);
int employeeIdExists(int employee_id);
int readInteger(int *value);
int readLine(char input[], int size);
int countDirectReports(Employee *employee);
int countEmployeesUnder(Employee *employee);
int countAllEmployees(Employee *employee);
void addEmployee(void);
void removeEmployee(void);
void displayEmployeeHierarchy(void);
void displayCompleteHierarchy(void);
void displayReportsRecursively(Employee *employee, int depth);
void displayOrganizationRecursively(Employee *employee, int depth);
void freeEmployees(Employee *employee);


// Find an employee using their ID
Employee *findEmployee(int employee_id)
{
    Employee *current_employee = employee_list;

    while(current_employee != NULL)
    {
        if(current_employee->employee_id == employee_id)
            return current_employee;

        current_employee = current_employee->next_employee;
    }

    return NULL;
}


// Check whether an employee ID already exists
int employeeIdExists(int employee_id)
{
    return findEmployee(employee_id) != NULL;
}


// Read an integer and reject extra characters on the same line
int readInteger(int *value)
{
    int character;
    int extra_input = 0;

    if(scanf("%d", value) != 1)
    {
        while((character = getchar()) != '\n' && character != EOF)
        {
            // Clear invalid input
        }

        return 0;
    }

    while((character = getchar()) != '\n' && character != EOF)
    {
        if(!isspace((unsigned char)character))
            extra_input = 1;
    }

    return !extra_input;
}


// Read a complete line safely
int readLine(char input[], int size)
{
    size_t length;
    int character;

    if(fgets(input, size, stdin) == NULL)
        return 0;

    length = strlen(input);

    if(length > 0 && input[length - 1] == '\n')
    {
        input[length - 1] = '\0';
    }
    else
    {
        character = getchar();

        if(character != '\n' && character != EOF)
        {
            while((character = getchar()) != '\n' && character != EOF)
            {
                // Clear remaining characters
            }

            return 0;
        }
    }

    if(input[0] == '\0')
        return 0;

    return 1;
}


// Count an employee's direct reports
int countDirectReports(Employee *employee)
{
    int count = 0;
    Employee *current_report = employee->reports;

    while(current_report != NULL)
    {
        count++;
        current_report = current_report->next_report;
    }

    return count;
}


// Count all employees reporting under an employee recursively
int countEmployeesUnder(Employee *employee)
{
    int count = 0;
    Employee *current_report = employee->reports;

    while(current_report != NULL)
    {
        count++;
        count += countEmployeesUnder(current_report);

        current_report = current_report->next_report;
    }

    return count;
}


// Count all employees in the master list
int countAllEmployees(Employee *employee)
{
    int count = 0;

    while(employee != NULL)
    {
        count++;
        employee = employee->next_employee;
    }

    return count;
}


// Display an employee and their reporting hierarchy recursively
void displayReportsRecursively(Employee *employee, int depth)
{
    Employee *current_report = employee->reports;

    while(current_report != NULL)
    {
        for(int index = 0; index < depth; index++)
            printf("    ");

        printf("%d - %s - %s\n",
               current_report->employee_id,
               current_report->name,
               current_report->designation);

        displayReportsRecursively(current_report, depth + 1);

        current_report = current_report->next_report;
    }
}


// Display the complete organization recursively
void displayOrganizationRecursively(Employee *employee, int depth)
{
    if(employee == NULL)
        return;

    for(int index = 0; index < depth; index++)
        printf("    ");

    printf("%d - %s - %s\n",
           employee->employee_id,
           employee->name,
           employee->designation);

    displayOrganizationRecursively(employee->reports, depth + 1);
    displayOrganizationRecursively(employee->next_report, depth);
}


// Add a new employee
void addEmployee(void)
{
    Employee *new_employee;
    Employee *manager = NULL;
    int manager_id;

    new_employee = malloc(sizeof(Employee));

    if(new_employee == NULL)
    {
        printf("Memory allocation failed\n");
        return;
    }

    printf("Enter Employee ID: ");

    if(!readInteger(&new_employee->employee_id) ||
       new_employee->employee_id <= 0)
    {
        printf("Invalid employee ID\n");
        free(new_employee);
        return;
    }

    if(employeeIdExists(new_employee->employee_id))
    {
        printf("Employee ID already exists\n");
        free(new_employee);
        return;
    }

    printf("Enter Employee Name: ");

    if(!readLine(new_employee->name, sizeof(new_employee->name)))
    {
        printf("Invalid employee name\n");
        free(new_employee);
        return;
    }

    printf("Enter Designation: ");

    if(!readLine(new_employee->designation,
                 sizeof(new_employee->designation)))
    {
        printf("Invalid designation\n");
        free(new_employee);
        return;
    }

    printf("Enter Reporting Manager ID (0 for top-level employee): ");

    if(!readInteger(&manager_id) || manager_id < 0)
    {
        printf("Invalid manager ID\n");
        free(new_employee);
        return;
    }

    if(manager_id != 0)
    {
        manager = findEmployee(manager_id);

        if(manager == NULL)
        {
            printf("Reporting manager not found\n");
            free(new_employee);
            return;
        }
    }

    new_employee->manager = manager;
    new_employee->reports = NULL;
    new_employee->next_report = NULL;

    // Add employee to the master list
    new_employee->next_employee = employee_list;
    employee_list = new_employee;

    // Add employee to the manager's reports list
    if(manager != NULL)
    {
        new_employee->next_report = manager->reports;
        manager->reports = new_employee;
    }

    printf("Employee added successfully\n");
}


// Remove an employee
void removeEmployee(void)
{
    int employee_id;
    Employee *employee;
    Employee *manager;
    Employee *sibling;
    Employee *least_busy_sibling = NULL;
    Employee *previous_employee;
    Employee *current_report;

    printf("Enter Employee ID to remove: ");

    if(!readInteger(&employee_id) || employee_id <= 0)
    {
        printf("Invalid employee ID\n");
        return;
    }

    employee = findEmployee(employee_id);

    if(employee == NULL)
    {
        printf("Employee not found\n");
        return;
    }

    if(employee->manager == NULL)
    {
        printf("The top-level employee cannot be removed\n");
        return;
    }

    manager = employee->manager;

    // Find the sibling with the fewest direct reports
    sibling = manager->reports;

    while(sibling != NULL)
    {
        if(sibling != employee)
        {
            if(least_busy_sibling == NULL ||
               countDirectReports(sibling) <
               countDirectReports(least_busy_sibling))
            {
                least_busy_sibling = sibling;
            }
        }

        sibling = sibling->next_report;
    }

    // Transfer reports to the least busy sibling or the manager
    while(employee->reports != NULL)
    {
        current_report = employee->reports;
        employee->reports = current_report->next_report;

        if(least_busy_sibling != NULL)
        {
            current_report->manager = least_busy_sibling;
            current_report->next_report = least_busy_sibling->reports;
            least_busy_sibling->reports = current_report;
        }
        else
        {
            current_report->manager = manager;
            current_report->next_report = manager->reports;
            manager->reports = current_report;
        }
    }

    // Remove employee from the manager's reports list
    if(manager->reports == employee)
    {
        manager->reports = employee->next_report;
    }
    else
    {
        sibling = manager->reports;

        while(sibling != NULL && sibling->next_report != employee)
            sibling = sibling->next_report;

        if(sibling != NULL)
            sibling->next_report = employee->next_report;
    }

    // Remove employee from the master list
    if(employee_list == employee)
    {
        employee_list = employee->next_employee;
    }
    else
    {
        previous_employee = employee_list;

        while(previous_employee != NULL &&
              previous_employee->next_employee != employee)
        {
            previous_employee = previous_employee->next_employee;
        }

        if(previous_employee != NULL)
            previous_employee->next_employee = employee->next_employee;
    }

    free(employee);

    printf("Employee removed successfully\n");
}


// Display one employee's hierarchy
void displayEmployeeHierarchy(void)
{
    int employee_id;
    Employee *employee;

    printf("Enter Employee ID: ");

    if(!readInteger(&employee_id) || employee_id <= 0)
    {
        printf("Invalid employee ID\n");
        return;
    }

    employee = findEmployee(employee_id);

    if(employee == NULL)
    {
        printf("Employee not found\n");
        return;
    }

    printf("%d - %s - %s\n",
           employee->employee_id,
           employee->name,
           employee->designation);

    displayReportsRecursively(employee, 1);

    printf("Total Employees Under %d: %d\n",
           employee_id,
           countEmployeesUnder(employee));
}


// Display the complete organization hierarchy
void displayCompleteHierarchy(void)
{
    Employee *current_employee = employee_list;
    int total_employees = countAllEmployees(employee_list);

    printf("===== ORGANIZATION HIERARCHY =====\n");

    while(current_employee != NULL)
    {
        if(current_employee->manager == NULL)
        {
            displayOrganizationRecursively(current_employee, 0);
        }

        current_employee = current_employee->next_employee;
    }

    printf("Total Employees: %d\n", total_employees);
}


// Free all dynamically allocated employees
void freeEmployees(Employee *employee)
{
    Employee *next_employee;

    while(employee != NULL)
    {
        next_employee = employee->next_employee;
        free(employee);
        employee = next_employee;
    }
}


int main(void)
{
    int menu_choice;

    do
    {
        printf("\n===== EMPLOYEE HIERARCHY MANAGEMENT =====\n");
        printf("1. Add Employee\n");
        printf("2. Remove Employee\n");
        printf("3. Display Employee Hierarchy\n");
        printf("4. Display Complete Hierarchy\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");

        if(!readInteger(&menu_choice))
        {
            printf("Invalid menu choice\n");
            continue;
        }

        switch(menu_choice)
        {
            case 1:
                addEmployee();
                break;

            case 2:
                removeEmployee();
                break;

            case 3:
                displayEmployeeHierarchy();
                break;

            case 4:
                displayCompleteHierarchy();
                break;

            case 5:
                freeEmployees(employee_list);
                employee_list = NULL;
                printf("Exiting program\n");
                break;

            default:
                printf("Invalid menu choice\n");
        }

    } while(menu_choice != 5);

    return 0;
}