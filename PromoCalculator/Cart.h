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
    std::map <unsigned int, uint64_t> loyCardsMap ;
    std::map <void*, CartRow> cartItemsMap ;
    std::map <uint64_t, Item> itemsLocalCopyMap ;
    
    std::map <uint64_t, long> barcodesMap ;
    std::map <uint64_t, Totals> totalsMap ;
    uint32_t number ;
    uint32_t itemsNumber ;
    unsigned int loyCardsNumber ;
    uint32_t nextRequestId ;
    long state ;
    string basePath = "./" ;
    string cartFileName ;
    string tmpTransactionFileName ;
    src::severity_logger_mt< boost::log::trivial::severity_level > my_logger_ca;
    bool dummyRCS ;
    
public:
    Cart( string pBasePath, uint32_t pNumber, unsigned int pAction, bool pDummyRCS ) ;
    
    uint32_t getNumber() const ;
    void setNumber( uint32_t pNumber ) ;
    
    void writeTransactionRow( string row ) ;
    bool updateLocalItemMap(Item pItem, Department pDept) ;
    long getItemPrice( Item* pItem, uint64_t pBarcode, unsigned int pBCodeType, bool pPriceChangesWhileShopping) ;
    long addItemByBarcode( Item& pItem, uint64_t pBarcode, uint32_t pPrice ) ;
    long addItemByBarcode( Item& pItem, uint64_t pBarcode, uint32_t pQtyItem, uint32_t pPrice ) ;
    long removeItemByBarcode( Item& pItem, uint64_t pBarcode, uint32_t pPrice ) ;
    long addLoyCard( uint64_t pLoyCardNumber, unsigned int maxLoyCards ) ;
    long removeLoyCard( uint64_t pLoyCardNumber ) ;
    long getState() const ;
    void setState( unsigned int pState ) ;
    uint32_t getRequestId() ;
    uint32_t getNextRequestId() ;
    unsigned int getLoyCardsNumber() const ;
    long voidAllCart() ;
    long printCart() ;
    long persist( ) ;
	long sendToPos(uint32_t pPosNumber, string pScanInPath, string pStoreId) ;
    string getAllCartJson( ArchiveMap<Item> pAllItemsMap, bool pWithBarcodes ) ;
    long close( ) ;
    std::map <uint64_t, Totals> getTotals() ;
    
    friend bool operator== (const Cart& p1, const Cart& p2)
    {
        return p1.getNumber() == p2.getNumber() ;
    }
    
    
    //http://www.cplusplus.com/forum/beginner/49924/
    friend std::ostream& operator<<(std::ostream& os, const Cart& s)
	{
		// write out individual members of s with an end of line between each one
		os << s.basePath << s.cartFileName ;
		return os;
	}
    
	// Extraction operator
	friend std::istream& operator>>(std::istream& is, Cart& s)
	{
		// read in individual members of s
		is >> s.basePath >> s.cartFileName;
		return is;
	}
    
} ;

#endif /* defined(__PromoCalculator__Cart__) */