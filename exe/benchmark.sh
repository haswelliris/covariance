rm cov
cd ../src
make clean
make
cd ../exe
cp ../src/cov ./
echo "exp1:"
rm exp.ini
echo "4000 8000 10" >> exp.ini
./cov
echo "exp2:"
rm exp.ini
echo "12000 10000 16" >> exp.ini
./cov
#echo "exp3:"
#rm exp.ini
#echo "50000 50000 20" >> exp.ini
#./cov