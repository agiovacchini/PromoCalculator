//
//  Cart.cpp
//  PromoCalculator
//
//  Created by Andrea Giovacchini on 04/01/14.
//  Copyright (c) 2014 Andrea Giovacchini. All rights reserved.
//

#include "BaseTypes.h"
#include "Cart.h"

#include <sstream>
#include <stdio.h>
#include <chrono>
#include <ctime>
#include <locale>
#include <fstream>
#include <mutex>
//#include <pcap.h>
#include "boost/format.hpp"
#include "boost/filesystem.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <boost/date_time/posix_time/posix_time_io.hpp>
//static string basePath = "./" ;

using namespace std;

std::mutex cart_sendtopos_mutex ;

Cart::Cart( string pBasePath, unsigned long pNumber, unsigned int pAction )
{
    std::stringstream tempStringStream;
    std::ofstream tmpTransactionFile;
    number = pNumber ;
    itemsNumber = 0 ;
    nextRequestId = 1 ;
    loyCardsNumber = 0 ;
    basePath = pBasePath ;
    
    
    itemsMap.clear() ;
    CartRow totalCartRow = { TOTAL, 0 } ;
    
    totalsMap[0].totalAmount = 0 ;
    totalsMap[0].itemsNumber = 0 ;
    itemsMap[&totalsMap[0]] = totalCartRow ;
    cartFileName = (boost::format("%sCARTS/%010lu.cart") % basePath % number).str() ;
    tmpTransactionFileName = (boost::format("%sCARTS/%010lu.transaction_in_progress") % basePath % number).str() ;
    tmpTransactionFile.open( tmpTransactionFileName, fstream::app );
    tmpTransactionFile.close() ;

    BOOST_LOG_SEV(my_logger_ca, lt::info) << "- CART " << this->number << " - " << cartFileName << " - " << tmpTransactionFileName ;
    
    switch (pAction)
    {
        case GEN_CART_NEW:
            state = CART_STATE_READY_FOR_ITEM ;
            break ;
        case GEN_CART_LOAD:
            state = CART_TMPFILE_LOADING ;            
            break;
        default:
            break;
    }
}


void Cart::setNumber( unsigned long pNumber )
{
    this->number = pNumber ;
}

unsigned long Cart::getNumber() const
{
    return this->number ;
}

unsigned int Cart::getState() const {
    return this->state;
}

void Cart::setState( unsigned int pState ) {
    //BOOST_LOG_SEV(my_logger_ca, lt::info) << "Carrello: passaggio di stato da " << this->state << " a " << pState ;
    this->state = pState ;
}

unsigned long Cart::getRequestId()
{
    return this->nextRequestId ;
}

unsigned long Cart::getNextRequestId()
{
    this->nextRequestId++ ;
    return this->nextRequestId ;
}

unsigned int Cart::getLoyCardsNumber() const
{
    return this->loyCardsNumber ;
}
/*Totals Cart::addItemByBarcode(unsigned long long pBarcode)
 {
 //bs.itemsMap[6945339]
 }*/

void Cart::writeTransactionRow( string row )
{
    std::ofstream tmpTransactionFile;
    tmpTransactionFile.open( tmpTransactionFileName, fstream::app );
	tmpTransactionFile << boost::posix_time::to_iso_string(boost::posix_time::microsec_clock::universal_time()) << "," << row << "\n";
    tmpTransactionFile.close() ;
}

int Cart::addLoyCard( unsigned long long pLoyCardNumber, unsigned int maxCardNumber )
{
    std::stringstream tempStringStream;
    if (this->loyCardsNumber < maxCardNumber )
    {
        if ( (this->getState()==CART_STATE_READY_FOR_ITEM) || (this->getState()==CART_TMPFILE_LOADING) )
        {
            //std::cout << "{Loy card" << pLoyCardNumber << " aggiunta}" ;
            typedef std::map<unsigned int, unsigned long long>::iterator it_type;
            for(it_type iterator = loyCardsMap.begin(); iterator != loyCardsMap.end(); iterator++) {
                if (iterator->second==pLoyCardNumber)
                {
                    return RC_LOY_CARD_ALREADY_PRESENT ;
                }
            }
            loyCardsMap[loyCardsNumber] = pLoyCardNumber ;
            tempStringStream.str(std::string());
            tempStringStream.clear();
            tempStringStream << "L," << pLoyCardNumber << "," << "1" ;
            this->writeTransactionRow(tempStringStream.str() );
            this->loyCardsNumber++ ;
            return RC_OK ;
        } else {
            return RC_CART_NOT_READY ;
        }
        //respStringStream << "{Loy card" << barcode << " aggiunta}" ;

    } else {
        return RC_LOY_MAX_CARD_NUMBER ;
        //respStringStream << "{Numero massimo di carte fedelta superato}" ;
    }    
    return RC_OK ;
}

