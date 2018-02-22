#include <stdio.h>
#include <math.h>
#include <omp.h>

double* MType;
double* COV;
int NVAR;
int NROW;
int NV;

int IsMissingPheno(double d)
{
    //isnan function
    int32_t hx, lx;
    EXTRACT_WORDS (hx, lx, d);
    hx &= 0x7fffffff;
    hx |= (uint32_t) (lx | (-lx)) >> 31;
    hx = 0x7ff00000 - hx;
    if((int) (((uint32_t) hx) >> 31))
    {
        return 1;
    }
    else if(d - 65.0 <= 1.e-03 && d - 65.0 >= -1.e-03)
    {
        return 1;
    }
    return 0;
}

void findcov_(int* _NVAR, int* _NROW, int* _NV, double* _MType, double* _COV)
{
    COV = _COV;
    MType = _MType;
    NVAR = *_NVAR;
    NROW = *_NROW;
    NV = *_NV;

    //printf("C code, NVAR = %d, NROW = %d, NV = %d\n", NVAR, NROW, NV);

    int iv1, iv2, i2, iobs;

    #pragma omp parallel for private(iv1, iv2, i2, iobs)
    for(iv1 = 0; iv1 < NVAR; iv1++)
    {
        for(iv2 = 0; iv2 < NV; iv2++)
        {
            int I2 = (iv1 + iv2 + 1) % NVAR;

            int NumMissing = 0;
            double MeanX1 = 0.0, MeanX2 = 0.0;
            double Covariance = 0.0;

            double* X1, *X2;
            X1 = MType + iv1;
            X2 = MType + I2;

            for(iobs = 0; iobs < NROW; iobs++)
            {
                if(IsMissingPheno(X1[iobs * NVAR]) || IsMissingPheno(X2[iobs * NVAR]))
                {
                    NumMissing++;
                }
                else
                {
                    MeanX1 += X1[iobs * NVAR];
                    MeanX2 += X2[iobs * NVAR];
                    Covariance += X1[iobs * NVAR] * X2[iobs * NVAR];
                }
            }

            MeanX1=MeanX1/(NROW-NumMissing);
            MeanX2=MeanX2/(NROW-NumMissing);

            Covariance=Covariance/(NROW-NumMissing)-MeanX1*MeanX2;

            COV[iv1 + iv2 * NVAR] = Covariance;
        }
    }
}