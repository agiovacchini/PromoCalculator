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

#endif /* defined(__PromoCalculator__Cart__) */
#include "Item.h"

#include "BaseTypes.h"



class Cart {
    //Totals totals ;
    std::map <string, string> configurationMap ;
    std::map <void*, int> itemsMap ;
    std::map <unsigned long long, Totals> totalsMap ;
    
public:
    Cart() ;
    Totals addItem(Item& pItem) ;
    Totals removeItem(Item& pItem);
    void printConfiguration();
    void printCart();
} ;
