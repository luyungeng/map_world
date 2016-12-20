//
//  WorldMapReader.hpp
//  WorldMapProcess
//
//  Created by LuClyde on 2016/12/20.
//  Copyright © 2016年 LuClyde. All rights reserved.
//

#ifndef WorldMapReader_hpp
#define WorldMapReader_hpp

#include <stdio.h>
#include <string>

class WorldMapReader
{
public:
    WorldMapReader(int width,int height);
    
    void generateData(const std::string& baseDir);
    void readMap(const std::string& dem,const std::string& precipitation);
    
    unsigned char* getData(){return colorDate;}
    void saveData(const std::string& filename);
    
private:
    unsigned char* colorDate;
    int WIDTH;
    int HEIGHT;
    
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
    
    COLOR getColor(unsigned char temperature, unsigned char precipitation,short height,int light);
};

#endif /* WorldMapReader_hpp */
