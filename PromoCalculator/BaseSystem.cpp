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
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>


#include "BaseSystem.h"
#include "BaseTypes.h"

const int max_length = 256;

static unsigned long nextCartNumber ;

namespace qi = boost::spirit::qi;
namespace fs = boost::filesystem;
qi::rule<std::string::const_iterator, std::string()> quoted_string = '"' >> *(qi::char_ - '"') >> '"';
qi::rule<std::string::const_iterator, std::string()> valid_characters = qi::char_ - '"' - ',';
qi::rule<std::string::const_iterator, std::string()> item = *(quoted_string | valid_characters );
qi::rule<std::string::const_iterator, std::vector<std::string>()> csv_parser = item % ',';

using namespace std;

BaseSystem::BaseSystem( string pBasePath )
{
    boost::asio::io_service io_service;
    std::string line;
    std::string key;
    std::string value;
    unsigned long long currentTmpCartNumber = 0;
    this->basePath = pBasePath ;
    Cart* myCart = nullptr;

    nextCartNumber = 1 ;

    std::ifstream configFile( this->basePath + "PromoCalculator.ini" );
    if (!configFile) {
        std::cout << "No PromoCalculator.ini file found in " << basePath << " directory.\n" ;
        exit(-1);
    } else {
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
        
        if (!fs::exists(this->basePath + "CARTS"))
        {
            std::cout << "No CARTS subfolder found\n" ;
            exit(-1);
        }
        
        if (fs::is_directory(this->basePath + "CARTS"))
        {
            std::cout << "CARTS subfolder found\n" ;
            fs::recursive_directory_iterator it(this->basePath + "CARTS");
            fs::recursive_directory_iterator endit;
            while(it != endit)
            {
                char rAction = ' ' ;
                unsigned long long rCode = 0 ;
                long rQty = 0 ;
                Cart* tmpCart = nullptr ;
                //std::cout << "\nFile: " << it->path().filename() << "\n" ;
                if (fs::is_regular_file(*it) and it->path().extension() == ".transaction_in_progress")
                {
                    //ret.push_back(it->path().filename());
                    currentTmpCartNumber = atol(it->path().stem().c_str()) ;
                    std::cout << "==================================\n" ;
                    std::cout << "File tmpTrans: " << it->path().filename() << " num: " << currentTmpCartNumber << " next: " << nextCartNumber << "\n";
                    
                    newCart( CART_TMPFILE_LOADING ) ;
                    
                    typename std::map<unsigned long long, Cart>::iterator itCarts = cartsMap.find( currentTmpCartNumber );
                    
                    cout << "Dovrei caricare carrello" << currentTmpCartNumber << "\n" ;
                    if (itCarts != cartsMap.end()) {
                        myCart = &(itCarts->second) ;
                        cout << "Carico carrello\n" ;
                    }
                    std::cout << "Cart nr: " << myCart->getNumber() << "\n" ;
                    std::ifstream tmpTransactonFileToLoad( this->basePath + "CARTS/" + it->path().filename().c_str() );
                    
                    while( std::getline(tmpTransactonFileToLoad, line) )
                    {
                        //std::cout << "\n" << line ;
                        std::istringstream is_line(line);
                        std::string::const_iterator s_begin = line.begin();
                        std::string::const_iterator s_end = line.end();
                        std::vector<std::string> result;
                        
                        bool r = boost::spirit::qi::parse(s_begin, s_end, csv_parser, result);
                        assert(r == true);
                        assert(s_begin == s_end);
                        int column = 0 ;
                        for (auto i : result)
                        {
                            switch (column)
                            {
                                case 0:
                                    std::cout << "Action: " << i << "\n" ;
                                    rAction = i[0] ;
                                    break;
                                case 1:
                                    std::cout << "Barcode: " << i << "\n"  ;
                                    rCode = atol(i.c_str()) ;
                                    break;
                                case 2:
                                    std::cout << "Qty: " << i  << "\n" ;
                                    rQty = atol(i.c_str()) ;
                                    break;
                                default:
                                    break ;
                            }
                            column++ ;
                        }
                        switch (rAction)
                        {
                            case 'A':
                                myCart->addItemByBarcode(itemsMap[barcodesMap[rCode].getItemCode()], barcodesMap[rCode], rQty) ;
                                break;
                            case 'R':
                                myCart->removeItemByBarcode(itemsMap[barcodesMap[rCode].getItemCode()], barcodesMap[rCode]) ;
                                break;
                            default:
                                std::cout << "Row action not recognized" ;
                                break;
                        }
                    }
                    myCart->setState(CART_STATE_READY_FOR_ITEM) ;
                    myCart->printCart() ;
                    
                    tmpTransactonFileToLoad.close();
                    if (currentTmpCartNumber >= nextCartNumber)
                    {
                        nextCartNumber = currentTmpCartNumber + 1 ;
                    }
                }
                ++it;
            }
        }
        std::cout << "System initialized.\n" ;
    }
    
    try
    {
        std::cout << "Starting server\n" ;
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
    
    std::cout << "Config print start\n" ;
    for(configurationRows iterator = configurationMap.begin(); iterator != configurationMap.end(); iterator++) {
        std::cout << "key: " << iterator->first << ", value: " << iterator->second << "\n";
    }
    std::cout << "Config print end\n" ;
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
        std::cout << "File " + archiveFileName + " not found\n" ;
        exit(-1);
    } else {
        std::cout << "File " + archiveFileName + " found\n" ;
    }
    
    std::string line;
    while( std::getline(archiveFile, line) )
    {
        
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
    std::cout << "Finished loading file " + pFileName + "\n" ;

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
        std::cout << "File " + archiveFileName + " not found\n" ;
        exit(-1);
    } else {
        std::cout << "File " + archiveFileName + " loaded\n" ;
    }
    
    std::string line;
    while( std::getline(archiveFile, line) )
    {
        
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
    std::cout << "Finished loading file " + pFileName + "\n" ;
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
        std::cout << "File " + archiveFileName + " not found\n" ;
        exit(-1);
    } else {
        std::cout << "File " + archiveFileName + " loaded\n" ;
    }
    
    std::string line;
    while( std::getline(archiveFile, line) )
    {
 
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
    std::cout << "Finished loading file " + pFileName + "\n" ;
}

void BaseSystem::readArchives()
{
    this->readDepartmentArchive( "DEPARTMENTS.CSV" ) ;
    this->readItemArchive( "ITEMS.CSV" ) ;
    this->readBarcodesArchive( "BARCODES.CSV" ) ;
}

void BaseSystem::sendRespMsg(socket_ptr pSock, string pMsg)
{
    boost::asio::write(*pSock, boost::asio::buffer(pMsg, pMsg.size()));
    std::cout << "pMsg: " << pMsg << ", size: " << pMsg.size() << "\n" ;
}

void BaseSystem::salesSession(socket_ptr pSock)
{
    string msg = "" ;
    std::uint64_t cartId = 0 ;
    std::uint64_t barcode = 0 ;
    std::int32_t qty = 0 ;
    char data[max_length];
    
    try
    {
        for (;;)
        {
            memset(data,0,max_length);

            boost::system::error_code error;
            size_t length = pSock->read_some(boost::asio::buffer(data), error);
            if (error == boost::asio::error::eof)
            {
                std::cout << "Connection reset by peer\n" ;
                break; // Connection closed cleanly by peer.
            } else if (error)
            {
                std::cout << "Oh my God! There's been a bad fault on socket!\n" ;
                throw boost::system::system_error(error); // Some other error.
            }
            ptree pt2;
            std::istringstream is (data);

            read_json (is, pt2);
            
            std::string action = pt2.get<std::string> ("action");
            if (action.compare("init")==0)
            {
                cartId = this->newCart( GEN_CART_NEW ) ;
                sendRespMsg(pSock, "{\"cartId\":" + std::to_string( cartId ) + "}\n") ;
            } else {
                cartId = pt2.get<std::uint64_t> ("cartId");
            }
            
            typename std::map<unsigned long long, Cart>::iterator it = cartsMap.find(cartId);
            Cart* myCart = nullptr;

            if (it != cartsMap.end()) {
                myCart = &(it->second) ;
            }
            
            if ( (myCart != nullptr) && (myCart->getState() != CART_NOT_INITIALIZED) && (myCart->getState() != CART_STATE_CLOSED) )
            {
                if (action.compare("save")==0)
                {
                    myCart->persist() ;
                    sendRespMsg(pSock, "{\"cartId\":" + std::to_string( cartId ) + ",\"action\":\"save\",\"status\":\"ok\"}\n") ;
                }
                
                if (action.compare("add")==0)
                {
                    barcode = pt2.get<std::uint64_t> ("barcode");
                    qty = pt2.get<std::int32_t> ("qty");
                    
                    myCart->addItemByBarcode(itemsMap[barcodesMap[barcode].getItemCode()], barcodesMap[barcode], qty) ;

                    sendRespMsg(pSock, "{\"cartId\":" + std::to_string( cartId ) + ",\"action\":\"barcodeAdd\",\"status\":\"ok\",\"barcode\":" + std::to_string( barcode ) + "}\n") ;

                    myCart->printCart() ;
                }
                //std::cout << "\noooo1y\n" ;
                if (action.compare("remove")==0)
                {
                    barcode = pt2.get<std::uint64_t> ("barcode");
                    myCart->removeItemByBarcode(itemsMap[barcodesMap[barcode].getItemCode()], barcodesMap[barcode]) ;
                    
                    sendRespMsg(pSock, "{\"cartId\":" + std::to_string( cartId ) + ",\"action\":\"barcodeRemove\",\"status\":\"ok\",\"barcode\":" + std::to_string( barcode ) + "}\n") ;
                    
                    myCart->printCart() ;
                }
                
                if (action.compare("print")==0)
                {
                    myCart->printCart() ;
                    sendRespMsg(pSock, "{\"cartId\":" + std::to_string( cartId ) + ",\"action\":\"print\",\"status\":\"ok\"}\n") ;
                }
                
                if (action.compare("close")==0)
                {
                    myCart->close() ;
                    sendRespMsg(pSock, "{\"cartId\":" + std::to_string( cartId ) + ",\"action\":\"close\",\"status\":\"ok\"}\n") ;

                }
            }
            else
            {
                std::cout << "Invalid cart state\n" ;
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

unsigned long BaseSystem::newCart( unsigned int pAction )
{
    unsigned long long thisCartNumber = nextCartNumber ;

    cartsMap.emplace( std::piecewise_construct, std::make_tuple(thisCartNumber), std::make_tuple(this->basePath,thisCartNumber,pAction) ) ;
    
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
