//
//  Barcodes.h
//  PromoCalculator
//
//  Created by Andrea Giovacchini on 08/01/14.
//  Copyright (c) 2014 Andrea Giovacchini. All rights reserved.
//

#ifndef __PromoCalculator__Barcodes__
#define __PromoCalculator__Barcodes__

#include <iostream>
#include <string>
#include <inttypes.h>
#include "base/Archive.h"

using namespace std;

class Barcodes : public Archive {
    
    uint64_t code ;
    uint64_t itemCode ;

public:
    
    void setCode( uint64_t pCode ) ;
    
    uint64_t getCode() const ;

    void setItemCode( uint64_t pItemCode ) ;
    
    uint64_t getItemCode() const ;
    
    string toStr() const ;
} ;

#endif /* defined(__PromoCalculator__Barcodes__) */
