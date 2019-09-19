//
//  fish.cpp
//  ofBBController
//
//  Created by Quasi on 8/5/19.
//

#include "fish.h"

void fish::setup(int _wallIndex, int _controllerIndex, int _driverIndex, ofVec2f _vec, bool _isLead, int _groupIndex)
{
    
    wallIndex = _wallIndex;
    controllerIndex = _controllerIndex;
    driverIndex = _driverIndex;

    groupIndex = _groupIndex;
    loc = _vec;
    
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

    /*
    panel.setName("Bass ID");
    panel.add(fishID.setup("ID",wallIndex,0,65));
    panel.add(fishControllerID.setup("Controller",controllerIndex,0,21));
    panel.add(fishControllerIndex.setup("Driver Index",driverIndex,0,2));
     */
    
    //panelGroup.add(fishControllerID.set("Controller ID",0,0,21));
    //panelGroup.add(fishControllerIndex.set("Bass Index",0,0,2));
    //panelGroup.add(fishControllerIndex.set("Group",0,0,2));

    /*
    panel.setDefaultHeight(25);
    panel.setDefaultWidth(ofGetWidth()/10);
    panel.setup(panelGroup, "bass_settings.xml");
    panel.setPosition(150, 200);
    panel.loadFromFile("bass_settings.xml");
    */

    ofAddListener(eventTouchDown, this, &fish::onTouchDown);
    ofAddListener(eventTouchUp, this, &fish::onTouchUp);
    ofAddListener(eventTouchMove, this, &fish::onTouchMove);

    ttf.load("mono.ttf", 8);
    ttf.setLineHeight(10);

}

void fish::onTouchDown(ofxInterface::TouchEvent &event)
{
    /*****
     * event.position is always in global space, use toLocal
     */
    
    ofVec2f local = toLocal(event.position);
    
    ofLog() << "down";
}

void fish::onTouchUp(ofxInterface::TouchEvent &event)
{

    ofVec2f local = toLocal(event.position);
    
    ofLog() << "up";
}


void fish::onTouchMove(ofxInterface::TouchEvent &event)
{

    ofVec2f local = toLocal(event.position);
    //offset = local;
    
    //setPosition(local);

    //ofVec2f parentPos = ((Node*)parent)->toLocal(event.position);
    
    ofLog() << "move " << local;
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

void fish::drawLabel()
{
    ofPushStyle();
    ofFill();

    ofDrawRectangle(getPosition(), 30, 20);
    ofSetColor(0);
    
    ttf.drawString("id:" + ofToString(wallIndex) + "\nctrl:" + ofToString(controllerIndex) + "\nmotor:" + ofToString(driverIndex), getPosition().x, getPosition().y);
    //ttf.drawString("ctrl: " + ofToString(controllerIndex), getPosition().x, getPosition().y + );
    //ttf.drawString("motor: " + ofToString(driverIndex), getPosition().x, getPosition().y + );
    
    ofPopStyle();

}

void fish::draw(int _x, int _y, float _scale)
{

    setPosition(_x, _y, 0);

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
    arrBassImg[displayState].draw(_xf, _yf, _scale * FISH_IMG_WIDTH, _scale * FISH_IMG_HEIGHT);
    //arrBassImg[displayState].draw(loc, scaledSize.x, scaledSize.y);

    ofNoFill();
    ofDrawRectangle(getPosition().x, getPosition().y , getSize().x, getSize().y);
    //ofDrawRectangle(getPosition().x - .5*getSize().x, getPosition().y - .5*getSize().y, getSize().x, getSize().y);
    
    drawLabel();
}

