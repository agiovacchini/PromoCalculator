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
#include <chrono>
#include <ctime>
#include <locale>
#include <sys/time.h>
#include "boost/format.hpp"

//static string basePath = "./" ;

Cart::Cart( string pBasePath, unsigned long pNumber, unsigned int pAction )
{
    number = pNumber ;
    nextRequestId = 0 ;
    basePath = pBasePath ;
    
    
    itemsMap.clear() ;
    CartRow totalCartRow = { TOTAL, 0 } ;
    
    totalsMap[0].totalAmount = 0.0 ;
    totalsMap[0].itemsNumber = 0 ;
    itemsMap[&totalsMap[0]] = totalCartRow ;
    cartFileName = (boost::format("%sCARTS/%010lu.cart") % basePath % number).str() ;
    tmpTransactionFileName = (boost::format("%sCARTS/%010lu.transaction_in_progress") % basePath % number).str() ;
    std::cout << "\n" << cartFileName << " - " << tmpTransactionFileName << "\n" ;
    
    switch (pAction)
    {
        case GEN_CART_NEW:
            state = CART_STATE_READY_FOR_ITEM ;
            break ;
        case GEN_CART_LOAD:
            state = CART_TMPFILE_LOADING ;            
            break;
        default:
            break;
    }
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

void Cart::setState( unsigned int pState ) {
    //std::cout << "Carrello: passaggio di stato da " << this->state << " a " << pState ;
    this->state = pState ;
}

unsigned long Cart::getNextRequestId()
{
    this->nextRequestId++ ;
    return this->nextRequestId ;
}

/*Totals Cart::addItemByBarcode(unsigned long long pBarcode)
 {
 //bs.itemsMap[6945339]
 }*/

void Cart::writeTransactionRow( string row )
{
    timeval curTime;
    gettimeofday(&curTime, NULL);
    int milli = curTime.tv_usec / 1000;
    char timeStamp[84] = "" ;
    
    std::strftime( timeStamp, 100, "%Y-%m-%d %H:%M:%S", localtime(&curTime.tv_sec) ) ;
    
    tmpTransactionFile.open( tmpTransactionFileName, fstream::app );
    tmpTransactionFile << timeStamp << "." << std::to_string(milli) << "," << row << "\n";
    tmpTransactionFile.close() ;
}

int Cart::addItemByBarcode( Item& pItem, Barcodes& pBarcode )
{
    long pQtyItem = 1 ;
    return addItemByBarcode(pItem,pBarcode,pQtyItem) ;
}

int Cart::addItemByBarcode( Item& pItem, Barcodes& pBarcode, unsigned long pQtyItem )
{
    if ( (this->getState()==CART_TMPFILE_LOADING) || (this->getState()==CART_STATE_READY_FOR_ITEM) )
    {
        
        long qtyItem = itemsMap[&pItem].quantity + pQtyItem ;
        long qtyBarcode = barcodesMap[&pBarcode] + pQtyItem ;
        
        totalsMap[0].itemsNumber = totalsMap[0].itemsNumber + pQtyItem ;
        totalsMap[0].totalAmount = totalsMap[0].totalAmount + ( pItem.getPrice() * pQtyItem ) ;
        totalsMap[pItem.getDepartment().getCode()].itemsNumber = totalsMap[pItem.getDepartment().getCode()].itemsNumber + pQtyItem ;
        totalsMap[pItem.getDepartment().getCode()].totalAmount = totalsMap[pItem.getDepartment().getCode()].totalAmount + ( pItem.getPrice() * pQtyItem ) ;
        
        
        itemsMap[&pItem] = { ITEM, qtyItem } ;
        barcodesMap[&pBarcode] = qtyBarcode ;
        std::cout << "Stato carrello: " << std::to_string(this->getState()) ;
        if (this->getState()==CART_STATE_READY_FOR_ITEM)
        {
            this->writeTransactionRow( "A," + std::to_string( pBarcode.getCode() ) + "," + std::to_string( pQtyItem ) ) ;
        }
    }
    return RC_OK ;
}

int Cart::removeItemByBarcode( Item& pItem, Barcodes& pBarcode )
{
    if ( (this->getState()==CART_TMPFILE_LOADING) || (this->getState()==CART_STATE_READY_FOR_ITEM) )
    {
        
        long qtyItem ; //= itemsMap[&pItem].quantity ;
        long qtyBarcode ; //= barcodesMap[&pBarcode] ;
        
        if ((itemsMap.find(&pItem) == itemsMap.end()))
        {
            std::cout << "Not found: " << pItem.getDescription() << "\n" ;
            return RC_ERR ;
        }
        else
        {
            std::cout << "Found: " << pItem.getDescription() << "\n" ;
            qtyItem = itemsMap[&pItem].quantity - 1 ;
            qtyBarcode = barcodesMap[&pBarcode] - 1 ;
            totalsMap[0].itemsNumber-- ;
            totalsMap[0].totalAmount = totalsMap[0].totalAmount - pItem.getPrice() ;
            totalsMap[pItem.getDepartment().getCode()].itemsNumber-- ;
            totalsMap[pItem.getDepartment().getCode()].totalAmount = totalsMap[pItem.getDepartment().getCode()].totalAmount - pItem.getPrice() ;

            if (qtyItem < 1)
            {
                std::cout << "Erasing item\n" ;
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
            
            if (this->getState()==CART_STATE_READY_FOR_ITEM)
            {
                this->writeTransactionRow( "R," + std::to_string( pBarcode.getCode() ) + "," + "1" ) ;
            }
        }
    }
    return RC_OK ;
}

int Cart::printCart()
{
    typedef std::map<void*, CartRow>::iterator itemRows;
    Item* itmRow ;
    //Totals* totalsRow ;
    
    std::cout << "Cart #" << this->number <<  "print start\n" ;
    for(itemRows iterator = itemsMap.begin(); iterator != itemsMap.end(); iterator++) {
        CartRow key = iterator->second ;
        switch (key.type) {
            case ITEM:
                itmRow = (Item*)iterator->first;
                std::cout << itmRow->getDescription() << " - " << itmRow->getPrice() << " - " << itmRow->getDepartment().getDescription() << " qty: " << key.quantity << "\n" ;
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
    std::cout << "State: " << this->getState() << "\n" ;
    std::cout << "Totals start\n" ;
    typedef std::map<unsigned long long, Totals>::iterator configurationRows;
    
    for(configurationRows iterator = totalsMap.begin(); iterator != totalsMap.end(); iterator++) {
        std::cout << "Dept: " << iterator->first << ", value: " << iterator->second.totalAmount << ", items: " << iterator->second.itemsNumber << "\n" ;
    }
    std::cout << "Totals end\n" ;
    std::cout << "Cart print end\n" ;
    return RC_OK ;
}

int Cart::persist( )
{
    typedef std::map<void*, CartRow>::iterator itemRows;
    typedef std::map<void*, long>::iterator barcodesRows;
    Barcodes* barcodesRow ;
    long qty = 0 ;
    
    std::cout << "cartFileName: " << cartFileName << "\n" ;
    
    std::ofstream cartFile( cartFileName );
    
    for(barcodesRows iterator = barcodesMap.begin(); iterator != barcodesMap.end(); iterator++)
    {
        barcodesRow = (Barcodes*)iterator->first;
        qty = iterator->second ;
        cartFile << barcodesRow->getCode() << "," << qty << "\n";
    }
    
    cartFile.close() ;
    return RC_OK ;
}

int Cart::close()
{
    //tmpTransactionFile.close() ;
    state = CART_STATE_CLOSED ;
    return RC_OK ;
}