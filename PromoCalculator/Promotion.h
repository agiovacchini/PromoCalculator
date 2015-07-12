//
//  Promotion.h
//  PromoCalculator
//
//  Created by Andrea Giovacchini on 10/07/15.
//  Copyright (c) 2015 Andrea Giovacchini. All rights reserved.
//

#include <iostream>
#include <string>
using namespace std;

#ifndef PromoCalculator_Promotion_h
#define PromoCalculator_Promotion_h

#include "base/Archive.h"
#include <sstream>
#include <stdint.h>

class Promotion : public Archive {
    
    uint64_t code ;
    uint64_t itemCode ;
    int64_t discount ;
    int64_t discountType ;
    string description ;
    
public:
    Promotion();
    
    Promotion(uint64_t pCode, uint64_t pItemCode, int64_t pDiscount, int64_t pDiscountType, string pDescription);
    
    void setCode( uint64_t pCode ) ;
    
    uint64_t getCode() const ;
    
    
    void setItemCode( uint64_t pItemCode ) ;
    
    uint64_t getItemCode() const ;
    
    
    void setDiscount( uint64_t pDiscount ) ;
    
    uint64_t getDiscount() const ;
    
    
    void setDiscountType( uint64_t pDiscountType ) ;
    
    uint64_t getDiscountType() const ;
    
    
    void setDescription( string pDescription ) ;
    
    string getDescription() const ;
    
    string toStr() const ;
    
    //Per la mappa, le funzioni che chiamano devono avere il modificatore const per attestare che non
    //hanno effetti collaterali
    friend bool operator== (const Promotion& p1, const Promotion& p2)
    {
        return p1.getCode() == p2.getCode() ;
    }
    
    friend bool operator< (const Promotion& p1, const Promotion& p2)
    {
        return p1.getCode() < p2.getCode() ;
    }
    
    friend bool operator> (const Promotion& p1, const Promotion& p2)
    {
        return p1.getCode() > p2.getCode() ;
    }
    
    //Copy constructor
    Promotion& operator=( const Promotion& other ) {
        code = other.code ;
        itemCode = other.itemCode ;
        discount = other.discount ;
        discountType = other.discountType;
        description = other.description ;
        return *this;
    }
    
} ;

#endif
