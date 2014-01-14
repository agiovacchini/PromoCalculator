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
    long totalAmount ;
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

#define CART_STATE_READY_FOR_ITEM 0x00
#define CART_STATE_TOTAL 0x01
#define CART_STATE_ASKING 0x02
#define CART_STATE_PAYING 0x03
#define CART_STATE_CLOSED 0x20
#define CART_NOT_INITIALIZED 0xFE
#define CART_STATE_ERROR 0xFF


#endif
