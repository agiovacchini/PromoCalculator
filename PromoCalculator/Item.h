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


class Item : public Archive {
    
    unsigned long long int code ;
    long price ;
    string description ;
	unsigned long long int departmentCode ;
    unsigned long long int linkedBarCode ;

public:
	Item();

	Item(unsigned long long pCode, long pPrice, string pDescription, unsigned long long int pDepartment);
    
    void setCode( unsigned long long int pCode ) ;
    
    unsigned long long int getCode() const ;
    
    void setPrice( long pPrice ) ;
    
    long getPrice() const ;
    
    void setDescription( string pDescription ) ;
    
    string getDescription() const ;

	void setDepartmentCode( unsigned long long int pDepartmentCode );
        
	unsigned long long int getDepartmentCode() ;
        
    void setLinkedBarCode( unsigned long long int pLinkedBarCode ) ;
    
    unsigned long long int getLinkedBarCode() const ;
    
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
