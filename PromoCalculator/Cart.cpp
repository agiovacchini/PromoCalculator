//
//  Cart.cpp
//  PromoCalculator
//
//  Created by Andrea Giovacchini on 04/01/14.
//  Copyright (c) 2014 Andrea Giovacchini. All rights reserved.
//

#include "BaseTypes.h"
#include "Cart.h"
#include <sstream>
#include <stdio.h>
#include "boost/format.hpp"

//static string basePath = "./" ;

Cart::Cart( string pBasePath, unsigned long pNumber )
{
    number = pNumber ;
    basePath = pBasePath ;
    state = CART_STATE_READY_FOR_ITEM ;

    itemsMap.clear() ;
    CartRow totalCartRow = { TOTAL, 0 } ;
    
    totalsMap[0].totalAmount = 0.0 ;
    totalsMap[0].itemsNumber = 0 ;
    itemsMap[&totalsMap[0]] = totalCartRow ;
    cartFileName = (boost::format("%sCARTS/%010lu.cart") % basePath % number).str() ;
    tmpTransactionFileName = (boost::format("%sCARTS/%010lu.transaction_in_progress") % basePath % number).str() ;
    std::cout << "\n" << cartFileName << " - " << tmpTransactionFileName << "\n" ;
}

void Cart::setNumber( unsigned long pNumber )
{
    this->number = pNumber ;
}

unsigned long Cart::getNumber() const
{
    return this->number ;
}

unsigned int Cart::getState() const {
    return this->state;
}

void Cart::initialize( string pBasePath, unsigned long pNumber )
{

}


/*Totals Cart::addItemByBarcode(unsigned long long pBarcode)
{
        //bs.itemsMap[6945339]
}*/

Totals Cart::addItemByBarcode( Item& pItem, Barcodes& pBarcode )
{
    long pQtyItem = 1 ;
    return addItemByBarcode(pItem,pBarcode,pQtyItem) ;
}

Totals Cart::addItemByBarcode( Item& pItem, Barcodes& pBarcode, unsigned long pQtyItem )
{
    long qtyItem = itemsMap[&pItem].quantity + pQtyItem ;
    long qtyBarcode = barcodesMap[&pBarcode] + pQtyItem ;
    
    totalsMap[0].itemsNumber = totalsMap[0].itemsNumber + pQtyItem ;
    totalsMap[0].totalAmount = totalsMap[0].totalAmount + ( pItem.getPrice() * pQtyItem ) ;
    totalsMap[pItem.getDepartment().getCode()].itemsNumber = totalsMap[pItem.getDepartment().getCode()].itemsNumber + pQtyItem ;
    totalsMap[pItem.getDepartment().getCode()].totalAmount = totalsMap[pItem.getDepartment().getCode()].totalAmount + ( pItem.getPrice() * pQtyItem ) ;
    
    //std::cout << "\ndept: " << pItem.getDepartment().getCode() ;
    //pItem.setQuantity(qty) ;
    itemsMap[&pItem] = { ITEM, qtyItem } ;
    barcodesMap[&pBarcode] = qtyBarcode ;
    tmpTransactionFile.open( tmpTransactionFileName, fstream::app );
    tmpTransactionFile << "A," << pBarcode.getCode() << "," << pQtyItem << "\n";
    tmpTransactionFile.close() ;
    return totalsMap[0] ;
}

/*
Totals Cart::addItemByCode(Item& pItem)
{
    long qty = itemsMap[&pItem].quantity + 1 ;
    
    totalsMap[0].itemsNumber++ ;
    totalsMap[0].totalAmount = totalsMap[0].totalAmount + pItem.getPrice() ;
    totalsMap[pItem.getDepartment().getCode()].itemsNumber++ ;
    totalsMap[pItem.getDepartment().getCode()].totalAmount = totalsMap[pItem.getDepartment().getCode()].totalAmount + pItem.getPrice() ;

    std::cout << "\ndept: " << pItem.getDepartment().getCode() ;
    //pItem.setQuantity(qty) ;
    itemsMap[&pItem] = { ITEM, qty } ;
    return totalsMap[0] ;
}
 */

