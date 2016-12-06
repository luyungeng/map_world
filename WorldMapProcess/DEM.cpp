//
//  DEM.cpp
//  WorldMapProcess
//
//  Created by LuClyde on 2016/12/3.
//  Copyright © 2016年 LuClyde. All rights reserved.
//
#include <iostream>
#include <string>
#include <vector>

#include "DEM.h"

const size_t totalRows = 4800+6000 +4800+6000;
const size_t totalColumns = 10800*4;
const size_t totalSize = totalRows*totalColumns;

DEM::DEM()
:targetWidth(0),targetHeight(0),areaData(0),wholeData(0)
{
    
}

DEM::~DEM(){
    if(areaData)delete[]areaData;
    if(wholeData)delete[]wholeData;
}

struct mapInfo{
    std::string filename;
    int latitudeMin;
    int latitudeMax;
    int longitudeMin;
    int longitudeMax;
    int elevationMin;
    int elevationMax;
    int columns;
    int rows;
    short* data;
};

short* DEM::getMapInfos(const std::string& baseDir){
    if(wholeData==0){
        std::cout<<"get map info..."<<std::endl;
        std::vector<mapInfo> infos;
        infos.resize(16);
        infos[0]={"/a10g",    50,90,   -180,-90,  1,6098,        10800,4800, 0};
        infos[1]={"/b10g",    50,90,   -90,0,       1,3940,        10800,4800, 0};
        infos[2]={"/c10g",    50,90,   0,90,        -30,4010,     10800,4800, 0};
        infos[3]={"/d10g",    50,90,   90,180,    1,4588,        10800,4800, 0};
        infos[4]={"/e10g",    0,50,     -180,-90,  -84,5443,    10800,6000, 0};
        infos[5]={"/f10g",     0,50,     -90,0,       -40,6085 ,   10800,6000, 0};
        infos[6]={"/g10g",    0,50,     0,90,        -407,8752,   10800,6000, 0};
        infos[7]={"/h10g",    0,50,     90,180,    -63,7491,     10800,6000, 0};
        infos[8]={"/i10g",     -50,0,    -180,-90,  1,2732,        10800,6000, 0};
        infos[9]={"/j10g",     -50,0,    -90,0,       -127,6798,   10800,6000, 0};
        infos[10]={"/k10g",  -50,0,    0,90,        1,5825,        10800,6000, 0};
        infos[11]={"/l10g",   -50,0,    90,180,    1,5179,        10800,6000, 0};
        infos[12]={"/m10g", -90,-50, -180,-90, 1,4009,         10800,4800, 0};
        infos[13]={"/n10g",  -90,-50, -90,0,      1,4743,         10800,4800, 0};
        infos[14]={"/o10g",  -90,-50, 0,90,       1,4039,         10800,4800, 0};
        infos[15]={"/p10g",  -90,-50, 90,180,   1,4363,         10800,4800, 0};
        
        if(wholeData)delete[]wholeData;
        wholeData = new short[totalSize];
        
        for(int i=0;i<infos.size();++i){
            std::cout<<infos[i].filename<<std::endl;
            std::string filepath = baseDir+infos[i].filename;
            size_t count = infos[i].columns*infos[i].rows;
            short* readdate =  new short[count];
            
            FILE* fr = fopen(filepath.c_str(), "rb");
            fread(readdate, sizeof(short), count, fr);
            fclose(fr);
            
            int blockRow = i/4;;
            int blockColumn = i%4;
            
            size_t xoffset = blockColumn*infos[0].columns;
            size_t yoffset = (blockRow==0?0:(blockRow==1?4800:(blockRow==2?10800:16800)));
            
            for(size_t x=0;x<infos[i].columns;x++){
                for(size_t y=0;y<infos[i].rows;y++){
                    size_t srcoffset = y*infos[i].columns + x;
                    size_t taroffset = (y+yoffset)*totalColumns + x+xoffset;
                    short info = readdate[srcoffset];
                    wholeData[taroffset] = info;
                }
            }
        }
    }
    return wholeData;
}


short* DEM::getArea(const std::string& baseDir, int latiMin, int latiMax, int longiMin, int longiMax, size_t targetwidth, size_t targetheight){
    
    if(wholeData) {delete[]wholeData; wholeData = 0;}
    if(areaData){delete[] areaData; areaData = 0;}
    
    targetWidth = targetwidth;
    targetHeight = targetheight;
    
    getMapInfos(baseDir);
    
    size_t xoffset = (latiMin+180)*totalColumns/360;
    size_t width = (latiMax-latiMin)*totalColumns/360;
    size_t yoffset = (longiMin+90)*totalRows/180;
    size_t height = (longiMax-longiMin)*totalRows/180;
    
    areaData = new short[targetwidth*targetheight];
    for(size_t x=0;x<targetwidth;++x){
        for(size_t y=0;y<targetheight;++y){
            size_t srcX = x*width/targetwidth;
            size_t srcY = y*height/targetheight;
            short data = wholeData[(yoffset+srcY)*totalColumns+xoffset+srcX];
            areaData[y*targetwidth+x] = data;
        }
    }
    return areaData;
}


void DEM::saveRAW(const std::string& rawFilename){
    
    if(!areaData)return;
    
    unsigned char* writedata =  new unsigned char[targetWidth*targetHeight];
    int divid = 8751+500;
    for(size_t i=0;i<targetHeight*targetWidth;i++){
        short info = areaData[i]+499;
        if(info>divid)info=divid-1;
        else if(info<=0)info=0;
        else {
            info=255*info/divid;
        }
        writedata[i]=info;
    }
    
    FILE* fw = fopen(rawFilename.c_str(), "wb");
    fwrite(writedata, 1, targetWidth*targetHeight, fw);
    fclose(fw);
    
    delete[] writedata;
}
void DEM::saveDEM(const std::string& demFilename)
{
    if(areaData==0)return;
    
    FILE* fw = fopen(demFilename.c_str(), "wb");
    fwrite(areaData, sizeof(short), targetWidth*targetHeight, fw);
    fclose(fw);
}

short* DEM::loadDEM(const std::string& pemFilename, size_t targetwidth, size_t targetheight){
    targetWidth = targetwidth;
    targetHeight = targetheight;
    
    if(areaData) delete[] areaData;
    areaData = new short[targetheight*targetwidth];
    
    FILE* fw = fopen(pemFilename.c_str(), "rb");
    fread(areaData, sizeof(short), targetWidth*targetHeight, fw);
    fclose(fw);
    
    return  areaData;
}
