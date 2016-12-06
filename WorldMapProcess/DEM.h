//
//  DEM.h
//  WorldMapProcess
//
//  Created by LuClyde on 2016/12/3.
//  Copyright © 2016年 LuClyde. All rights reserved.
//
#ifndef DEM_h
#define DEM_h

#include <string>

class DEM
{
public:
    //A baseDir implies a directory has all the dem files. like a10g, b10g, c10g ... p10g
    short* getArea(const std::string& baseDir,int latiMin, int latiMax, int longiMin, int longiMax, size_t targetwidth, size_t targetheight);
    
    short* loadDEM(const std::string& demFilename, size_t targetwidth, size_t targetheight);
    
    void saveRAW(const std::string& rawFilename);
    void saveDEM(const std::string& demFilename);
    
    DEM();
    ~DEM();
private:
    
    size_t targetWidth;
    size_t targetHeight;
    
    short* wholeData;
    short* areaData;
    
    short* getMapInfos(const std::string& baseDir);
    
};

#endif /* DEM_h */
