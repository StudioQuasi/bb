//
//  fish.hpp
//  ofBBController
//
//  Created by Quasi on 8/5/19.
//

#ifndef fish_h
#define fish_h

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxInterface.h"

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

class fish : public ofxInterface::Node {

public:

    fish(int wallIndex, int controllerIndex, int driverIndex, ofVec2f loc, bool isLead, int groupIndex);
    ~fish();

    void draw(int _x, int _y, float _scaledSize);
    void update();

    void setBodyState(int _mouthState, int _bodyState);

    int getBodyState();
    
    bool isLead;
    
    void onTouchDown(ofxInterface::TouchEvent &event);
    void onTouchMove(ofxInterface::TouchEvent &event);
    void onTouchUp(ofxInterface::TouchEvent &event);
    void onTouchClick(ofxInterface::TouchEvent &event);

protected:
    
    int stateMouth;
    int stateBody;
    int displayState;

    int wallIndex;
    int controllerIndex;
    int driverIndex;

    int groupIndex;
    
    ofImage arrBassImg[6];
    ofVec2f loc;
    ofVec2f scaledSize;

    /*
    ofxPanel panel;
    ofParameterGroup panelGroup;

    ofxIntField fishID;
    ofxIntField fishControllerID;
    ofxIntSlider fishControllerIndex;
    ofParameter<int> fishGroupID;
    */

};

#endif /* fish_hpp */
