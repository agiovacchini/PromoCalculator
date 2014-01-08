//
//  BaseSystem.cpp
//  PromoCalculator
//
//  Created by Andrea Giovacchini on 08/01/14.
//  Copyright (c) 2014 Andrea Giovacchini. All rights reserved.
//

#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <boost/spirit/include/qi.hpp>

#include "BaseSystem.h"

namespace qi = boost::spirit::qi;
using namespace std;

void BaseSystem::readDepartmentArchive( string pFileName )
{
    //http://stackoverflow.com/questions/18365463/how-to-parse-csv-using-boostspirit
    
    string configFilePath = "/Users/andreagiovacchini/Documents/Sviluppo/Siti/PromoCalculator/PromoCalculator/ARCHIVES/" ;
    
    // Tokenizer
    typedef boost::tokenizer< boost::escaped_list_separator<char> , std::string::const_iterator, std::string> Tokenizer;
    boost::escaped_list_separator<char> seps('\\', ',', '\"');
    
    
    std::ifstream archiveFile( configFilePath + pFileName );
    if (!archiveFile) {
        std::cout << "\nFile " + pFileName + " not found" ;
        exit(-1);
    } else {
        std::cout << "\nFile " + pFileName + " found" ;
    }
    
    std::string line;
    while( std::getline(archiveFile, line) )
    {
        qi::rule<std::string::const_iterator, std::string()> quoted_string = '"' >> *(qi::char_ - '"') >> '"';
        qi::rule<std::string::const_iterator, std::string()> valid_characters = qi::char_ - '"' - ',';
        qi::rule<std::string::const_iterator, std::string()> item = *(quoted_string | valid_characters );
        qi::rule<std::string::const_iterator, std::vector<std::string>()> csv_parser = item % ',';
        
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
                    tempDepartment.setCode(atol(i.c_str())) ;
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
    std::cout << "\nFinished loading file " + pFileName ;

}

void BaseSystem::readItemArchive( string pFileName )
{
    //http://stackoverflow.com/questions/18365463/how-to-parse-csv-using-boostspirit
    
    string configFilePath = "/Users/andreagiovacchini/Documents/Sviluppo/Siti/PromoCalculator/PromoCalculator/ARCHIVES/" ;
    
    // Tokenizer
    typedef boost::tokenizer< boost::escaped_list_separator<char> , std::string::const_iterator, std::string> Tokenizer;
    boost::escaped_list_separator<char> seps('\\', ',', '\"');
    
    
    std::ifstream archiveFile( configFilePath + pFileName );
    if (!archiveFile) {
        std::cout << "\nFile " + pFileName + " not found" ;
        exit(-1);
    } else {
        std::cout << "\nFile " + pFileName + " loaded" ;
    }
    
    std::string line;
    while( std::getline(archiveFile, line) )
    {
        qi::rule<std::string::const_iterator, std::string()> quoted_string = '"' >> *(qi::char_ - '"') >> '"';
        qi::rule<std::string::const_iterator, std::string()> valid_characters = qi::char_ - '"' - ',';
        qi::rule<std::string::const_iterator, std::string()> item = *(quoted_string | valid_characters );
        qi::rule<std::string::const_iterator, std::vector<std::string>()> csv_parser = item % ',';
        
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
                    tempItm.setCode(atol(i.c_str())) ;
                    break;
                case 2:
                    tempItm.setDescription(i) ;
                    break;
                case 16:
                    tempItm.setDepartment(deparmentsMap[atol(i.c_str())]);
                    break;
                case 26:
                    tempItm.setPrice(atof(i.c_str())/100.) ;
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
    std::cout << "\nFinished loading file " + pFileName ;
}

void BaseSystem::readBarcodesArchive( string pFileName )
{
    //http://stackoverflow.com/questions/18365463/how-to-parse-csv-using-boostspirit
    
    string configFilePath = "/Users/andreagiovacchini/Documents/Sviluppo/Siti/PromoCalculator/PromoCalculator/ARCHIVES/" ;
    
    // Tokenizer
    typedef boost::tokenizer< boost::escaped_list_separator<char> , std::string::const_iterator, std::string> Tokenizer;
    boost::escaped_list_separator<char> seps('\\', ',', '\"');
    
    
    std::ifstream archiveFile( configFilePath + pFileName );
    if (!archiveFile) {
        std::cout << "\nFile " + pFileName + " not found" ;
        exit(-1);
    } else {
        std::cout << "\nFile " + pFileName + " loaded" ;
    }
    
    std::string line;
    while( std::getline(archiveFile, line) )
    {
        qi::rule<std::string::const_iterator, std::string()> quoted_string = '"' >> *(qi::char_ - '"') >> '"';
        qi::rule<std::string::const_iterator, std::string()> valid_characters = qi::char_ - '"' - ',';
        qi::rule<std::string::const_iterator, std::string()> item = *(quoted_string | valid_characters );
        qi::rule<std::string::const_iterator, std::vector<std::string>()> csv_parser = item % ',';
        
        // Boost Spirit Qi
        
        std::string::const_iterator s_begin = line.begin();
        std::string::const_iterator s_end = line.end();
        std::vector<std::string> result;
        
        bool r = boost::spirit::qi::parse(s_begin, s_end, csv_parser, result);
        assert(r == true);
        assert(s_begin == s_end);
        int column = 0 ;
        Barcodes tempBarcode ;
        for (auto i : result)
        {
            //std::cout << "ggg" << i << std::endl;
            switch (column)
            {
                case 1:
                    tempBarcode.setCode(atol(i.c_str())) ;
                    break;
                case 2:
                    tempBarcode.setItemCode(atol(i.c_str())) ;
                    break;
            }
            column++ ;
        }
        //std::cout << "\n" << tempBarcode.toStr() ;
        barcodesMap[tempBarcode.getCode()] = tempBarcode ;
        
        //std::cout << "\n" << itemsMap[tempItm.getCode()].toStr();
        
    }
    std::cout << "\nFinished loading file " + pFileName ;
}

void BaseSystem::readArchives()
{
    this->readDepartmentArchive( "DEPARTMENTS.CSV" ) ;
    this->readItemArchive( "ITEMS.CSV" ) ;
    this->readBarcodesArchive( "BARCODES.CSV" ) ;
}

Item BaseSystem::getItemByIntCode( unsigned long long pIntcode )
{
    return itemsMap[pIntcode];
}
