      subroutine checkRealEqu(v1, v2, res)
         IMPLICIT REAL(8) (V)
         REAL*8, INTENT(IN) :: v1
         REAL*8, INTENT(IN) :: v2
         logical:: res
         parameter(vlimit=2.e-03)

         res = .false.
         
         vdiff = v2 - v1
         if (vdiff .le. vlimit .and. vdiff .ge. -vlimit) then
            res = .true.
         endif
      end subroutine checkRealEqu

    LOGICAL FUNCTION IsMissingPheno(Phenotype)
    
        IMPLICIT NONE 
        REAL*8 MissingPheno
        REAL*8, INTENT(IN) :: Phenotype
        PARAMETER(MissingPheno=65.0)
        IsMissingPheno = .FALSE.
        
        IF(ISNAN(Phenotype)) THEN
            IsMissingPheno = .TRUE. 
        ELSE
            IF(ABS(Phenotype-MissingPheno) .LE. 1.e-03) &
                IsMissingPheno = .TRUE. 
        ENDIF

        RETURN
     
    ENDFUNCTION IsMissingPheno