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
#include <boost/tokenizer.hpp>
#include <stdlib.h>

#include <boost/spirit/include/qi.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "reply.hpp"
#include "request.hpp"
#include "request_parser.hpp"
#include "request_handler.hpp"

#include "Item.h"
#include "Department.h"
#include "Promotion.h"

#include "BaseSystem.h"

const int max_length = 8192;

static uint32_t nextCartNumber ;
boost::mutex cartnumber_mutex ;
namespace qi = boost::spirit::qi;
namespace fs = boost::filesystem;

qi::rule<std::string::const_iterator, std::string()> quoted_string = '"' >> *(qi::char_ - '"') >> '"';
qi::rule<std::string::const_iterator, std::string()> valid_characters = qi::char_ - '"' - ',';
qi::rule<std::string::const_iterator, std::string()> item = *(quoted_string | valid_characters );
qi::rule<std::string::const_iterator, std::vector<std::string>()> csv_parser = item % ',';

using namespace std;

BaseSystem::BaseSystem( string pBasePath, string pIniFileName )
{
    using namespace logging::trivial;
    
    this->basePath = pBasePath ;
    this->iniFileName = pIniFileName ;
    this->nodeId = 0 ;
    this->baseSystemRunning = false ;
    this->dummyRCS = false ;
    this->cartsPriceChangesWhileShopping = false ;
    this->ean13 = "\\d{13}"  ;
    this->ean13PriceReq = "2\\d{12}" ;
    this->upc = "\\d{12}" ;
    this->ean8 = "\\d{8}" ;
    this->loyCard = "260\\d{9}" ;
    this->loyCardNoCheck = "260\\d{8}" ;
    
    if ( this->loadConfiguration() == 0 )
    {
        this->printConfiguration() ;
        
        this->baseSystemRunning = true ;
        
        this->readArchives() ;
        
		this->loadCartsInProgress();
        
        boost::thread newThread(boost::bind(&BaseSystem::checkForVariationFiles, this));
        
        boost::this_thread::sleep(boost::posix_time::milliseconds(10000));
        
        BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - System initialized." ;
    } else {
        BOOST_LOG_SEV(my_logger_bs, fatal) << "- BS - Bad configuration error, aborting start" ;
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

long BaseSystem::loadConfiguration()
{
    BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - Config file: " << this->basePath << this->iniFileName ;
    BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - Config load start" ;
    boost::property_tree::ptree pt;
    long rc = 0 ;
    
    try {
        boost::property_tree::ini_parser::read_ini(this->basePath + this->iniFileName, pt);
        rc = rc + setConfigValue("MainStoreChannel", "Main.StoreChannel", &pt );
        rc = rc + setConfigValue("MainStoreLoyChannel", "Main.StoreLoyChannel", &pt );
        rc = rc + setConfigValue("MainStoreId", "Main.StoreId", &pt );
        rc = rc + setConfigValue("MainVarCheckDelaySeconds", "Main.VarCheckDelaySeconds", &pt );
        rc = rc + setConfigValue("MainReturnSeparateLinkedBarcode", "Main.ReturnSeparateLinkedBarcode", &pt );
        rc = rc + setConfigValue("MainDummyRCS", "Main.DummyRCS", &pt );
        rc = rc + setConfigValue("CartsPriceChangesWhileShopping", "Carts.PriceChangesWhileShopping", &pt );
        rc = rc + setConfigValue("SelfScanScanInDir", "SelfScan.ScanInDir", &pt );
        rc = rc + setConfigValue("SelfScanScanOutDir", "SelfScan.ScanOutDir", &pt );
        rc = rc + setConfigValue("LoyCardPrefix", "Loy.CardPrefix", &pt );
        rc = rc + setConfigValue("LoyMaxCardsPerTransaction", "Loy.MaxCardsPerTransaction", &pt );
        rc = rc + setConfigValue("LoyOnlyOneShoppingSessionPerCard", "Loy.OnlyOneShoppingSessionPerCard", &pt );
        rc = rc + setConfigValue("BarcodesType01", "Barcodes.Type", &pt );
        rc = rc + setConfigValue("NodeId", "Node.Id", &pt );
        rc = rc + setConfigValue("WebAddress", "Web.Address", &pt );
        rc = rc + setConfigValue("WebPort", "Web.Port", &pt );
        rc = rc + setConfigValue("WebThreads", "Web.Threads", &pt );
		configurationMap["MainArchivesDir"] = configurationMap["MainStoreChannel"] + "/" + configurationMap["MainStoreId"] + "/";
        
        this->nodeId = pt.get<uint32_t>("Node.Id") ;
        
        if (atoi(configurationMap["MainDummyRCS"].c_str()) == 1)
		{
			this->dummyRCS = true;
		}
		else {
			this->dummyRCS = false;
		}
        
		if (atoi(configurationMap["CartsPriceChangesWhileShopping"].c_str()) == 1)
		{
			this->cartsPriceChangesWhileShopping = true;
		}
		else {
			this->cartsPriceChangesWhileShopping = false;
		}
        
		if (atoi(configurationMap["MainReturnSeparateLinkedBarcode"].c_str()) == 1)
		{
			this->mainReturnSeparateLinkedBarcode = true;
		}
		else {
			this->mainReturnSeparateLinkedBarcode = false;
		}
        this->varFolderName = this->basePath +  "ARCHIVES/" + configurationMap["MainArchivesDir"] + "VARS" ;
        this->cartFolderName = this->basePath + "ARCHIVES/" + configurationMap["MainArchivesDir"] + "CARTS" ;
        this->varCheckDelaySeconds = atol(configurationMap["MainVarCheckDelaySeconds"].c_str()) * 1000L ;
    }
    catch (std::exception const& e)
    {
        BOOST_LOG_SEV(my_logger_bs, lt::fatal) << "- BS - Config error: " << e.what() ;
        return RC_ERR ;
    }
    BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - Config load end" ;
    return rc ;
}

long BaseSystem::setConfigValue(string confMapKey, string treeSearchKey, boost::property_tree::ptree* configTree)
{
    configurationMap[confMapKey] = configTree->get<std::string>(treeSearchKey) ;
    return RC_OK ;
}

void BaseSystem::printConfiguration()
{
    typedef std::map<string, string>::iterator configurationRows;
    
    BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - Config print start" ;
    BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - Node Id: " << this->nodeId ;
    for(configurationRows iterator = configurationMap.begin(); iterator != configurationMap.end(); iterator++) {
        BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - Key: " << iterator->first << ", value: " << iterator->second ;
    }
    BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - Config print end" ;
}

void BaseSystem::readDepartmentArchive( string pFileName )
{
    //http://stackoverflow.com/questions/18365463/how-to-parse-csv-using-boostspirit
    
    // Tokenizer
    typedef boost::tokenizer< boost::escaped_list_separator<char> , std::string::const_iterator, std::string> Tokenizer;
    boost::escaped_list_separator<char> seps('\\', ',', '\"');
    string archiveFileName = this->basePath + "ARCHIVES/" + configurationMap["MainArchivesDir"] + pFileName ;
    std::string line;
    std::ifstream archiveFile( archiveFileName );
    bool r = false ;
    int column = 0 ;
    Department tempDepartment ;
    
    if (!archiveFile) {
        BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - File " + archiveFileName + " not found" ;
        exit(-1);
    } else {
        BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - File " + archiveFileName + " found" ;
    }
    
    while( std::getline(archiveFile, line) )
    {
        
        // Boost Spirit Qi
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
                    tempDepartment.setCode(strtoull(i.c_str(),nullptr,10)) ;
                    break;
                case 1:
                    tempDepartment.setParentCode(strtoull(i.c_str(),nullptr,10)) ;
                    break;
                case 2:
                    tempDepartment.setDescription(i) ;
                    break;
                default:
                    break ;
            }
            column++ ;
        }
        departmentsMap.addElement(tempDepartment) ;
    }
    BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - Finished loading file " << pFileName ;
}

