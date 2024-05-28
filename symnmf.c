#include "symnmf.h"


#define EPSILON 0.0001
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

    if(allocateMatrix(vectorCount, vectorCount, &similarityMatrix, &similarityMatrixP) == 1)
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

    if(allocateMatrix(vectorCount, vectorCount, &degreeMatrix, &degreeMatrixP) == 1)
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

    if(allocateMatrix(vectorCount, vectorCount, &normalSimilarityMatrix, &normalMatrixP) == 1)
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
    freeMatrix(similarityMatrix, similarityMatrixP);
    freeMatrix(degreeMatrix, degreeMatrixP);
    freeMatrix(normalSimilarityMatrix, normalMatrixP);

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
    int functionStatus = 0;

    f = fopen(filePath, "r");
    if (f == NULL)
    {
        printf("An Error Has Occurred\n");
        functionStatus = 1;
        goto cleanup;
    }

    if(fscanf(f, "%lf", &temp) != 1){
        printf("An Error Has Occurred\n");
        functionStatus = 1;
        goto cleanup;
    }
    while (fscanf(f, ",%lf", &temp) == 1)
    {
        tempDimension++;
    }
    *dimension = tempDimension;

    fseek(f, 0, SEEK_SET);

    tempVector = (double *)malloc(*dimension * sizeof (double));
    if(fscanf(f, "%lf", &(tempVector[i])) != 1){
        printf("An Error Has Occurred\n");
        functionStatus = 1;
        goto cleanup;
    }
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

    if(*dimension == 1){
        while (fscanf(f, "%lf\n", &temp) == 1){
            tempVector = (double *)malloc(*dimension * sizeof (double));
            tempVector[0] = temp;

            tail->next = (node *) malloc(sizeof (node));
            tail->next->vector = tempVector;
            tail->next->next = NULL;
            tail = tail->next;
            tempCount++;
        }
    }
    else{
        while (fscanf(f, "%lf,", &temp) == 1){
            tempVector = (double *)malloc(*dimension * sizeof (double));
            tempVector[0] = temp;
            for (i = 1; i < *dimension - 1; i++)
            {
                if(fscanf(f, "%lf,", &(tempVector[i])) != 1){
                    printf("An Error Has Occurred\n");
                    functionStatus = 1;
                    goto cleanup;
                }
            }
            if(fscanf(f, "%lf\n", &tempVector[i]) != 1){
                printf("An Error Has Occurred\n");
                functionStatus = 1;
                goto cleanup;
            }

            tail->next = (node *) malloc(sizeof (node));
            tail->next->vector = tempVector;
            tail->next->next = NULL;
            tail = tail->next;
            tempCount++;
        }
    }
    

    *vectorList = list;
    *vectorCount = tempCount;

    cleanup:
    fclose(f);
    if(functionStatus == 1 && list!=NULL){
        freeVectorList(list);
    }
    return functionStatus;

}

