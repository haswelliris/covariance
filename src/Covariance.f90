    SUBROUTINE FindCOV(NVAR, NROW, NV, MType, COV)

        IMPLICIT NONE

        INTEGER, INTENT(IN)    :: NVAR              !Number of variables
        INTEGER, INTENT(IN)    :: NROW              !Number of rows in MType
        INTEGER, INTENT(IN)    :: NV                !Number of cals in MType
        REAL*8,    INTENT(IN)    :: MType(NVAR, NROW) !Data used to calculate covariance
        REAL*8,    INTENT(INOUT) :: COV(NVAR, NV)   !Covariance matrix, output

        INTEGER :: IVar1
        INTEGER :: IVar2
        INTEGER :: I2 

        REAL*8,EXTERNAL :: Covariance
        !$OMP PARALLEL DO private(IVar1,IVar2,I2) shared(NVAR,COV,MType)
        DO IVar1 = 1, NVAR
        DO IVar2 = 1, NV
           I2 = mod(IVar1 + IVar2 - 1, NVAR) + 1
           COV(IVar1, IVar2) = Covariance(NROW, MType(IVar1,:), &
           MType(I2,:))
        ENDDO
        ENDDO
        !$OMP END PARALLEL DO
        RETURN

    END SUBROUTINE FindCOV

    REAL*8 FUNCTION Covariance(NObs, X1, X2)

        IMPLICIT NONE
        
        INTEGER, INTENT(IN) :: NObs
        REAL*8,    INTENT(IN) :: X1(NObs)
        REAL*8,    INTENT(IN) :: X2(NObs)

        INTEGER :: IObs, NumMissing
        REAL*8 :: MeanX1, MeanX2
        LOGICAL,EXTERNAL :: IsMissingPheno
        NumMissing=0
        MeanX1=0.0
        MeanX2=0.0
        Covariance=0.0 

        DO IObs=1, NObs
            IF(IsMissingPheno(X1(IObs)) .OR. &
                IsMissingPheno(X2(IObs))) THEN
                NumMissing=NumMissing+1
       
            ELSE
                MeanX1=MeanX1+X1(IObs)
                MeanX2=MeanX2+X2(IObs)

                Covariance=Covariance+X1(IObs)*X2(IObs)
     
            ENDIF
            
        ENDDO

        MeanX1=MeanX1/(NObs-NumMissing)
        MeanX2=MeanX2/(NObs-NumMissing)

        IF(NObs-NumMissing .LE. 0) THEN
            Covariance=0.0
        ELSE
            Covariance=Covariance/(NObs-NumMissing)-MeanX1*MeanX2
   
        ENDIF


        RETURN

    END FUNCTION Covariance
