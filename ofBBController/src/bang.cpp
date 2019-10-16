//
//  bang.cpp
//  ofBBController
//
//  Created by Adam Lassy on 10/13/19.
//

#include "bang.h"

bang::bang(bbcmd * _bcmd) {

    //setPosition(_vec);
    ttf.load("mono.ttf", TIME_SEGMENT);

    tcmd = _bcmd;

    setSize(TIME_SEGMENT,TIME_SEGMENT);
    
    ofAddListener(eventTouchDown, this, &bang::onTouchDown);
    ofAddListener(eventTouchUp, this, &bang::onTouchUp);
    ofAddListener(eventTouchMove, this, &bang::onTouchMove);

    bMoved = false;
}

void bang::draw() {

    //_loc = tWidth * (cmd.timecode / timeRange) * _scale + _offsetX;
    ofVec2f _vec = getPosition();

    ofPushStyle();
    //ofNoFill();
    ofFill();

    if (bMoved)
        ofSetColor(255,255,0);
    else
        ofSetColor(255);

    ofDrawRectangle(_vec.x, _vec.y, TIME_SIZE, TIME_SIZE);
    
    ofSetColor(0);
    ttf.drawString(ofToString(getCmd()), _vec.x+3, _vec.y+TIME_SIZE-1);
    ofPopStyle();

    //ttf.drawString(ofToString(tcmd->cmd), _vec.x, _vec.y);

    //ofDrawLine(_vec.x, _vec.y, _vec.x, _vec.y + 20);
}

int bang::getCmd()
{
    return tcmd->cmd;
}

float bang::getTimecode()
{
    return tcmd->timecode;
}

void bang::setTimecode(float _t)
{
    tcmd->timecode = _t;
}

void bang::onTouchDown(ofxInterface::TouchEvent &event)
{

    /*****
     * event.position is always in global space, use toLocal
     */
    bMoved = true;

    anchorX = toLocal(event.position).x;
    anchorTimecodeX = getTimecode();
    
    tcmd->isDelete = true;
}

void bang::onTouchUp(ofxInterface::TouchEvent &event)
{

    bMoved = false;
    //ofVec2f local = toLocal(event.position);
    //ofLog() << "up";
}


void bang::onTouchMove(ofxInterface::TouchEvent &event)
{

    ofVec2f local = toLocal(event.position);
    //offset = local;

    //ofVec2f parentPos = ((Node*)parent)->toLocal(event.position);
    //setPosition(parentPos);
    //ofLog() << "move " << anchor.x << " " << local.x;

    float _d = (-anchorX + local.x) / (scale * 10);
    
    setTimecode(anchorTimecodeX + _d);
    //setPosition(anchorX + local.x, getPosition().y);
}
