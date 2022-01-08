/*
  File:             pagerank.c
  Purpose:          A C-program that calls MATLAB to calculate
                    the PageRank of a web.txt file.

  Authors:          Michael Parhar and Gordon Cheung
  Student Numbers:  54023643 and 12589511
  CWLs:             mparhar1 and ghwc
  Date:             November 30th, 2021
*/

#define _CRT_SECURE_NO_WARNINGS

/* Preprocessor directives */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"

/* Definitions */
#define  BUFSIZE 2048

/* Function Prototypes */
double* parseFile(FILE* file, int dimension);
int getSize(FILE* file);

/* Main Function */
int main(void) {
    /* Variables */
    Engine* ep = NULL; // A pointer to a MATLAB engine object
    mxArray* inputMat = NULL, * result = NULL; // mxArray is the fundamental type underlying MATLAB data
    double* connectivityMat = NULL;
    int dimension = 0;
    char buffer[BUFSIZE + 1];
    FILE* webMat = NULL;

    /* Starts a MATLAB Process */
    if (!(ep = engOpen(NULL))) {
        fprintf(stderr, "\nCan't start MATLAB engine\n");
        system("pause");
        return 1;
    }

    /* Opens File */
    webMat = fopen("web.txt", "r");

    /* Gets the Connectivity Matrix and its Dimension */
    if (webMat) {
        dimension = getSize(webMat);
        connectivityMat = parseFile(webMat, dimension);
    }
    else {
        printf("Unable to parse");
        system("pause");
    }

    /* MATLAB-Friendly Variable for our Data */
    inputMat = mxCreateDoubleMatrix(dimension, dimension, mxREAL);

    /* Copies the Data from the Local Array into the MATLAB-Friendly Variable */
    memcpy((void*)mxGetPr(inputMat), (void*)connectivityMat, (double) dimension * dimension * sizeof(double));

    /* Places the Variables into MATLAB */
    if (engPutVariable(ep, "inputMat", inputMat)) {
        fprintf(stderr, "\nCannot write the connectivity matrix to MATLAB \n");
        system("pause");
        exit(1); // Same as return 1;
    }

    /* Transposes the Matrix to Convert from Row-Major Order to Column-Major Order */
    if (engEvalString(ep, "inputMat = inputMat'")) {
        fprintf(stderr, "\nError determining: [rows, columns]  \n");
        system("pause");
        exit(1);
    }

    /* Executes the 14 MATLAB Commands */
    if (engEvalString(ep, "[rows, columns] = size(inputMat)")) {
        fprintf(stderr, "\nError determining: [rows, columns]  \n");
        system("pause");
        exit(1);
    }
    if (engEvalString(ep, "dimension = size(inputMat, 1)")) {
        fprintf(stderr, "\nError determining: dimension  \n");
        system("pause");
        exit(1);
    }
    if (engEvalString(ep, "columnsums = sum(inputMat, 1)")) {
        fprintf(stderr, "\nError determining: columnsums  \n");
        system("pause");
        exit(1);
    }
    if (engEvalString(ep, "p = 0.85")) {
        fprintf(stderr, "\nError determining: p  \n");
        system("pause");
        exit(1);
    }
    if (engEvalString(ep, "zerocolumns = find(columnsums~=0)")) {
        fprintf(stderr, "\nError determining: zerocolumns  \n");
        system("pause");
        exit(1);
    }
    if (engEvalString(ep, "D = sparse( zerocolumns, zerocolumns, 1./columnsums(zerocolumns), dimension, dimension)")) {
        fprintf(stderr, "\nError determining: D  \n");
        system("pause");
        exit(1);
    }
    if (engEvalString(ep, "StochasticMatrix = inputMat * D")) {
        fprintf(stderr, "\nError determining: StochasticMatrix  \n");
        system("pause");
        exit(1);
    }
    if (engEvalString(ep, "[row, column] = find(columnsums==0)")) {
        fprintf(stderr, "\nError determining: [row, column]  \n");
        system("pause");
        exit(1);
    }
    if (engEvalString(ep, "StochasticMatrix(:, column) = 1./dimension")) {
        fprintf(stderr, "\nError determining: StochasticMatrix(:, column)  \n");
        system("pause");
        exit(1);
    }
    if (engEvalString(ep, "Q = ones(dimension, dimension)")) {
        fprintf(stderr, "\nError determining: Q  \n");
        system("pause");
        exit(1);
    }
    if (engEvalString(ep, "TransitionMatrix = p * StochasticMatrix + (1 - p) * (Q/dimension)")) {
        fprintf(stderr, "\nError determining: TransitionMatrix  \n");
        system("pause");
        exit(1);
    }
    if (engEvalString(ep, "PageRank = ones(dimension, 1)")) {
        fprintf(stderr, "\nError initializing: PageRank  \n");
        system("pause");
        exit(1);
    }
    if (engEvalString(ep, "for i = 1:100 PageRank = TransitionMatrix * PageRank; end")) {
        fprintf(stderr, "\nError executing: for loop  \n");
        system("pause");
        exit(1);
    }
    if (engEvalString(ep, "PageRank = PageRank / sum(PageRank)")) {
        fprintf(stderr, "\nError determining: PageRank  \n");
        system("pause");
        exit(1);
    }

    /* Gets the Result from MATLAB and Returns it to the C Program */
    if ((result = engGetVariable(ep, "PageRank")) == NULL) {
        fprintf(stderr, "\nFailed to retrieve the PageRank\n");
        system("pause");
        exit(1);
    }
    else {
        /* Prints PageRank */
        size_t sizeOfPageRank = mxGetNumberOfElements(result);
        int i = 0;
        printf("NODE   RANK\n----   ----\n");
        for (i = 0; i < sizeOfPageRank; i++) {
            printf("%-4d   %.4f\n", i + 1, *(mxGetPr(result) + i));
        }
    }

    /* Creates Buffer */
    if (engOutputBuffer(ep, buffer, BUFSIZE)) {
        fprintf(stderr, "\nCan't create buffer for MATLAB output\n");
        system("pause");
        return 1;
    }
    buffer[BUFSIZE] = '\0';

    /* Records a List of All Current MATLAB Variables into Buffer and then Prints */
    engEvalString(ep, "whos"); // whos is a handy MATLAB command that generates a list of all current variables
    printf("%s\n", buffer);

    /* Memory Management */
    mxDestroyArray(inputMat);
    mxDestroyArray(result);
    inputMat = NULL;
    result = NULL;
    if (engClose(ep)) {
        fprintf(stderr, "\nFailed to close MATLAB engine\n");
    }

    /* End of Function */
    system("pause"); // So the terminal window remains open long enough for you to read it
    return 0; // Because main returns 0 for successful completion
}

