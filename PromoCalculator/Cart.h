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

class Cart {
    //Totals totals ;
    std::map <unsigned int, unsigned long long> loyCardsMap ;
    std::map <void*, CartRow> itemsMap ;
    std::map <unsigned long long, long> barcodesMap ;
    std::map <unsigned long long, Totals> totalsMap ;
    unsigned long number ;
    unsigned long itemsNumber ;
    unsigned int loyCardsNumber ;
    unsigned long nextRequestId ;
    unsigned int state ;
    string basePath = "./" ;
    string cartFileName ;
    string tmpTransactionFileName ;
    src::severity_logger_mt< boost::log::trivial::severity_level > my_logger_ca;
    
public:
    Cart( string pBasePath, unsigned long pNumber, unsigned int pAction ) ;
    
    unsigned long getNumber() const ;
    void setNumber( unsigned long pNumber ) ;
    
    void writeTransactionRow( string row ) ;
    int addItemByBarcode(Item& pItem, unsigned long long pBarcode, unsigned long long pPrice, unsigned int pBCodeType ) ;
    int addItemByBarcode( Item& pItem, unsigned long long pBarcode, unsigned long pQtyItem, unsigned long long pPrice, unsigned int pBCodeType ) ;
    int removeItemByBarcode( Item& pItem, unsigned long long pBarcode, unsigned long long pPrice, unsigned int pBCodeType ) ;
    int addLoyCard( unsigned long long pLoyCardNumber, unsigned int maxLoyCards ) ;
    unsigned int getState() const ;
    void setState( unsigned int pState ) ;
    unsigned long getRequestId() ;
    unsigned long getNextRequestId() ;
    unsigned int getLoyCardsNumber() const ;
    int printCart() ;
    int persist( ) ;
	int sendToPos(unsigned long pPosNumber, string pScanInPath);
    int close( ) ;
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