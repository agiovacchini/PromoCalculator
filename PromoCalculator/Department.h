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
#include <stdint.h>
#include "base/Archive.h"

class Department : public Archive {
    
    uint64_t code ;
    uint64_t parentCode ;
    string description ;

public:
	Department();

	Department(uint64_t pCode, uint64_t pParentCode, string pDescription);

    void setCode( uint64_t pCode ) ;
    
    uint64_t getCode() const ;

    void setParentCode( uint64_t pParentCode ) ;
    
    uint64_t getParentCode() const ;

    void setDescription( string pDescription ) ;
    
    string getDescription() const ;

    string toStr() const ;

} ;



#endif /* defined(__PromoCalculator__Department__) */
