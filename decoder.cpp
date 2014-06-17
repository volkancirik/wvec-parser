/*********************************************************************
 *
 * jason eisner's projective dependency decoder
 * In Matlab compile with mex : 
 * >> mex decoder.cpp
 * And run
 * >> headVector = decoder(adjacencyMatrix)  %adjacencyMatrix is  T+1 x T+1 matrix where T is #of tokens
 * decode responsibly.
 ********************************************************************/
#include <matrix.h>
#include <mex.h>
#include <math.h>
#include <stdlib.h>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <limits>
#define NEGATIVE_INFINITY -INFINITY

/* Definitions to keep compatibility with earlier versions of ML */
#ifndef MWSIZE_MAX
typedef int mwSize;
typedef int mwIndex;
typedef int mwSignedIndex;

#if (defined(_LP64) || defined(_WIN64)) && !defined(MX_COMPAT_32)
/* Currently 2^48 based on hardware limitations */
# define MWSIZE_MAX    281474976710655UL
# define MWINDEX_MAX   281474976710655UL
# define MWSINDEX_MAX  281474976710655L
# define MWSINDEX_MIN -281474976710655L
#else
# define MWSIZE_MAX    2147483647UL
# define MWINDEX_MAX   2147483647UL
# define MWSINDEX_MAX  2147483647L
# define MWSINDEX_MIN -2147483647L
#endif
#define MWSIZE_MIN    0UL
#define MWINDEX_MIN   0UL
#endif

void free_sentence_structures(double **adjacency_matrix,int N) {
    for (int i = 0; i < N; i++) {
        free(adjacency_matrix[i]);
    }
    free(adjacency_matrix);
    adjacency_matrix = NULL;
}

void destroy_parent_table(int***** table, int length) {
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < length; j++) {
            for (int k = 0; k < 2; k++) {
                for (int t = 0; t < 2; t++) {
                    free(table[i][j][k][t]);
                }
                free(table[i][j][k]);
            }
            free(table[i][j]);
        }
        free(table[i]);
    }
    free(table);
}
void destroy_score_table(double**** table, int length) {
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < length; j++) {
            for (int k = 0; k < 2; k++) {
                free(table[i][j][k]);
            }
            free(table[i][j]);
        }
        free(table[i]);
    }
    free(table);
}

double**** init_score_table(int length) {
    double**** table = (double****) malloc(sizeof (double***) * length);
    for (int i = 0; i < length; i++) {
        table[i] = (double***) malloc(sizeof (double**) * length);
        for (int j = 0; j < length; j++) {
            table[i][j] = (double**) malloc(sizeof (double*) * 2);
            for (int k = 0; k < 2; k++) {
                table[i][j][k] = (double*) calloc(2, sizeof (double));
            }
        }
    }
    return table;
}

int***** init_parent_table(int length) {
    int***** table = (int*****) malloc(sizeof (int****) * length);
    for (int i = 0; i < length; i++) {
        table[i] = (int****) malloc(sizeof (int***) * length);
        for (int j = 0; j < length; j++) {
            table[i][j] = (int***) malloc(sizeof (int**) * 2);
            for (int k = 0; k < 2; k++) {
                table[i][j][k] = (int**) malloc(sizeof (int*) * 2);
                for (int t = 0; t < 2; t++) {
                    table[i][j][k][t] = (int*) calloc(length, sizeof (int));
                    for (int q = 0; q < length; q++)
                        table[i][j][k][t][q] = -1;
                }
            }
        }
    }
    return table;
}

void addAll(int *t, int *s, int l) {
    for (int i = 1; i < l; i++) {
        if (s[i] != -1)
            t[i] = s[i];
    }
    return;
}
void add(int *t, int parent, int child) {
    t[child] = parent;
    return;
}