void BaseSystem::readItemArchive( string pFileName )
{
    //http://stackoverflow.com/questions/18365463/how-to-parse-csv-using-boostspirit
    
    // Tokenizer
    typedef boost::tokenizer< boost::escaped_list_separator<char> , std::string::const_iterator, std::string> Tokenizer;
    boost::escaped_list_separator<char> seps('\\', ',', '\"');
    
    string archiveFileName = this->basePath + "ARCHIVES/" + configurationMap["MainArchivesDir"] + pFileName ;
    std::ifstream archiveFile( archiveFileName );
	string tmp = "" ;
    bool r = false ;
    int column = 0 ;
    Item tempItm ;
    
    if (!archiveFile) {
        BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - " << "File " + archiveFileName + " not found" ;
        exit(-1);
    } else {
        BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - " << "File " + archiveFileName + " found" ;
    }
    
    std::string line;
    while( std::getline(archiveFile, line) )
    {
        
        // Boost Spirit Qi
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
					tmp = std::string(i);
					tempItm.setCode(strtoul(tmp.c_str(), nullptr, 10));
                    break;
                case 1:
                    tempItm.setDescription(i) ;
                    break;
                case 2:
					tempItm.setDepartmentCode(strtoull(i.c_str(), nullptr, 10));
                    break;
                case 3:
					tempItm.setPrice(strtol(i.c_str(), nullptr, 10));
                    break;
                case 4:
                    tempItm.setLinkedBarCode(strtoull(i.c_str(),nullptr,10)) ;
                    break;
                default:
                    break ;
            }
            column++ ;
        }
		//itemsMap.insert(std::pair<uint64_t, Item>(tempItm.getCode(), std::move(tempItm)));
        itemsMap.addElement(tempItm) ;
    }
    BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - Finished loading file " << pFileName ;
}

void BaseSystem::readBarcodesArchive( string pFileName )
{
    //http://stackoverflow.com/questions/18365463/how-to-parse-csv-using-boostspirit
    
    ItemCodePrice itmCodePrice ;
    // Tokenizer
    typedef boost::tokenizer< boost::escaped_list_separator<char> , std::string::const_iterator, std::string> Tokenizer;
    boost::escaped_list_separator<char> seps('\\', ',', '\"');
    
    string archiveFileName = this->basePath + "ARCHIVES/" + configurationMap["MainArchivesDir"] + pFileName ;
    std::ifstream archiveFile( archiveFileName );
	string tmp = "" ;
    bool r = false ;
    int column = 0 ;
    uint64_t bcdWrk = 0 ;
    Barcodes tempBarcode ;
    
    if (!archiveFile) {
        BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - File " + archiveFileName + " not found" ;
        exit(-1);
    } else {
        BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - File " + archiveFileName + " loaded" ;
    }
    
    std::string line;
    while( std::getline(archiveFile, line) )
    {
        
        // Boost Spirit Qi
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
					tmp = std::string(i);
                    bcdWrk = strtoull(tmp.c_str(),nullptr,10) ;
                    itmCodePrice = decodeBarcode( bcdWrk ) ;
                    tempBarcode.setCode(itmCodePrice.barcode) ;
                    break;
                case 1:
                    tempBarcode.setItemCode(strtoull(i.c_str(),nullptr,10)) ;
                    break;
            }
            column++ ;
        }
		//barcodesMap.insert(std::pair<uint64_t, Barcodes>(tempBarcode.getCode(), std::move(tempBarcode)));
        barcodesMap.addElement(tempBarcode);
    }
    BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - " << "Finished loading file " << pFileName ;
}


void BaseSystem::readPromotionsArchive( string pFileName )
{
    //http://stackoverflow.com/questions/18365463/how-to-parse-csv-using-boostspirit
    
    // Tokenizer
    typedef boost::tokenizer< boost::escaped_list_separator<char> , std::string::const_iterator, std::string> Tokenizer;
    boost::escaped_list_separator<char> seps('\\', ',', '\"');
    
    string archiveFileName = this->basePath + "ARCHIVES/" + configurationMap["MainArchivesDir"] + pFileName ;
    std::ifstream archiveFile( archiveFileName );
    string tmp = "" ;
    bool r = false ;
    int column = 0 ;
    Promotion tempPromotion ;
    
    if (!archiveFile) {
        BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - File " + archiveFileName + " not found" ;
        exit(-1);
    } else {
        BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - File " + archiveFileName + " loaded" ;
    }
    
    std::string line;
    while( std::getline(archiveFile, line) )
    {
        
        // Boost Spirit Qi
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
                    tempPromotion.setPromoCode(strtoull(i.c_str(),nullptr,10)) ;
                    break;
                case 1:
                    tempPromotion.setCode(strtoull(i.c_str(),nullptr,10)) ;
                    break;
                case 2:
                    tempPromotion.setDiscount(strtoull(i.c_str(),nullptr,10)) ;
                    break;
                case 3:
                    tempPromotion.setDiscountType(strtoull(i.c_str(),nullptr,10)) ;
                    break;
                case 4:
                    tempPromotion.setDescription(std::string(i)) ;
                    break;
            }
            column++ ;
        }
        promotionsMap.addElement(tempPromotion);
        cout << tempPromotion.toStr() << endl;
    }
    BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - " << "Finished loading file " << pFileName ;
}


void BaseSystem::readArchives()
{
    this->readDepartmentArchive( "DEPARTMENTS.CSV" ) ;
    this->readItemArchive( "ITEMS.CSV" ) ;
    this->readBarcodesArchive( "BARCODES.CSV" ) ;
    this->readPromotionsArchive( "PROMOTIONS.CSV" ) ;
}

