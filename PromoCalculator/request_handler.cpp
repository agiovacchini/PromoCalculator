//
// request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include "BaseTypes.h"


#include "request_handler.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/spirit/include/qi.hpp>.
#include <boost/fusion/include/std_pair.hpp>
#include "mime_types.hpp"
#include "reply.hpp"
#include "request.hpp"


namespace http {
    namespace server3 {
        
        request_handler::request_handler(const std::string& doc_root)
        : doc_root_(doc_root)
        {
        }
        
        void request_handler::handle_request(const request& req, reply& rep)
        {
            // Decode url to path.
            std::string request_path;
            if (!url_decode(req.uri, request_path))
            {
                rep = reply::stock_reply(reply::bad_request);
                return;
            }
            
            // Request path must be absolute and not contain "..".
            if (request_path.empty() || request_path[0] != '/'
                || request_path.find("..") != std::string::npos)
            {
                rep = reply::stock_reply(reply::bad_request);
                return;
            }
            
            // If path ends in slash (i.e. is a directory) then add "index.html".
            if (request_path[request_path.size() - 1] == '/')
            {
                request_path += "index.html";
            }
            
            // Determine the file extension.
            std::size_t last_slash_pos = request_path.find_last_of("/");
            std::size_t last_dot_pos = request_path.find_last_of(".");
            std::string extension;
            if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
            {
                extension = request_path.substr(last_dot_pos + 1);
            }
            

            
            
            
            
            /*GIOVAC
             *
             * Gestisci qui le tue risposte
             *
             */
            // Open the file to send back.
            /*std::string full_path = doc_root_ + request_path;
            std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
            if (!is)
            {
                rep = reply::stock_reply(reply::not_found);
                return;
            }*/
            
            std::cout << "request_path: " << request_path << std::endl ;
            std::string servlet = "" ;
            std::string servletFunction = "" ;
            std::string servletFunctionAction = "" ;
            std::string servletFunctionPar = "" ;

            
            //boost::regex exrp( "((.*))([?]+)((.*))" );
            boost::regex exrp( "(/(.*))(/(.*))(/(.*))([?]+)((.*))" );
            boost::match_results<std::string::const_iterator> what;
            if( regex_search( request_path, what, exrp ) ) {
                servlet = std::string( what[2].first, what[2].second );
                servletFunction = std::string( what[4].first, what[4].second );
                servletFunctionAction = std::string( what[6].first, what[6].second );
                servletFunctionPar = std::string( what[8].first, what[8].second );
                
                std::cout << "servlet: " << servlet << std::endl << "servletFunction: " << servletFunction << std::endl << "servletFunctionAction: " << servletFunctionAction << std::endl << "servletFunctionPar: " << servletFunctionPar << std::endl ;
            }
            
            
            
            //std::string input("key1=value1&key3=value3");  // input to parse
            std::string::iterator begin = servletFunctionPar.begin();
            std::string::iterator end = servletFunctionPar.end();
            
            keys_and_values<std::string::iterator> paramsParser;    // create instance of parser
            std::map<std::string, std::string> urlParamsMap;        // map to receive results
            bool result = qi::parse(begin, end, paramsParser, urlParamsMap);   // returns true if successful
            typedef std::map<std::string, std::string>::iterator urlParamRows;
            std::cout << "Url params:" ;
            for(urlParamRows iterator = urlParamsMap.begin(); iterator != urlParamsMap.end(); iterator++) {
                std::cout << "\t" << iterator->first << " " << iterator->second << std::endl ;
            }
            
            
            
            // Fill out the reply to be sent to the client.
            rep.status = reply::ok;
            char buf[512];
            
            //while (is.read(buf, sizeof(buf)).gcount() > 0)
                //rep.content.append(buf, is.gcount());
            
            rep.content.append("CiaoCiao");
            rep.headers.resize(2);
            rep.headers[0].name = "Content-Length";
            rep.headers[0].value = boost::lexical_cast<std::string>(rep.content.size());
            rep.headers[1].name = "Content-Type";
            rep.headers[1].value = mime_types::extension_to_type(extension);
        }
        
        bool request_handler::url_decode(const std::string& in, std::string& out)
        {
            out.clear();
            out.reserve(in.size());
            for (std::size_t i = 0; i < in.size(); ++i)
            {
                if (in[i] == '%')
                {
                    if (i + 3 <= in.size())
                    {
                        int value = 0;
                        std::istringstream is(in.substr(i + 1, 2));
                        if (is >> std::hex >> value)
                        {
                            out += static_cast<char>(value);
                            i += 2;
                        }
                        else
                        {
                            return false;
                        }
                    }
                    else
                    {
                        return false;
                    }
                }
                else if (in[i] == '+')
                {
                    out += ' ';
                }
                else
                {
                    out += in[i];
                }
            }
            return true;
        }
        
    } // namespace server3
} // namespace http