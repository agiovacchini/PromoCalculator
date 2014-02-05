//
//  BaseSystem.cpp
//  PromoCalculator
//
//  Created by Andrea Giovacchini on 08/01/14.
//  Copyright (c) 2014 Andrea Giovacchini. All rights reserved.
//

#include "BaseTypes.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>
#include <stdlib.h>

#include <boost/spirit/include/qi.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>

#include "reply.hpp"
#include "request.hpp"
#include "request_parser.hpp"
#include "request_handler.hpp"


#include "BaseSystem.h"

const int max_length = 8192;
int bCodeType = 0 ;
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
    using namespace logging::trivial;
 
    boost::asio::io_service io_service;
    
    this->basePath = pBasePath ;
    
    if ( this->loadConfiguration() == 0 )
    {
        this->printConfiguration() ;
        
        this->readArchives() ;
        
        this->loadCartsInProgress() ;
        
        BOOST_LOG_SEV(my_logger_bs, lt::info) << "System initialized." ;
        //}
        
        /*try
        {
            BOOST_LOG_SEV(my_logger_bs, lt::info) << "Server starting...";
            
            using namespace std;
            salesServer(io_service, atol(configurationMap["NetworkPort"].c_str()));
        }
        catch (std::exception& e)
        {
            BOOST_LOG_SEV(my_logger_bs, fatal) << e.what() ;
        }*/
    } else {
        BOOST_LOG_SEV(my_logger_bs, fatal) << "Bad configuration error, aborting start" ;
    }
}

string BaseSystem::getConfigValue( string pParamName )
{
    return configurationMap[pParamName] ;
}

string BaseSystem::getBasePath() const
{
    return this->basePath ;
}

void BaseSystem::setBasePath( string pBasePath )
{
    this->basePath = pBasePath ;
}

int BaseSystem::loadConfiguration()
{
    BOOST_LOG_SEV(my_logger_bs, lt::info) << "Config load start" ;
    boost::property_tree::ptree pt;
    int rc = 0 ;
    
    try {
        boost::property_tree::ini_parser::read_ini(this->basePath + "PromoCalculator.ini", pt);
        rc = rc + setConfigValue("NetworkPort", "Network.Port", &pt );
        rc = rc + setConfigValue("SelfScanScanInDir", "SelfScan.ScanInDir", &pt );
        rc = rc + setConfigValue("SelfScanScanOutDir", "SelfScan.ScanOutDir", &pt );
        rc = rc + setConfigValue("LoyCardPrefix", "Loy.CardPrefix", &pt );
        rc = rc + setConfigValue("LoyMaxCardsPerTransaction", "Loy.MaxCardsPerTransaction", &pt );
        rc = rc + setConfigValue("BarcodesType01", "Barcodes.Type", &pt );
        rc = rc + setConfigValue("NodeId", "Node.Id", &pt );
        rc = rc + setConfigValue("WebAddress", "Web.Address", &pt );
        rc = rc + setConfigValue("WebPort", "Web.Port", &pt );
        rc = rc + setConfigValue("WebThreads", "Web.Threads", &pt );
        
        this->nodeId = pt.get<std::uint32_t>("Node.Id") ;
    }
    catch (std::exception const& e)
    {
        BOOST_LOG_SEV(my_logger_bs, lt::fatal) << "Config error: " << e.what() ;
        return 1 ;
    }
    BOOST_LOG_SEV(my_logger_bs, lt::info) << "Config load end" ;
    return rc ;
}

int BaseSystem::setConfigValue(string confMapKey, string treeSearchKey, boost::property_tree::ptree* configTree)
{
    configurationMap[confMapKey] = configTree->get<std::string>(treeSearchKey) ;
    return 0 ;
}