void BaseSystem::dumpArchivesFromMemory()
{
    this->departmentsMap.dumpToFile(this->basePath + "ARCHIVES/" + configurationMap["MainArchivesDir"] + "DEPARTMENTS_DUMP.CSV" ) ;
    this->itemsMap.dumpToFile(this->basePath + "ARCHIVES/" + configurationMap["MainArchivesDir"] + "ITEMS_DUMP.CSV" ) ;
    this->barcodesMap.dumpToFile(this->basePath + "ARCHIVES/" + configurationMap["MainArchivesDir"] + "BARCODES_DUMP.CSV" ) ;
    this->promotionsMap.dumpToFile(this->basePath + "ARCHIVES/" + configurationMap["MainArchivesDir"] + "PROMOTIONS_DUMP.CSV" ) ;
}

ItemCodePrice BaseSystem::decodeBarcode( uint64_t rCode )
{
    std::stringstream tempStringStream ;
    std::string barcodeWrkStr = "" ;
    ItemCodePrice rValues = {0,0,0,0};
    
    tempStringStream.str( std::string() ) ;
    tempStringStream.clear() ;
    tempStringStream << rCode ;
    
    rValues.type = BCODE_NOT_RECOGNIZED ;
    
    if (regex_match( tempStringStream.str(), loyCard ) )
    {
        rValues.type = BCODE_LOYCARD ;
    } else {
        if (regex_match( tempStringStream.str(), loyCardNoCheck ) )
        {
            rValues.type = BCODE_LOYCARD ;
        } else {
            if (regex_match( tempStringStream.str(), ean13 ) )
            {
                if (regex_match( tempStringStream.str(), ean13PriceReq ) )
                {
                    rValues.type = BCODE_EAN13_PRICE_REQ ;
                } else {
                    rValues.type = BCODE_EAN13 ;
                }
            } else {
                if (regex_match( tempStringStream.str(), upc ) )
                {
                    rValues.type = BCODE_UPC ;
                } else {
                    if (regex_match( tempStringStream.str(), ean8 ) )
                    {
                        rValues.type = BCODE_EAN8 ;
                    }
                }
            }
        }
    }
    
    switch (rValues.type)
    {
        case BCODE_EAN13_PRICE_REQ:
            barcodeWrkStr = tempStringStream.str().substr(0,7) + "000000" ;
            rValues.barcode = atoll(barcodeWrkStr.c_str()) ;
            break;
        default:
            rValues.barcode = rCode ;
            break;
    }
    
    if ((barcodesMap[rValues.barcode].getCode() == rValues.barcode))
    {
        rValues.code = barcodesMap[rValues.barcode].getItemCode() ;
    }
    
    return rValues ;
}

void BaseSystem::checkForVariationFiles()
{
	src::severity_logger_mt< boost::log::trivial::severity_level > my_logger_var ;
    std::string varFileName = "" ;
    bool varsFound = false ;
    std::string line;
    std::string key;
    std::string value;
    int column = 0 ;
    bool r = false ;
    char rowType = 0, rowAction = 0 ;
    Item itmTemp ;
    
    bool updatedItems = false ;
    bool updatedBarcodes = false ;
    bool updatedDepts = false ;
    bool updatedVats = false ;
    
    while ( baseSystemRunning )
    {
        varsFound = false ;
		BOOST_LOG_SEV(my_logger_var, lt::info) << "- BS - Checking for variation files";
        if (!fs::exists(varFolderName))
        {
			BOOST_LOG_SEV(my_logger_var, lt::info) << "- BS - No " << this->varFolderName << " subfolder found";
        } else {
			fs::recursive_directory_iterator it(varFolderName);
            fs::recursive_directory_iterator endit;
            while(it != endit)
            {
                //std::cout << it->path().stem().string() << " " << it->path().extension() << std::endl ;
                if (fs::is_regular_file(*it) && it->path().extension() == ".VAR")
                {
                    varFileName = it->path().filename().string() ;
					BOOST_LOG_SEV(my_logger_var, lt::info) << "- BS - Var file found : " << varFileName;
                    
                    std::ifstream varFileToLoad(varFolderName + "/" + varFileName);
                    
                    while( std::getline(varFileToLoad, line) )
                    {
                        std::istringstream is_line(line);
                        
                        std::string::const_iterator s_begin ;
                        std::string::const_iterator s_end ;
                        std::vector<std::string> result;
                        s_begin = line.begin();
                        s_end = line.end();
                        
                        r = boost::spirit::qi::parse(s_begin, s_end, csv_parser, result);
                        assert(r == true);
                        assert(s_begin == s_end);
                        column = 0 ;
                        
                        for (auto i : result)
                        {
                            if (column == 0 )
                            {
                                rowType = i[0] ;
                            } else {
                                if (column == 1 ) {
                                    rowAction = i[0] ;
                                } else {
                                    switch (rowType)
                                    {
                                        case 'I':
                                        {
                                            updatedItems = true ;
                                            switch (column)
                                            {
                                                case 2:
													itmTemp.setCode(strtoull(i.c_str(), nullptr, 10));
                                                    break;
                                                case 3:
                                                    itmTemp.setDescription(i) ;
                                                    break;
                                                case 4:
													itmTemp.setDepartmentCode(strtoull(i.c_str(), nullptr, 10));
                                                    break;
                                                case 5:
													itmTemp.setPrice(strtol(i.c_str(), nullptr, 10));
                                                    break ;
                                                default:
                                                    break ;
                                            }
                                        }
                                    }
                                    
                                }
                            }
                            column++ ;
                        }
                        switch (rowType)
                        {
                            case 'I':
                            {
                                if (itmTemp.getCode()>0)
                                {
                                    itemsMap[itmTemp.getCode()] = itmTemp ;
                                }
                                break ;
                            }
                            default:
                            {
                                break;
                            }
                        }
                    }
                    
					BOOST_LOG_SEV(my_logger_var, lt::info) << "- BS - Renaming Var file " << varFileName << " in " << varFileName + ".OLD";
                    fileMove(varFolderName + "/" + varFileName, varFolderName + "/" + varFileName + ".OLD") ;
                    varsFound = true ;
                }
                ++it;
            }
        }
        
        if (!varsFound)
        {
			BOOST_LOG_SEV(my_logger_var, lt::info) << "- BS - No variation files found";
        } else {
            if (updatedItems == true)
            {
                if ( fileMove(this->basePath + "ARCHIVES/" + configurationMap["MainArchivesDir"] + "ITEMS.CSV", this->basePath + "ARCHIVES/" + configurationMap["MainArchivesDir"] + "ITEMS.OLD") )
                {
					BOOST_LOG_SEV(my_logger_var, lt::info) << "- BS - Dumping ITEMS: Rename old file ok";
					BOOST_LOG_SEV(my_logger_var, lt::info) << "- BS - Dumping ITEMS: Start";
					this->itemsMap.dumpToFile(this->basePath + "ARCHIVES/" + configurationMap["MainArchivesDir"] + "ITEMS.CSV" ) ;
                    BOOST_LOG_SEV(my_logger_var, lt::info) << "- BS - Dumping ITEMS: End ";
                } else {
					BOOST_LOG_SEV(my_logger_var, lt::error) << "- BS - Dumping ITEMS: Rename old file failed";
                }
                
            }
            if (updatedBarcodes == true)
            {
                if ( fileMove(this->basePath + "ARCHIVES/" + configurationMap["MainArchivesDir"] + "BARCODES.CSV", this->basePath + "ARCHIVES/" + configurationMap["MainArchivesDir"] + "BARCODES.OLD") )
                {
					BOOST_LOG_SEV(my_logger_var, lt::info) << "- BS - Dumping BARCODES: Rename old file ok";
					BOOST_LOG_SEV(my_logger_var, lt::info) << "- BS - Dumping BARCODES: Start";
					this->barcodesMap.dumpToFile(this->basePath + "ARCHIVES/" + configurationMap["MainArchivesDir"] + "BARCODES.CSV" ) ;
                    BOOST_LOG_SEV(my_logger_var, lt::info) << "- BS - Dumping BARCODES: End ";
                } else {
					BOOST_LOG_SEV(my_logger_var, lt::error) << "- BS - Dumping BARCODES: Rename old file failed";
                }
            }
            if (updatedDepts == true)
            {
                if ( fileMove(this->basePath + "ARCHIVES/" + configurationMap["MainArchivesDir"] + "DEPARTMENTS.CSV", this->basePath + "ARCHIVES/" + configurationMap["MainArchivesDir"] + "DEPARTMENTS.OLD") )
                {
					BOOST_LOG_SEV(my_logger_var, lt::info) << "- BS - Dumping DEPARTMENTS: Rename old file ok";
					BOOST_LOG_SEV(my_logger_var, lt::info) << "- BS - Dumping DEPARTMENTS: Start";
					this->departmentsMap.dumpToFile(this->basePath + "ARCHIVES/" + configurationMap["MainArchivesDir"] + "DEPARTMENTS.CSV" ) ;
                    BOOST_LOG_SEV(my_logger_var, lt::info) << "- BS - Dumping DEPARTMENTS: End ";
                } else {
					BOOST_LOG_SEV(my_logger_var, lt::error) << "- BS - Dumping DEPARTMENTS: Rename old file failed";
                }
            }
            if (updatedVats == true)
            {
                //todo
            }
        }
        boost::this_thread::sleep(boost::posix_time::milliseconds(varCheckDelaySeconds));
    }
    std::cout << "Esco\n" ;
}

