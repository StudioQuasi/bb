//
//  controller.hpp
//  ofBBController
//
//  Created by Quasi on 9/9/19.
//

#ifndef controller_h
#define controller_h

const int CONTROLLER_NUM_FISH = 3;

#include "ofMain.h"
#include "fish.h"

class controller {
    
public:
    
    controller(int controllerIndex, fish* arrFish[3]);
    ~controller();
    
    void draw();
    
protected:

    fish *arrFish[3];

    int controllerIndex;
    
    ofVec2f loc;
    
};

#endif /* fish_hpp */

