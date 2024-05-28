#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "symnmf.h"

static PyObject* calc_sym(PyObject *self, PyObject *args)
{
    int functionStatus = 0;
    int i = 0;
    int j = 0;

    int dimension = 0;
    int vectorCount = 0;
    double ** vectorMatrix = NULL;
    double * vectorMatrixP = NULL;
    double ** similarityMatrix = NULL;
    double * similarityMatrixP = NULL;
    PyObject * pySimilarityMatrix = NULL;
    PyObject * pyCurrentRow = NULL;
    PyObject * pyDataPoints = NULL;
    PyObject * pyEntry = NULL;


    if(!PyArg_ParseTuple(args, "Oii",
                         &pyDataPoints,
                         &vectorCount,
                         &dimension)) {
        return NULL;
    }

    /* create vector matrix */
    if(allocateMatrix(vectorCount, dimension, &vectorMatrix, &vectorMatrixP) == 1)
    {
        functionStatus = 1;
        goto cleanup;
    }
    /* fill vector matrix */
    for (i = 0; i < vectorCount; i++)
    {
        pyCurrentRow = PyList_GetItem(pyDataPoints, i);
        for (j = 0; j < dimension; j++)
        {
            pyEntry = PyList_GetItem(pyCurrentRow, j);
            vectorMatrix[i][j] = PyFloat_AsDouble(pyEntry);
        }
    }
    
    /* main logic */
    if(allocateMatrix(vectorCount, vectorCount, &similarityMatrix, &similarityMatrixP) == 1)
    {
        functionStatus = 1;
        goto cleanup;
    }
    calcSimilarityMatrix(dimension, vectorCount, vectorMatrix, similarityMatrix);

    /* build result for python */
    pySimilarityMatrix = PyList_New(vectorCount);
    for (i = 0; i < vectorCount; i++)
    {
        pyCurrentRow = PyList_New(vectorCount);
        for (j = 0; j < vectorCount; j++)
        {
            PyList_SetItem(pyCurrentRow, j, Py_BuildValue("d", similarityMatrix[i][j]));
        }
        PyList_SetItem(pySimilarityMatrix, i, pyCurrentRow);
    }


/* free everything */
    cleanup:
    freeMatrix(vectorMatrix, vectorMatrixP);
    freeMatrix(similarityMatrix, similarityMatrixP);

    if (functionStatus == 0)
    {
        return pySimilarityMatrix;
    }
    else
    {
        return NULL;
    }

}


static PyObject* calc_ddg(PyObject *self, PyObject *args)
{
    int functionStatus = 0;
    int i = 0;
    int j = 0;

    int dimension = 0;
    int vectorCount = 0;
    double ** vectorMatrix = NULL;
    double * vectorMatrixP = NULL;
    double ** similarityMatrix = NULL;
    double * similarityMatrixP = NULL;
    double ** degreeMatrix = NULL;
    double * degreeMatrixP = NULL;
    PyObject * pyDegreeMatrix = NULL;
    PyObject * pyCurrentRow = NULL;
    PyObject * pyDataPoints = NULL;
    PyObject * pyEntry = NULL;

    if(!PyArg_ParseTuple(args, "Oii",
                         &pyDataPoints,
                         &vectorCount,
                         &dimension)) {
        return NULL;
    }

    /* create vector matrix */
    if(allocateMatrix(vectorCount, dimension, &vectorMatrix, &vectorMatrixP) == 1)
    {
        functionStatus = 1;
        goto cleanup;
    }
    /* fill vector matrix */
    for (i = 0; i < vectorCount; i++)
    {
        pyCurrentRow = PyList_GetItem(pyDataPoints, i);
        for (j = 0; j < dimension; j++)
        {
            pyEntry = PyList_GetItem(pyCurrentRow, j);
            vectorMatrix[i][j] = PyFloat_AsDouble(pyEntry);
        }
    }

    /* main logic */

    if(allocateMatrix(vectorCount, vectorCount, &similarityMatrix, &similarityMatrixP) == 1)
    {
        functionStatus = 1;
        goto cleanup;
    }

    calcSimilarityMatrix(dimension, vectorCount, vectorMatrix, similarityMatrix);

    if(allocateMatrix(vectorCount, vectorCount, &degreeMatrix, &degreeMatrixP) == 1)
    {
        functionStatus = 1;
        goto cleanup;
    }

    calcDiagonalDegreeMatrix(vectorCount, similarityMatrix, degreeMatrix);

    /* build python return value */
    pyDegreeMatrix = PyList_New(vectorCount);
    for (i = 0; i < vectorCount; i++)
    {
        pyCurrentRow = PyList_New(vectorCount);
        for (j = 0; j < vectorCount; j++)
        {
            PyList_SetItem(pyCurrentRow, j, Py_BuildValue("d", degreeMatrix[i][j]));
        }
        PyList_SetItem(pyDegreeMatrix, i, pyCurrentRow);
    }


/* free everything */
    cleanup:
    freeMatrix(vectorMatrix, vectorMatrixP);
    freeMatrix(similarityMatrix, similarityMatrixP);
    freeMatrix(degreeMatrix, degreeMatrixP);

    if (functionStatus == 0)
    {
        return pyDegreeMatrix;
    }
    else
    {
        return NULL;
    }
}

