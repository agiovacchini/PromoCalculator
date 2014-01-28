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
    std::map <void*, CartRow> itemsMap ;
    std::map <void*, long> barcodesMap ;
    std::map <unsigned long long, Totals> totalsMap ;
    unsigned long number ;
    unsigned long nextRequestId ;
    unsigned int state ;
    string basePath = "./" ;
    string cartFileName ;
    string tmpTransactionFileName ;

public:
    Cart( string pBasePath, unsigned long pNumber, unsigned int pAction ) ;
    
    unsigned long getNumber() const ;
    void setNumber( unsigned long pNumber ) ;
    
    void writeTransactionRow( string row ) ;
    int addItemByBarcode(Item& pItem, Barcodes& pBarcode) ;
    int addItemByBarcode( Item& pItem, Barcodes& pBarcode, unsigned long pQtyItem ) ;
    int removeItemByBarcode( Item& pItem, Barcodes& pBarcode ) ;
    unsigned int getState() const ;
    void setState( unsigned int pState ) ;
    unsigned long getNextRequestId() ;
    int printCart() ;
    int persist( ) ;
    int close( ) ;
    
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