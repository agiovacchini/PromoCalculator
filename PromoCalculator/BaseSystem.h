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
#include <iostream>
#include <string>
#include <sstream>

#include <boost/thread/thread.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/regex.hpp>

#include "Department.h"
#include "Item.h"
#include "Barcodes.h"
#include "Cart.h"
#include "base/ArchiveMap.h"

using boost::asio::ip::tcp;
using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;


class BaseSystem {
    string basePath ;
    string iniFileName ;
    uint32_t nodeId ;
    src::severity_logger_mt< boost::log::trivial::severity_level > my_logger_bs;
    bool baseSystemRunning ;
    bool dummyRCS ;
    bool cartsPriceChangesWhileShopping ;
    bool mainReturnSeparateLinkedBarcode;
    std::map <string, string> configurationMap ;
    
    
    ArchiveMap<Department> departmentsMap ;
    ArchiveMap<Item> itemsMap ;
    ArchiveMap<Barcodes> barcodesMap ;
    
    std::map <uint64_t, Cart> cartsMap ;
    std::map <uint64_t, uint64_t> allLoyCardsMap ;
    boost::regex ean13 ;
    boost::regex ean13PriceReq ;
    boost::regex upc ;
    boost::regex ean8 ;
    boost::regex loyCard ;
    boost::regex loyCardNoCheck ;
    std::string varFolderName ;
    std::string cartFolderName ;
    uint32_t varCheckDelaySeconds ;


public:
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
    ItemCodePrice decodeBarcode(uint64_t rCode);
    int64_t getItemPrice(Item* pItem, uint64_t pBarcode, unsigned int pBCodeType);
    string salesActionsFromWebInterface(int pAction, std::map<std::string, std::string> pUrlParamsMap);

    uint32_t newCart( unsigned int pAction ) ;
    Cart* getCart( uint32_t pCartNumber ) ;
    bool persistCarts( ) ;
    
    Item getItemByIntCode( uint64_t pIntcode ) ;
    std::string fromLongToStringWithDecimals( int64_t pValue ) ;
};


#endif /* defined(__PromoCalculator__BaseSystem__) */