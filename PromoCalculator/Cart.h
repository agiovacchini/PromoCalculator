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
    std ::map <unsigned long long, Totals> totalsMap ;
    unsigned long number ;
    unsigned int state ;
    string basePath = "./" ;
    string cartFileName ;
    string tmpTransactionFileName ;
    std::ofstream tmpTransactionFile ;

public:
    Cart( string pBasePath, unsigned long pNumber, unsigned int pAction ) ;
    
    unsigned long getNumber() const ;
    void setNumber( unsigned long pNumber ) ;
    
    //Totals addItemByItemCode(Item& pItem) ;
    Totals addItemByBarcode(Item& pItem, Barcodes& pBarcode) ;
    Totals addItemByBarcode( Item& pItem, Barcodes& pBarcode, unsigned long pQtyItem ) ;
    Totals removeItemByBarcode( Item& pItem, Barcodes& pBarcode ) ;
    //Totals removeItem(Item& pItem) ;
    unsigned int getState() const ;
    void initialize( string pBasePath, unsigned long pNumber ) ;
    string getBasePath() const ;
    void printConfiguration() ;
    void printCart() ;
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
		return os;
	}
    
	// Extraction operator
	friend std::istream& operator>>(std::istream& is, Cart& s)
	{
		// read in individual members of s
		//is >> s.weight >> s.size;
		return is;
	}
    
} ;

#endif /* defined(__PromoCalculator__Cart__) */