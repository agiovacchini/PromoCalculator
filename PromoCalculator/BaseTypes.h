//
//  BaseTypes.h
//  PromoCalculator
//
//  Created by Andrea Giovacchini on 05/01/14.
//  Copyright (c) 2014 Andrea Giovacchini. All rights reserved.
//

#ifndef PromoCalculator_BaseTypes_h
#define PromoCalculator_BaseTypes_h

struct Totals {
    long itemsNumber ;
    double totalAmount ;
};

struct CartRow {
    int type ;
    long quantity ;
};

typedef Totals Totals ;


#define ITEM        0x01
#define DEPT        0x02
#define TOTAL       0x03
#define LOY_CARD    0x04

#define CARD_PREFIX 0x10001


#endif
