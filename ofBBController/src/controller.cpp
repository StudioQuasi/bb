//
//  controller.cpp
//  ofBBController
//
//  Created by Quasi on 9/9/19.
//

#include "controller.h"

controller::controller(int _controllerIndex, fish* _arrFish[3])
{
    
    controllerIndex = _controllerIndex;
    
    for (int _i=0; _i<CONTROLLER_NUM_FISH; _i++)
    {
        arrFish[_i] = _arrFish[_i];
    }

}

controller::~controller()
{
    
}

void controller::draw()
{
    
}
