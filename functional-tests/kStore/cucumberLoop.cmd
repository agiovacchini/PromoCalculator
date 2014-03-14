del d:\Listati\Siti\PromoCalculator\PromoCalculator.deploy\ARCHIVES\001\12003\CARTS\*.transaction_in_progress 
:inizio
call cucumber TEST_ENV=local ..\servicesTest\features\kStoreMineLinkedItem.feature
goto inizio
