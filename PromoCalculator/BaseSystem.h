//
//  BaseSystem.h
//  PromoCalculator
//
//  Created by Andrea Giovacchini on 08/01/14.
//  Copyright (c) 2014 Andrea Giovacchini. All rights reserved.
//

#ifndef __PromoCalculator__BaseSystem__
#define __PromoCalculator__BaseSystem__


#include "Department.h"
#include "Item.h"
#include "Cart.h"
#include "BaseTypes.h"

class BaseSystem {
public:
    std::map <unsigned long long, Item> itemsMap ;
    std::map <unsigned long long, Department> deparmentsMap ;
    void readDepartmentArchive( string pFileName ) ;
    void readItemArchive( string pFileName ) ;
    void readArchives( ) ;
    Item getItemByIntCode( unsigned long long pIntcode ) ;
};


#endif /* defined(__PromoCalculator__BaseSystem__) */