Totals Cart::removeItemByBarcode( Item& pItem, Barcodes& pBarcode )
{
    long qtyItem ; //= itemsMap[&pItem].quantity ;
    long qtyBarcode ; //= barcodesMap[&pBarcode] ;
    
    if ((itemsMap.find(&pItem) == itemsMap.end()))
    {
        std::cout << "\nNot found: " << pItem.getDescription() ;
    }
    else
    {
        std::cout << "\nFound: " << pItem.getDescription() ;
        qtyItem = itemsMap[&pItem].quantity - 1 ;
        qtyBarcode = barcodesMap[&pBarcode] - 1 ;
        totalsMap[0].itemsNumber-- ;
        totalsMap[0].totalAmount = totalsMap[0].totalAmount - pItem.getPrice() ;
        totalsMap[pItem.getDepartment().getCode()].itemsNumber-- ;
        totalsMap[pItem.getDepartment().getCode()].totalAmount = totalsMap[pItem.getDepartment().getCode()].totalAmount - pItem.getPrice() ;
        tmpTransactionFile.open( tmpTransactionFileName, fstream::app );
        tmpTransactionFile << "R," << pBarcode.getCode() << "," << "1" << "\n";
        tmpTransactionFile.close();
        if (qtyItem < 1)
        {
            std::cout << "\nErasing item" ;
            itemsMap.erase(&pItem) ;
        } else
        {
            itemsMap[&pItem] = { ITEM, qtyItem } ;
        }

        if (qtyBarcode < 1)
        {
            barcodesMap.erase(&pBarcode) ;
        } else
        {
            barcodesMap[&pBarcode] = qtyBarcode ;
        }
    }
    
    return totalsMap[0] ;
}

void Cart::printCart()
{
    typedef std::map<void*, CartRow>::iterator itemRows;
    Item* itmRow ;
    //Totals* totalsRow ;
    
    std::cout << "\nCart #" << this->number <<  "print start" ;
    for(itemRows iterator = itemsMap.begin(); iterator != itemsMap.end(); iterator++) {
        CartRow key = iterator->second ;
        switch (key.type) {
            case ITEM:
                itmRow = (Item*)iterator->first;
                std::cout << "\n" << itmRow->getDescription() << " - " << itmRow->getPrice() << " - " << itmRow->getDepartment().getDescription() << " qty: " << key.quantity;
                break;
            case DEPT:
                break;
            case TOTAL:
                //totalsRow = (Totals*)iterator->first;
                //std::cout << "\nTotale: " << totalsRow->totalAmount << " Item Nr.: " << totalsRow->itemsNumber ;
                break;
            default:
                break;
        }
        //std::cout << "\nkey: " << key ;
    }
    std::cout << "\nState: " << this->getState() ;
    std::cout << "\nTotals start" ;
    typedef std::map<unsigned long long, Totals>::iterator configurationRows;

    for(configurationRows iterator = totalsMap.begin(); iterator != totalsMap.end(); iterator++) {
        std::cout << "\nDept: " << iterator->first << ", value: " << iterator->second.totalAmount << ", items: " << iterator->second.itemsNumber ;
    }
    std::cout << "\nTotals end\n" ;
    std::cout << "\nCart print end\n" ;
}

int Cart::persist( )
{
    typedef std::map<void*, CartRow>::iterator itemRows;
    typedef std::map<void*, long>::iterator barcodesRows;
    Barcodes* barcodesRow ;
    long qty = 0 ;
 
    std::cout << "\ncartFileName: " << cartFileName << "\n" ;

    std::ofstream cartFile( cartFileName );

    for(barcodesRows iterator = barcodesMap.begin(); iterator != barcodesMap.end(); iterator++)
    {
        barcodesRow = (Barcodes*)iterator->first;
        qty = iterator->second ;
        cartFile << barcodesRow->getCode() << "," << qty << "\n";
    }
    
    cartFile.close() ;
    
    return 0 ;
}

int Cart::close()
{
    //tmpTransactionFile.close() ;
    state = CART_STATE_CLOSED ;
    return 0 ;
}