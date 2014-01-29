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

#include <boost/spirit/include/qi.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>


#include "BaseSystem.h"

const int max_length = 256;
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
    boost::asio::io_service io_service;
    
    this->basePath = pBasePath ;
    
    this->loadConfiguration() ;
    this->printConfiguration() ;
    
    this->readArchives() ;
    
    this->loadCartsInProgress() ;
    
    std::cout << "System initialized.\n" ;
    //}
    
    try
    {
        std::cout << "Server starting...\n" ;
        using namespace std;
        salesServer(io_service, atol(configurationMap["NetworkPort"].c_str()));
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

void BaseSystem::loadConfiguration()
{
    std::cout << "Config load start\n" ;
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(this->basePath + "PromoCalculator.ini", pt);
    configurationMap["NetworkPort"] = pt.get<std::string>("Network.Port") ;
    configurationMap["LoyCardPrefix"] = pt.get<std::string>("Loy.CardPrefix") ;
    configurationMap["BarcodesType01"] = pt.get<std::string>("Barcodes.Type") ;
	configurationMap["SelfScanScanInDir"] = pt.get<std::string>("SelfScan.ScanInDir");
	configurationMap["SelfScanScanOutDir"] = pt.get<std::string>("SelfScan.ScanOutDir");
    //configurationMap["NodeId"] = pt.get<std::string>("Node.Id") ;
    this->nodeId = pt.get<std::uint32_t>("Node.Id") ;
    //std::cout << pt.get<std::string>("Loy.CardPrefix") << std::endl;
    //std::cout << pt.get<std::string>("Barcodes.Type") << std::endl;
    std::cout << "Config load end\n" ;
}

void BaseSystem::printConfiguration()
{
    typedef std::map<string, string>::iterator configurationRows;
    
    std::cout << "Config print start\n" ;
    std::cout << "\tNode Id: " << this->nodeId << "\n" ;
    for(configurationRows iterator = configurationMap.begin(); iterator != configurationMap.end(); iterator++) {
        std::cout << "\tkey: " << iterator->first << ", value: " << iterator->second << "\n";
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
	string tmp = "" ;
    
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
        Item tempItm ;
        for (auto i : result)
        {
            //std::cout << "ggg" << i << std::endl;
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
	string tmp = "";
	std::stringstream tempStringStream ;
	
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
                            tmpBcd = atol(tmp.c_str()) ;
                        } else {
                            tmpBcd = bcdWrk ;
                        }
                        //std::cout << "(" << tmpBcd << ") - " ;
                        tempBarcode.setCode( tmpBcd ) ;
                    }
                    //std::cout << i.c_str() << "\n";
                    break;
                case 2:
                    tempBarcode.setItemCode(atoll(i.c_str())) ;
                    break;
            }
            column++ ;
        }
        //std::cout << "Barcode.getcode: " << tempBarcode.getCode() << " type: " << bCodeType << "\n" ;
        barcodesMap[tempBarcode.getCode()] = tempBarcode ;
        //barcodesMap.emplace( std::piecewise_construct, std::make_tuple(tempBarcode.getCode()), std::make_tuple(tempBarcode) ) ;
		//std::cout << "toStr: " << barcodesMap[tempBarcode.getCode()].toStr() << "\n" ;
        
    }
    std::cout << "Finished loading file " + pFileName + "\n" ;
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
            rAction = ' ' ;
            rCode = 0 ;
            rQty = 0 ;
            //Cart* tmpCart = nullptr ;
            //std::cout << "\nFile: " << it->path().filename() << "\n" ;
            if (fs::is_regular_file(*it) && it->path().extension() == ".transaction_in_progress")
            {
                //ret.push_back(it->path().filename());
                currentTmpCartNumber = atol(it->path().stem().string().c_str()) ;
                
                //nextCartNumber is saved and then restored to avoid problems with the max cart number when leaving this function in case of sorting problems of filenames from filesystem
                nextCartNumberTmp = nextCartNumber ;
                nextCartNumber = currentTmpCartNumber ;
                std::cout << "==================================\n" ;
                std::cout << "File tmpTrans: " << it->path().filename() << " num: " << currentTmpCartNumber << " next: " << nextCartNumber << "\n";
                
                newCart( GEN_CART_LOAD ) ;
                
                nextCartNumber = nextCartNumberTmp ;
                
                itCarts = cartsMap.find( currentTmpCartNumber );
                
                //cout << "Dovrei caricare carrello" << currentTmpCartNumber << "\n" ;
                if (itCarts != cartsMap.end()) {
                    myCart = &(itCarts->second) ;
                    //cout << "Carico carrello\n" ;
                }
                //std::cout << "Cart nr: " << myCart->getNumber() << "\n" ;
                std::ifstream tmpTransactonFileToLoad( this->basePath + "CARTS/" + it->path().filename().string() );
                
                while( std::getline(tmpTransactonFileToLoad, line) )
                {
                    //std::cout << "\n" << line ;
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
                                //std::cout << "Action: " << i << "\n" ;
                                rAction = i[0] ;
                                break;
                            case 2:
                                //std::cout << "Barcode: " << i << "\n"  ;
                                rCode = atoll(i.c_str()) ;
                                break;
                            case 3:
                                //std::cout << "Qty: " << i  << "\n" ;
                                rQty = atoll(i.c_str()) ;
                                break;
                            default:
                                break ;
                        }
                        column++ ;
                    }
                    
                    std::cout << "Debug recupero riga carrello, rcode: " << rCode << ", barcode: " << barcodesMap[rCode].toStr() << "\n";
                    
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
    int type = 0 ;
    regex ean13( "\\d{13}" ) ;
	regex ean13PriceReq( "2\\d{12}" ) ;
	regex upc( "\\d{12}" ) ;
	regex ean8( "\\d{8}" ) ;
	regex loyCard( "260\\d{9}") ;
	std::stringstream tempStringStream ;
    
    tempStringStream.str( std::string() ) ;
	tempStringStream.clear() ;
	tempStringStream << pBarcode ;
	//std::cout << "Barcode: ---" << tempStringStream.str() << "---\n" ;
    if (regex_match( tempStringStream.str(), loyCard ) )
    {
        //std::cout << "Barcode type: Loyalty Card\n" ;
        return BCODE_LOYCARD ;
    }
    
    if (regex_match( tempStringStream.str(), ean13 ) )
    {
        if (regex_match( tempStringStream.str(), ean13PriceReq ) )
        {
            //std::cout << "Barcode type: EAN13 with price\n" ;
            return BCODE_EAN13_PRICE_REQ ;
        } else {
            //std::cout << "Barcode type: EAN13\n" ;
            return BCODE_EAN13 ;
        }
    }
    
    if (regex_match( tempStringStream.str(), upc ) )
    {
        //std::cout << "Barcode type: UPC\n" ;
        return BCODE_UPC ;
    }
    
    if (regex_match( tempStringStream.str(), ean8 ) )
    {
        //std::cout << "Barcode type: EAN8\n" ;
        return BCODE_EAN8 ;
    }
    
    
    
    //std::cout << "Barcode type: not recognized\n" ;
    return BCODE_NOT_RECOGNIZED ;
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
    std::uint64_t barcodeWrk = 0 ;
    std::int32_t qty = 0 ;
	std::ostringstream tempStringStream ;
	std::ostringstream streamCartId ;
	
    char tmp[13] ;
    
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
				tempStringStream << "{\"cartId\":" << cartId << ",\"rc\":" << rc << "}\n" ;
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
                    tempStringStream << "{\"cartId\":" << strCartId << ",\"rc\":" << rc << ",\"requestId\":" << requestId << ",\"action\":\"save\",\"status\":\"ok\"}\n" ;
					sendRespMsg(pSock, tempStringStream.str() );
                }
                
                if (action.compare("sendToPos")==0)
                {
                    posNumber = pt2.get<std::uint32_t> ("posNumber");
					rc = myCart->sendToPos(posNumber, this->configurationMap["SelfScanScanInDir"]);
                    tempStringStream << "{\"cartId\":" << strCartId << ",\"posNumber\":" << posNumber  << ",\"rc\":" << rc << ",\"requestId\":" << requestId << ",\"action\":\"sendToPos\",\"status\":\"ok\"}\n" ;
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
                                strcpy( tmp, tempStringStream.str().c_str() );
                                tmp[7] = '0' ;
                                tmp[8] = '0' ;
                                tmp[9] = '0' ;
                                tmp[10] = '0' ;
                                tmp[11] = '0' ;
                                tmp[12] = '0' ;
                                barcodeWrk = strtoull(tmp, nullptr, 10) ;
                            } else {
                                barcodeWrk = barcode ;
                            }
                            std::cout << "barcodeWrk: " << barcodeWrk << "\n" ;
                            rc = myCart->addItemByBarcode(itemsMap[barcodesMap[barcode].getItemCode()], barcodesMap[barcode], qty) ;
                        } else {
                            rc = RC_OK ;
                        }
                    } else {
                        rc = RC_ERR ;
                    }
                    tempStringStream << "{\"cartId\":" << strCartId << ",\"rc\":" << rc << ",\"requestId\":" << requestId << ",\"action\":\"barcodeAdd\",\"status\":\"ok\",\"barcode\":" << barcode << "}\n" ;
                    sendRespMsg(pSock, tempStringStream.str() ) ;
                    //myCart->printCart() ;
                }
                //std::cout << "\noooo1y\n" ;
                if (action.compare("remove")==0)
                {
                    barcode = pt2.get<std::uint64_t> ("barcode");
                    rc = myCart->removeItemByBarcode(itemsMap[barcodesMap[barcode].getItemCode()], barcodesMap[barcode]) ;
                    tempStringStream << "{\"cartId\":" << strCartId << ",\"rc\":" << rc << ",\"requestId\":" << requestId << ",\"action\":\"barcodeRemove\",\"status\":\"ok\",\"barcode\":" << barcode << "}\n" ;
                    sendRespMsg(pSock, tempStringStream.str() ) ;
                    
                    //myCart->printCart() ;
                }
                
                if (action.compare("print")==0)
                {
                    rc = myCart->printCart() ;
					tempStringStream << "{\"cartId\":" << strCartId << ",\"rc\":" << rc << ",\"requestId\":" << requestId << ",\"action\":\"print\",\"status\":\"ok\"}\n" ;
                    sendRespMsg(pSock, tempStringStream.str()) ;
                }
                
                if (action.compare("close")==0)
                {
                    rc = myCart->close() ;
					tempStringStream << "{\"cartId\":" << strCartId << ",\"rc\":" << rc << ",\"requestId\":" << requestId << ",\"action\":\"close\",\"status\":\"ok\"}\n" ;
                    sendRespMsg(pSock, tempStringStream.str() ) ;
                    
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
        std::cout << "Server started" ;
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
