rm ./promoCalculator
#make -f makefile_dbg.osx
make -f makefile_min.osx
#./PromoCalculator /Users/andreagiovacchini/Documents/Sviluppo/Siti/PromoCalculator/PromoCalculator.deploy/ PromoCalculatorOsx.ini
rm ./valgrind.log
valgrind  --tool=memcheck  -v --error-limit=no --log-file=./valgrind.log --leak-check=full --leak-resolution=high --num-callers=50 --freelist-vol=100000000 --trace-children=yes --malloc-fill=ac  --free-fill=fe --dsymutil=yes --track-origins=yes ./PromoCalculator /Users/andreagiovacchini/Documents/Sviluppo/Siti/PromoCalculator/PromoCalculator.deploy/ PromoCalculatorOsx.ini

