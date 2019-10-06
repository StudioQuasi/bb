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

using namespace ofxInterface;

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

const int RENDER_SIZE = 275;
const float SCALE_FISH = .5;

const int FISH_IMG_WIDTH = 500;
const int FISH_IMG_HEIGHT = 304;

class fish : public ofxInterface::Node {

public:

    void setup(int wallIndex, char controllerIndex, int driverIndex, ofVec2f loc, bool isLead, int groupIndex, ofxPanel * panelFish);

    void draw(float _scaledSize, bool _bShowDebug);
    //void draw(int _x, int _y, float _scaledSize, bool _bShowDebug);
    
    void update();

    void setBodyState(int _mouthState, int _bodyState);

    int getBodyState();
    
    bool isLead;
    
    void onTouchDown(TouchEvent &event);
    void onTouchMove(TouchEvent &event);
    void onTouchUp(TouchEvent &event);

    char controllerIndex;
    int driverIndex;
    char groupID;
    int id;

protected:

    int stateMouth;
    int stateBody;
    int displayState;

    ofImage arrBassImg[6];
    ofVec2f loc;
    ofVec2f scaledSize;
    ofVec2f offset;

    ofxPanel * panelFish;

    void drawLabel();

    ofTrueTypeFont ttf;

    bool bMoved;
    bool isVisible;
    
    /*
    ofxPanel panel;
    ofParameterGroup panelGroup;

    ofxIntField fishID;
    ofxIntField fishControllerID;
    ofxIntSlider fishControllerIndex;
     */

};

#endif /* fish_hpp */
