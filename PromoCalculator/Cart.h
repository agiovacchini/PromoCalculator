//
//  Cart.h
//  PromoCalculator
//
//  Created by Andrea Giovacchini on 04/01/14.
//  Copyright (c) 2014 Andrea Giovacchini. All rights reserved.
//

#ifndef __PromoCalculator__Cart__
#define __PromoCalculator__Cart__

#include <iostream>
#include "map"

#include "Item.h"
#include "BaseTypes.h"



class Cart {
    //Totals totals ;
    std::map <string, string> configurationMap ;
    std::map <void*, CartRow> itemsMap ;
    std::map <unsigned long long, Totals> totalsMap ;
    
    unsigned long number ;
    
public:
    Cart( ) ;
    
    unsigned long getNumber() const ;
    void setNumber( unsigned long pNumber ) ;
    
    Totals addItem(Item& pItem) ;
    Totals removeItem(Item& pItem);
    void printConfiguration();
    void printCart();
    
    friend bool operator== (const Cart& p1, const Cart& p2)
    {
        return p1.getNumber() == p2.getNumber() ;
    }
    
} ;

#endif /* defined(__PromoCalculator__Cart__) */