//
//  Precipitation.hpp
//  WorldMapProcess
//
//  Created by LuClyde on 2016/12/20.
//  Copyright © 2016年 LuClyde. All rights reserved.
//

#ifndef Precipitation_hpp
#define Precipitation_hpp

#include <stdio.h>
#include <string>

class Precipitation{
public:
    Precipitation():rawData(0){}
    void processPrecipitation(const std::string& baseDir);
    
    void savePrecipitation(const std::string& filename);
    void loadPrecipitation(const std::string& filename);
    
    unsigned char getPrecipitationData(float x, float y);
    unsigned char* getPrecipitationData(){return rawData;}
    
private:
    void addPrecipitation(const std::string filename,float* mPrecipitationData, float& maxPrecipitation);
    unsigned char* rawData;
};


#endif /* Precipitation_hpp */
