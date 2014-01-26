//
//  BaseTypes.h
//  PromoCalculator
//
//  Created by Andrea Giovacchini on 05/01/14.
//  Copyright (c) 2014 Andrea Giovacchini. All rights reserved.
//
#if defined _WIN32
    #include <winsock2.h>
#endif

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
#define CART_TMPFILE_LOADING 0x30
#define CART_NOT_INITIALIZED 0x40
#define CART_STATE_ERROR 0x50

#define GEN_CART_NEW       0x01
#define GEN_CART_LOAD      0x02

#define RC_OK              0x00
#define RC_ERR             0x1

#define BCODE_EAN13         0x00
#define BCODE_EAN13_PRICE_REQ         0x01
#define BCODE_UPC           0x02
#define BCODE_EAN8          0x03
#define BCODE_CODE39        0x04
#define BCODE_CODE128       0x05

#define BCODE_LOYCARD       0x80
#define BCODE_NOT_RECOGNIZED    0xff

#endif
