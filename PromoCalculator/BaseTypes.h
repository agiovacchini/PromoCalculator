//
//  BaseTypes.h
//  PromoCalculator
//
//  Created by Andrea Giovacchini on 05/01/14.
//  Copyright (c) 2014 Andrea Giovacchini. All rights reserved.
//

#ifndef PromoCalculator_BaseTypes_h
#undef __STRICT_ANSI__ 
#define BOOST_ALL_DYN_LINK

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


#define PromoCalculator_BaseTypes_h

struct Totals {
    long itemsNumber ;
    long totalAmount ;
};

struct CartRow {
    int type ;
    long quantity ;
};

typedef Totals Totals ;


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

#define GEN_CART_NEW       0x01
#define GEN_CART_LOAD      0x02

#define RC_OK              0x00
#define RC_ERR             0x01
#define RC_LOY_MAX_CARD_NUMBER 0x100

#define BCODE_EAN13         0x00
#define BCODE_EAN13_PRICE_REQ         0x01
#define BCODE_UPC           0x02
#define BCODE_EAN8          0x03
#define BCODE_CODE39        0x04
#define BCODE_CODE128       0x05

#define BCODE_LOYCARD       0x80
#define BCODE_NOT_RECOGNIZED    0xff


/**
 * Global logger - Start
 * http://www.boost.org/doc/libs/1_54_0/libs/log/doc/html/log/detailed/sources.html
*/


enum severity_level
{
    info,
    normal,
    warning,
    error,
    fatal
};

struct my_handler
{
    typedef void result_type;
    
    void operator() (std::runtime_error const& e) const
    {
        std::cout << "std::runtime_error: " << e.what() << std::endl;
    }
    void operator() (std::logic_error const& e) const
    {
        std::cout << "std::logic_error: " << e.what() << std::endl;
        throw;
    }
};
    

// A logger class that allows to intercept exceptions and supports severity level
class my_logger_mt :
public src::basic_composite_logger<
    char
    ,my_logger_mt
    ,src::multi_thread_model< boost::shared_mutex >
    ,src::features<
        src::severity< severity_level >
        ,src::exception_handler
    >
>
{
    BOOST_LOG_FORWARD_LOGGER_MEMBERS(my_logger_mt)
};


BOOST_LOG_INLINE_GLOBAL_LOGGER_INIT(my_logger, my_logger_mt)
{
    my_logger_mt lg (keywords::format =
                     (
                      expr::stream
                      << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
                      << ": <" << logging::trivial::severity
                      << "> " << expr::smessage
                      )
                     );
    
    // Set up exception handler: all exceptions that occur while
    // logging through this logger, will be suppressed
    lg.set_exception_handler(logging::make_exception_suppressor());
    
    return lg;
}

/**
 * Global logger - End
 */


#endif
