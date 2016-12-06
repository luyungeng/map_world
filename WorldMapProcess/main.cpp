//
//  main.cpp
//  WorldMapProcess
//
//  Created by LuClyde on 2016/11/22.
//  Copyright © 2016年 LuClyde. All rights reserved.
//

#include <iostream>
#include <string>
#include <vector>

#include "DEM.h"

const std::string baseDir = "/Users/ClydeLu/Documents/git/worldMap/dates/";


void addPrecipitation(const std::string filename, float* precipitation, float& maxPrecipitation){
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
            precipitation[i]+=day_precipitation[i];
            if(precipitation[i]>maxPrecipitation)maxPrecipitation = precipitation[i];
        }
    }while(day_count<28);
    fclose(pfile);
}

unsigned char* processPrecipitation(){
    float precipitation[360*180] = {0};
    float maxPrecipitation = 0;
    
    std::string filedir = "precipitations/gpcp_1dd_v1.2_p1d.";
    std::string exts[12] = {"199701","199702","199703","199704","199705","199706","199707","199708","199709","199710","199711","199712"};
    
    for(int i=0;i<12;++i){
        std::string filename = baseDir+ filedir + exts[i];
        addPrecipitation(filename, precipitation,maxPrecipitation);
    }
    
    unsigned char* rawData = new unsigned char[360*180];
    for(int i=0;i<360*180;++i){
        int iSrc = (i%360<180)?i+180:i-180;
        rawData[i] = precipitation[iSrc]*255/maxPrecipitation;
    }
    
    std::string rawfilename = baseDir + "precipitation.raw";
    FILE* rawfile = fopen(rawfilename.c_str(), "wb");
    fwrite(rawData, 1, 360*180, rawfile);
    fclose(rawfile);
    
    return rawData;
}

struct COLOR
{
    COLOR():r(0),g(0),b(0){}
    COLOR(unsigned char _r,unsigned char _g,unsigned char _b):r(_r),g(_g),b(_b){}
    unsigned char r;
    unsigned char g;
    unsigned char b;
    COLOR operator*(float m){
        return COLOR(clamp(r*m),clamp(g*m),clamp(b*m));
    }
    COLOR operator+(COLOR c){
        return COLOR(clamp(r+c.r),clamp(g+c.g),clamp(b+c.b));
    }
    unsigned char clamp(float v){
        if(v>=255)return 255;
        if(v<=0)return 0;
        return (unsigned char)v;
    }
};
COLOR getColor(unsigned char temperature, unsigned char precipitation,short height,int light){
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

int main(int argc, const char * argv[]) {

    int WIDTH = 2048;
    int HEIGHT = 1024;
    
    bool GET_PEM_ORI_DATE = false;
    if(GET_PEM_ORI_DATE){
        DEM dem;
        dem.getArea(baseDir+"all10", -179, 180, -90, 90,WIDTH,HEIGHT);
        dem.saveDEM(baseDir+"mapdate.dem");
    }
    
    unsigned char* preDate = processPrecipitation();
    
    DEM dem;
    short* demDate = dem.loadDEM(baseDir+"mapdate.dem", WIDTH, HEIGHT);
    //dem.saveRAW(baseDir+"mapdate.raw");
    
    //unsigned char* tempratureDate = new unsigned char[1024*512];
    unsigned char* colorDate = new unsigned char[HEIGHT*WIDTH*3];
    for(int y=0;y<HEIGHT;++y){
        for(int x=0;x<WIDTH;++x){
            short height = demDate[y*WIDTH+x];
            
            float preX = x*360.0f/WIDTH;
            float preY = y*180.0f/HEIGHT;
            unsigned char precipitation = preDate[int(preX)+int(preY)*360];
            if(preX<359 && preY<179){
                unsigned char p2 = preDate[int(preX+1)+int(preY)*360];
                unsigned char p3 = preDate[int(preX)+int(preY+1)*360];
                unsigned char p4 = preDate[int(preX+1)+int(preY +1)*360];
                
                float xFactor = preX - (int)preX;
                float yFactor = preY - (int)preY;
                unsigned char pTop = precipitation*(1.0f-xFactor) + p2*xFactor;
                unsigned char pDown = p3*(1.0f-xFactor)+p4*xFactor;
                
                precipitation = pTop*(1.0f-yFactor) + pDown*yFactor;
            }
            
            
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
    
//    std::string tempratureFile = baseDir + "temprature.raw";
//    FILE* fw = fopen(tempratureFile.c_str(), "wb");
//    fwrite(tempratureDate, 1, 1024*512, fw);
//    fclose(fw);
        std::string colorFile = baseDir + "color.raw";
        FILE* fw = fopen(colorFile.c_str(), "wb");
        fwrite(colorDate, 3, WIDTH*HEIGHT, fw);
        fclose(fw);
    
    return 0;
}
