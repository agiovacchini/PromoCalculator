//
//  Cart.cpp
//  PromoCalculator
//
//  Created by Andrea Giovacchini on 04/01/14.
//  Copyright (c) 2014 Andrea Giovacchini. All rights reserved.
//

#include "BaseTypes.h"
#include "Cart.h"
#include <fstream>
#include <sstream>

Cart::Cart( )
{
    itemsMap.clear() ;
    CartRow totalCartRow = { TOTAL, 0 } ;
    CartRow itemCartRow = { ITEM, 0 } ;
    //totals.totalAmount = 0.0 ;
    //totals.itemsNumber = 0 ;
    //const char config[] = "cardPrefix=0260\n";
    string configFilePath = "/Users/andreagiovacchini/Documents/Sviluppo/Siti/PromoCalculator/PromoCalculator/" ;
    
    totalsMap[0].totalAmount = 0.0 ;
    totalsMap[0].itemsNumber = 0 ;
    itemsMap[&totalsMap[0]] = totalCartRow ;
    
    std::ifstream configFile( configFilePath + "PromoCalculator.ini" );
    if (!configFile) {
        std::cout << "\nNo PromoCalculator.ini file found in " << configFilePath << " directory." ;
        exit(-1);
    } else {
        std::cout << "\nCart initialized." ;
    }
    
    std::string line;
    while( std::getline(configFile, line) )
    {
        //std::cout << "\n" << line ;
        std::istringstream is_line(line);
        std::string key;
        if( std::getline(is_line, key, '=') )
        {
            std::string value;
            if( std::getline(is_line, value) )
                //store_line(key, value);
                configurationMap[key] = value ;
                //std::cout << "\n" << key << " - " << value ;
        }
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

void Cart::printConfiguration()
{
    typedef std::map<string, string>::iterator configurationRows;
    
    std::cout << "\nConfig print start" ;
    for(configurationRows iterator = configurationMap.begin(); iterator != configurationMap.end(); iterator++) {
        std::cout << "\nkey: " << iterator->first << ", value: " << iterator->second ;
    }
    std::cout << "\nConfig print end\n" ;
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

Totals Cart::addItemByBarcode( Item& pItem, Barcodes& pBarcode, long pQtyItem )
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
    
    std::cout << "\nTotals start" ;
    typedef std::map<unsigned long long, Totals>::iterator configurationRows;

    for(configurationRows iterator = totalsMap.begin(); iterator != totalsMap.end(); iterator++) {
        std::cout << "\nDept: " << iterator->first << ", value: " << iterator->second.totalAmount << ", items: " << iterator->second.itemsNumber ;
    }
    std::cout << "\nTotals end\n" ;
    std::cout << "\nCart print end\n" ;
}

int Cart::persist( string pBasePath )
{
    typedef std::map<void*, CartRow>::iterator itemRows;
    Item* itmRow ;

    for(itemRows iterator = itemsMap.begin(); iterator != itemsMap.end(); iterator++) {
        CartRow key = iterator->second ;
        switch (key.type) {
            case ITEM:
                itmRow = (Item*)iterator->first;
                std::cout << "\n" << itmRow->getCode() ;
                break;
            case DEPT:
                break;
            case TOTAL:
                break;
            default:
                break;
        }
        
        //std::cout << "\nkey: " << key ;
    }
    return 0 ;
}