#!/bin/bash
~/Library/Developer/Xcode/DerivedData/PromoCalculator-erbamcwoxvipvoelhbvhonetxbsp/Build/Products/Release/PromoCalculator /Users/andreagiovacchini/Documents/Sviluppo/Siti/PromoCalculator/PromoCalculator.deploy/ PromoCalculatorOsx.ini &

sleep 10

for i in {1..100}
do
	#cucumber TEST_ENV=local ../servicesTest/features/kStoreMine.feature
	cucumber TEST_ENV=local ../servicesTest/features/kStoreMineLinkedItem.feature
    sleep 1
done

