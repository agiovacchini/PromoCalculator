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

using boost::asio::ip::tcp;
using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;


class BaseSystem {
    string basePath ;
    string iniFileName ;
    unsigned long nodeId ;
    src::severity_logger_mt< boost::log::trivial::severity_level > my_logger_bs;
    bool baseSystemRunning ;
    
public:
    std::map <string, string> configurationMap ;
    std::map <unsigned long long, Department> deparmentsMap ;
    std::map <unsigned long long, Item> itemsMap ;
    std::map <unsigned long long, Barcodes> barcodesMap ;
    std::map <unsigned long long, Cart> cartsMap ;
    std::map <unsigned long long, unsigned long long> allLoyCardsMap ;
    
    typedef boost::shared_ptr<tcp::socket> socket_ptr;
	std::map<unsigned long long, Cart>::iterator mainIterator ;

    BaseSystem( string pBasePath, string pIniFileName ) ;
    string getBasePath() const ;
    void setBasePath( string pBasePath ) ;
    
    long loadConfiguration() ;
    long setConfigValue(string confMapKey, string treeSearchKey, boost::property_tree::ptree* configTree) ;
    void printConfiguration() ;
    string getConfigValue( string pParamName ) ;
    void readDepartmentArchive( string pFileName ) ;
    void dumpDepartmentArchive( string pFileName ) ;
    void readItemArchive( string pFileName ) ;
    void dumpItemArchive( string pFileName ) ;
    void readBarcodesArchive( string pFileName ) ;
    void dumpBarcodesArchive( string pFileName ) ;
    void readArchives( ) ;
    void dumpArchivesFromMemory( ) ;
    void loadCartsInProgress( ) ;
    string getCartsList( ) ;
    void checkForVariationFiles( ) ;
    ItemCodePrice decodeBarcode(unsigned long long rCode);
    //void salesServer(boost::asio::io_service& io_service, short port) ;
    //void salesSession(socket_ptr sock) ;
    string salesActionsFromWebInterface(int pAction, std::map<std::string, std::string> pUrlParamsMap);

    void sendRespMsg( socket_ptr pSock, string pMsg ) ;
    unsigned long newCart( unsigned int pAction ) ;
    Cart* getCart( unsigned long pCartNumber ) ;
    bool persistCarts( ) ;
    
    Item getItemByIntCode( unsigned long long pIntcode ) ;
    std::string fromLongToStringWithDecimals( unsigned long long pValue ) ;
};


#endif /* defined(__PromoCalculator__BaseSystem__) */