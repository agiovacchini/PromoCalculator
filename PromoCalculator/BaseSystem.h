//
//  BaseSystem.h
//  PromoCalculator
//
//  Created by Andrea Giovacchini on 08/01/14.
//  Copyright (c) 2014 Andrea Giovacchini. All rights reserved.
//

#ifndef __PromoCalculator__BaseSystem__
#define __PromoCalculator__BaseSystem__

#include "BaseTypes.h"
#include <boost/asio.hpp>
#include <cstdlib>
//#include <thread>
#include <iostream>
#include <string>
#include <sstream>

#include <boost/thread/thread.hpp>
//#include <boost/bind.hpp>
//#include <boost/smart_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "Department.h"
#include "Item.h"
#include "Barcodes.h"
#include "Cart.h"


#define BOOST_ALL_DYN_LINK
using boost::asio::ip::tcp;
using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;


class BaseSystem {
    string basePath ;
    unsigned long nodeId ;
public:
    std::map <string, string> configurationMap ;
    std::map <unsigned long long, Department> deparmentsMap ;
    std::map <unsigned long long, Item> itemsMap ;
    std::map <unsigned long long, Barcodes> barcodesMap ;
    std::map <unsigned long long, Cart> cartsMap ;
    typedef boost::shared_ptr<tcp::socket> socket_ptr;
	std::map<unsigned long long, Cart>::iterator mainIterator ;

    BaseSystem( string pBasePath ) ;
    string getBasePath() const ;
    void setBasePath( string pBasePath ) ;
    
    int loadConfiguration() ;
    int setConfigValue(string confMapKey, string treeSearchKey, boost::property_tree::ptree* configTree) ;
    void printConfiguration() ;
    void readDepartmentArchive( string pFileName ) ;
    void readItemArchive( string pFileName ) ;
    void readBarcodesArchive( string pFileName ) ;
    void readArchives( ) ;
    void loadCartsInProgress( ) ;
    void salesServer(boost::asio::io_service& io_service, short port) ;
    void salesSession(socket_ptr sock) ;
    void sendRespMsg( socket_ptr pSock, string pMsg ) ;
    unsigned long newCart( unsigned int pAction ) ;
    Cart* getCart( unsigned long pCartNumber ) ;
    bool persistCarts( ) ;
    int checkBarcodeType( unsigned long long  pBarcode ) ;
    
    Item getItemByIntCode( unsigned long long pIntcode ) ;
};


#endif /* defined(__PromoCalculator__BaseSystem__) */