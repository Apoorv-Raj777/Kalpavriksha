#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 10

// Display matrix using pointer arithmetic
void displayMatrix(int *matrix, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%4d", *(matrix + i * n + j));
        }
        printf("\n");
    }
}

// Generate random matrix
void generateMatrix(int *matrix, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            *(matrix + i * n + j) = rand() % 256;
        }
    }
}

// Rotate matrix 90 degrees clockwise in-place
void rotateMatrix(int *matrix, int n)
{
    // Transpose the matrix
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            int *p1 = matrix + i * n + j;
            int *p2 = matrix + j * n + i;

            int temp = *p1;
            *p1 = *p2;
            *p2 = temp;
        }
    }

    // Reverse each row
    for (int i = 0; i < n; i++)
    {
        int low = 0;
        int high = n - 1;

        while (low < high)
        {
            int *p1 = matrix + i * n + low;
            int *p2 = matrix + i * n + high;

            int temp = *p1;
            *p1 = *p2;
            *p2 = temp;

            low++;
            high--;
        }
    }
}

// Apply 3x3 smoothing filter in-place
void smoothMatrix(int *matrix, int n)
{
    int previous[MAX];
    int current[MAX];
    int next[MAX];

    // Preserve the first row
    for (int j = 0; j < n; j++)
    {
        previous[j] = *(matrix + j);
    }

    for (int i = 0; i < n; i++)
    {
        // Save the current row
        for (int j = 0; j < n; j++)
        {
            current[j] = *(matrix + i * n + j);
        }

        // Save the next row before modifying the current row
        if (i + 1 < n)
        {
            for (int j = 0; j < n; j++)
            {
                next[j] = *(matrix + (i + 1) * n + j);
            }
        }

        for (int j = 0; j < n; j++)
        {
            int sum = 0;
            int count = 0;

            // Visit valid neighboring rows and columns
            for (int r = i - 1; r <= i + 1; r++)
            {
                for (int c = j - 1; c <= j + 1; c++)
                {
                    if (r >= 0 && r < n && c >= 0 && c < n)
                    {
                        int value;

                        if (r == i)
                        {
                            value = current[c];
                        }
                        else if (r == i - 1)
                        {
                            value = previous[c];
                        }
                        else
                        {
                            value = next[c];
                        }

                        sum += value;
                        count++;
                    }
                }
            }

            // Replace the current cell with its average
            *(matrix + i * n + j) = sum / count;
        }

        // Preserve the original current row for the next iteration
        for (int j = 0; j < n; j++)
        {
            previous[j] = current[j];
        }
    }
}

int main(void)
{
    int n;

    srand((unsigned int)time(NULL));

    printf("Enter matrix size (2-10): ");

    if (scanf("%d", &n) != 1 || n < 2 || n > MAX)
    {
        printf("Invalid matrix size.\n");
        return 1;
    }

    // A single N*N matrix is used for all operations
    int matrix[MAX * MAX];

    generateMatrix(matrix, n);

    printf("\nOriginal Randomly Generated Matrix:\n");
    displayMatrix(matrix, n);

    rotateMatrix(matrix, n);

    printf("\nMatrix after 90 Degree Clockwise Rotation:\n");
    displayMatrix(matrix, n);

    smoothMatrix(matrix, n);

    printf("\nMatrix after Applying 3x3 Smoothing Filter:\n");
    displayMatrix(matrix, n);

    return 0;
}