void freeVectorList(node * head)
{
    node * temp;
    while (head != NULL)
    {
        temp = head;
        free(head->vector);
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

/* Creates normalized similarity matrix as specified in section 1.3, overrides the dgree matrix in the process (to create D^(-0.5))*/
void calcNormalizedSimilarityMatrix(int vectorCount,
                                    double ** similarityMatrix,
                                    double ** degreeMatrix,
                                    double ** normalSimilarityMatrix)
{
    int i = 0;
    double ** tempMatrix = NULL;
    double * p = NULL;
    for (i = 0; i < vectorCount; i++)
    {
        degreeMatrix[i][i] = 1 / sqrt(degreeMatrix[i][i]);
    }

    allocateMatrix(vectorCount, vectorCount, &tempMatrix, &p);
    multiplyMatrix(vectorCount, vectorCount, vectorCount, degreeMatrix, similarityMatrix, tempMatrix);
    multiplyMatrix(vectorCount, vectorCount, vectorCount, tempMatrix, degreeMatrix, normalSimilarityMatrix);

    freeMatrix(tempMatrix, p);
}


int calcAssociationMatrix (int vectorCount, int k, double ** normalSimilarityMatrix, double ** associationMatrix)
{
    int i = 0;
    int j = 0;
    int iterCount = 0;
    int functionStatus = 0;
    double temp = 0;
    double frobeniusNorm = 1;

    double ** transposedAssociationMatrix = NULL;
    double * p1 = NULL;
    double ** tempMatrix = NULL;
    double * p2 = NULL;
    double ** denominatorMatrix = NULL;
    double * p3 = NULL;
    double ** numeratorMatrix = NULL;
    double * p4 = NULL;


    /* allocate temp matrices */
    if (allocateMatrix(k, vectorCount, &transposedAssociationMatrix, &p1) == 1)
    {
        functionStatus = 1;
        goto cleanup;
    }
    if (allocateMatrix(vectorCount, vectorCount, &tempMatrix, &p2) == 1)
    {
        functionStatus = 1;
        goto cleanup;
    }
    if (allocateMatrix(vectorCount, k, &denominatorMatrix, &p3) == 1)
    {
        functionStatus = 1;
        goto cleanup;
    }
    if (allocateMatrix(vectorCount, k, &numeratorMatrix, &p4) == 1)
    {
        functionStatus = 1;
        goto cleanup;
    }

    while (iterCount < MAX_ITER && frobeniusNorm >= EPSILON)
    {
        transposeMatrix(vectorCount, k, associationMatrix, transposedAssociationMatrix);

        multiplyMatrix(vectorCount, k, vectorCount, associationMatrix, transposedAssociationMatrix, tempMatrix);

        multiplyMatrix(vectorCount, vectorCount, k, tempMatrix, associationMatrix, denominatorMatrix);

        multiplyMatrix(vectorCount, vectorCount, k ,normalSimilarityMatrix, associationMatrix, numeratorMatrix);
        frobeniusNorm = 0;
        for (i = 0; i < vectorCount; i++)
        {
            for (j = 0; j < k; j++)
            {
                temp = associationMatrix[i][j];
                associationMatrix[i][j] = associationMatrix[i][j] * (1 - BETA + (BETA * (numeratorMatrix[i][j] / denominatorMatrix[i][j])));
                frobeniusNorm += pow(associationMatrix[i][j] - temp, 2);
            }
        }
        iterCount++;
    }

    cleanup:
    freeMatrix(transposedAssociationMatrix, p1);
    freeMatrix(tempMatrix, p2);
    freeMatrix(denominatorMatrix, p3);
    freeMatrix(numeratorMatrix, p4);

    return functionStatus;
}

/* result (m x n) = (A (n x m))transposed */
void transposeMatrix(int n, int m, double ** matrix, double ** resultMatrix)
{
    int i = 0;
    int j = 0;

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
        {
            resultMatrix[j][i] = matrix[i][j];
        }
    }
}

/* matrix = A (n x m) */
int allocateMatrix(int n, int m, double *** matrix, double ** p)
{
    int i = 0;

    *p = (double *) malloc(n * m * sizeof (double ));
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
        (*matrix)[i] = *p + (i * m);
    }

    return 0;
}

void freeMatrix(double ** matrix, double * p)
{
    free(matrix);
    free(p);
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
 * result (n x t) = A (n x m) * B (m x t)
 */
void multiplyMatrix(int n, int m, int t, double ** matrixA, double ** matrixB, double ** matrixResult)
{
    int i = 0;
    int j = 0;
    int k = 0;
    double sum = 0;

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < t; j++)
        {
            sum = 0;
            for (k = 0; k < m; k++)
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
        for (j = 0; j < n - 1; j++)
        {
            printf("%.4f,", matrix[i][j]);
        }
        printf("%.4f", matrix[i][n - 1]);
        printf("\n");
    }
}