int Cart::removeLoyCard( unsigned long long pLoyCardNumber )
{
    typedef std::map<unsigned int, unsigned long long>::iterator it_type;
    for(it_type iterator = loyCardsMap.begin(); iterator != loyCardsMap.end(); iterator++) {
        if (iterator->second==pLoyCardNumber)
        {
            loyCardsMap.erase(iterator->first) ;
        }
    }
    return RC_OK ;
}

int Cart::addItemByBarcode( Item& pItem, unsigned long long pBarcode, unsigned long pPrice, unsigned int pBCodeType )
{
    long pQtyItem = 1 ;
    return addItemByBarcode(pItem, pBarcode, pQtyItem, pPrice, pBCodeType) ;
}

int Cart::addItemByBarcode( Item& pItem, unsigned long long pBarcode, unsigned long pQtyItem, unsigned long pPrice, unsigned int pBCodeType )
{
    std::stringstream tempStringStream;
    if ( (this->getState()==CART_TMPFILE_LOADING) || (this->getState()==CART_STATE_READY_FOR_ITEM) )
    {
        try {
            long qtyItem = itemsMap[&pItem].quantity + pQtyItem ;
            long qtyBarcode = barcodesMap[pBarcode] + pQtyItem ;
            
            totalsMap[0].itemsNumber = totalsMap[0].itemsNumber + pQtyItem ;
            totalsMap[0].totalAmount = totalsMap[0].totalAmount + ( pPrice * pQtyItem ) ;
            totalsMap[pItem.getDepartment().getCode()].itemsNumber = totalsMap[pItem.getDepartment().getCode()].itemsNumber + pQtyItem ;
            totalsMap[pItem.getDepartment().getCode()].totalAmount = totalsMap[pItem.getDepartment().getCode()].totalAmount + ( pPrice * pQtyItem ) ;
            
            itemsNumber = itemsNumber + pQtyItem ;
            
            itemsMap[&pItem] = { ITEM, qtyItem } ;
            barcodesMap[pBarcode] = qtyBarcode ;
            
            tempStringStream.str(std::string());
            tempStringStream.clear();
            tempStringStream << this->getState();
            
            BOOST_LOG_SEV(my_logger_ca, lt::info) << "- CART# " << this->number << " - " <<  "Cart state " << tempStringStream.str() ;
            if (this->getState()==CART_STATE_READY_FOR_ITEM)
            {
                tempStringStream.str(std::string());
                tempStringStream.clear();
                tempStringStream << "A," << pBarcode << "," << pQtyItem ;
                this->writeTransactionRow(tempStringStream.str() );
            }
        } catch (std::exception const& e)
        {
            BOOST_LOG_SEV(my_logger_ca, lt::error) << "- CART# " << this->number << " - " << "Cart addItemBarcode error: " << e.what();
        }
    }
    return RC_OK ;
}

