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
#include <sstream>


class Item {
    
    unsigned long long int code ;
    long price ;
    string description ;
    long quantity ;
    Department *department ;
    unsigned long long int linkedBarCode ;

public:
    Item() ;
    
    void setCode( unsigned long long int pCode ) ;
    
    unsigned long long int getCode() const ;
    
    void setPrice( long pPrice ) ;
    
    long getPrice() const ;
    
    void setDescription( string pDescription ) ;
    
    string getDescription() const ;

    void setDepartment( Department& pDepartment ) ;
    
    void setDepartmentByCode( Department& pDepartment ) ;
    
    Department& getDepartment() const ;
        
    void setLinkedBarCode( unsigned long long int pLinkedBarCode ) ;
    
    unsigned long long int getLinkedBarCode() const ;
    
    string toStr() ;
    
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
        quantity = other.quantity ;
        department = other.department ;
        linkedBarCode = other.linkedBarCode ;

        return *this;
    }
    
} ;

#endif /* defined(__PromoCalculator__Item__) */
