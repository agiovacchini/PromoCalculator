//
//  BaseTypes.cpp
//  PromoCalculator
//
//  Created by Andrea Giovacchini on 03/03/14.
//  Copyright (c) 2014 Andrea Giovacchini. All rights reserved.
//

#include "BaseTypes.h"

bool fileMove(std::string fileOri, std::string fileDest)
{
    boost::filesystem::path src(fileOri);
    boost::filesystem::path dest(fileDest);
    
    try {
        boost::filesystem::rename(src, dest);
    }
    catch (std::exception const& e)
    {
        return false;
    }
    return boost::filesystem::exists(dest);
}
