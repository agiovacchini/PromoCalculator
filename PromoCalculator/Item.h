//
//  Item.h
//  PromoCalculator
//
//  Created by Andrea Giovacchini on 04/01/14.
//  Copyright (c) 2014 Andrea Giovacchini. All rights reserved.
//

#include <iostream>
#include <string>
using namespace std;

#ifndef __PromoCalculator__Item__
#define __PromoCalculator__Item__

#include "Department.h"
#include "base/Archive.h"
#include <sstream>
#include <stdint.h>

class Item : public Archive {
    
    uint64_t code ;
    int64_t price ;
    string description ;
	uint64_t departmentCode ;
    uint64_t linkedBarCode ;

public:
	Item();

	Item(uint64_t pCode, int32_t pPrice, string pDescription, uint64_t pDepartment);
    
    void setCode( uint64_t pCode ) ;
    
    uint64_t getCode() const ;
    
    void setPrice( int64_t pPrice ) ;
    
    int64_t getPrice() const ;
    
    void setDescription( string pDescription ) ;
    
    string getDescription() const ;

	void setDepartmentCode( uint64_t pDepartmentCode );
        
	uint64_t getDepartmentCode() const ;
        
    void setLinkedBarCode( uint64_t pLinkedBarCode ) ;
    
    uint64_t getLinkedBarCode() const ;
    
    string toStr() const ;
    
    //Per la mappa, le funzioni che chiamano devono avere il modificatore const per attestare che non
    //hanno effetti collaterali
    friend bool operator== (const Item& p1, const Item& p2)
    {
        return p1.getCode() == p2.getCode() ;
    }

    friend bool operator< (const Item& p1, const Item& p2)
    {
        return p1.getCode() < p2.getCode() ;
    }

    friend bool operator> (const Item& p1, const Item& p2)
    {
        return p1.getCode() > p2.getCode() ;
    }
    
    //Copy constructor
    Item& operator=( const Item& other ) {
        code = other.code ;
        price = other.price ;
        description = other.description ;
		departmentCode = other.departmentCode;
        linkedBarCode = other.linkedBarCode ;
        return *this;
    }
    
} ;

#endif /* defined(__PromoCalculator__Item__) */