void BaseSystem::printConfiguration()
{
    typedef std::map<string, string>::iterator configurationRows;
    
    BOOST_LOG_SEV(my_logger_bs, lt::info) << "Config print start" ;
    BOOST_LOG_SEV(my_logger_bs, lt::info) << "\tNode Id: " << this->nodeId ;
    for(configurationRows iterator = configurationMap.begin(); iterator != configurationMap.end(); iterator++) {
        BOOST_LOG_SEV(my_logger_bs, lt::info) << "\tkey: " << iterator->first << ", value: " << iterator->second ;
    }
    BOOST_LOG_SEV(my_logger_bs, lt::info) << "Config print end" ;
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
        BOOST_LOG_SEV(my_logger_bs, lt::info) << "File " + archiveFileName + " not found" ;
        exit(-1);
    } else {
        BOOST_LOG_SEV(my_logger_bs, lt::info) << "File " + archiveFileName + " found" ;
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
            //BOOST_LOG_SEV(my_logger_bs, lt::info) << "ggg" << i;
            switch (column)
            {
                case 1:
                    tempDepartment.setCode(strtoull(i.c_str(),nullptr,10)) ;
                    break;
                case 4:
                    tempDepartment.setDescription(i) ;
                    break;
                default:
                    break ;
            }
            column++ ;
        }
        //BOOST_LOG_SEV(my_logger_bs, lt::info) << "\n" << tempItm.toStr() ;
        
        deparmentsMap[tempDepartment.getCode()] = tempDepartment ;
        
        //BOOST_LOG_SEV(my_logger_bs, lt::info) << "\n" << deparmentsMap[tempDepartment.getCode()].toStr();
        
    }
    BOOST_LOG_SEV(my_logger_bs, lt::info) << "Finished loading file " << pFileName ;
    
}

void BaseSystem::readItemArchive( string pFileName )
{
    //http://stackoverflow.com/questions/18365463/how-to-parse-csv-using-boostspirit
    
    // Tokenizer
    typedef boost::tokenizer< boost::escaped_list_separator<char> , std::string::const_iterator, std::string> Tokenizer;
    boost::escaped_list_separator<char> seps('\\', ',', '\"');
    
    string archiveFileName = this->basePath + "ARCHIVES/" + pFileName ;
	string tmp = "" ;
    
    std::ifstream archiveFile( archiveFileName );
    if (!archiveFile) {
        BOOST_LOG_SEV(my_logger_bs, lt::info) << "File " + archiveFileName + " not found" ;
        exit(-1);
    } else {
        BOOST_LOG_SEV(my_logger_bs, lt::info) << "File " + archiveFileName + " found" ;
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
            //BOOST_LOG_SEV(my_logger_bs, lt::info) << "ggg" << i << std::endl;
            switch (column)
            {
                case 1:
					tmp = std::string(i);
					tempItm.setCode( atoll(tmp.c_str()) );
                    break;
                case 2:
                    tempItm.setDescription(i) ;
                    break;
                case 16:
                    tempItm.setDepartment(deparmentsMap[strtoull(i.c_str(),nullptr,10)]);
                    break;
                case 26:
                    tempItm.setPrice(strtoull(i.c_str(),nullptr,10)) ;
                    break;
                default:
                    break ;
            }
            column++ ;
        }
        //BOOST_LOG_SEV(my_logger_bs, lt::info) << "\n" << tempItm.toStr() ;
        tempItm.setQuantity(0) ;
        itemsMap[tempItm.getCode()] = tempItm ;
        
        //BOOST_LOG_SEV(my_logger_bs, lt::info) << "\n" << itemsMap[tempItm.getCode()].toStr();
        
    }
    BOOST_LOG_SEV(my_logger_bs, lt::info) << "Finished loading file " << pFileName ;
}

