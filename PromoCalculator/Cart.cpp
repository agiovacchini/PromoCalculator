//
//  Cart.cpp
//  PromoCalculator
//
//  Created by Andrea Giovacchini on 04/01/14.
//  Copyright (c) 2014 Andrea Giovacchini. All rights reserved.
//

#include "BaseTypes.h"
#include "Cart.h"

Cart::Cart()
{
    totals.totalAmount = 0.0 ;
    totals.itemsNumber = 0 ;
}

Totals Cart::addItem(Item pItem)
{
    totals.totalAmount = totals.totalAmount + pItem.getPrice() ;
    totals.itemsNumber++ ;
    itemsMap[pItem] = ITEM ;
    //std::cout << "\n" << pItem.getDescription() << " - " << pItem.getPrice() << " - " << pItem.getDepartment().getDescription() ;
    return totals ;
}


void Cart::printCart()
{
    typedef std::map<Item, int>::iterator itemRows;
    
    std::cout << "\nInizio carrello" ;
    for(itemRows iterator = itemsMap.begin(); iterator != itemsMap.end(); iterator++) {
        Item itmRow = iterator->first ;
        std::cout << "\n" << itmRow.getDescription() << " - " << itmRow.getPrice() << " - " << itmRow.getDepartment().getDescription() ;
        // iterator->second = value
        // Repeat if you also want to iterate through the second map.
    }
    std::cout << "\nFine carrello\n" ;
}