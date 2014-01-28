//
//  main.cpp
//  PromoCalculator
//
//  Created by Andrea Giovacchini on 04/01/14.
//  Copyright (c) 2014 Andrea Giovacchini. All rights reserved.
//
#include "BaseTypes.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <sstream>

//#include "gmock/gmock.h"

#include "BaseSystem.h"

using namespace std;

const int max_length = 1024;


int main(int argc, const char * argv[])
{
    //BaseSystem bs = BaseSystem("/Users/andreagiovacchini/Documents/Sviluppo/Siti/PromoCalculator/PromoCalculator/") ;
	//BaseSystem bs = BaseSystem("d:\\Listati\\Siti\\PromoCalculator\\PromoCalculator.deploy\\");
	BaseSystem bs = BaseSystem(argv[1]);

	return 0;
}