void BaseSystem::readBarcodesArchive( string pFileName )
{
    //http://stackoverflow.com/questions/18365463/how-to-parse-csv-using-boostspirit
    
    // Tokenizer
    typedef boost::tokenizer< boost::escaped_list_separator<char> , std::string::const_iterator, std::string> Tokenizer;
    boost::escaped_list_separator<char> seps('\\', ',', '\"');
    
    string archiveFileName = this->basePath + "ARCHIVES/" + pFileName ;
	string tmp = "";
	std::stringstream tempStringStream ;
	
    std::ifstream archiveFile( archiveFileName );
    if (!archiveFile) {
        BOOST_LOG_SEV(my_logger_bs, lt::info) << "File " + archiveFileName + " not found" ;
        exit(-1);
    } else {
        BOOST_LOG_SEV(my_logger_bs, lt::info) << "File " + archiveFileName + " loaded" ;
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
        
        
        unsigned long long tmpBcd = 0, bcdWrk = 0;
        Barcodes tempBarcode ;
        for (auto i : result)
        {
            
            switch (column)
            {
                case 1:
					tmp = std::string(i);
                    bcdWrk = atoll(tmp.c_str()) ;
                    bCodeType = checkBarcodeType( bcdWrk ) ;
                    if (bcdWrk > 0 ) {
                        if (bCodeType == BCODE_EAN13_PRICE_REQ)
                        {
                            //strcpy( tmp, std::to_string(bcdWrk).c_str() );
                            tempStringStream.str( std::string() ) ;
							tempStringStream.clear() ;
							tempStringStream << bcdWrk ;
							tmp = tempStringStream.str().substr(0,7) + "000000" ;
                            tmpBcd = atoll(tmp.c_str()) ;
                        } else {
                            tmpBcd = bcdWrk ;
                        }
                        //BOOST_LOG_SEV(my_logger_bs, lt::info) << "(" << tmpBcd << ") - " ;
                        tempBarcode.setCode( tmpBcd ) ;
                    }
                    //BOOST_LOG_SEV(my_logger_bs, lt::info) << i.c_str() << "\n";
                    break;
                case 2:
                    tempBarcode.setItemCode(atoll(i.c_str())) ;
                    break;
            }
            column++ ;
        }
        //BOOST_LOG_SEV(my_logger_bs, lt::info) << "Barcode.getcode: " << tempBarcode.getCode() << " type: " << bCodeType << "\n" ;
        barcodesMap[tempBarcode.getCode()] = tempBarcode ;
        //barcodesMap.emplace( std::piecewise_construct, std::make_tuple(tempBarcode.getCode()), std::make_tuple(tempBarcode) ) ;
		//BOOST_LOG_SEV(my_logger_bs, lt::info) << "toStr: " << barcodesMap[tempBarcode.getCode()].toStr() << "\n" ;
        
    }
    BOOST_LOG_SEV(my_logger_bs, lt::info) << "Finished loading file " << pFileName ;
}

void BaseSystem::readArchives()
{
    this->readDepartmentArchive( "DEPARTMENTS.CSV" ) ;
    this->readItemArchive( "ITEMS.CSV" ) ;
    this->readBarcodesArchive( "BARCODES.CSV" ) ;
}

