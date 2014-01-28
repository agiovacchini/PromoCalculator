@echo off
set path=%path%;d:\Listati\Siti\PromoCalculator\PromoCalculator.deploy\;
del PromoCalculator.exe
make -f makefile.win > buildLog.log 2>&1
type buildLog.log
if not exist PromoCalculator.exe goto errore

goto fine

:errore
echo ====================
echo   Errore di build!
echo ====================
goto fine

:fine
echo ====================
echo   Build ok, eseguo
echo ====================
rem PromoCalculator.exe d:\Listati\Siti\PromoCalculator\PromoCalculator.deploy\
