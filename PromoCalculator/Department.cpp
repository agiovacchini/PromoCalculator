//
//  Department.cpp
//  PromoCalculator
//
//  Created by Andrea Giovacchini on 05/01/14.
//  Copyright (c) 2014 Andrea Giovacchini. All rights reserved.
//

#include "Department.h"

void Department::setCode( unsigned long long int pCode ) {
    this->code = pCode ;
}

unsigned long long int Department::getCode() const {
    return this->code ;
}

void Department::setDescription( string pDescription ) {
    this->description = pDescription ;
}

string Department::getDescription() const {
    return this->description ;
}

string Department::toStr() const {
    return std::to_string(this->code)
    + ", " + this->description
    ;
}