void BaseSystem::loadCartsInProgress()
{
    std::string line;
    std::string key;
    std::string value;
    std::map<unsigned long long, Cart>::iterator itCarts ;
    unsigned long long currentTmpCartNumber = 0, nextCartNumberTmp = 0 ;
    char rAction = ' ' ;
    unsigned long long rCode = 0 ;
    long rQty = 0 ;
    bool r = false ;
    int column = 0 ;
    
    Cart* myCart = nullptr;
    
    nextCartNumber = 1 ;
    
    if (!fs::exists(this->basePath + "CARTS"))
    {
        BOOST_LOG_SEV(my_logger_bs, lt::info) << "No CARTS subfolder found" ;
        exit(-1);
    }
    
    if (fs::is_directory(this->basePath + "CARTS"))
    {
        BOOST_LOG_SEV(my_logger_bs, lt::info) << "CARTS subfolder found" ;
        fs::recursive_directory_iterator it(this->basePath + "CARTS");
        fs::recursive_directory_iterator endit;
        while(it != endit)
        {
            rAction = ' ' ;
            rCode = 0 ;
            rQty = 0 ;
            //Cart* tmpCart = nullptr ;
            //BOOST_LOG_SEV(my_logger_bs, lt::info) << "\nFile: " << it->path().filename() << "\n" ;
            if (fs::is_regular_file(*it) && it->path().extension() == ".transaction_in_progress")
            {
                //ret.push_back(it->path().filename());
                currentTmpCartNumber = atol(it->path().stem().string().c_str()) ;
                
                //nextCartNumber is saved and then restored to avoid problems with the max cart number when leaving this function in case of sorting problems of filenames from filesystem
                nextCartNumberTmp = nextCartNumber ;
                nextCartNumber = currentTmpCartNumber ;
                BOOST_LOG_SEV(my_logger_bs, lt::info) << "==================================" ;
                BOOST_LOG_SEV(my_logger_bs, lt::info) << "File tmpTrans: " << it->path().filename() << " num: " << currentTmpCartNumber << " next: " << nextCartNumber ;
                
                newCart( GEN_CART_LOAD ) ;
                
                nextCartNumber = nextCartNumberTmp ;
                
                itCarts = cartsMap.find( currentTmpCartNumber );
                
                //cout << "Dovrei caricare carrello" << currentTmpCartNumber << "\n" ;
                if (itCarts != cartsMap.end()) {
                    myCart = &(itCarts->second) ;
                    //cout << "Carico carrello\n" ;
                }
                //BOOST_LOG_SEV(my_logger_bs, lt::info) << "Cart nr: " << myCart->getNumber() << "\n" ;
                std::ifstream tmpTransactonFileToLoad( this->basePath + "CARTS/" + it->path().filename().string() );
                
                while( std::getline(tmpTransactonFileToLoad, line) )
                {
                    //BOOST_LOG_SEV(my_logger_bs, lt::info) << "\n" << line ;
                    std::istringstream is_line(line);
                    std::string::const_iterator s_begin = line.begin();
                    std::string::const_iterator s_end = line.end();
                    std::vector<std::string> result;
                    
                    r = boost::spirit::qi::parse(s_begin, s_end, csv_parser, result);
                    assert(r == true);
                    assert(s_begin == s_end);
                    column = 0 ;
                    for (auto i : result)
                    {
                        switch (column)
                        {
                            case 0:
                                //timeStamp
                                break;
                            case 1:
                                //BOOST_LOG_SEV(my_logger_bs, lt::info) << "Action: " << i << "\n" ;
                                rAction = i[0] ;
                                break;
                            case 2:
                                //BOOST_LOG_SEV(my_logger_bs, lt::info) << "Barcode: " << i << "\n"  ;
                                rCode = atoll(i.c_str()) ;
                                break;
                            case 3:
                                //BOOST_LOG_SEV(my_logger_bs, lt::info) << "Qty: " << i  << "\n" ;
                                rQty = atoll(i.c_str()) ;
                                break;
                            default:
                                break ;
                        }
                        column++ ;
                    }
                    
                    BOOST_LOG_SEV(my_logger_bs, lt::info) << "Debug recupero riga carrello, rcode: " << rCode << ", barcode: " << barcodesMap[rCode].toStr() ;
                    
                    switch (rAction)
                    {
                            
                        case 'A':
                            myCart->addItemByBarcode(itemsMap[barcodesMap[rCode].getItemCode()], barcodesMap[rCode], rQty) ;
                            break;
                        case 'R':
                            myCart->removeItemByBarcode(itemsMap[barcodesMap[rCode].getItemCode()], barcodesMap[rCode]) ;
                            break;
                        default:
                            BOOST_LOG_SEV(my_logger_bs, lt::info) << "Row action not recognized" ;
                            break;
                    }
                }
                
                myCart->setState(CART_STATE_READY_FOR_ITEM) ;
                //myCart->printCart() ;
                
                tmpTransactonFileToLoad.close();
                if (currentTmpCartNumber >= nextCartNumber)
                {
                    nextCartNumber = currentTmpCartNumber + 1 ;
                }
            }
            ++it;
        }
    }
}

