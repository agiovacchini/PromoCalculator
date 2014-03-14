//
//  Item.cpp
//  PromoCalculator
//
//  Created by Andrea Giovacchini on 04/01/14.
//  Copyright (c) 2014 Andrea Giovacchini. All rights reserved.
//

#include "Item.h"

Item::Item()
{
    this->code = 0 ;
    this->price = 0 ;
	this->description = "" ;
    this->department = nullptr ;
}

void Item::setCode( unsigned long long int pCode ) {
    this->code = pCode ;
}

unsigned long long int Item::getCode() const {
    return this->code ;
}

void Item::setPrice( long pPrice ) {
    this->price = pPrice ;
}

long Item::getPrice() const {
    return this->price ;
}

void Item::setDescription( string pDescription ) {
    this->description = pDescription ;
}

string Item::getDescription() const {
    return this->description ;
}

//E' una referenza all'oggetto reparto
void Item::setDepartment( Department& pDepartment ) {
    this->department = &pDepartment ;
}

Department& Item::getDepartment() const {
    return *department ;
}

void Item::setLinkedBarCode( unsigned long long int pLinkedBarCode ) {
    this->linkedBarCode = pLinkedBarCode ;
}

unsigned long long int Item::getLinkedBarCode() const {
    return this->linkedBarCode ;
}

string Item::toStr() {
	std::stringstream tempStringStream;
	tempStringStream.str(std::string());
	tempStringStream.clear();
	tempStringStream << this->code
		<< ",\"" << this->description << "\""
        << "," << this->department->getCode()
        << "," << this->price
        << "," << this->linkedBarCode
    ;

	return tempStringStream.str();
}