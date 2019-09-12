//
//  fish.cpp
//  ofBBController
//
//  Created by Quasi on 8/5/19.
//

#include "fish.h"

fish::fish(int _wallIndex, int _controllerIndex, int _driverIndex, ofVec2f _vec, bool _isLead)
{
    
    wallIndex = _wallIndex;
    controllerIndex = _controllerIndex;
    driverIndex = _driverIndex;

    loc = _vec;
    
    stateBody = STATE_BODY_OFF;
    stateMouth = STATE_MOUTH_CLOSE;
    
    //Create Bass Image
    arrBassImg[DISPLAY_STATE_CLOSE].load("images/bass_close.png");
    arrBassImg[DISPLAY_STATE_CLOSE_TAIL].load("images/bass_close_tail.png");
    arrBassImg[DISPLAY_STATE_CLOSE_HEAD].load("images/bass_close_head.png");
    arrBassImg[DISPLAY_STATE_OPEN].load("images/bass_open.png");
    arrBassImg[DISPLAY_STATE_OPEN_TAIL].load("images/bass_open_tail.png");
    arrBassImg[DISPLAY_STATE_OPEN_HEAD].load("images/bass_open_head.png");

    displayState = DISPLAY_STATE_CLOSE;
    
    scaledSize.x = arrBassImg[DISPLAY_STATE_CLOSE].getWidth() * SCALE_FISH;
    scaledSize.y = arrBassImg[DISPLAY_STATE_CLOSE].getHeight() * SCALE_FISH;

    isLead = _isLead;
}

fish::~fish()
{
    
}

int fish::getBodyState()
{
    return stateBody;
}

void fish::setBodyState(int _mouthState, int _bodyState)
{
    if (_mouthState != -1)
        stateMouth = _mouthState;
    
    if (_bodyState != -1)
        stateBody = _bodyState;

    if (stateMouth == STATE_MOUTH_CLOSE) {
        
        switch (stateBody) {
        
            case STATE_BODY_OFF:
                displayState = DISPLAY_STATE_CLOSE;
                break;
                
            case STATE_BODY_TAIL:
                displayState = DISPLAY_STATE_CLOSE_TAIL;
                break;
                
            case STATE_BODY_HEAD:
                displayState = DISPLAY_STATE_CLOSE_HEAD;
                break;
        }

    } else {

        switch (stateBody) {
                
            case STATE_BODY_OFF:
                displayState = DISPLAY_STATE_OPEN;
                break;
                
            case STATE_BODY_TAIL:
                displayState = DISPLAY_STATE_OPEN_TAIL;
                break;
                
            case STATE_BODY_HEAD:
                displayState = DISPLAY_STATE_OPEN_HEAD;
                break;
        }
    }
}

void fish::update()
{
    
}

void fish::draw(int _x, int _y)
{
    //Get image based on state mouth and body
    string _bassImg = "bass_" + ofToString(stateMouth) + "_" + ofToString(stateBody) + ".png";

    //Draw image
    arrBassImg[displayState].draw(_x, _y, scaledSize.x, scaledSize.y);
    //arrBassImg[displayState].draw(loc, scaledSize.x, scaledSize.y);

}