int Cart::removeItemByBarcode( Item& pItem, unsigned long long pBarcode, unsigned long pPrice, unsigned int pBCodeType  )
{
    std::stringstream tempStringStream;
    if ( (this->getState()==CART_TMPFILE_LOADING) || (this->getState()==CART_STATE_READY_FOR_ITEM) )
    {
        
        long qtyItem ; //= itemsMap[&pItem].quantity ;
        long qtyBarcode ; //= barcodesMap[&pBarcode] ;
        
        if ((itemsMap.find(&pItem) == itemsMap.end()))
        {
            BOOST_LOG_SEV(my_logger_ca, lt::info) << "- CART# " << this->number << " - " << "Not found: " << pItem.getDescription() ;
            return RC_ERR ;
        }
        else
        {
            BOOST_LOG_SEV(my_logger_ca, lt::info) << "- CART# " << this->number << " - " << "Found: " << pItem.getDescription() ;
            qtyItem = itemsMap[&pItem].quantity - 1 ;
            qtyBarcode = barcodesMap[pBarcode] - 1 ;
            totalsMap[0].itemsNumber-- ;
            totalsMap[0].totalAmount = totalsMap[0].totalAmount - pPrice ;
            totalsMap[pItem.getDepartment().getCode()].itemsNumber-- ;
            totalsMap[pItem.getDepartment().getCode()].totalAmount = totalsMap[pItem.getDepartment().getCode()].totalAmount - pPrice;
            
            itemsNumber-- ;
            
            if (qtyItem < 1)
            {
                BOOST_LOG_SEV(my_logger_ca, lt::info) << "- CART# " << this->number << " - " << "Erasing item" ;
                itemsMap.erase(&pItem) ;
            } else
            {
                itemsMap[&pItem] = { ITEM, qtyItem } ;
            }
            
            if (qtyBarcode < 1)
            {
                barcodesMap.erase(pBarcode) ;
            } else
            {
                barcodesMap[pBarcode] = qtyBarcode ;
            }
            
            if (this->getState()==CART_STATE_READY_FOR_ITEM)
            {
				tempStringStream.str(std::string());
				tempStringStream.clear();
				tempStringStream << "R," << pBarcode << "," << "1" ;
				this->writeTransactionRow(tempStringStream.str() );
            }
        }
    }
    return RC_OK ;
}

int Cart::printCart()
{
    typedef std::map<void*, CartRow>::iterator itemRows;
    Item* itmRow ;
    //Totals* totalsRow ;
    
    BOOST_LOG_SEV(my_logger_ca, lt::info) << "- CART# " << this->number << " - " << "Print start" ;
    for(itemRows iterator = itemsMap.begin(); iterator != itemsMap.end(); iterator++) {
        CartRow key = iterator->second ;
        switch (key.type) {
            case ITEM:
                itmRow = (Item*)iterator->first;
                BOOST_LOG_SEV(my_logger_ca, lt::info) << "- CART# " << this->number << " - " << itmRow->getDescription() << " - " << itmRow->getPrice() << " - " << itmRow->getDepartment().getDescription() << " qty: " << key.quantity ;
                break;
            case DEPT:
                break;
            case TOTAL:
                //totalsRow = (Totals*)iterator->first;
                //BOOST_LOG_SEV(my_logger_ca, lt::info) << "\nTotale: " << totalsRow->totalAmount << " Item Nr.: " << totalsRow->itemsNumber ;
                break;
            default:
                break;
        }
        //BOOST_LOG_SEV(my_logger_ca, lt::info) << "\nkey: " << key ;
    }
    BOOST_LOG_SEV(my_logger_ca, lt::info) << "- CART# " << this->number << " - " << "State: " << this->getState();
    BOOST_LOG_SEV(my_logger_ca, lt::info) << "- CART# " << this->number << " - " << "Totals start" ;
    typedef std::map<unsigned long long, Totals>::iterator configurationRows;
    
    for(configurationRows iterator = totalsMap.begin(); iterator != totalsMap.end(); iterator++) {
        BOOST_LOG_SEV(my_logger_ca, lt::info) << "- CART# " << this->number << " - " << "Dept: " << iterator->first << ", value: " << iterator->second.totalAmount << ", items: " << iterator->second.itemsNumber ;
    }
    BOOST_LOG_SEV(my_logger_ca, lt::info) << "- CART# " << this->number << " - " << "Totals end" ;
    BOOST_LOG_SEV(my_logger_ca, lt::info) << "- CART# " << this->number << " - " << "Cart print end" ;
    return RC_OK ;
}

int Cart::persist( )
{
    typedef std::map<void*, CartRow>::iterator itemRows;
    typedef std::map<unsigned long long, long>::iterator barcodesRows;
    unsigned long long rowBarcode ;
    long qty = 0 ;
    
    BOOST_LOG_SEV(my_logger_ca, lt::info) << "- CART# " << this->number << " - " << "cartFileName: " << cartFileName ;
    
    std::ofstream cartFile( cartFileName.c_str() );
    
    for(barcodesRows iterator = barcodesMap.begin(); iterator != barcodesMap.end(); iterator++)
    {
        rowBarcode = (unsigned long long)iterator->first;
        qty = iterator->second ;
        cartFile << rowBarcode << "," << qty << "\n";
    }
    
    cartFile.close() ;
    return RC_OK ;
}