int BaseSystem::checkBarcodeType( unsigned long long pBarcode )
{
	using namespace boost;
    //int type = 0 ;
    regex ean13( "\\d{13}" ) ;
	regex ean13PriceReq( "2\\d{12}" ) ;
	regex upc( "\\d{12}" ) ;
	regex ean8( "\\d{8}" ) ;
	regex loyCard( "260\\d{9}") ;
	std::stringstream tempStringStream ;
    
    tempStringStream.str( std::string() ) ;
	tempStringStream.clear() ;
	tempStringStream << pBarcode ;
	//BOOST_LOG_SEV(my_logger_bs, lt::info) << "Barcode: ---" << tempStringStream.str() << "---\n" ;
    if (regex_match( tempStringStream.str(), loyCard ) )
    {
        //BOOST_LOG_SEV(my_logger_bs, lt::info) << "Barcode type: Loyalty Card\n" ;
        return BCODE_LOYCARD ;
    }
    
    if (regex_match( tempStringStream.str(), ean13 ) )
    {
        if (regex_match( tempStringStream.str(), ean13PriceReq ) )
        {
            //BOOST_LOG_SEV(my_logger_bs, lt::info) << "Barcode type: EAN13 with price\n" ;
            return BCODE_EAN13_PRICE_REQ ;
        } else {
            //BOOST_LOG_SEV(my_logger_bs, lt::info) << "Barcode type: EAN13\n" ;
            return BCODE_EAN13 ;
        }
    }
    
    if (regex_match( tempStringStream.str(), upc ) )
    {
        //BOOST_LOG_SEV(my_logger_bs, lt::info) << "Barcode type: UPC\n" ;
        return BCODE_UPC ;
    }
    
    if (regex_match( tempStringStream.str(), ean8 ) )
    {
        //BOOST_LOG_SEV(my_logger_bs, lt::info) << "Barcode type: EAN8\n" ;
        return BCODE_EAN8 ;
    }
    
    
    
    //BOOST_LOG_SEV(my_logger_bs, lt::info) << "Barcode type: not recognized\n" ;
    return BCODE_NOT_RECOGNIZED ;
}

void BaseSystem::sendRespMsg(socket_ptr pSock, string pMsg)
{
    boost::asio::write(*pSock, boost::asio::buffer(pMsg, pMsg.size()));
    //std::cout << "pMsg: " << pMsg ;
    BOOST_LOG_SEV(my_logger_bs, lt::info) << "pMsg: " << pMsg << ", size: " << pMsg.size() ;
}

string BaseSystem::salesActionsFromWebInterface(int pAction, std::map<std::string, std::string> pUrlParamsMap)
{
    std::uint64_t cartId = 0 ;
    std::string resp = "Ciao" ;
    std::ostringstream streamCartId ;
    Cart* myCart = nullptr;
    int rc = 0 ;
    
    if (pAction==WEBI_SESSION_INIT)
    {
        cartId = this->newCart( GEN_CART_NEW ) ;
        resp = "InitResp" ;
        std::cout << endl << "InitResp - Che figata" << endl ;
    } else {
        
        streamCartId.str( std::string() ) ;
        streamCartId.clear() ;
        streamCartId << pUrlParamsMap["devSessId"] ;
        std::string strCartId = streamCartId.str() ;
        mainIterator = cartsMap.find(atoll(strCartId.c_str()));
        unsigned long posNumber = 0 ;
        std::cout << endl << "InitResp - pos: " << pUrlParamsMap["payStationID"] << " sess: " << strCartId << endl ;
        if (mainIterator != cartsMap.end()) {
            myCart = &(mainIterator->second);
            
            if (pAction==WEBI_SESSION_END)
            {
                rc = myCart->sendToPos(atol(pUrlParamsMap["payStationID"].c_str()), this->configurationMap["SelfScanScanInDir"]);
                std::cout << endl << "InitResp - Che figata - rc:" << rc << endl ;
            }
            
        }
    }
    


    return resp ;

}


