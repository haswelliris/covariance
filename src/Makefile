FC=ifort
CC=icc
LD= ifort
OPT=
FFLAGS= -qopenmp -g -O3
CFLAGS= -qopenmp -g -xCORE-AVX512 -O3 -qopt-report=5 -qopt-report-phase:vec

cov: main.o Covariance1.o Covariance${DBG}.o check.o
	${FC} ${FFLAGS} main.o Covariance1.o Covariance.o check.o -o cov
main.o: main.f90
	${FC} ${FFLAGS} -c main.f90
Covariance1.o: Covariance1.f90 
	${FC} ${FFLAGS} -c Covariance1.f90
Covariance.o: Covariance.f90 
#	${FC} ${FFLAGS} -c Covariance.f90
	${CC} ${CFLAGS} -c Covariance.c
check.o: check.f90
	${FC} ${FFLAGS} -c check.f90

clean:
	rm -rf *.o *.i cov
