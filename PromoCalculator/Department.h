//
//  Department.h
//  PromoCalculator
//
//  Created by Andrea Giovacchini on 05/01/14.
//  Copyright (c) 2014 Andrea Giovacchini. All rights reserved.
//

#include <string>
using namespace std;


#ifndef __PromoCalculator__Department__
#define __PromoCalculator__Department__

#include <iostream>
#include <sstream>

class Department {
    
    unsigned long long int code ;
    unsigned long long int parentCode ;
    string description ;

public:
	Department();

	Department(unsigned long long int pCode, unsigned long long int pParentCode, string pDescription);

    void setCode( unsigned long long int pCode ) ;
    
    unsigned long long int getCode() const ;

    void setParentCode( unsigned long long int pParentCode ) ;
    
    unsigned long long int getParentCode() const ;

    void setDescription( string pDescription ) ;
    
    string getDescription() const ;

    string toStr() ;

} ;



#endif /* defined(__PromoCalculator__Department__) */
