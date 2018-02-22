#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <stdint.h>

#if __FLOAT_WORD_ORDER__ == __ORDER_BIG_ENDIAN__

typedef union
{
  double value;
  struct
  {
    u_int32_t msw;
    u_int32_t lsw;
  } parts;
  struct
  {
    u_int64_t w;
  } xparts;
} ieee_double_shape_type;

#endif

#if __FLOAT_WORD_ORDER__ == __ORDER_LITTLE_ENDIAN__

typedef union
{
  double value;
  struct
  {
    u_int32_t lsw;
    u_int32_t msw;
  } parts;
  struct
  {
    u_int64_t w;
  } xparts;
} ieee_double_shape_type;

#endif

#define EXTRACT_WORDS(ix0,ix1,d)				\
do {								\
  ieee_double_shape_type ew_u;					\
  ew_u.value = (d);						\
  (ix0) = ew_u.parts.msw;					\
  (ix1) = ew_u.parts.lsw;					\
} while (0)

double* MType;
double* COV;
int NVAR;
int NROW;
int NV;

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

                //isnan function
                int32_t hx, lx;
                double d = X1[iobs * NVAR];
                EXTRACT_WORDS (hx, lx, d);
                hx &= 0x7fffffff;
                hx |= (uint32_t) (lx | (-lx)) >> 31;
                hx = 0x7ff00000 - hx;
                if((int) (((uint32_t) hx) >> 31))
                {
                    NumMissing++;
                }
                else if(d - 65.0 <= 1.e-03 && d - 65.0 >= -1.e-03)
                {
                    NumMissing++;
                }
                else {
                    d = X2[iobs * NVAR];
                    EXTRACT_WORDS (hx, lx, X2[iobs * NVAR]);
                    hx &= 0x7fffffff;
                    hx |= (uint32_t) (lx | (-lx)) >> 31;
                    hx = 0x7ff00000 - hx;
                    if((int) (((uint32_t) hx) >> 31))
                    {
                        NumMissing++;
                    }
                    else if(d - 65.0 <= 1.e-03 && d - 65.0 >= -1.e-03)
                    {
                        NumMissing++;
                    }else {
                        MeanX1 += X1[iobs * NVAR];
                        MeanX2 += X2[iobs * NVAR];
                        Covariance += X1[iobs * NVAR] * X2[iobs * NVAR];

                    }
                    
                }
            }

            MeanX1=MeanX1/(NROW-NumMissing);
            MeanX2=MeanX2/(NROW-NumMissing);

            Covariance=Covariance/(NROW-NumMissing)-MeanX1*MeanX2;

            COV[iv1 + iv2 * NVAR] = Covariance;
        }
    }
}