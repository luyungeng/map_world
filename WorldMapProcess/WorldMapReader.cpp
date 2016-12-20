//
//  WorldMapReader.cpp
//  WorldMapProcess
//
//  Created by LuClyde on 2016/12/20.
//  Copyright © 2016年 LuClyde. All rights reserved.
//

#include "WorldMapReader.h"
#include "DEM.h"
#include "Precipitation.h"

#include <iostream>

WorldMapReader::WorldMapReader(int width,int height)
:WIDTH(width)
,HEIGHT(height)
{
    
}

void WorldMapReader::readMap(const std::string& demFile, const std::string& precipitionFile){
    
    DEM dem;
    short* demDate = dem.loadDEM(demFile, WIDTH, HEIGHT);
    //dem.saveRAW(baseDir+"mapdate.raw");
    Precipitation pre;
    pre.loadPrecipitation(precipitionFile);
    
    //unsigned char* tempratureDate = new unsigned char[1024*512];
    if(colorDate)delete[]colorDate;
    colorDate = new unsigned char[HEIGHT*WIDTH*3];
    
    for(int y=0;y<HEIGHT;++y){
        for(int x=0;x<WIDTH;++x){
            short height = demDate[y*WIDTH+x];
            
            float preX = x*360.0f/WIDTH;
            float preY = y*180.0f/HEIGHT;
            
            unsigned char precipitation =  pre.getPrecipitationData(preX, preY);
            
            int light = (y==0?0:(height-demDate[(y-1)*WIDTH+x]));
            //if(y<256)light = -light;
            
            int temprature = 255 - abs((int)(((float)y/HEIGHT-0.5f)*512.0f*0.8f)) - (height+500)*216/9251  - precipitation*0.3;
            if(temprature<0)temprature=0;
            if(temprature>255)temprature=255;
            
            //tempratureDate[y*1024+x] = temprature;
            COLOR color = getColor(temprature, precipitation,height,light);
            colorDate[(y*WIDTH+x)*3] = color.r;
            colorDate[(y*WIDTH+x)*3+1] = color.g;
            colorDate[(y*WIDTH+x)*3+2] = color.b;
        }
    }
}

WorldMapReader::COLOR WorldMapReader::getColor(unsigned char temperature, unsigned char precipitation,short height,int light){
    COLOR color;
    
    COLOR colorDesert(238, 211, 117);
    COLOR colorGreen(49,146,1);
    COLOR colorSnow(255,255,255);
    COLOR colorSea(27,40,75);
    COLOR colorGray(127,127,127);
    
    if(height<=-500)return colorSea;
    
    float p = precipitation/255.0f;
    float pM = 2.4f;
    p = (p*pM)>1.0?1.0:p*pM;
    color = colorDesert*(1.0f-p) + colorGreen*p;
    
    float t = temperature/255.0f;
    float tM = 2.0f;
    t = (t*tM)>1.0?1.0:t*tM;
    color = color*t + colorSnow*(1.0f - t);
    
    float l = light/4000.0f+1.0f;
    if(l>1.5f)l=1.5f;
    if(l<0.5f)l=0.5f;
    color = color*l;
    
    return color;
}

void WorldMapReader::saveData(const std::string& filename)
{
    FILE* fw = fopen(filename.c_str(), "wb");
    fwrite(colorDate, 3, WIDTH*HEIGHT, fw);
    fclose(fw);
}

void WorldMapReader::generateData(const std::string& baseDir)
{
    DEM dem;
    dem.getArea(baseDir+"all10", -179, 180, -90, 90,WIDTH,HEIGHT);
    dem.saveDEM(baseDir+"mapdate.dem");
    
    Precipitation pre;
    pre.processPrecipitation(baseDir);
    pre.savePrecipitation(baseDir+"precipitation.raw");
}
