//
//  BaseSystem.cpp
//  PromoCalculator
//
//  Created by Andrea Giovacchini on 08/01/14.
//  Copyright (c) 2014 Andrea Giovacchini. All rights reserved.
//

#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "BaseSystem.h"

const int max_length = 1024;

static unsigned long nextCartNumber ;

namespace qi = boost::spirit::qi;

using namespace std;

BaseSystem::BaseSystem( string pBasePath )
{
    boost::asio::io_service io_service;
    this->basePath = pBasePath ;
    
    nextCartNumber = 1 ;

    std::ifstream configFile( this->basePath + "PromoCalculator.ini" );
    if (!configFile) {
        std::cout << "\nNo PromoCalculator.ini file found in " << basePath << " directory." ;
        exit(-1);
    } else {
        std::cout << "\nCart initialized." ;
    }
    
    std::string line;
    while( std::getline(configFile, line) )
    {
        //std::cout << "\n" << line ;
        std::istringstream is_line(line);
        std::string key;
        if( std::getline(is_line, key, '=') )
        {
            std::string value;
            if( std::getline(is_line, value) )
                //store_line(key, value);
                configurationMap[key] = value ;
            //std::cout << "\n" << key << " - " << value ;
        }
    }

    configFile.close() ;
    
    this->readArchives() ;
    
    try
    {
        std::cout << "Starting server" ;
        using namespace std; // For atoi.
        salesServer(io_service, 50000);
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    
}

string BaseSystem::getBasePath() const
{
    return this->basePath ;
}

void BaseSystem::setBasePath( string pBasePath )
{
    this->basePath = pBasePath ;
}

void BaseSystem::printConfiguration()
{
    typedef std::map<string, string>::iterator configurationRows;
    
    std::cout << "\nConfig print start" ;
    for(configurationRows iterator = configurationMap.begin(); iterator != configurationMap.end(); iterator++) {
        std::cout << "\nkey: " << iterator->first << ", value: " << iterator->second ;
    }
    std::cout << "\nConfig print end\n" ;
}

void BaseSystem::readDepartmentArchive( string pFileName )
{
    //http://stackoverflow.com/questions/18365463/how-to-parse-csv-using-boostspirit
    
    //string configFilePath = "./ARCHIVES/" ;
    
    // Tokenizer
    typedef boost::tokenizer< boost::escaped_list_separator<char> , std::string::const_iterator, std::string> Tokenizer;
    boost::escaped_list_separator<char> seps('\\', ',', '\"');
    
    string archiveFileName = this->basePath + "ARCHIVES/" + pFileName ;
    
    std::ifstream archiveFile( archiveFileName );
    if (!archiveFile) {
        std::cout << "\nFile " + archiveFileName + " not found" ;
        exit(-1);
    } else {
        std::cout << "\nFile " + archiveFileName + " found" ;
    }
    
    std::string line;
    while( std::getline(archiveFile, line) )
    {
        qi::rule<std::string::const_iterator, std::string()> quoted_string = '"' >> *(qi::char_ - '"') >> '"';
        qi::rule<std::string::const_iterator, std::string()> valid_characters = qi::char_ - '"' - ',';
        qi::rule<std::string::const_iterator, std::string()> item = *(quoted_string | valid_characters );
        qi::rule<std::string::const_iterator, std::vector<std::string>()> csv_parser = item % ',';
        
        // Boost Spirit Qi
        
        std::string::const_iterator s_begin = line.begin();
        std::string::const_iterator s_end = line.end();
        std::vector<std::string> result;
        
        bool r = boost::spirit::qi::parse(s_begin, s_end, csv_parser, result);
        assert(r == true);
        assert(s_begin == s_end);
        int column = 0 ;
        Department tempDepartment ;
        for (auto i : result)
        {
            //std::cout << "ggg" << i << std::endl;
            switch (column)
            {
                case 1:
                    tempDepartment.setCode(atol(i.c_str())) ;
                    break;
                case 4:
                    tempDepartment.setDescription(i) ;
                    break;
                default:
                    break ;
            }
            column++ ;
        }
        //std::cout << "\n" << tempItm.toStr() ;
        
        deparmentsMap[tempDepartment.getCode()] = tempDepartment ;
        
        //std::cout << "\n" << deparmentsMap[tempDepartment.getCode()].toStr();
        
    }
    std::cout << "\nFinished loading file " + pFileName ;

}

void BaseSystem::readItemArchive( string pFileName )
{
    //http://stackoverflow.com/questions/18365463/how-to-parse-csv-using-boostspirit
    
    // Tokenizer
    typedef boost::tokenizer< boost::escaped_list_separator<char> , std::string::const_iterator, std::string> Tokenizer;
    boost::escaped_list_separator<char> seps('\\', ',', '\"');
    
    string archiveFileName = this->basePath + "ARCHIVES/" + pFileName ;
    
    std::ifstream archiveFile( archiveFileName );
    if (!archiveFile) {
        std::cout << "\nFile " + archiveFileName + " not found" ;
        exit(-1);
    } else {
        std::cout << "\nFile " + archiveFileName + " loaded" ;
    }
    
    std::string line;
    while( std::getline(archiveFile, line) )
    {
        qi::rule<std::string::const_iterator, std::string()> quoted_string = '"' >> *(qi::char_ - '"') >> '"';
        qi::rule<std::string::const_iterator, std::string()> valid_characters = qi::char_ - '"' - ',';
        qi::rule<std::string::const_iterator, std::string()> item = *(quoted_string | valid_characters );
        qi::rule<std::string::const_iterator, std::vector<std::string>()> csv_parser = item % ',';
        
        // Boost Spirit Qi
        
        std::string::const_iterator s_begin = line.begin();
        std::string::const_iterator s_end = line.end();
        std::vector<std::string> result;
        
        bool r = boost::spirit::qi::parse(s_begin, s_end, csv_parser, result);
        assert(r == true);
        assert(s_begin == s_end);
        int column = 0 ;
        Item tempItm ;
        for (auto i : result)
        {
            //std::cout << "ggg" << i << std::endl;
            switch (column)
            {
                case 1:
                    tempItm.setCode(atol(i.c_str())) ;
                    break;
                case 2:
                    tempItm.setDescription(i) ;
                    break;
                case 16:
                    tempItm.setDepartment(deparmentsMap[atol(i.c_str())]);
                    break;
                case 26:
                    tempItm.setPrice(atol(i.c_str())) ;
                    break;
                default:
                    break ;
            }
            column++ ;
        }
        //std::cout << "\n" << tempItm.toStr() ;
        tempItm.setQuantity(0) ;
        itemsMap[tempItm.getCode()] = tempItm ;
        
        //std::cout << "\n" << itemsMap[tempItm.getCode()].toStr();

    }
    std::cout << "\nFinished loading file " + pFileName ;
}

void BaseSystem::readBarcodesArchive( string pFileName )
{
    //http://stackoverflow.com/questions/18365463/how-to-parse-csv-using-boostspirit
    
    // Tokenizer
    typedef boost::tokenizer< boost::escaped_list_separator<char> , std::string::const_iterator, std::string> Tokenizer;
    boost::escaped_list_separator<char> seps('\\', ',', '\"');

    string archiveFileName = this->basePath + "ARCHIVES/" + pFileName ;
    
    std::ifstream archiveFile( archiveFileName );
    if (!archiveFile) {
        std::cout << "\nFile " + archiveFileName + " not found" ;
        exit(-1);
    } else {
        std::cout << "\nFile " + archiveFileName + " loaded" ;
    }
    
    std::string line;
    while( std::getline(archiveFile, line) )
    {
        qi::rule<std::string::const_iterator, std::string()> quoted_string = '"' >> *(qi::char_ - '"') >> '"';
        qi::rule<std::string::const_iterator, std::string()> valid_characters = qi::char_ - '"' - ',';
        qi::rule<std::string::const_iterator, std::string()> item = *(quoted_string | valid_characters );
        qi::rule<std::string::const_iterator, std::vector<std::string>()> csv_parser = item % ',';
        
        // Boost Spirit Qi
        
        std::string::const_iterator s_begin = line.begin();
        std::string::const_iterator s_end = line.end();
        std::vector<std::string> result;
        
        bool r = boost::spirit::qi::parse(s_begin, s_end, csv_parser, result);
        assert(r == true);
        assert(s_begin == s_end);
        int column = 0 ;
        Barcodes tempBarcode ;
        for (auto i : result)
        {
            //std::cout << "ggg" << i << std::endl;
            switch (column)
            {
                case 1:
                    tempBarcode.setCode(atol(i.c_str())) ;
                    break;
                case 2:
                    tempBarcode.setItemCode(atol(i.c_str())) ;
                    break;
            }
            column++ ;
        }
        //std::cout << "\n" << tempBarcode.toStr() ;
        barcodesMap[tempBarcode.getCode()] = tempBarcode ;
        
        //std::cout << "\n" << itemsMap[tempItm.getCode()].toStr();
        
    }
    std::cout << "\nFinished loading file " + pFileName ;
}

void BaseSystem::readArchives()
{
    this->readDepartmentArchive( "DEPARTMENTS.CSV" ) ;
    this->readItemArchive( "ITEMS.CSV" ) ;
    this->readBarcodesArchive( "BARCODES.CSV" ) ;
}

void BaseSystem::salesSession(socket_ptr sock)
{
    //unsigned long initializedCartNumber = 0 ;
    string msg = "" ;
    std::uint64_t cartId = 0 ;
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
                cartId = this->newCart() ;
                msg = "Initialized cart " + std::to_string( cartId ) + "\n" ;
                boost::asio::write(*sock, boost::asio::buffer(msg, sizeof(msg)));
            } else {
                cartId = pt2.get<std::uint64_t> ("cartId");
            }
            
            //std::cout << "\nState: " << cartsMap.find(cartId)->getState() ;
            //myCart->getState() << "\n" ;
            //typedef std::map<unsigned long long, Cart>::iterator = carts;
            typename std::map<unsigned long long, Cart>::iterator it = cartsMap.find(cartId);
            Cart* myCart = nullptr;

            if (it != cartsMap.end()) {
                myCart = &(it->second) ;
            }
            std::cout << "\nState: " << myCart->getState() ;
            //for (carts=cartsMap.begin(); carts!=cartsMap.end(); carts++) {
                    //currentCart = carts->second;
            //}
            
            if (myCart->getState() != CART_NOT_INITIALIZED)
            {
                if (action.compare("save")==0)
                {
                    myCart->persist() ;
                    msg = "Cart #" + std::to_string( cartId ) + " saved\n" ;
                    boost::asio::write(*sock, boost::asio::buffer(msg, sizeof(msg)));
                }
                
                if (action.compare("add")==0)
                {
                    barcode = pt2.get<std::uint64_t> ("barcode");
                    qty = pt2.get<std::int32_t> ("qty");
                    
                    myCart->addItemByBarcode(itemsMap[barcodesMap[barcode].getItemCode()], barcodesMap[barcode], qty) ;
                    msg = "Cart #" + std::to_string( cartId ) + ", added barcode " + std::to_string( barcode ) + "\n" ;
                    boost::asio::write(*sock, boost::asio::buffer(msg, sizeof(msg)));
                    myCart->printCart() ;
                }
                
                if (action.compare("remove")==0)
                {
                    barcode = pt2.get<std::uint64_t> ("barcode");
                    
                    myCart->removeItemByBarcode(itemsMap[barcodesMap[barcode].getItemCode()], barcodesMap[barcode]) ;
                    msg = "Cart #" + std::to_string( cartId ) + ", removed barcode " + std::to_string( barcode ) + "\n" ;
                    boost::asio::write(*sock, boost::asio::buffer(msg, sizeof(msg)));
                    myCart->printCart() ;
                }
                
                if (action.compare("print")==0)
                {
                    myCart->printCart() ;
                    msg = "Cart #" + std::to_string( cartId ) + " printed\n" ;
                    boost::asio::write(*sock, boost::asio::buffer(msg, sizeof(msg)));
                }
                
                if (action.compare("close")==0)
                {
                    boost::asio::write(*sock, boost::asio::buffer(action, sizeof(action)));
                }
            }
            
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception in thread: " << e.what() << "\n";
    }
}

void BaseSystem::salesServer(boost::asio::io_service& io_service, short port)
{
    tcp::acceptor tcpAcceptor(io_service, tcp::endpoint(tcp::v4(), port));
    for (;;)
    {
        socket_ptr sock(new tcp::socket(io_service));
        tcpAcceptor.accept(*sock);
        //void *funcPtr = boost::bind(session, sock) ;
        boost::thread newThread(boost::bind(&BaseSystem::salesSession, this, sock));
    }
}

Item BaseSystem::getItemByIntCode( unsigned long long pIntcode )
{
    return itemsMap[pIntcode];
}

unsigned long BaseSystem::newCart()
{
    unsigned long long thisCartNumber = nextCartNumber ;

    cartsMap.emplace( std::piecewise_construct, std::make_tuple(thisCartNumber), std::make_tuple(this->basePath,thisCartNumber) ) ;
    
    nextCartNumber++;
    return thisCartNumber ;
}

/*Cart* BaseSystem::getCart( unsigned long pCartNumber )
{
    return &cartsMap[pCartNumber] ;
}*/

bool BaseSystem::persistCarts( )
{
    return true ;
}
