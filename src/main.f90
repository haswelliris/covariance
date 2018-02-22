      PROGRAM MAIN

       INTEGER :: NROW, NROW_ORI
       INTEGER :: NVAR, NVAR_ORI
       INTEGER :: NV, NV_ORI 
       real(8), parameter:: d_nan = &
       TRANSFER((/ Z'00000000', Z'7FF80000'/),1.0_8)

       REAL*8, PARAMETER:: minx=65.0
       REAL*8:: z, rcsum, rcsum1
       REAL*8, ALLOCATABLE::MType(:, :)
       REAL*8, ALLOCATABLE::COV(:, :)
       REAL*8, ALLOCATABLE::COV_check(:, :)

       CHARACTER str*128
  
      INTEGER:: i, i1, N
      LOGICAL:: lreal
     
      INTEGER:: count1, count_rate1, count_max1
      INTEGER:: count2, count_rate2, count_max2

      OPEN(233,FILE='exp.ini',FORM='formatted')
      READ(233, '(a)') str
      read(str,*)NVAR, NROW, NV
      print *, "NVAR, NROW, NV = ", NVAR, NROW, NV
      !print *, "1.e-03 - 0.001 = ", 1.e-03 - 0.001
      CLOSE(233)

      NROW_ORI = NROW 
      NVAR_ORI = NVAR 
      NV_ORI = NV 

      ALLOCATE(MType(NVAR, NROW))
      ALLOCATE(COV(NVAR, NV))
      ALLOCATE(COV_check(NVAR, NV))


      !$OMP PARALLEL 
      !$OMP DO PRIVATE(i,j)
      !SHARED(MType,minx,NVAR,NROW) 
       DO i = 1, NVAR 
       DO j = 1, NROW
            !MType(i,j) = minx + (- mod(i + j, 1000) + 500) * 0.01 
            MType(i,j) = minx + mod(i + j, 1000) * 0.01 
       END DO
       END DO
       !$OMP END DO
       !$OMP END PARALLEL

       DO i = 1, NVAR
       DO j = 1, NROW/10
           call random_number(z)
           N=floor((NROW-1)*z)+1

           if (mod(N, 2) .eq. 0) then
              !MType(i, N) = minx + (- mod(N, 15) + 10) * 0.0001
              MType(i, N) = minx + mod(N, 15) * 0.0001
           else 
              MType(i, N) = d_nan
           endif
       END DO
       END DO 

       !print *, "random numbers generated"

!     data process
      CALL SYSTEM_CLOCK(count1, count_rate1, count_max1)
      call FindCOV(NVAR_ORI, NROW_ORI, NV_ORI, MType, COV)
      CALL SYSTEM_CLOCK(count2, count_rate2, count_max2)
      print *, "total time: ", (count2-count1)*1.0/count_rate2

!     calculate the result of original version      
      call FindCOV_check(NVAR, NROW, NV, MType, COV_check)


      !print *, "standard result generated"

!     correctness verification
      loop:do i = 1, NVAR 
        do j=1, NV
         rcsum=COV(i,j)
         rcsum1=COV_check(i,j)
         call checkRealEqu(rcsum, rcsum1, lreal)
         IF ( lreal.eq..false. ) then
            print *, "verification: incorrect, cal=", rcsum, &
            "orginal=", rcsum1,"i=", i,"j=",j
            exit loop
         ENDIF   
      end do
      end do loop
      IF((i.eq.NVAR+1).and.(j.eq.NV+1))then
        print *, "verification: correct"
      ENDIF
      END

