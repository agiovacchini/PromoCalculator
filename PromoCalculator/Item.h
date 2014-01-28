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

public:
    
    void setCode( unsigned long long int pCode ) ;
    
    unsigned long long int getCode() const ;
    
    void setPrice( long pPrice ) ;
    
    long getPrice() const ;
    
    void setDescription( string pDescription ) ;
    
    string getDescription() const ;

    void setDepartment( Department& pDepartment ) ;
    
    void setDepartmentByCode( Department& pDepartment ) ;
    
    Department& getDepartment() const ;
    
    void setQuantity( long pQuantity ) ;
    
    long getQuantity() const ;
    
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
    
} ;

#endif /* defined(__PromoCalculator__Item__) */
