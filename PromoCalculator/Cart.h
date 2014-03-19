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
#include <fstream>

#include "map"

#include "Item.h"
#include "Barcodes.h"
#include "BaseTypes.h"
#include "base/ArchiveMap.h"

class Cart {
    //Totals totals ;
    std::map <unsigned int, unsigned long long> loyCardsMap ;
    std::map <void*, CartRow> cartItemsMap ;
    std::map <unsigned long long, Item> itemsLocalCopyMap ;
    
    std::map <unsigned long long, long> barcodesMap ;
    std::map <unsigned long long, Totals> totalsMap ;
    unsigned long number ;
    unsigned long itemsNumber ;
    unsigned int loyCardsNumber ;
    unsigned long nextRequestId ;
    long state ;
    string basePath = "./" ;
    string cartFileName ;
    string tmpTransactionFileName ;
    src::severity_logger_mt< boost::log::trivial::severity_level > my_logger_ca;
    bool dummyRCS ;
    
public:
    Cart( string pBasePath, unsigned long pNumber, unsigned int pAction, bool pDummyRCS ) ;
    
    unsigned long getNumber() const ;
    void setNumber( unsigned long pNumber ) ;
    
    void writeTransactionRow( string row ) ;
    bool updateLocalItemMap(Item& pItem, Department& pDept) ;
    long getItemPrice( Item& pItem, unsigned long long pBarcode, unsigned int pBCodeType, bool pPriceChangesWhileShopping) ;
    long addItemByBarcode( Item& pItem, unsigned long long pBarcode, unsigned long pPrice ) ;
    long addItemByBarcode( Item& pItem, unsigned long long pBarcode, unsigned long pQtyItem, unsigned long pPrice ) ;
    long removeItemByBarcode( Item& pItem, unsigned long long pBarcode, unsigned long pPrice ) ;
    long addLoyCard( unsigned long long pLoyCardNumber, unsigned int maxLoyCards ) ;
    long removeLoyCard( unsigned long long pLoyCardNumber ) ;
    long getState() const ;
    void setState( unsigned int pState ) ;
    unsigned long getRequestId() ;
    unsigned long getNextRequestId() ;
    unsigned int getLoyCardsNumber() const ;
    long voidAllCart() ;
    long printCart() ;
    long persist( ) ;
	long sendToPos(unsigned long pPosNumber, string pScanInPath, string pStoreId) ;
    string getAllCartJson( ArchiveMap<Item>& pAllItemsMap, bool pWithBarcodes ) ;
    long close( ) ;
    std::map <unsigned long long, Totals> getTotals() ;
    
    //const volatile Cart&& operator=(Cart&) volatile const && ;
    
    friend bool operator== (const Cart& p1, const Cart& p2)
    {
        return p1.getNumber() == p2.getNumber() ;
    }
    
    
    //http://www.cplusplus.com/forum/beginner/49924/
    friend std::ostream& operator<<(std::ostream& os, const Cart& s)
	{
		// write out individual members of s with an end of line between each one
		//os << s.weight << '\n';
		//os << s.size;
		os << s.basePath << s.cartFileName ;
		return os;
	}
    
	// Extraction operator
	friend std::istream& operator>>(std::istream& is, Cart& s)
	{
		// read in individual members of s
		//is >> s.weight >> s.size;
		is >> s.basePath >> s.cartFileName;
		return is;
	}
    
} ;

#endif /* defined(__PromoCalculator__Cart__) */