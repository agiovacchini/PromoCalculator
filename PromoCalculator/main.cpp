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

#include "gmock/gmock.h"

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
    unsigned long initializedCartNumber = 0 ;
    string msg = "" ;
    std::uint32_t cartId = 0 ;
    std::uint64_t barcode = 0 ;
    std::int32_t qty = 0 ;
    
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

            if (action.compare("init")==0)
            {
                initializedCartNumber = bs.newCart() ;
                msg = "Initialized cart " + std::to_string( initializedCartNumber ) + "\n" ;
                boost::asio::write(*sock, boost::asio::buffer(msg, sizeof(msg)));
            }
            
            if (action.compare("add")==0)
            {
                barcode = pt2.get<std::uint64_t> ("barcode");
                qty = pt2.get<std::int32_t> ("qty");
                cartId = pt2.get<std::uint32_t> ("cartId");
                
                bs.getCart(cartId)->addItemByBarcode(bs.itemsMap[bs.barcodesMap[barcode].getItemCode()], bs.barcodesMap[barcode], qty) ;
                msg = "Cart #" + std::to_string( cartId ) + ", added barcode " + std::to_string( barcode ) + "\n" ;
                boost::asio::write(*sock, boost::asio::buffer(msg, sizeof(msg)));
                bs.getCart(cartId)->printCart() ;
            }

            if (action.compare("remove")==0)
            {
                barcode = pt2.get<std::uint64_t> ("barcode");
                cartId = pt2.get<std::uint32_t> ("cartId");
                
                bs.getCart(cartId)->removeItemByBarcode(bs.itemsMap[bs.barcodesMap[barcode].getItemCode()], bs.barcodesMap[barcode]) ;
                msg = "Cart #" + std::to_string( cartId ) + ", removed barcode " + std::to_string( barcode ) + "\n" ;
                boost::asio::write(*sock, boost::asio::buffer(msg, sizeof(msg)));
                bs.getCart(cartId)->printCart() ;
            }

            if (action.compare("print")==0)
            {
                cartId = pt2.get<std::uint32_t> ("cartId");
                bs.getCart(cartId)->printCart() ;
                msg = "Cart #" + std::to_string( cartId ) + " printed\n" ;
                boost::asio::write(*sock, boost::asio::buffer(msg, sizeof(msg)));
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


int main(int argc, const char * argv[])
{
    
    //unsigned long cart1 = bs.newCart() ;
    //unsigned long cart2 = bs.newCart() ;
    
    //std::cout << "\nNew Cart: " << cart1 ;
    //std::cout << "\nNew Cart: " << cart2 ;

    bs.readArchives() ;
    /*
    bs.getCart(cart2)->addItem(bs.itemsMap[6945339]) ;
    bs.getCart(cart1)->addItem(bs.itemsMap[6560998]) ;
    bs.getCart(cart2)->addItem(bs.itemsMap[5227470]) ;
    bs.getCart(cart2)->addItem(bs.itemsMap[6455173]) ;
    bs.getCart(cart1)->addItem(bs.itemsMap[6765110]) ;
    bs.getCart(cart1)->addItem(bs.itemsMap[6945339]) ;
    bs.getCart(cart2)->addItem(bs.itemsMap[bs.barcodesMap[8033604138826].getItemCode()]) ;
    bs.getCart(cart1)->printCart() ;
    bs.getCart(cart2)->printCart() ;
    */

    //std::cout << "\n" << bs.getItemByIntCode(6945339).toStr() ;
    //std::cout << "\n" << bs.getItemByIntCode(6962864).toStr() ;
    //std::thread t1(myCarts);
    //std::thread t2(myCarts);
    //t1.join();
    //t2.join();
    
    //readArchives();

    
    std::cout << "\n";
    try
    {
        std::cout << "Starting server" ;
        boost::asio::io_service io_service;
        
        using namespace std; // For atoi.
        
        server(io_service, 50000);
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
