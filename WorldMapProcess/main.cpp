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
#include "Precipitation.h"
#include "WorldMapReader.h"



int main(int argc, const char * argv[]) {

    int WIDTH = 2048;
    int HEIGHT = 1024;
    const std::string baseDir = "/Users/ClydeLu/Documents/git/worldMap/dates/";
    
    WorldMapReader reader(WIDTH,HEIGHT);
    //reader.generateData(baseDir);
    reader.readMap(baseDir+"mapdate.dem", baseDir+"precipitation.raw");
    reader.saveData(baseDir + "color.raw");
    
    return 0;
}
