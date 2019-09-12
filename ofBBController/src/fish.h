//
//  fish.hpp
//  ofBBController
//
//  Created by Quasi on 8/5/19.
//

#ifndef fish_h
#define fish_h

#include "ofMain.h"

const int STATE_MOUTH_OPEN = 0;
const int STATE_MOUTH_CLOSE = 1;
const int STATE_BODY_TAIL = 2;
const int STATE_BODY_HEAD = 3;
const int STATE_BODY_OFF = 4;

const int DISPLAY_STATE_CLOSE = 0;
const int DISPLAY_STATE_CLOSE_TAIL = 1;
const int DISPLAY_STATE_CLOSE_HEAD = 2;
const int DISPLAY_STATE_OPEN = 3;
const int DISPLAY_STATE_OPEN_TAIL = 4;
const int DISPLAY_STATE_OPEN_HEAD = 5;

const int RENDER_SIZE = 100;
const float SCALE_FISH = .5;

class fish {
    
public:
    
    fish(int wallIndex, int controllerIndex, int driverIndex, ofVec2f loc);
    ~fish();

    void draw();
    void update();

    void setBodyState(int _mouthState, int _bodyState);

protected:
    
    int stateMouth;
    int stateBody;
    
    int displayState;

    int wallIndex;
    int controllerIndex;
    int driverIndex;

    ofImage arrBassImg[6];
    ofVec2f loc;
    ofVec2f scaledSize;
    
};

#endif /* fish_hpp */
