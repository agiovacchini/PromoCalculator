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

Totals Cart::addItem(Item& pItem)
{
    long qty = pItem.getQuantity() + 1;
    totals.totalAmount = totals.totalAmount + pItem.getPrice() ;
    totals.itemsNumber++ ;
    pItem.setQuantity(qty) ;
    /*if ((itemsMap[&pItem] != NULL) && (itemsMap[&pItem] == ITEM))
    {
        std::cout << "\nTrovato: " << pItem.getDescription() ;
        
    }*/
    itemsMap[&pItem] = ITEM ;
    itemsMap[&totals] = TOTAL ;
    //std::cout << "\nPuntatore:" << &pItem << " " <<  qty;
    //std::cout << "\nDimensione carrello: " << itemsMap.size();
    //std::cout << "\n" << pItem.getDescription() << " - " << pItem.getPrice() << " - " << pItem.getDepartment().getDescription() ;
    return totals ;
}

Totals Cart::removeItem(Item& pItem)
{
    long qty = 0 ;
    
    //totals.totalAmount = totals.totalAmount + pItem.getPrice() ;
    //totals.itemsNumber++ ;
    //pItem.setQuantity(qty) ;
    if (itemsMap.find(&pItem) == itemsMap.end())
    {
        std::cout << "\nNon trovato: " << pItem.getDescription() ;
    } else
    {
        std::cout << "\nTrovato: " << pItem.getDescription() ;
        qty = pItem.getQuantity() - 1;
        
        totals.totalAmount = totals.totalAmount - pItem.getPrice() ;
        totals.itemsNumber-- ;
        
        if (qty < 1)
        {
            itemsMap.erase(&pItem) ;
        } else
        {
            pItem.setQuantity(qty) ;
        }
    }
    //itemsMap[&pItem] = ITEM ;
    //itemsMap[&totals] = TOTAL ;
    //std::cout << "\nPuntatore:" << &pItem << " " <<  qty;
    //std::cout << "\nDimensione carrello: " << itemsMap.size();
    //std::cout << "\n" << pItem.getDescription() << " - " << pItem.getPrice() << " - " << pItem.getDepartment().getDescription() ;
    return totals ;
}

void Cart::printCart()
{
    typedef std::map<void*, int>::iterator itemRows;
    Item* itmRow ;
    Totals* totalsRow ;
    
    std::cout << "\nInizio carrello" ;
    for(itemRows iterator = itemsMap.begin(); iterator != itemsMap.end(); iterator++) {
        //Item itmRow =  ;
        int key = iterator->second ;
        switch (key) {
            case ITEM:
                itmRow = (Item*)iterator->first;
                std::cout << "\n" << itmRow->getDescription() << " - " << itmRow->getPrice() << " - " << itmRow->getDepartment().getDescription() << " qta: " << itmRow->getQuantity();
                break;
            case DEPT:
                break;
            case TOTAL:
                totalsRow = (Totals*)iterator->first;
                std::cout << "\nTotale: " << totalsRow->totalAmount << " Nr.Articoli: " << totalsRow->itemsNumber ;
                break;
            default:
                break;
        }
        
        std::cout << "\nkey: " << key ;
        // iterator->second = value
        // Repeat if you also want to iterate through the second map.
    }
    std::cout << "\nFine carrello\n" ;
}