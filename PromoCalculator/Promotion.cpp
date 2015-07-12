//
//  Promotion.cpp
//  PromoCalculator
//
//  Created by Andrea Giovacchini on 10/07/15.
//  Copyright (c) 2015 Andrea Giovacchini. All rights reserved.
//

#include "Promotion.h"

Promotion::Promotion()
{
    this->code = 0 ;
    this->itemCode = 0 ;
    this->discount = 0 ;
    this->discountType = 0 ;
    this->description = "" ;
}

Promotion::Promotion(uint64_t pCode, uint64_t pItemCode, int64_t pDiscount, int64_t pDiscountType, string pDescription)
{
    this->code = pCode ;
    this->itemCode = pItemCode ;
    this->discount = pDiscount ;
    this->discountType = pDiscountType ;
    this->description = pDescription ;
}

void Promotion::setCode( uint64_t pCode ) {
    this->code = pCode ;
}

uint64_t Promotion::getCode() const {
    return this->code ;
}

void Promotion::setItemCode( uint64_t pItemCode ) {
    this->itemCode = pItemCode ;
}

uint64_t Promotion::getItemCode() const {
    return this->itemCode ;
}

void Promotion::setDiscount( uint64_t pDiscount ) {
    this->discount = pDiscount ;
}

uint64_t Promotion::getDiscount() const {
    return this->discount ;
}

void Promotion::setDiscountType( uint64_t pDiscountType ) {
    this->discountType = pDiscountType ;
}

uint64_t Promotion::getDiscountType() const {
    return this->discountType ;
}

void Promotion::setDescription( string pDescription ) {
    this->description = pDescription ;
}

string Promotion::getDescription() const {
    return this->description ;
}


string Promotion::toStr() const {
    std::stringstream tempStringStream;
    tempStringStream.str(std::string());
    tempStringStream.clear();
    tempStringStream << this->code
    << "," << this->itemCode
    << "," << this->discount
    << "," << this->discountType
    << ",\"" << this->description << "\""
    ;
    
    return tempStringStream.str();
}
