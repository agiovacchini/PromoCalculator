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

class Department {
    
    unsigned long long int code ;
    string description ;

public:
    void setCode( unsigned long long int pCode ) ;
    
    unsigned long long int getCode() const ;

    void setDescription( string pDescription ) ;
    
    string getDescription() const ;

} ;



#endif /* defined(__PromoCalculator__Department__) */
