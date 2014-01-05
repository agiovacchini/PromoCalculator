//
//  Cart.cpp
//  PromoCalculator
//
//  Created by Andrea Giovacchini on 04/01/14.
//  Copyright (c) 2014 Andrea Giovacchini. All rights reserved.
//

#include "Cart.h"

Cart::Cart() {
    totals.totalAmount = 0.0 ;
    totals.itemsNumber = 0 ;
}

Totals Cart::addItem(Item pItem) {
    totals.totalAmount = totals.totalAmount + pItem.getPrice() ;
    totals.itemsNumber++ ;
    itemsMap[pItem] = totals.itemsNumber ;
    std::cout << "\n" << pItem.getDescription() << " - " << pItem.getPrice() << " - " << pItem.getDepartment().getDescription() ;
    return totals ;
}
