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
    float totalAmount ;
};

typedef Totals Totals ;

#define ITEM    0x01
#define DEPT    0x02
#define TOTAL   0x03

#endif
