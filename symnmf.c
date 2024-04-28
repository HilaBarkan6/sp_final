#include "symnmf.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define EPSILON (exp(-4))
#define MAX_ITER 300
#define BETA 0.5

int main(int argc, char *argv[]) {
    /* get arguments from command line */

    char * goal;
    char * filePath;

    if (argc < 3) {
        printf("An Error Has Occurred\n");
        return 1;
        /* no cleanup here because no memory was allocated. */
    }

    goal = argv[1];
    filePath = argv[2];

    return runGoal(goal, filePath);
}

int runGoal(char * goal, char * filePath)
{
    int functionStatus = 0;
    node * vectorList;
    double ** vectorMatrix = NULL;
    int dimension = 0;
    int vectorCount = 0;
    double ** similarityMatrix = NULL;
    double * similarityMatrixP = NULL;
    double ** degreeMatrix = NULL;
    double * degreeMatrixP = NULL;
    double ** normalSimilarityMatrix = NULL;
    double * normalMatrixP = NULL;

    if(readVectorsFromFile(filePath, &vectorList, &dimension, &vectorCount) == 1)
    {
        functionStatus = 1;
        goto cleanup;
    }
    createMatrixFromList(vectorCount, vectorList, &vectorMatrix);

    if(allocateMatrix(vectorCount, &similarityMatrix, &similarityMatrixP) == 1)
    {
        functionStatus = 1;
        goto cleanup;
    }

    calcSimilarityMatrix(dimension, vectorCount, vectorMatrix, similarityMatrix);

    if (strcmp(goal, "sym") == 0)
    {
        printMatrix(vectorCount, similarityMatrix);
        goto cleanup;
    }

    if(allocateMatrix(vectorCount, &degreeMatrix, &degreeMatrixP) == 1)
    {
        functionStatus = 1;
        goto cleanup;
    }

    calcDiagonalDegreeMatrix(vectorCount, similarityMatrix, degreeMatrix);

    if (strcmp(goal, "ddg") == 0)
    {
        printMatrix(vectorCount, degreeMatrix);
        goto cleanup;
    }

    if(allocateMatrix(vectorCount, &normalSimilarityMatrix, &normalMatrixP) == 1)
    {
        functionStatus = 1;
        goto cleanup;
    }

    calcNormalizedSimilarityMatrix(vectorCount, similarityMatrix, degreeMatrix, normalSimilarityMatrix);

    if (strcmp(goal, "norm") == 0)
    {
        printMatrix(vectorCount, normalSimilarityMatrix);
        goto cleanup;
    }

    cleanup:
    freeVectorList(vectorList);
    freeVectorMatrix(vectorMatrix);
    freeTempMatrix(similarityMatrix, similarityMatrixP);
    freeTempMatrix(degreeMatrix, degreeMatrixP);
    freeTempMatrix(normalSimilarityMatrix, normalMatrixP);

    return functionStatus;

}

int readVectorsFromFile(char * filePath, node ** vectorList, int * dimension, int * vectorCount)
{
    FILE * f = NULL;
    double temp = 0;
    int tempDimension = 1;
    int tempCount = 0;
    node * list;
    node * tail;
    double * tempVector = NULL;
    int i = 0;

    f = fopen(filePath, "r");
    if (f == NULL)
    {
        printf("An Error Has Occurred\n");
        return 1;
    }

    fscanf(f, "%lf", &temp);
    while (fscanf(f, ",%lf", &temp) == 1)
    {
        tempDimension++;
    }
    *dimension = tempDimension;

    fseek(f, 0, SEEK_SET);

    tempVector = (double *)malloc(*dimension * sizeof (double));
    fscanf(f, "%lf", &(tempVector[i]));
    i++;
    while (fscanf(f, ",%lf", &(tempVector[i])) == 1)
    {
        i++;
    }
    i = 0;

    list = (node *)malloc(sizeof (node));
    list->vector = tempVector;
    list->next = NULL;
    tail = list;
    tempCount++;

    while (fscanf(f, "%lf,", &temp) == 1)
    {
        tempVector = (double *)malloc(*dimension * sizeof (double));
        tempVector[0] = temp;
        for (i = 1; i < *dimension - 1; i++)
        {
            fscanf(f, "%lf,", &(tempVector[i]));
        }
        fscanf(f, "%lf\n", &tempVector[i]);

        tail->next = (node *) malloc(sizeof (node));
        tail->next->vector = tempVector;
        tail->next->next = NULL;
        tail = tail->next;
        tempCount++;
    }


    *vectorList = list;
    *vectorCount = tempCount;
    return 0;
}

