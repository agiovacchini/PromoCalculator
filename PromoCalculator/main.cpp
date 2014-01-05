//
//  main.cpp
//  PromoCalculator
//
//  Created by Andrea Giovacchini on 04/01/14.
//  Copyright (c) 2014 Andrea Giovacchini. All rights reserved.
//

#include <iostream>
#include <string>

#include "Department.h"
#include "Item.h"
#include "Cart.h"
#include "BaseTypes.h"

using namespace std;


int main(int argc, const char * argv[])
{
    Department dept1, dept2 ;
    Item articolo1, articolo2, articolo3  ;
    Cart carrello ;
    Totals tmpTotals ;
    //std::unique_ptr<Department> dept3(new Department);
    

    dept1.setCode(1);
    dept1.setDescription("Salumi");
    dept2.setCode(2);
    dept2.setDescription("Formaggi");
    dept2.setCode(2);
    dept2.setDescription("Cicci");
    
    articolo1.setPrice(5.9) ;
    articolo1.setDescription("Parmigiano") ;
    articolo1.setCode(90882) ;
    articolo1.setDepartment(dept2);
    
    articolo2.setPrice(2.1) ;
    articolo2.setDescription("Prosciutto") ;
    articolo2.setCode(123213) ;
    articolo2.setDepartment(dept1);
    
    
    articolo3.setPrice(1.3) ;
    articolo3.setDescription("Salame") ;
    articolo3.setCode(123212) ;
    articolo3.setDepartment(dept1);
    
    //std::cout << "\nPrice: " << articolo.getPrice() ;
    
    tmpTotals = carrello.addItem(articolo1) ;
    std::cout << "\nCart items number: " << tmpTotals.itemsNumber << " val: " << tmpTotals.totalAmount ;
    tmpTotals = carrello.addItem(articolo2) ;
    std::cout << "\nCart items number: " << tmpTotals.itemsNumber << " val: " << tmpTotals.totalAmount ;
    tmpTotals = carrello.addItem(articolo2) ;
    std::cout << "\nCart items number: " << tmpTotals.itemsNumber << " val: " << tmpTotals.totalAmount ;
    tmpTotals = carrello.addItem(articolo3) ;
    std::cout << "\nCart items number: " << tmpTotals.itemsNumber << " val: " << tmpTotals.totalAmount ;
    std::cout << "\n\n" ;
    return 0;
}
