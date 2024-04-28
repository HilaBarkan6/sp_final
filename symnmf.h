
#ifndef SP_FINAL_SYMNMF_H
#define SP_FINAL_SYMNMF_H

struct node {
    double * vector;
    struct node * next;
};
typedef struct node node;

void squaredDistance(int dimension, double coordinates1[], double coordinates2[], double *dist);
double rowSum(int n, double ** matrix, int rowIndex);
void multiplyMatrix(int n, double ** matrixA, double ** matrixB, double ** matrixResult);
int allocateMatrix(int n, double *** matrix, double ** p);
void freeTempMatrix(double ** matrix, double * p);
void transposeMatrix(int n, double ** matrix, double ** resultMatrix);
int readVectorsFromFile(char * filePath, node ** vectorList, int * dimension, int * vectorCount);
void freeVectorList(node * head);
void createMatrixFromList(int vectorCount, node * vectorList, double *** vectorMatrix);
void freeVectorMatrix(double ** vectorMatrix);
void calcSimilarityMatrix(int dimension, int vectorCount, double ** vectorMatrix, double ** similarityMatrix);
void calcDiagonalDegreeMatrix(int vectorCount, double ** similarityMatrix, double ** degreeMatrix);
void calcNormalizedSimilarityMatrix(int vectorCount,
                                    double ** similarityMatrix,
                                    double ** degreeMatrix,
                                    double ** normalSimilarityMatrix);
int calcAssociationMatrix (int vectorCount, double ** normalSimilarityMatrix, double ** associationMatrix);
void printMatrix(int n, double ** matrix);
int runGoal(char * goal, char * filePath);


#endif
