//
//  main.cpp
//  PromoCalculator
//
//  Created by Andrea Giovacchini on 04/01/14.
//  Copyright (c) 2014 Andrea Giovacchini. All rights reserved.
//

#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <sstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>


#include "Department.h"
#include "Item.h"
#include "Cart.h"
#include "BaseTypes.h"
#include "BaseSystem.h"

using namespace std;
using boost::asio::ip::tcp;
using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;

const int max_length = 1024;

typedef boost::shared_ptr<tcp::socket> socket_ptr;

BaseSystem bs ;

void session(socket_ptr sock)
{
    try
    {
        for (;;)
        {
            char data[max_length];
            
            boost::system::error_code error;
            size_t length = sock->read_some(boost::asio::buffer(data), error);
            if (error == boost::asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw boost::system::system_error(error); // Some other error.
            
            ptree pt2;
            std::istringstream is (data);
            read_json (is, pt2);
            
            std::string action = pt2.get<std::string> ("action");
            
            if ( (action.compare("add")==0) || (action.compare("remove")==0) )
            {
                std::string barcode = pt2.get<std::string> ("barcode");
                
                std::cout << "action:" << action ;
                std::cout << "barcode:" << barcode ;
                
                boost::asio::write(*sock, boost::asio::buffer(action, sizeof(action)));
                boost::asio::write(*sock, boost::asio::buffer(barcode, sizeof(barcode)));
            }
            
            if (action.compare("close")==0)
            {
                boost::asio::write(*sock, boost::asio::buffer(action, sizeof(action)));
            }



        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception in thread: " << e.what() << "\n";
    }
}

void server(boost::asio::io_service& io_service, short port)
{
    tcp::acceptor a(io_service, tcp::endpoint(tcp::v4(), port));
    for (;;)
    {
        socket_ptr sock(new tcp::socket(io_service));
        a.accept(*sock);
        boost::thread t(boost::bind(session, sock));
    }
}

void myCarts () {
    //Department dept1, dept2 ;
    //Item articolo1, articolo2, articolo3, articolo4  ;
    Cart carrello ;
    Totals tmpTotals ;
    
    carrello.printConfiguration();
    /*
    dept1.setCode(1);
    dept1.setDescription("Salumi");
    dept2.setCode(2);
    dept2.setDescription("Formaggi");
    
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
    
    articolo4.setPrice(1.34) ;
    articolo4.setDescription("Pecorino") ;
    articolo4.setCode(1233) ;
    articolo4.setDepartment(dept2);
    */
    
    //tmpTotals = carrello.addItem(articolo2) ;
    //tmpTotals = carrello.addItem(articolo2) ;
    //tmpTotals = carrello.addItem(articolo3) ;
    //tmpTotals = carrello.addItem(articolo4) ;
    //tmpTotals = carrello.addItem(articolo4) ;
    //tmpTotals = carrello.removeItem(articolo4) ;
    //tmpTotals = carrello.removeItem(articolo4) ;
    
    tmpTotals = carrello.addItem(bs.itemsMap[6945339]) ;
    /*tmpTotals = carrello.addItem(bs.itemsMap[6560998]) ;
    tmpTotals = carrello.addItem(bs.itemsMap[5227470]) ;
    tmpTotals = carrello.addItem(bs.itemsMap[6455173]) ;
    tmpTotals = carrello.addItem(bs.itemsMap[6765110]) ;
    tmpTotals = carrello.addItem(bs.itemsMap[6945339]) ;
    */
    carrello.printCart();
    
}



int main(int argc, const char * argv[])
{
    

    bs.readArchives() ;
    //std::cout << "\n" << bs.getItemByIntCode(6945339).toStr() ;
    //std::cout << "\n" << bs.getItemByIntCode(6962864).toStr() ;
    std::thread t1(myCarts);
    //std::thread t2(myCarts);
    t1.join();
    //t2.join();
    
    //readArchives();

    
    std::cout << "\n";
    try
    {
        std::cout << "Starting server" ;
        boost::asio::io_service io_service;
        
        using namespace std; // For atoi.
        
        //server(io_service, 50000);
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