void freeVectorList(node * head)
{
    node * temp;
    while (head != NULL)
    {
        temp = head;
        if (head->vector != NULL)
        {
            free(head->vector);
        }
        head = head->next;
        free(temp);
    }
}

void createMatrixFromList(int vectorCount, node * vectorList, double *** vectorMatrix)
{
    int i = 0;
    double ** tempMatrix;

    tempMatrix = (double **) malloc(vectorCount * sizeof (double *));
    for (i = 0; i < vectorCount; i++)
    {
        tempMatrix[i] = vectorList->vector;
        vectorList = vectorList->next;
    }
    *vectorMatrix = tempMatrix;
}

void freeVectorMatrix(double ** vectorMatrix)
{
    if (vectorMatrix != NULL)
    {
        free(vectorMatrix);
    }
}

void calcSimilarityMatrix(int dimension, int vectorCount, double ** vectorMatrix, double ** similarityMatrix)
{
    int i = 0;
    int j = 0;
    double dist = 0;

    for (i = 0; i < vectorCount; i++)
    {
        for (j = 0; j < vectorCount; j++)
        {
            if (i == j)
            {
                similarityMatrix[i][j] = 0;
            }
            else
            {
                squaredDistance(
                        dimension,
                        vectorMatrix[i],
                        vectorMatrix[j],
                        &dist);
                similarityMatrix[i][j] = exp((0 - dist)/2);
            }
        }
    }
}


void calcDiagonalDegreeMatrix(int vectorCount, double ** similarityMatrix, double ** degreeMatrix)
{
    int i = 0;
    int j = 0;

    for (i = 0; i < vectorCount; i++)
    {
        for (j = 0; j < vectorCount; j++)
        {
            if (i == j)
            {
                degreeMatrix[i][i] = rowSum(vectorCount, similarityMatrix, i);
            }
            else
            {
                degreeMatrix[i][j] = 0;
            }
        }
    }
}

void calcNormalizedSimilarityMatrix(int vectorCount,
                                    double ** similarityMatrix,
                                    double ** degreeMatrix,
                                    double ** normalSimilarityMatrix)
{
    int i = 0;
    double ** tempMatrix;
    double * p;
    for (i = 0; i < vectorCount; i++)
    {
        degreeMatrix[i][i] = 1 / sqrt(degreeMatrix[i][i]);
    }

    allocateMatrix(vectorCount, &tempMatrix, &p);
    multiplyMatrix(vectorCount, degreeMatrix, similarityMatrix, tempMatrix);
    multiplyMatrix(vectorCount, tempMatrix, degreeMatrix, normalSimilarityMatrix);

    freeTempMatrix(tempMatrix, p);
}


