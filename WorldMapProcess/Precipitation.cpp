//
//  Precipitation.cpp
//  WorldMapProcess
//
//  Created by LuClyde on 2016/12/20.
//  Copyright © 2016年 LuClyde. All rights reserved.
//

#include "Precipitation.h"


void Precipitation::addPrecipitation(const std::string filename,float* precipitationData,float& maxPrecipitation)
{
    
    FILE* pfile = fopen(filename.c_str(), "rb");
    
    char header[1440];
    fread(header, 1440, 1, pfile);
    
    int day_count = 0;
    do{
        float day_precipitation[360*180];
        fread(day_precipitation, 4, 360*180, pfile);
        day_count++;
        for(int i=0;i<360*180;++i){
            unsigned int biValue = *((unsigned int*)(&day_precipitation[i]));
            unsigned int convert = ((biValue&0xff)<<24) | ((biValue&0xff00)<<8) | ((biValue&0xff0000)>>8) | ((biValue&0xff000000)>>24);
            memcpy(day_precipitation+i, &convert, sizeof(float));
        }
        for(int i=0;i<360*180;++i){
            precipitationData[i]+=day_precipitation[i];
            if(precipitationData[i]>maxPrecipitation)maxPrecipitation = precipitationData[i];
        }
    }while(day_count<28);
    fclose(pfile);
}

void Precipitation::processPrecipitation(const std::string& baseDir){
    float precipitation[360*180] = {0};
    float maxPrecipitation = 0;
    
    std::string filedir = "precipitations/gpcp_1dd_v1.2_p1d.";
    std::string exts[12] = {"199701","199702","199703","199704","199705","199706","199707","199708","199709","199710","199711","199712"};
    
    for(int i=0;i<12;++i){
        std::string filename = baseDir+ filedir + exts[i];
        addPrecipitation(filename, precipitation,maxPrecipitation);
    }
    
    if(rawData)delete[]rawData;
    rawData = new unsigned char[360*180];
    
    for(int i=0;i<360*180;++i){
        int iSrc = (i%360<180)?i+180:i-180;
        rawData[i] = precipitation[iSrc]*255/maxPrecipitation;
    }
    
}

void Precipitation::savePrecipitation(const std::string& filename)
{
    if(rawData==0)return;
    //std::string rawfilename = baseDir + "precipitation.raw";
    FILE* rawfile = fopen(filename.c_str(), "wb");
    fwrite(rawData, 1, 360*180, rawfile);
    fclose(rawfile);
}

void Precipitation::loadPrecipitation(const std::string& filename){
    if(rawData==0) rawData = new unsigned char[360*180];
    FILE* rawfile = fopen(filename.c_str(), "rb");
    fread(rawData, 1, 360*180, rawfile);
    fclose(rawfile);
}


unsigned char Precipitation::getPrecipitationData(float preX, float preY)
{
    if(!rawData)return 0;
    
    while(preX>=360.0f)preX-=360.0f;
    while(preY>=180.0f)preY-=180.0f;
    while(preX<0.0f)preX+=360.0f;
    while(preY<0.0f)preY+=180.0f;
    
    unsigned char precipitation = rawData[int(preX)+int(preY)*360];
    if(preX<359 && preY<179){
        unsigned char p2 = rawData[int(preX+1)+int(preY)*360];
        unsigned char p3 = rawData[int(preX)+int(preY+1)*360];
        unsigned char p4 = rawData[int(preX+1)+int(preY +1)*360];
        
        float xFactor = preX - (int)preX;
        float yFactor = preY - (int)preY;
        unsigned char pTop = precipitation*(1.0f-xFactor) + p2*xFactor;
        unsigned char pDown = p3*(1.0f-xFactor)+p4*xFactor;
        
        precipitation = pTop*(1.0f-yFactor) + pDown*yFactor;
    }
    return precipitation;
}