int Cart::sendToPos( unsigned long pPosNumber, string pScanInPath, string pStoreId )
{
    cart_sendtopos_mutex.lock() ;
    
    typedef std::map<void*, CartRow>::iterator itemRows;
    typedef std::map<unsigned long long, long>::iterator barcodesRows;
    std::stringstream tempStringStream;
    
    tempStringStream.str(std::string());
    tempStringStream.clear();

    unsigned long long rowBarcode ;
    long qty = 0 ;
	string scanInTmpFileName = (boost::format("%s/POS%03lu.TMP") % pScanInPath % pPosNumber).str();
	string scanInFileName = (boost::format("%s/POS%03lu.IN") % pScanInPath % pPosNumber).str();
	BOOST_LOG_SEV(my_logger_ca, lt::info) << "- CART# " << this->number << " - " << "Sending to pos with file: " << cartFileName ;
    boost::posix_time::time_facet *facet = new boost::posix_time::time_facet("%H%M%S%Y%m%d");
    std::stringstream date_stream;
    date_stream.imbue(std::locale(date_stream.getloc(), facet));
    date_stream << boost::posix_time::microsec_clock::universal_time();
    std::ofstream scanInFile( scanInTmpFileName.c_str() );
    
    tempStringStream << loyCardsMap.begin()->second ;
    
    scanInFile << "03;" << date_stream.str() //tsInit
        << ";" << date_stream.str() //tsEnd
        << ";" << pPosNumber
        << ";" << tempStringStream.str().substr(1,8) //customerCode
        << ";" << pStoreId //serverId
        << ";" << this->number
        << ";" << "0"
        << ";" << this->itemsNumber
        << ";" << totalsMap[0].totalAmount
        << ";" << totalsMap[0].totalAmount
        << ";" << "0" << loyCardsMap.begin()->second
        << endl;
    
    for(barcodesRows iterator = barcodesMap.begin(); iterator != barcodesMap.end(); iterator++)
    {
        rowBarcode = (unsigned long long)iterator->first;
        qty = iterator->second ;
		if (qty > 0)
		{
			for (long repetitions = 0; repetitions < qty; repetitions++ )
			{
				scanInFile << "04;" << rowBarcode << ";" << "0.00" << ";" << date_stream.str() << endl;
			}
		}
    }
    
    scanInFile.close() ;

	boost::filesystem::rename(scanInTmpFileName.c_str(), scanInFileName);
    
    cart_sendtopos_mutex.unlock() ;
    
    return RC_OK ;
}

string Cart::getAllCartJson( )
{
    typedef std::map<void*, CartRow>::iterator itemRows ;
    typedef std::map<unsigned long long, long>::iterator barcodesRows ;
    typedef std::map<unsigned int, unsigned long long>::iterator loyCardRows ;
    
    std::stringstream tempStringStream;
    
    tempStringStream.str(std::string());
    tempStringStream.clear();
    
    unsigned long long rowBarcode, rowCardCode ;
    long qty = 0 ;
    bool firstRow = true ;
    
	BOOST_LOG_SEV(my_logger_ca, lt::info) << "- CART# " << this->number << " - Getting full cart in JSON format" ;
    
    
    tempStringStream << "{\"loyCards\":{" ;
    firstRow = true ;
    for(loyCardRows iterator = loyCardsMap.begin(); iterator != loyCardsMap.end(); iterator++)
    {
        rowCardCode = (unsigned long long)iterator->second;
        tempStringStream << rowCardCode ;
        firstRow = false ;
    }
    tempStringStream << "},\"items:\"{" ;
    
    firstRow = true ;
    for(barcodesRows iterator = barcodesMap.begin(); iterator != barcodesMap.end(); iterator++)
    {
        rowBarcode = (unsigned long long)iterator->first;
        qty = iterator->second ;
		if (qty > 0)
		{
			for (long repetitions = 0; repetitions < qty; repetitions++ )
			{
                if (!firstRow)
                {
                    tempStringStream << "," ;
                }
				tempStringStream << "{\"barcode\":" << rowBarcode << "}" << endl;
                firstRow = false ;
			}
		}
    }
    
    tempStringStream << "}" ;
    
    return tempStringStream.str() ;
}

std::map <unsigned long long, Totals> Cart::getTotals()
{
    return totalsMap ;
}

int Cart::close()
{
    //tmpTransactionFile.close() ;
    state = CART_STATE_CLOSED ;
    return RC_OK ;
}