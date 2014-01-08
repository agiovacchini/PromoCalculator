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
using namespace std;

class Barcodes {
    
    unsigned long long int code ;
    unsigned long long int itemCode ;

public:
    
    void setCode( unsigned long long int pCode ) ;
    
    unsigned long long int getCode() const ;

    void setItemCode( unsigned long long int pItemCode ) ;
    
    unsigned long long int getItemCode() const ;
    
    string toStr() const ;
} ;

#endif /* defined(__PromoCalculator__Barcodes__) */