static PyObject* calc_norm(PyObject *self, PyObject *args)
{
    int functionStatus = 0;
    int i = 0;
    int j = 0;

    int dimension = 0;
    int vectorCount = 0;
    double ** vectorMatrix = NULL;
    double * vectorMatrixP = NULL;
    double ** similarityMatrix = NULL;
    double * similarityMatrixP = NULL;
    double ** degreeMatrix = NULL;
    double * degreeMatrixP = NULL;
    double ** normalSimilarityMatrix = NULL;
    double * normalMatrixP = NULL;
    PyObject * pyNormalMatrix = NULL;
    PyObject * pyCurrentRow = NULL;
    PyObject * pyDataPoints = NULL;
    PyObject * pyEntry = NULL;

    if(!PyArg_ParseTuple(args, "Oii",
                         &pyDataPoints,
                         &vectorCount,
                         &dimension)) {
        return NULL;
    }

    /* create vector matrix */
    if(allocateMatrix(vectorCount, dimension, &vectorMatrix, &vectorMatrixP) == 1)
    {
        functionStatus = 1;
        goto cleanup;
    }
    /* fill vector matrix */
    for (i = 0; i < vectorCount; i++)
    {
        pyCurrentRow = PyList_GetItem(pyDataPoints, i);
        for (j = 0; j < dimension; j++)
        {
            pyEntry = PyList_GetItem(pyCurrentRow, j);
            vectorMatrix[i][j] = PyFloat_AsDouble(pyEntry);
        }
    }

    /* main logic */

    if(allocateMatrix(vectorCount, vectorCount, &similarityMatrix, &similarityMatrixP) == 1)
    {
        functionStatus = 1;
        goto cleanup;
    }

    calcSimilarityMatrix(dimension, vectorCount, vectorMatrix, similarityMatrix);

    if(allocateMatrix(vectorCount, vectorCount, &degreeMatrix, &degreeMatrixP) == 1)
    {
        functionStatus = 1;
        goto cleanup;
    }

    calcDiagonalDegreeMatrix(vectorCount, similarityMatrix, degreeMatrix);

    if(allocateMatrix(vectorCount, vectorCount, &normalSimilarityMatrix, &normalMatrixP) == 1)
    {
        functionStatus = 1;
        goto cleanup;
    }

    calcNormalizedSimilarityMatrix(vectorCount, similarityMatrix, degreeMatrix, normalSimilarityMatrix);

    /* build python return value */
    pyNormalMatrix = PyList_New(vectorCount);
    for (i = 0; i < vectorCount; i++)
    {
        pyCurrentRow = PyList_New(vectorCount);
        for (j = 0; j < vectorCount; j++)
        {
            PyList_SetItem(pyCurrentRow, j, Py_BuildValue("d", normalSimilarityMatrix[i][j]));
        }
        PyList_SetItem(pyNormalMatrix, i, pyCurrentRow);
    }

/* free everything */
    cleanup:
    freeMatrix(vectorMatrix, vectorMatrixP);
    freeMatrix(similarityMatrix, similarityMatrixP);
    freeMatrix(degreeMatrix, degreeMatrixP);
    freeMatrix(normalSimilarityMatrix, normalMatrixP);

    if (functionStatus == 0)
    {
        return pyNormalMatrix;
    }
    else
    {
        return NULL;
    }
}