void BaseSystem::loadCartsInProgress()
{
    std::string line;
    std::string key;
    std::string value;
    std::string tmp = "" ;
    std::map<uint64_t, Cart>::iterator itCarts ;
    std::stringstream tempStringStream ;
    std::string barcodeWrkStr = "" ;
    uint32_t currentTmpCartNumber = 0, nextCartNumberTmp = 0 ;
    char rAction = ' ' ;
    char rObject = ' ' ;
    uint64_t rCode = 0 ;
    uint32_t rQty = 0 ;
    bool r = false ;
    int column = 0 ;
    ItemCodePrice itmCodePrice ;
    Cart* myCart = nullptr;
    Item tempItm ;
    Department tempDepartment ;
    std::string::const_iterator s_begin ;
    std::string::const_iterator s_end ;
    std::vector<std::string> result ;
    
    nextCartNumber = 1 ;
    
	if (!fs::exists(cartFolderName))
    {
        BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - " << "No " << cartFolderName << " subfolder found" ;
        exit(-1);
    }
    
	if (fs::is_directory(cartFolderName))
    {
        BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - " << "CARTS subfolder found" ;
		fs::recursive_directory_iterator it(cartFolderName);
        fs::recursive_directory_iterator endit;
        while(it != endit)
        {
            rAction = ' ' ;
            rCode = 0 ;
            rQty = 0 ;
            if (fs::is_regular_file(*it) && it->path().extension() == ".transaction_in_progress")
            {
                currentTmpCartNumber = atol(it->path().stem().string().c_str()) ;
                
				nextCartNumberTmp = nextCartNumber ;
                nextCartNumber = currentTmpCartNumber ;
                BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - " << "==================================" ;
                BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - " << "File tmpTrans: " << it->path().filename() << " num: " << currentTmpCartNumber << " next: " << nextCartNumber ;
                
                newCart( GEN_CART_LOAD ) ;
                
                nextCartNumber = nextCartNumberTmp ;
                
                itCarts = cartsMap.find( currentTmpCartNumber );
                if (itCarts != cartsMap.end()) {
                    myCart = &(itCarts->second) ;
                }
				std::ifstream tmpTransactonFileToLoad(this->basePath + "ARCHIVES/" + configurationMap["MainArchivesDir"] + "CARTS/" + it->path().filename().string());
                
                while( std::getline(tmpTransactonFileToLoad, line) )
                {
                    std::istringstream is_line(line) ;
                    s_begin = line.begin();
                    s_end = line.end();
					result.clear();
                    r = boost::spirit::qi::parse(s_begin, s_end, csv_parser, result);
                    assert(r == true);
                    assert(s_begin == s_end);
                    column = 0 ;
                    for (auto i : result)
                    {
						BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - column: " << column << ",i: " << i;
                        switch (column)
                        {
                            case 0:
                                //timeStamp
                                break;
                            case 1:
                                rObject = i[0] ;
                                break;
                            case 2:
                                rAction = i[0] ;
                                break;
                            default:
                                switch (rObject)
                            {
                                case 'I':
                                case 'L':
                                    switch (column)
                                {
                                    case 3:
                                        rCode = strtoull(i.c_str(), nullptr, 10);
                                        break;
                                    case 4:
                                        rQty = atol(i.c_str()) ;
                                        break;
                                    default:
                                        break ;
                                }
                                    break ;
                                case 'K':
                                    switch (rAction)
                                {
                                    case 'D':
                                        switch (column)
                                    {
                                        case 3:
                                            tempDepartment.setCode(strtoull(i.c_str(),nullptr,10)) ;
                                            break;
                                        case 4:
                                            tempDepartment.setParentCode(strtoull(i.c_str(),nullptr,10)) ;
                                            break;
                                        case 5:
                                            tempDepartment.setDescription(i) ;
                                            break;
                                    }
                                        break;
                                    case 'I':
                                        switch (column)
                                    {
                                        case 3:
                                            tmp = std::string(i);
                                            tempItm.setCode(strtoull(i.c_str(), nullptr, 10));
                                            break;
                                        case 4:
                                            tempItm.setDescription(i) ;
                                            break;
                                        case 5:
                                            tempItm.setDepartmentCode(strtoull(i.c_str(),nullptr,10));
                                            break;
                                        case 6:
                                            tempItm.setPrice(strtol(i.c_str(), nullptr, 10));
                                            break;
                                        case 7:
                                            tempItm.setLinkedBarCode(strtoull(i.c_str(),nullptr,10)) ;
                                            break;
                                        default:
                                            break;
                                    }
                                        break ;
                                }
                                    break ;
                                default:
                                    break ;
                            }
                        }
                        column++ ;
                    }
                    
                    myCart->getNextRequestId() ;
                    switch (rObject)
                    {
                            
                        case 'I':
                            if (rAction == 'A')
                            {
								BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - Debug recupero riga carrello, IA rcode: " << rCode << " qty:" << rQty << ", addr: " << &itemsMap[itmCodePrice.code];
                                itmCodePrice = decodeBarcode( rCode ) ;
                                itmCodePrice.price = myCart->getItemPrice(&itemsMap[itmCodePrice.code], rCode, itmCodePrice.type, cartsPriceChangesWhileShopping) ;
                                myCart->addItemByBarcode(itemsMap[itmCodePrice.code], rCode, rQty, itmCodePrice.price, 0 ) ;
                            } else {
								BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - Debug recupero riga carrello, IV rcode: " << rCode << rCode << " qty:" << rQty;
								itmCodePrice = decodeBarcode(rCode);
                                itmCodePrice.price = myCart->getItemPrice(&itemsMap[itmCodePrice.code], rCode, itmCodePrice.type, cartsPriceChangesWhileShopping) ;
                                myCart->removeItemByBarcode(itemsMap[itmCodePrice.code], rCode, itmCodePrice.price, 0 ) ;
                            }
                            break;
                        case 'L':
                            if (rAction == 'A')
                            {
								BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - Debug recupero riga carrello, LA rcode: " << rCode;
								myCart->addLoyCard(rCode, atoi(configurationMap["LoyMaxCardsPerTransaction"].c_str()));
                                allLoyCardsMap[rCode] = currentTmpCartNumber ;
                            } else {
								BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - Debug recupero riga carrello, LV rcode: " << rCode;
								myCart->removeLoyCard(rCode);
                                allLoyCardsMap.erase(rCode) ;
                            }
                            break;
                        case 'C':
                            if (rAction == 'V')
                            {
								BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - Debug recupero riga carrello, CV";
                                myCart->setState(CART_STATE_VOIDED) ;
                            }
                            break;
                            if (rAction == 'C')
                            {
								BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - Debug recupero riga carrello, CC";
								myCart->setState(CART_STATE_CLOSED);
                            }
                            break;
                        case 'K':
                            switch (rAction)
                        {
                            case 'I':
                                BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - Debug recupero riga carrello, KI";
                                if (!cartsPriceChangesWhileShopping)
                                {
                                    myCart->updateLocalItemMap(tempItm, departmentsMap[tempItm.getDepartmentCode()]) ;
                                }
                                break;
                            case 'D':
                                BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - Debug recupero riga carrello, KD";
                                break;
                        }
                            break;
                        default:
                            BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - " << "Row action not recognized" ;
                            break;
                    }
                }
                
                if ( (myCart->getState()!=CART_STATE_VOIDED) && (myCart->getState()!=CART_STATE_CLOSED) )
                {
                    myCart->setState(CART_STATE_READY_FOR_ITEM) ;
                }
                
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


std::string BaseSystem::fromLongToStringWithDecimals( int64_t pValue )
{
    std::ostringstream tempStringStream, returnStream ;
    tempStringStream.str( std::string() ) ;
    tempStringStream.clear() ;
    tempStringStream << pValue ;
    BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - PrezziStrani - pValue: " << pValue ;
    BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - PrezziStrani - tempStringStream: " << tempStringStream.str() ;
    
    if (tempStringStream.str().size()<3)
    {
        returnStream << "0." << tempStringStream.str() ;
    } else {
        returnStream << tempStringStream.str().substr(0,tempStringStream.str().size()-2) << "." << tempStringStream.str().substr(tempStringStream.str().size()-2,tempStringStream.str().size()) ;
    }
    
    return returnStream.str() ;
}

int64_t BaseSystem::getItemPrice(Item* pItem, uint64_t pBarcode, unsigned int pBCodeType)
{
    std::stringstream tempStringStream ;
    std::string barcodeWrkStr ;
    
    if (pBCodeType!=BCODE_EAN13_PRICE_REQ)
    {
        return pItem->getPrice();
    } else {
        tempStringStream.str( std::string() ) ;
        tempStringStream.clear() ;
        tempStringStream << pBarcode ;
        barcodeWrkStr = tempStringStream.str().substr(0,7) + "000000" ;
        return atol(tempStringStream.str().substr(7,5).c_str()) ;
    }
    
}

string BaseSystem::salesActionsFromWebInterface(int pAction, std::map<std::string, std::string> pUrlParamsMap)
{
    uint64_t cartId = 0 ;
    uint64_t promoVal = 0 ;
    std::string resp = " " ;
    std::ostringstream respStringStream ;
    std::ostringstream tempStringStream ;
    uint32_t requestId = 0, qty = 0 ;
    uint64_t barcode = 0 ;
    std::map <uint64_t, Totals> tmpTotalsMap ;
    std::map <uint64_t, Cart>::iterator mainIterator ;
    
    Cart* myCart = nullptr;
    ItemCodePrice itmCodePrice ;
    long rc = 0 ;
    respStringStream.str( std::string() ) ;
    respStringStream.clear() ;
	
    if (pAction == WEBI_SESSION_INIT)
    {
        cartId = this->newCart( GEN_CART_NEW ) ;
        respStringStream << "{\"status\":0,\"deviceReqId\":1,\"sessionId\":" << cartId << "}" ;
        BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - " << "InitResp - Cool" ;
    } else if (pAction == WEBI_GET_STORE_INFO) {
        respStringStream << "{\"status\":0,\"loyChannel\":" << configurationMap["MainStoreLoyChannel"] << ",\"Channel\":" << configurationMap["MainStoreChannel"] << ",\"id\":" << configurationMap["MainStoreId"] << "}" ;
        BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - " << endl << "WEBI_SESSION_GET_STORE_INFO - Cool - result:" << respStringStream.str() ;
    } else if (pAction == WEBI_GET_CARTS_LIST) {
        respStringStream << this->getCartsList( ) ;
        BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - WEBI_GET_CARTS_LIST - Cool - result:" << respStringStream.str() ;
    } else if (pAction == WEBI_ITEM_INFO) {
        if ( barcode == 0 )
        {
            barcode = strtoull(pUrlParamsMap["barcode"].c_str(), nullptr, 10);
        }
        qty = 1 ;
        
        itmCodePrice = decodeBarcode( barcode ) ;
        
        itmCodePrice.price = getItemPrice(&itemsMap[itmCodePrice.code], barcode, itmCodePrice.type) ;
        if ( itmCodePrice.type != BCODE_NOT_RECOGNIZED )
        {
            try {
                if ( itmCodePrice.code != 0 )
                {
                    respStringStream << "{" ;
                    respStringStream << "\"status\":\"0\",\"itemInfo\":{\"itemId\":\"" << barcode << "\",\"description\":\"" << itemsMap[itmCodePrice.code].getDescription() << "\",\"price\":" << fromLongToStringWithDecimals(itmCodePrice.price) << "}" ;
                    respStringStream << "}" ;
                    
                }
                else {
                    rc = BCODE_ITEM_NOT_FOUND;
                    if ((rc>0)&&(dummyRCS))
                    {
                        rc = 3 ;
                    }
                    respStringStream << "{\"status\":" << rc << "}" ;
                }
            }
            catch (std::exception const& e)
            {
                BOOST_LOG_SEV(my_logger_bs, lt::error) << "- BS - " << "Sales session error: " << e.what();
            }
            BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - " << "WEBI_ITEM_INFO - Cool - rc:" << rc << ", barcode: " << barcode << ", qty: " << qty << ",addr: " << &itemsMap[itmCodePrice.code] ;
        } else {
            rc = BCODE_NOT_RECOGNIZED ;
            if ((rc>0)&&(dummyRCS))
            {
                rc = 3 ;
            }
            respStringStream << "{\"status\":" << rc << "}" ;
        }
        
    } else if (pAction == WEBI_ACTION_NOT_RECOGNIZED) {
        rc = WEBI_ACTION_NOT_RECOGNIZED ;
        if ((rc>0)&&(dummyRCS))
        {
            rc = 3 ;
        }
        respStringStream << "{\"status\":" << rc << "}" ;
        BOOST_LOG_SEV(my_logger_bs, lt::warning) << "- BS - Web action not recognized :(" ;
    } else {
        tempStringStream.str( std::string() ) ;
        tempStringStream.clear() ;
        tempStringStream << pUrlParamsMap["devSessId"] ;
        std::string strCartId = tempStringStream.str() ;
        cartId = atoll(strCartId.c_str()) ;
        mainIterator = cartsMap.find(cartId) ;
        
        if (mainIterator != cartsMap.end()) {
            myCart = &(mainIterator->second);
            requestId = myCart->getNextRequestId() ;
            barcode = 0 ;
            switch (pAction)
            {
                case WEBI_MANAGE_RESCAN:
                {
                    bool rescanRequired = false;
                    rc = 0;
                    
                    if (pUrlParamsMap["required"].compare("true") == 0) {
                        rescanRequired = true;
                    }
                    
                    myCart->setRescan(rescanRequired);
                    BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - " << "WEBI_MANAGE_RESCAN - Cool - rc:" << rc << ", required: " << rescanRequired;
                    respStringStream << "{\"status\":" << rc << "}";
                    break;
                }
                case WEBI_ITEM_ADD:
                {
                    if ( barcode == 0 )
                    {
						barcode = strtoull(pUrlParamsMap["barcode"].c_str(), nullptr, 10);
                    }
                    qty = 1 ;
                    
                    itmCodePrice = decodeBarcode( barcode ) ;
                    
                    if ( itmCodePrice.type != BCODE_NOT_RECOGNIZED )
                    {
                        try {
                            if ( itmCodePrice.code != 0 )
                            {
								if (!cartsPriceChangesWhileShopping)
								{
									myCart->updateLocalItemMap(itemsMap[itmCodePrice.code], departmentsMap[itemsMap[itmCodePrice.code].getDepartmentCode()]);
								}
                                

                                
								itmCodePrice.price = myCart->getItemPrice(&itemsMap[itmCodePrice.code], barcode, itmCodePrice.type, cartsPriceChangesWhileShopping) ;
                                
                                respStringStream << "{" ;
                                
                                cout << "Prezzo: " << itmCodePrice.price << ", sconto: " << promoVal << ", descr: " << promotionsMap[itmCodePrice.code].getDescription() << std::endl ;
                                
                                
                                if (promotionsMap[itmCodePrice.code].getCode() == itmCodePrice.code) {
                                    // cout << "Disc: " << promotionsMap[itmCodePrice.code].toStr() << ", type: " << promotionsMap[itmCodePrice.code].getDiscountType() << std::endl;
                                    switch (promotionsMap[itmCodePrice.code].getDiscountType()) {
                                        case PROMO_TYPE_DSC_VAL:
                                            promoVal = promotionsMap[itmCodePrice.code].getDiscount();
                                            // cout << "perc" << endl;
                                            break;
                                        case PROMO_TYPE_DSC_PERC:
                                            promoVal = ( itmCodePrice.price / 100.) * promotionsMap[itmCodePrice.code].getDiscount();
                                            // cout << "val" << endl;
                                            break;
                                    }
                                }
                                
                                rc = myCart->addItemByBarcode(itemsMap[itmCodePrice.code], barcode, qty, (itmCodePrice.price), promoVal) ;
                                
                                
								if ((rc>0)&&(dummyRCS))
                                {
                                    rc = 3 ;
                                }
                                
                                if ( (rc==0) && (itemsMap[itmCodePrice.code].getLinkedBarCode()>0) )
                                {
                                    uint64_t barCodeTmp = itemsMap[itmCodePrice.code].getLinkedBarCode() ;
                                    
                                    if (!cartsPriceChangesWhileShopping)
                                    {
										myCart->updateLocalItemMap(itemsMap[barcodesMap[barCodeTmp].getItemCode()], departmentsMap[itemsMap[barcodesMap[barCodeTmp].getItemCode()].getDepartmentCode()]);
                                    }
                                    
                                    long rcLinked  = myCart->addItemByBarcode(itemsMap[barcodesMap[barCodeTmp].getItemCode()], barCodeTmp, qty, itemsMap[barcodesMap[barCodeTmp].getItemCode()].getPrice(), 0 );
									if ((rcLinked>0)&&(dummyRCS))
                                    {
                                        rcLinked = 3 ;
                                    }
									if (mainReturnSeparateLinkedBarcode)
                                    {
                                        respStringStream << "\"addItemResponse\":{\"status\":" << rcLinked << ",\"deviceReqId\":" << requestId << ",\"itemId\":\"" << barCodeTmp << "\",\"description\":\"" << itemsMap[barcodesMap[barCodeTmp].getItemCode()].getDescription() << "\",\"price\":" << fromLongToStringWithDecimals(itemsMap[barcodesMap[barCodeTmp].getItemCode()].getPrice()) << ",\"voidFlag\":\"false\",\"quantity\":1,\"itemType\":\"NormalSaleItem\"}," ;
                                    } else {
                                        itmCodePrice.price = itmCodePrice.price + itemsMap[barcodesMap[barCodeTmp].getItemCode()].getPrice() ;
                                    }
                                }
                                respStringStream << "\"addItemResponse\":{\"status\":" << rc << ",\"deviceReqId\":" << requestId << ",\"itemId\":\"" << barcode << "\",\"description\":\"" << itemsMap[itmCodePrice.code].getDescription() << "\",\"price\":" << fromLongToStringWithDecimals(itmCodePrice.price) << ",\"voidFlag\":\"false\",\"quantity\":1,\"itemType\":\"NormalSaleItem\"}" ;
  
                                
                                
                                if (promoVal != 0) {
                                    respStringStream << ",\"promoResponse\":{\"status\":0,\"deviceReqId\":" << requestId << ",\"promoItemId\":\"" << promotionsMap[itmCodePrice.code].getPromoCode() << "\",\"promoDescr\":\""  << promotionsMap[itmCodePrice.code].getDescription() << "\",\"promoValue\":-" << fromLongToStringWithDecimals(promoVal) << ",\"voidFlag\":\"false\",\"promoQty\":1}";
                                }

                                
                                tmpTotalsMap = myCart->getTotals();
                                
                                respStringStream << ",\"getTotalResponse\":{\"status\":" << rc << ",\"deviceReqId\":" << requestId << ",\"totalItems\":" << tmpTotalsMap[0].itemsNumber << ",\"totalAmount\":" << fromLongToStringWithDecimals(tmpTotalsMap[0].totalAmount) << ",\"totalDiscounts\":" << fromLongToStringWithDecimals(tmpTotalsMap[0].totalDiscount) << " ,\"amountToPay\":" << fromLongToStringWithDecimals(tmpTotalsMap[0].totalAmount) << "}}" ;
                            }
                            else {
                                rc = BCODE_ITEM_NOT_FOUND;
                                if ((rc>0)&&(dummyRCS))
                                {
                                    rc = 3 ;
                                }
                                respStringStream << "{\"status\":" << rc << ",\"deviceReqId\":0}" ;
                            }
                        }
                        catch (std::exception const& e)
                        {
                            BOOST_LOG_SEV(my_logger_bs, lt::error) << "- BS - " << "Sales session error: " << e.what();
                        }
						BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - " << "WEBI_ITEM_ADD - Cool - rc:" << rc << ", barcode: " << barcode << ", qty: " << qty << ",addr: " << &itemsMap[itmCodePrice.code] ;
                    } else {
                        rc = BCODE_NOT_RECOGNIZED ;
                        if ((rc>0)&&(dummyRCS))
                        {
                            rc = 3 ;
                        }
                        respStringStream << "{\"status\":" << rc << ",\"deviceReqId\":0}" ;
                    }
                    
                    break;
                }
                case WEBI_GET_TOTALS:
                {
                    tmpTotalsMap = myCart->getTotals();
                    respStringStream << "{\"status\":" << rc << ",\"deviceReqId\":" << requestId << ",\"totalItems\":" << tmpTotalsMap[0].itemsNumber << ",\"totalAmount\":" << fromLongToStringWithDecimals(tmpTotalsMap[0].totalAmount) << ",\"totalDiscounts\":0.0,\"amountToPay\":" << fromLongToStringWithDecimals(tmpTotalsMap[0].totalAmount) << "}";
                    BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - WEBI_GET_TOTALS - Cool - rc:" << rc ;
                    break;
                }
                case WEBI_ITEM_VOID:
                {
                    if ( barcode == 0 )
                    {
						barcode = strtoull(pUrlParamsMap["barcode"].c_str(), nullptr, 10);
                    }
                    itmCodePrice = decodeBarcode( barcode ) ;
                    
                    qty = -1 ;
                    try {
                        if (itemsMap[itmCodePrice.code].getCode() == itmCodePrice.code)
                        {
                            itmCodePrice.price = myCart->getItemPrice(&itemsMap[itmCodePrice.code], barcode, itmCodePrice.type, cartsPriceChangesWhileShopping) ;
                            
                            respStringStream << "{" ;
                            
                            rc = myCart->removeItemByBarcode(itemsMap[itmCodePrice.code], barcode, itmCodePrice.price, 0) ;
                            if ((rc>0)&&(dummyRCS))
                            {
                                rc = 3 ;
                            }
                            
                            if ( (rc==0) && (itemsMap[itmCodePrice.code].getLinkedBarCode()>0) )
                            {
                                uint64_t barCodeTmp = itemsMap[itmCodePrice.code].getLinkedBarCode() ;
                                long rcLinked  = myCart->removeItemByBarcode(itemsMap[barcodesMap[barCodeTmp].getItemCode()], barCodeTmp, itemsMap[barcodesMap[barCodeTmp].getItemCode()].getPrice(), 0 );
                                if ((rcLinked>0)&&(dummyRCS))
                                {
                                    rcLinked = 3 ;
                                }
								if (mainReturnSeparateLinkedBarcode)
                                {
                                    respStringStream << "\"addItemResponse\":{\"status\":" << rcLinked << ",\"deviceReqId\":" << requestId << ",\"itemId\":\"" << barCodeTmp << "\",\"description\":\"" << itemsMap[barcodesMap[barCodeTmp].getItemCode()].getDescription() << "\",\"price\":" << fromLongToStringWithDecimals(itemsMap[barcodesMap[barCodeTmp].getItemCode()].getPrice()) << ",\"voidFlag\":\"true\",\"quantity\":1,\"itemType\":\"NormalSaleItem\"}," ;
                                } else {
                                    itmCodePrice.price = itmCodePrice.price + itemsMap[barcodesMap[barCodeTmp].getItemCode()].getPrice() ;
                                }
                            }
                            tmpTotalsMap = myCart->getTotals();
                            respStringStream << "\"addItemResponse\":{\"status\":" << rc << ",\"deviceReqId\":" << requestId << ",\"itemId\":\"" << barcode << "\",\"description\":\"" << itemsMap[itmCodePrice.code].getDescription() << "\",\"price\":" << fromLongToStringWithDecimals(itmCodePrice.price) << ",\"voidFlag\":\"true\",\"quantity\":1,\"itemType\":\"NormalSaleItem\"}" ;
                            respStringStream << ",\"getTotalResponse\":{\"status\":" << rc << ",\"deviceReqId\":" << requestId << ",\"totalItems\":" << tmpTotalsMap[0].itemsNumber << ",\"totalAmount\":" << fromLongToStringWithDecimals(tmpTotalsMap[0].totalAmount) << ",\"totalDiscounts\":0.0,\"amountToPay\":" << fromLongToStringWithDecimals(tmpTotalsMap[0].totalAmount) << "}}" ;
                        }
                        else {
                            rc = BCODE_ITEM_NOT_FOUND;
                            if ((rc>0)&&(dummyRCS))
                            {
                                rc = 3 ;
                            }
                            respStringStream << "{\"status\":" << rc << ",\"deviceReqId\":" << requestId << "}" ;
                        }
                        BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - WEBI_ITEM_VOID - Cool - rc:" << rc ;
                    }
                    catch (std::exception const& e)
                {
                    BOOST_LOG_SEV(my_logger_bs, lt::error) << "- BS - Sales session error: " << e.what();
                }
                    break;
                }
                case WEBI_CUSTOMER_ADD:
                {
                    barcode = strtoull(pUrlParamsMap["customerId"].c_str(), nullptr, 10);
                    typedef std::map<uint64_t, uint64_t>::iterator loyCardsIteratorType;
                    for(loyCardsIteratorType cardIterator = allLoyCardsMap.begin(); cardIterator != allLoyCardsMap.end(); cardIterator++)
                    {
                        if (cardIterator->first == barcode)
                        {
                            if (cardIterator->second == cartId)
                            {
                                rc = RC_LOY_CARD_ALREADY_PRESENT ;
                            } else {
                                if (atoi(configurationMap["LoyOnlyOneShoppingSessionPerCard"].c_str())==1)
                                {
                                    rc = RC_LOY_CARD_IN_ANOTHER_TRANSACTION ;
                                }
                            }
                        }
                    }
                    
                    if (rc == 0)
                    {
                        allLoyCardsMap[barcode] = cartId ;
                        rc = myCart->addLoyCard(barcode, atoi(configurationMap["LoyMaxCardsPerTransaction"].c_str())) ;
                    }
                    
                    if ( (rc != 0) && (dummyRCS) )
                    {
                        rc = 3 ;
                    }
                    
                    BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - " << "WEBI_ADD_CUSTOMER - Cool - rc:" << rc << ", card: " << barcode ;
                    if (rc == RC_OK)
                    {
                        respStringStream << "{\"status\":" << rc << ",\"deviceReqId\":" << requestId << "}" ;
                    } else {
                        respStringStream << "{\"status\":" << rc << ",\"deviceReqId\":" << requestId << ",\"errorCode\":\"\",\"errorMessage\":\"\",\"resultExtension\":[]}" ;
                    }
                    break ;
                }
                case WEBI_CUSTOMER_VOID:
                {
                    barcode = strtoull(pUrlParamsMap["customerId"].c_str(), nullptr, 10);
                    rc = myCart->removeLoyCard(barcode) ;
					BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - " << "WEBI_REMOVE_CUSTOMER - Cool - rc:" << rc;
					if (rc == RC_OK)
                    {
                        allLoyCardsMap.erase( barcode ) ;
                        respStringStream << "{\"status\":" << rc << ",\"deviceReqId\":" << requestId << "}" ;
                    } else {
                        if ((rc>0)&&(dummyRCS))
                        {
                            rc = 3 ;
                        }
                        respStringStream << "{\"status\":" << rc << ",\"deviceReqId\":" << requestId << ",\"errorCode\":\"\",\"errorMessage\":\"\",\"resultExtension\":[]}" ;
                    }
                    
                    break;
                }
                case WEBI_SESSION_END:
                {
                    rc = myCart->sendToPos(atol(pUrlParamsMap["payStationID"].c_str()), this->configurationMap["SelfScanScanInDir"], this->configurationMap["MainStoreId"]);
                    if ((rc>0)&&(dummyRCS))
                    {
                        rc = 3 ;
                    }
                    // respStringStream << "{\"status\":" << rc << ",\"deviceReqId\":" << requestId << ",\"sessionId\":" << strCartId << ",\"terminalNum\":" << pUrlParamsMap["payStationID"] << "}" ;
                    respStringStream << "{\"status\":" << rc << ",\"deviceReqId\":" << requestId << "}" ;
                    BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - WEBI_SESSION_END - Cool - rc:" << rc ;
                    break;
                }
                case WEBI_GET_ALL_CART:
                {
                    respStringStream << myCart->getAllCartJson( itemsMap, false ) ;
                    BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - WEBI_SESSION_GET_ALL_CART - Cool - result:" << respStringStream.str() ;
                    break;
                }
                case WEBI_GET_ALL_CART_WITH_BARCODES:
                {
                    respStringStream << myCart->getAllCartJson( itemsMap, true ) ;
                    BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - WEBI_GET_ALL_CART_WITH_BARCODES - Cool - result:" << respStringStream.str() ;
                    break;
                }
                case WEBI_SESSION_VOID:
                {
                    rc = myCart->voidAllCart() ;
                    if ((rc>0)&&(dummyRCS))
                    {
                        rc = 3 ;
                    }
                    respStringStream << "{\"status\":" << rc << ",\"deviceReqId\":" << requestId << ",\"sessionId\":" << strCartId << "}" ;
                    BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - WEBI_SESSION_VOID - Cool - rc:" << rc ;
                    break;
                }
                default:
                {
                    BOOST_LOG_SEV(my_logger_bs, lt::warning) << "- BS - Web action not recognized :(, " << pAction;
                }
            }
            
		} else {
			rc = CART_NOT_FOUND;
            if ((rc>0)&&(dummyRCS))
            {
                rc = 3 ;
            }
			respStringStream << "{\"status\":" << rc << ",\"sessionId\":" << strCartId << "}";
		}
    }
    BOOST_LOG_SEV(my_logger_bs, lt::info) << "- BS - " << respStringStream.str() ;
    
    return respStringStream.str() ;
}


Item BaseSystem::getItemByIntCode( uint64_t pIntcode )
{
    return itemsMap[pIntcode];
}

uint32_t BaseSystem::newCart( unsigned int pAction )
{
    uint32_t thisCartNumber = nextCartNumber ;
    
	Cart newCart(this->basePath + "ARCHIVES/" + configurationMap["MainArchivesDir"] , thisCartNumber, pAction, this->dummyRCS);
    
    cartsMap.insert({thisCartNumber, std::move(newCart)}) ;
    //emplace non disponibile nel GCC di Debian 7
    //cartsMap.emplace( std::piecewise_construct, std::make_tuple(thisCartNumber), std::make_tuple(this->basePath,thisCartNumber,pAction) ) ;
    
    nextCartNumber++;
    return thisCartNumber ;
}

bool BaseSystem::persistCarts( )
{
    return true ;
}

string BaseSystem::getCartsList( )
{
    typedef std::map<uint64_t, Cart>::iterator carts ;
    uint64_t cartId = 0 ;
    bool firstRow = true ;
    
    Cart* tmpCart = nullptr ;
    std::ostringstream tempStringStream ;
    tempStringStream.str( std::string() ) ;
    tempStringStream.clear() ;
    tempStringStream << "{\"Carts\":{" ;
    
    for(carts iterator = cartsMap.begin(); iterator != cartsMap.end(); iterator++)
    {
        if (!firstRow)
        {
            tempStringStream << "," ;
        }
        cartId = iterator->first ;
        tempStringStream << "\"cart\":" << cartId ;
        firstRow = false ;
    }
    tempStringStream << "}}" ;
    return tempStringStream.str() ;
}