int* parse(double **adjacency_matrix,int N) {
    double ****E = init_score_table(N + 1);
    int *****P = init_parent_table(N + 1);

    for (int m = 1; m < N + 1; m++) {
        for (int s = 0; s < N + 1; s++) {
            int t = s + m;

            if (t > N)
                break;
            else {
                double bestscore = NEGATIVE_INFINITY;
                int bestq = -1;
                for (int q = s; q < t; q++) {
                    double score = E[s][q][1][0] + E[q + 1][t][0][0]
                            + adjacency_matrix[t][s];

                    if (score >= bestscore) {
                        bestscore = score;
                        bestq = q;
                    }
                }

                E[s][t][0][1] = bestscore;

                addAll(P[s][t][0][1], P[s][bestq][1][0], N + 1);
                addAll(P[s][t][0][1], P[bestq + 1][t][0][0], N + 1);


                add(P[s][t][0][1], t, s);

                bestscore = NEGATIVE_INFINITY;
                bestq = -1;
                for (int q = s; q < t; q++) {
                    double score = E[s][q][1][0] + E[q + 1][t][0][0]
                            + adjacency_matrix[s][t];

                    if (score >= bestscore) {
                        bestscore = score;
                        bestq = q;
                    }
                }
                E[s][t][1][1] = bestscore;

                addAll(P[s][t][1][1], P[s][bestq][1][0], N + 1);
                addAll(P[s][t][1][1], P[bestq + 1][t][0][0], N + 1);


                add(P[s][t][1][1], s, t);

                bestscore = NEGATIVE_INFINITY;
                bestq = -1;
                for (int q = s; q < t; q++) {
                    double score = E[s][q][0][0] + E[q][t][0][1];

                    if (score >= bestscore) {
                        bestscore = score;
                        bestq = q;
                    }
                }

                E[s][t][0][0] = bestscore;


                addAll(P[s][t][0][0], P[s][bestq][0][0], N + 1);
                addAll(P[s][t][0][0], P[bestq][t][0][1], N + 1);


                bestscore = NEGATIVE_INFINITY;
                bestq = -1;
                for (int q = s + 1; q <= t; q++) {
                    double score = E[s][q][1][1] + E[q][t][1][0];

                    if (score >= bestscore) {
                        bestscore = score;
                        bestq = q;
                    }
                }

                E[s][t][1][0] = bestscore;

                addAll(P[s][t][1][0], P[s][bestq][1][1], N + 1);
                addAll(P[s][t][1][0], P[bestq][t][1][0], N + 1);
            }

        }
    }
    int* r = P[0][N][1][0];
    int *parents = (int*) malloc((N + 1) * sizeof (int));

    for (int i = 0; i < N + 1; i++)
        parents[i] = r[i];

    destroy_score_table(E, N + 1);
    destroy_parent_table(P, N + 1);

    return parents;
}

double **MatrixM2C(double *a,int dimx,int dimy)
{
    double **ma;
    int i,j;
    ma = (double **)malloc(dimx*sizeof(double *));
    for(j=0;j<dimy;j++)
    {
      ma[j] = (double *)malloc(dimy*sizeof(double));
      for(i=0;i<dimx;i++)
        {
	  ma[j][i] = a[i*dimy+j];
        }
    }
    return ma;
}
void MatrixC2M(double **ma,int dimx,int dimy,double *cout)
{
  int i,j;
  for(i=0;i<dimx;i++)
    {
        for(j=0;j<dimy;j++)
        {
            cout[i*dimy+j] = ma[j][i];
	}
    }
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
//declare variables
    mxArray *a_in_m, *b_in_m, *c_out_m, *d_out_m;
    const mwSize *dims;
    double *a, *b, *c, *d;
    int dimx, dimy, numdims;
    int i,j;
    a_in_m = mxDuplicateArray(prhs[0]);
//figure out dimensions
    dims = mxGetDimensions(prhs[0]);
    numdims = mxGetNumberOfDimensions(prhs[0]);
    dimy = (int)dims[0];
//associate outputs
    c_out_m = plhs[0] = mxCreateDoubleMatrix(dimy,1,mxREAL); // dimx = 1??

    a = mxGetPr(a_in_m);
    c = mxGetPr(c_out_m);

    double **ma = MatrixM2C(a,dimy,dimy);
    int *h = parse(ma,dimy-1);
    double **heads = (double **)malloc(1*sizeof(double *));
    heads[0] = (double *)malloc(dimy*sizeof(double ));

    for(i=0;i<dimy;i++)
      heads[0][i]=h[i];
    MatrixC2M(heads,dimy,1,c);
    return;
}
