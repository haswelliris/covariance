#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <stdint.h>
#include <malloc.h>

double *MType;
double *COV;
int NVAR;
int NROW;
int NV;

int IsMissingPheno(double d)
{
    if(isnan(d))
    {
        return 1;
    }
    else if(d - 65.0 <= 1.e-03 && d - 65.0 >= -1.e-03)
    {
        return 1;
    }
    return 0;
}

void findcov_(int *_NVAR, int *_NROW, int *_NV, double *_MType, double *_COV)
{
    COV = _COV;
    MType = _MType;
    NVAR = *_NVAR;
    NROW = *_NROW;
    NV = *_NV;

    //printf("C code, NVAR = %d, NROW = %d, NV = %d\n", NVAR, NROW, NV);

    int iv1, iv2, i2, iobs;

    double *M = malloc((NROW + 4) * (NVAR + 4) * sizeof(double));
    memset(M, 0, sizeof(M));

    #pragma omp parallel for private(iv1, iobs)
    for (iv1 = 0; iv1 < NVAR; iv1++)
    {
        int temp = iv1*NROW;
        for (iobs = 0; iobs < NROW; iobs++)
        {
            int pos = iv1+iobs*NVAR;
            int pos2 = iobs + temp;
            M[pos2] = MType[pos];
        }
    }
    printf("ca");
    #pragma omp parallel for private(iv1, iv2, i2, iobs)
    for (iv1 = 0; iv1 < NVAR; iv1++)
    {
        for (iv2 = 0; iv2 < NV; iv2++)
        {
            int I2 = (iv1 + iv2 + 1) % NVAR;

            int NumMissing = 0;
            double MeanX1 = 0.0, MeanX2 = 0.0;
            double Covariance = 0.0;

            double *X1, *X2;
            X1 = M + iv1*NROW;
            X2 = M + I2*NROW;

            //for (iobs = 0; iobs < NROW; iobs++)
            #pragma ivdep
            for(iobs=0;iobs<NROW;iobs++)
            {
                if(IsMissingPheno(X1[iobs]) || IsMissingPheno(X2[iobs]))   
                // if(IsMissingPheno(X1[iobs * NVAR]) || IsMissingPheno(X2[iobs * NVAR]))        
        //        if (flag[iv1 + iobs * NVAR] == 1 || flag[I2 + iobs * NVAR] == 1)
                {
                    NumMissing++;
                }
                else
                {
                //     MeanX1 += X1[iobs * NVAR];
                //     MeanX2 += X2[iobs * NVAR];
                //     Covariance += X1[iobs * NVAR] * X2[iobs * NVAR];
                    #pragma vector aligned
                    MeanX1 += X1[iobs];
                    MeanX2 += X2[iobs];
                    Covariance += X1[iobs] * X2[iobs];
                }
            }
            MeanX1 = MeanX1 / (NROW - NumMissing);
            MeanX2 = MeanX2 / (NROW - NumMissing);

            Covariance = Covariance / (NROW - NumMissing) - MeanX1 * MeanX2;

            COV[iv1 + iv2 * NVAR] = Covariance;
        }
    }
}