//
//  BaseTypes.h
//  PromoCalculator
//
//  Created by Andrea Giovacchini on 05/01/14.
//  Copyright (c) 2014 Andrea Giovacchini. All rights reserved.
//

#ifndef PromoCalculator_BaseTypes_h
#define PromoCalculator_BaseTypes_h


#undef __STRICT_ANSI__ 

#if !defined _WIN32
	#define BOOST_ALL_DYN_LINK
	#define BOOST_LOG_DYN_LINK
#endif

//https://groups.google.com/forum/#!topic/cpp-netlib/gZ280gQbqcM
#define BOOST_SPIRIT_USE_PHOENIX_V3

#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/attributes/timer.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/channel_feature.hpp>
#include <boost/log/sources/channel_logger.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/exception_handler.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/filesystem.hpp>

#include <boost/thread/shared_mutex.hpp>


namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

#if defined _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <winsock2.h>
#include <winbase.h>
#include <aclapi.h>
#include <sddl.h>

#endif 


#include <sstream>




struct Totals {
    long itemsNumber ;
    long totalAmount ;
};

struct CartRow {
    int type ;
    long quantity ;
};

struct ItemCodePrice {
    unsigned long long code ;
    unsigned long price ;
    unsigned long long barcode ;
    unsigned int type ;
};

typedef Totals Totals ;


struct CardSessionRow {
    unsigned long long loyCode ;
    unsigned long long sessionId ;
};

#define ITEM        0x01
#define DEPT        0x02
#define TOTAL       0x03
#define LOY_CARD    0x04

#define CARD_PREFIX 0x10001

#define CART_STATE_READY_FOR_ITEM 0x00
#define CART_STATE_TOTAL 0x01
#define CART_STATE_ASKING 0x02
#define CART_STATE_PAYING 0x03
#define CART_STATE_CLOSED 0x20
#define CART_TMPFILE_LOADING 0x30
#define CART_NOT_INITIALIZED 0x40
#define CART_STATE_ERROR 0x50
#define CART_NOT_READY  0x98
#define CART_NOT_FOUND 0x99
#define CART_VOIDED 0x100

#define GEN_CART_NEW       0x01
#define GEN_CART_LOAD      0x02

#define RC_OK              0x00
#define RC_ERR             0x01
#define RC_LOY_CARD_NOT_PRESENT 0x90
#define RC_LOY_CARD_ALREADY_PRESENT 0x91
#define RC_LOY_CARD_IN_ANOTHER_TRANSACTION 0x92
#define RC_LOY_MAX_CARD_NUMBER 0x99

#define BCODE_EAN13         0x00
#define BCODE_EAN13_PRICE_REQ         0x01
#define BCODE_UPC           0x02
#define BCODE_EAN8          0x03
#define BCODE_CODE39        0x04
#define BCODE_CODE128       0x05

#define BCODE_ITEM_NOT_FOUND 0x03
#define BCODE_LOYCARD       0x80
#define BCODE_NOT_RECOGNIZED    0xff


#define WEBI_SESSION_INIT   0x01
#define WEBI_SESSION_END    0x02
#define WEBI_SESSION_VOID   0x03
#define WEBI_CUSTOMER_ADD   0x10
#define WEBI_CUSTOMER_VOID  0x11
#define WEBI_ITEM_ADD       0x20
#define WEBI_ITEM_VOID      0x21
#define WEBI_GET_TOTALS     0x30
#define WEBI_GET_CARTS_LIST 0x39
#define WEBI_GET_ALL_CART   0x40
#define WEBI_GET_ALL_CART_WITH_BARCODES 0x41
#define WEBI_GET_STORE_INFO 0x80
#define WEBI_ACTION_NOT_RECOGNIZED 0x80

namespace lt = boost::log::trivial;
namespace qi = boost::spirit::qi;

template <typename Iterator>
struct keys_and_values
: qi::grammar<Iterator, std::map<std::string, std::string>()>
{
    keys_and_values()
    : keys_and_values::base_type(query)
    {
        query =  pair >> *((qi::lit(';') | '&' ) >> pair);
        pair  =  key >> -('=' >> value);
        key   =  qi::char_("a-zA-Z_") >> *qi::char_("a-zA-Z_0-9");
        value = +qi::char_("a-zA-Z_0-9");
    }
    qi::rule<Iterator, std::map<std::string, std::string>()> query;
    qi::rule<Iterator, std::pair<std::string, std::string>()> pair;
    qi::rule<Iterator, std::string()> key, value;
};

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(my_logger, src::severity_logger_mt< >)

bool fileMove(std::string fileOri, std::string fileDest) ;

#endif