void BaseSystem::salesSession(socket_ptr pSock)
{
    string msg = "" ;
    std::uint64_t cartId = 0 ;
    std::uint64_t barcode = 0 ;
    std::uint64_t barcodeWrk = 0 ;
    std::int32_t qty = 0 ;
	std::ostringstream tempStringStream ;
	std::ostringstream streamCartId ;
	
    string tmp = "" ;
    
    int rc = 0 ;
    unsigned long requestId = 0 ;
    char data[max_length];
    
    try
    {
        for (;;)
        {
            memset(data,0,max_length);
            tempStringStream.str( std::string() ) ;
			tempStringStream.clear() ;
			
            boost::system::error_code error;
            size_t length = pSock->read_some(boost::asio::buffer(data), error);

            
            if (error == boost::asio::error::eof)
            {
                BOOST_LOG_SEV(my_logger_bs, lt::info) << "Connection reset by peer" ;
                break; // Connection closed cleanly by peer.
            } else if (error)
            {
                BOOST_LOG_SEV(my_logger_bs, lt::info) << "Oh my God! There's been a bad fault on socket!" ;
                throw boost::system::system_error(error); // Some other error.
            }
            ptree pt2;
            std::istringstream is (data);
            
            try {
                read_json (is, pt2);
            }
            catch (std::exception const& e)
            {
				BOOST_LOG_SEV(my_logger_bs, lt::error) << "Sales JSON parse error: " << e.what();
            }
            try {
                std::string action = pt2.get<std::string> ("action");
                if (action.compare("init")==0)
                {
                    cartId = this->newCart( GEN_CART_NEW ) ;
                    tempStringStream.str( std::string() ) ;
                    tempStringStream.clear() ;
                    tempStringStream << "{\"cartId\":" << cartId << ",\"rc\":" << rc << "}" ;
                    sendRespMsg(pSock, tempStringStream.str() ) ;
                } else {
                    cartId = pt2.get<std::uint64_t> ("cartId");
                }
                
                mainIterator = cartsMap.find(cartId);
                Cart* myCart = nullptr;
                streamCartId.str( std::string() ) ;
                streamCartId.clear() ;
                streamCartId << cartId ;
                std::string strCartId = streamCartId.str() ;
                
                unsigned long posNumber = 0 ;
                
                if (mainIterator != cartsMap.end()) {
                    myCart = &(mainIterator->second);
                }
                
                if ( (myCart != nullptr) && (myCart->getState() != CART_NOT_INITIALIZED) && (myCart->getState() != CART_STATE_CLOSED) )
                {
                    requestId = myCart->getNextRequestId() ;
                    if (action.compare("save")==0)
                    {
                        rc = myCart->persist() ;
                        tempStringStream.str( std::string() ) ;
                        tempStringStream.clear() ;
                        tempStringStream << "{\"cartId\":" << strCartId << ",\"rc\":" << rc << ",\"requestId\":" << requestId << ",\"action\":\"save\",\"status\":\"ok\"}" << endl ;
                        sendRespMsg(pSock, tempStringStream.str() );
                    }
                    
                    if (action.compare("sendToPos")==0)
                    {
                        posNumber = pt2.get<std::uint32_t> ("posNumber");
                        rc = myCart->sendToPos(posNumber, this->configurationMap["SelfScanScanInDir"]);
                        tempStringStream.str( std::string() ) ;
                        tempStringStream.clear() ;
                        tempStringStream << "{\"cartId\":" << strCartId << ",\"posNumber\":" << posNumber  << ",\"rc\":" << rc << ",\"requestId\":" << requestId << ",\"action\":\"sendToPos\",\"status\":\"ok\"}" << endl ;
                        sendRespMsg(pSock, tempStringStream.str() );
                    }
                    
                    if (action.compare("add")==0)
                    {
                        barcode = pt2.get<std::uint64_t> ("barcode");
                        qty = pt2.get<std::int32_t> ("qty");
                        
                        bCodeType = checkBarcodeType( barcode ) ;
                        
                        if ( ( bCodeType != BCODE_NOT_RECOGNIZED ) )
                        {
                            if (bCodeType != BCODE_LOYCARD)
                            {
                                
                                if (bCodeType == BCODE_EAN13_PRICE_REQ)
                                {
                                    tempStringStream.str( std::string() ) ;
                                    tempStringStream.clear() ;
                                    tempStringStream << barcode ;
                                    tmp = tempStringStream.str().substr(0,7) + "000000" ;
                                    barcodeWrk = atoll(tmp.c_str()) ;
                                } else {
                                    barcodeWrk = barcode ;
                                }
                                
                                BOOST_LOG_SEV(my_logger_bs, lt::info) << "barcodeWrk: " << barcodeWrk ;
								try {
									unsigned long long tItemCode = barcodesMap[barcodeWrk].getItemCode();
									map < unsigned long long, Item>::iterator it = itemsMap.find(tItemCode);
									if (it!=itemsMap.end())
									{
										rc = myCart->addItemByBarcode(itemsMap[barcodesMap[barcodeWrk].getItemCode()], barcodesMap[barcodeWrk], qty) ;
									}
									else {
										rc = BCODE_ITEM_NOT_FOUND;
									}
								}
								catch (std::exception const& e)
								{
									BOOST_LOG_SEV(my_logger_bs, lt::error) << "Sales session error: " << e.what(); 
								}
                            } else {
                                if (myCart->getLoyCardsNumber() < atoi(configurationMap["LoyMaxCardsPerTransaction"].c_str()))
                                {
                                    rc = myCart->addLoyCard(barcode) ;
                                } else {
                                    rc = RC_LOY_MAX_CARD_NUMBER ;
                                }
                            }
                        } else {
                            rc = RC_ERR ;
                        }
                        tempStringStream.str( std::string() ) ;
                        tempStringStream.clear() ;
                        tempStringStream << "{\"cartId\":" << strCartId << ",\"rc\":" << rc << ",\"requestId\":" << requestId << ",\"action\":\"barcodeAdd\",\"status\":\"ok\",\"barcode\":" << barcode << ",\"description\":\"" << itemsMap[barcodesMap[barcodeWrk].getItemCode()].getDescription() << "\"}" << endl ;
                        sendRespMsg(pSock, tempStringStream.str() ) ;
                        //myCart->printCart() ;
                    }
                    //BOOST_LOG_SEV(my_logger_bs, lt::info) << "\noooo1y\n" ;
                    if (action.compare("remove")==0)
                    {
                        barcode = pt2.get<std::uint64_t> ("barcode");
						try {
							unsigned long long tItemCode = barcodesMap[barcodeWrk].getItemCode();
							map < unsigned long long, Item>::iterator it = itemsMap.find(tItemCode);
							if (it != itemsMap.end())
							{
								rc = myCart->removeItemByBarcode(itemsMap[barcodesMap[barcode].getItemCode()], barcodesMap[barcode]);
							}
							else {
								rc = BCODE_ITEM_NOT_FOUND;
							}
							
						}
						catch (std::exception const& e)
						{
							BOOST_LOG_SEV(my_logger_bs, lt::error) << "Sales session error: " << e.what();
						}
						tempStringStream.str( std::string() ) ;
                        tempStringStream.clear() ;
                        tempStringStream << "{\"cartId\":" << strCartId << ",\"rc\":" << rc << ",\"requestId\":" << requestId << ",\"action\":\"barcodeRemove\",\"status\":\"ok\",\"barcode\":" << barcode << "}" << endl ;
                        sendRespMsg(pSock, tempStringStream.str() ) ;
                        
                        //myCart->printCart() ;
                    }
                    
                    if (action.compare("print")==0)
                    {
                        rc = myCart->printCart() ;
                        tempStringStream.str( std::string() ) ;
                        tempStringStream.clear() ;
                        tempStringStream << "{\"cartId\":" << strCartId << ",\"rc\":" << rc << ",\"requestId\":" << requestId << ",\"action\":\"print\",\"status\":\"ok\"}" << endl ;
                        sendRespMsg(pSock, tempStringStream.str()) ;
                    }
                    
                    if (action.compare("close")==0)
                    {
                        rc = myCart->close() ;
                        tempStringStream.str( std::string() ) ;
                        tempStringStream.clear() ;
                        tempStringStream << "{\"cartId\":" << strCartId << ",\"rc\":" << rc << ",\"requestId\":" << requestId << ",\"action\":\"close\",\"status\":\"ok\"}" << endl ;
                        sendRespMsg(pSock, tempStringStream.str() ) ;
                        
                    }
                }
                else
                {
                    BOOST_LOG_SEV(my_logger_bs, lt::info) << "Invalid cart state\n" ;
                }
            }
            catch (std::exception const& e)
            {
				BOOST_LOG_SEV(my_logger_bs, lt::error) << "Sales session error: " << e.what();
            }
            
        }
    }
    catch (std::exception& e)
    {
		BOOST_LOG_SEV(my_logger_bs, lt::error) << "Sales session error: " << e.what();
    }
}

void BaseSystem::salesServer(boost::asio::io_service& io_service, short port)
{
    tcp::acceptor tcpAcceptor(io_service, tcp::endpoint(tcp::v4(), port));
    for (;;)
    {
        socket_ptr sock(new tcp::socket(io_service));
        BOOST_LOG_SEV(my_logger_bs, lt::info) << "Server started" ;
        tcpAcceptor.accept(*sock);
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
