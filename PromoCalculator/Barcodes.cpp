//
//  Barcodes.cpp
//  PromoCalculator
//
//  Created by Andrea Giovacchini on 08/01/14.
//  Copyright (c) 2014 Andrea Giovacchini. All rights reserved.
//

#include "Barcodes.h"

void Barcodes::setCode( unsigned long long int pCode ) {
    this->code = pCode ;
}

unsigned long long int Barcodes::getCode() const {
    return this->code ;
}

void Barcodes::setItemCode( unsigned long long int pItemCode ) {
    this->itemCode = pItemCode ;
}

unsigned long long int Barcodes::getItemCode() const {
    return this->itemCode ;
}

string Barcodes::toStr() const {
    std::stringstream row ;
    
    row << this->code
    << "," << this->itemCode ;
    
    return row.str() ;
}