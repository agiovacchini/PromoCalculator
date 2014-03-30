del d:\Listati\Siti\PromoCalculator\PromoCalculator.deploy\ARCHIVES\001\12003\CARTS\*.transaction_in_progress 
:inizio
rem call cucumber TEST_ENV=lab_ravenna_development ..\servicesTest\features\kStoreMineLinkedItem.feature
call cucumber TEST_ENV=local ..\servicesTest\features\kStoreMineLinkedItem.feature
goto inizio