int calcAssociationMatrix (int vectorCount, double ** normalSimilarityMatrix, double ** associationMatrix)
{
    int i = 0;
    int j = 0;
    int iterCount = 0;
    int functionStatus = 0;
    double temp = 0;
    double frobeniusNorm = 0;
    double ** tempMatrix1 = NULL;
    double * p1 = NULL;
    double ** tempMatrix2 = NULL;
    double * p2 = NULL;
    double ** tempMatrix3 = NULL;
    double * p3 = NULL;

    double ** transposedAssociationMatrix = NULL;
    double ** denominatorMatrix = NULL;
    double ** numeratorMatrix = NULL;

    /* allocate temp matrices */
    if (allocateMatrix(vectorCount, &tempMatrix1, &p1) == 1)
    {
        functionStatus = 1;
        goto cleanup;
    }
    if (allocateMatrix(vectorCount, &tempMatrix2, &p2) == 1)
    {
        functionStatus = 1;
        goto cleanup;
    }
    if (allocateMatrix(vectorCount, &tempMatrix3, &p3) == 1)
    {
        functionStatus = 1;
        goto cleanup;
    }

    while (iterCount < MAX_ITER && frobeniusNorm > EPSILON)
    {
        transposedAssociationMatrix = tempMatrix1;

        transposeMatrix(vectorCount, associationMatrix, transposedAssociationMatrix);

        multiplyMatrix(vectorCount, associationMatrix, transposedAssociationMatrix, tempMatrix2);
        multiplyMatrix(vectorCount, tempMatrix2, associationMatrix, tempMatrix1);

        denominatorMatrix = tempMatrix1;
        numeratorMatrix = tempMatrix2;

        multiplyMatrix(vectorCount, normalSimilarityMatrix, associationMatrix, numeratorMatrix);

        frobeniusNorm = 0;
        for (i = 0; i < vectorCount; i++)
        {
            for (j = 0; j < vectorCount; j++)
            {
                temp = associationMatrix[i][j];
                associationMatrix[i][j] = associationMatrix[i][j] * (1 - BETA + (BETA * (numeratorMatrix[i][j] / denominatorMatrix[i][j])));
                frobeniusNorm += pow(associationMatrix[i][j] - temp, 2);
            }
        }
        iterCount++;
    }

    cleanup:
    freeTempMatrix(tempMatrix1, p1);
    freeTempMatrix(tempMatrix2, p2);
    freeTempMatrix(tempMatrix3, p3);

    return functionStatus;
}

void transposeMatrix(int n, double ** matrix, double ** resultMatrix)
{
    int i = 0;
    int j = 0;

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            resultMatrix[j][i] = matrix[i][j];
        }
    }
}

int allocateMatrix(int n, double *** matrix, double ** p)
{
    int i = 0;

    *p = (double *) malloc(n * n * sizeof (double ));
    if (*p == NULL)
    {
        printf("An Error Has Occurred\n");
        return 1;
    }
    *matrix = (double **) malloc(n * sizeof (double *));
    if (*matrix == NULL)
    {
        printf("An Error Has Occurred\n");
        return 1;
    }
    for (i = 0; i < n; i++)
    {
        (*matrix)[i] = *p + (i * n);
    }

    return 0;
}

void freeTempMatrix(double ** matrix, double * p)
{
    if (matrix != NULL)
    {
        free(matrix);
    }
    if (p != NULL)
    {
        free(p);
    }
}

double averageMatrix(int n, double ** matrix)
{
    int i = 0;
    int j = 0;
    double sum = 0;

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j ++)
        {
            sum += matrix[i][j];
        }
    }
    return sum / (n * n);
}

/*
 * result = A * B
 */
void multiplyMatrix(int n, double ** matrixA, double ** matrixB, double ** matrixResult)
{
    int i = 0;
    int j = 0;
    int k = 0;
    double sum = 0;

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            sum = 0;
            for (k = 0; k < n; k++)
            {
                sum += (matrixA[i][k] * matrixB[k][j]);
            }

            matrixResult[i][j] = sum;
        }
    }
}
double rowSum(int n, double ** matrix, int rowIndex)
{
    int i = 0;
    double sum = 0;

    for (i = 0; i < n; i++)
    {
        sum += matrix[rowIndex][i];
    }
    return sum;
}

void squaredDistance(int dimension, double coordinates1[], double coordinates2[], double *dist)
{
    double sum = 0;
    int i = 0;

    for (i = 0; i < dimension; i++)
    {
        sum += pow(coordinates1[i] - coordinates2[i], 2);
    }

    *dist = sum;
}

void printMatrix(int n, double ** matrix)
{
    int j = 0;
    int i = 0;

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            printf("%.4f,", matrix[i][j]);
        }
        printf("%.4f", matrix[i][n - 1]);
        printf("\n");
    }
}
