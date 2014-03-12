#!/bin/bash
for i in {1..100}
do
	cucumber TEST_ENV=local ../servicesTest/features/kStoreMineLinkedItem.feature
	sleep 1
done

