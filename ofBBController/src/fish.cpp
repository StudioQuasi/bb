//
//  fish.cpp
//  ofBBController
//
//  Created by Quasi on 8/5/19.
//

#include "fish.h"

void fish::setup(int _wallIndex, char _controllerIndex, int _driverIndex, ofVec2f _vec, bool _isLead, int _groupIndex, ofxPanel * _panelFish)
{
    
    ofLog() << "SETUP FISH " << _controllerIndex;
    
    id = _wallIndex;
    controllerIndex = _controllerIndex;
    driverIndex = _driverIndex;

    //groupIndex = _groupIndex;
    loc = _vec;
    setPosition(_vec);
    
    offset = ofVec2f(0,0);

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

    isVisible = true;

    ofAddListener(eventTouchDown, this, &fish::onTouchDown);
    ofAddListener(eventTouchUp, this, &fish::onTouchUp);
    ofAddListener(eventTouchMove, this, &fish::onTouchMove);

    //panelFish = _panelFish;

    ttf.load("mono.ttf", 8);
    ttf.setLineHeight(10);

    bMoved = false;

    arrGroupID.clear();
    
    initGroup = false;
}

bool fish::inGroup(int _gid)
{
    for (int i=0; i<arrGroupID.size(); i++)
    {
        if (_gid == arrGroupID[i])
            return true;
    }
    return false;
}

void fish::addGroup(int _id)
{
    arrGroupID.push_back(_id);
}

void fish::clearGroup()
{
    arrGroupID.clear();
}

void fish::onTouchDown(ofxInterface::TouchEvent &event)
{
    /*****
     * event.position is always in global space, use toLocal
     */
    
    ofVec2f local = toLocal(event.position);
    
    ofLog() << "down";

    //isVisible = !isVisible;
}

void fish::onTouchUp(ofxInterface::TouchEvent &event)
{

    ofVec2f local = toLocal(event.position);
    
    ofLog() << "up";
}


void fish::onTouchMove(ofxInterface::TouchEvent &event)
{
    bMoved = true;
    
    ofVec2f local = toLocal(event.position);
    //offset = local;

    ofVec2f parentPos = ((Node*)parent)->toLocal(event.position);
    setPosition(parentPos);
    
    ofLog() << "move " << local;
}

int fish::getBodyState()
{
    return stateBody;
}

void fish::setBodyState(int _mouthState, int _bodyState)
{

    if (_mouthState != -1) {
        stateMouth = _mouthState;
        
        lastSpokeTime = ofGetElapsedTimef();
    }
    
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

bool fish::update()
{
    /*
    if (ofGetElapsedTimef() > lastSpokeTime + 1 && stateBody == STATE_BODY_HEAD) {
        setBodyState(-1, STATE_BODY_OFF);
        return true;
    }
    */

    return false;
}

void fish::drawLabel()
{

    ofPushStyle();
    ofFill();

    string _label = "id:" + ofToString(id) + "\nc:" + controllerIndex + "\nm:" + ofToString(driverIndex);

    int _height = 30;
    
    if (arrGroupID.size() > 0) {
        
        string _g = "";
        for (int i=0; i<arrGroupID.size(); i++) {
            _g += ofToString(arrGroupID[i]) + ",";
        }

        _label += "\ng:" + _g;
        _height = 45;
    }

    ofSetColor(50,50,200);
    ofDrawRectangle(getPosition().x,getPosition().y-10, 40, _height);
    ofSetColor(255);

    


    ttf.drawString(_label, getPosition().x, getPosition().y);
    
    ofPopStyle();
}

void fish::draw(float _scale, bool _showDebug)
{
    /*
    if (!bMoved)
        
        setPosition(_x, _y, 0);
    else {

        _x = getPosition().x;
        _y = getPosition().y;
    }
    */
    
    int _x = getPosition().x;
    int _y = getPosition().y;
        
    int _xf = _x - FISH_IMG_WIDTH * _scale * .5 + getSize().x * .5;
    int _yf = _y - FISH_IMG_HEIGHT * _scale * .5 + getSize().y * .5;

    //int _x = _x - FISH_IMG_WIDTH * .5;
    //int _yf = _y - FISH_IMG_HEIGHT * .5;
    //Set the position

    setSize(RENDER_SIZE * _scale, RENDER_SIZE * _scale);

    //ofLog() << _x << "," << _y;
    //Get image based on state mouth and body

    string _bassImg = "bass_" + ofToString(stateMouth) + "_" + ofToString(stateBody) + ".png";

    //Draw image
    if (isVisible)
        arrBassImg[displayState].draw(_xf, _yf, _scale * FISH_IMG_WIDTH, _scale * FISH_IMG_HEIGHT);
    //arrBassImg[displayState].draw(loc, scaledSize.x, scaledSize.y);

    if (_showDebug) {
        ofNoFill();
        ofDrawRectangle(getPosition().x, getPosition().y , getSize().x, getSize().y);
        //ofDrawRectangle(getPosition().x - .5*getSize().x, getPosition().y - .5*getSize().y, getSize().x, getSize().y);
    
        drawLabel();
    }

}