static PyObject* calc_symnmf(PyObject *self, PyObject *args)
{
    int functionStatus = 0;
    int i = 0;
    int j = 0;
    int vectorCount = 0;
    int k = 0;

    double ** normalSimilarityMatrix = NULL;
    double ** initialAssociationMatrix = NULL;
    double * normalSimilarityMatrixP = NULL;
    double * initialAssociationMatrixP = NULL;

    PyObject * pyNormalSimilarityMatrix = NULL;
    PyObject * pyInitialAssociationMatrix = NULL;
    PyObject * pyCurrentRow = NULL;
    PyObject * pyEntry = NULL;
    PyObject * pyFinalAssociationMatrix = NULL;

    if(!PyArg_ParseTuple(args, "OOii",
                         &pyNormalSimilarityMatrix,
                         &pyInitialAssociationMatrix,
                         &k,
                         &vectorCount
                         )) {
        return NULL;
    }

    if(allocateMatrix(vectorCount, vectorCount, &normalSimilarityMatrix, &normalSimilarityMatrixP) == 1)
    {
        functionStatus = 1;
        goto cleanup;
    }

    if(allocateMatrix(vectorCount, k, &initialAssociationMatrix, &initialAssociationMatrixP) == 1)
    {
        functionStatus = 1;
        goto cleanup;
    }

    /* fill the normal similarity matrix */
    for (i = 0; i < vectorCount; i++)
    {
        pyCurrentRow = PyList_GetItem(pyNormalSimilarityMatrix, i);
        for (j = 0; j < vectorCount; j++)
        {
            pyEntry = PyList_GetItem(pyCurrentRow, j);
            normalSimilarityMatrix[i][j] = PyFloat_AsDouble(pyEntry);
        }
    }

    /* fill the initial matrix */
    for (i = 0; i < vectorCount; i++)
    {
        pyCurrentRow = PyList_GetItem(pyInitialAssociationMatrix, i);
        for (j = 0; j < k; j++)
        {
            pyEntry = PyList_GetItem(pyCurrentRow, j);
            initialAssociationMatrix[i][j] = PyFloat_AsDouble(pyEntry);
        }
    }

    /* main logic */
    calcAssociationMatrix(vectorCount, k, normalSimilarityMatrix, initialAssociationMatrix);

    /* return value */
    pyFinalAssociationMatrix = PyList_New(vectorCount);
    for (i = 0; i < vectorCount; i++)
    {
        pyCurrentRow = PyList_New(k);
        for (j = 0; j < k; j++)
        {
            PyList_SetItem(pyCurrentRow, j, Py_BuildValue("d", initialAssociationMatrix[i][j]));
        }
        PyList_SetItem(pyFinalAssociationMatrix, i, pyCurrentRow);
    }

    /* free everything */
    cleanup:

    freeMatrix(normalSimilarityMatrix, normalSimilarityMatrixP);
    freeMatrix(initialAssociationMatrix, initialAssociationMatrixP);

    if (functionStatus == 0)
    {
        return pyFinalAssociationMatrix;
    }
    else
    {
        return NULL;
    }

}

static PyMethodDef symnmfMethods[] = {
    {
        "calc_sym",
        (PyCFunction) calc_sym,
        METH_VARARGS,
        PyDoc_STR("This function calculates the similarity matrix. Please pass the following arguments: data points, vectors count (N), dimension")
    },
    {
        "calc_ddg",
        (PyCFunction) calc_ddg,
        METH_VARARGS,
        PyDoc_STR("This function calculates the degree matrix. Please pass the following arguments: data points, vectors count (N), dimension")

    },
    {
        "calc_norm",
        (PyCFunction) calc_norm,
        METH_VARARGS,
        PyDoc_STR("This function calculates the normalized similarity matrix. Please pass the following arguments: data points, vectors count (N), dimension")
    },
    {
        "calc_symnmf",
        (PyCFunction) calc_symnmf,
        METH_VARARGS,
        PyDoc_STR("This function performs the full symNMF. Please pass the following arguments:  W (normal similarity) matrix, Intial H matrix, K (number of clusters), vectors count")
    },
    { NULL, NULL, 0, NULL}

};

static struct PyModuleDef symnmfmodule = {
        PyModuleDef_HEAD_INIT,
        "mysymnmfsp",
        NULL,
        -1,
        symnmfMethods
};

PyMODINIT_FUNC PyInit_mysymnmfsp(void)
{
    PyObject * m;
    m = PyModule_Create(&symnmfmodule);
    if (!m)
    {
        return NULL;
    }
    return m;
}