/* Returns a 1-D Array Retrieved from web.txt (Inspired by Lab 4 Take-Home) */
double* parseFile(FILE* file, int dimension) {
    char line_buffer[BUFSIZE];
    int row = 0;
    int column = 0;
    double* matrix = NULL;

    /* Memory Allocation for the Matrix */
    matrix = (double*)calloc((double)dimension * dimension, sizeof(double));

    /* Copies Each Line of the File into line_buffer */
    while (fgets(line_buffer, BUFSIZE, file)) {
        int i = 0;
        /* Converts a 2-D Array into a 1-D Array */
        for (column = 0; column < 2 * dimension - 1; column += 2) {
            matrix[row * dimension + i] = line_buffer[column] - 48.0; // 48.0 is '0' in ASCII
            ++i;
        }
        row++;
    }

    return matrix;
}

/* Gets the Dimension from the web.txt file (Code Inspired by Lab 4 Take-Home */
int getSize(FILE* file) {
    int  dimension = 0;
    char line_buffer[BUFSIZE];

    dimension = strlen(fgets(line_buffer, BUFSIZE, file));

    fseek(file, 0, SEEK_SET);

    if (strchr(line_buffer, '\r') != NULL) {
        return ((dimension - 2) - 1) / 2 + 1;
    }
    else {
        return ((dimension - 1) - 1) / 2 + 1;
    }
}