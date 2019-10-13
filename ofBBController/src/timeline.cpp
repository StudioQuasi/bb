//
//  timeline.cpp
//  ofBBController
//
//  Created by Adam Lassy on 10/13/19.
//

#include "timeline.h"

timeline::timeline(int _w, int _h)
{
    tWidth = _w;
    tHeight = _h;

    ttf.load("mono.ttf", TIME_SIZE);
    
    ttfCmd.load("mono.ttf", CMD_SIZE);
}

void timeline::setRange(float _range, vector<bbcmd> _arrCmd)
{

    timeRange = _range;

    tArrCmd = _arrCmd;
    
    ofLog() << "TL " << _arrCmd.size() << " - " << tArrCmd.size();

    bracketSize = tWidth * (TIME_SEGMENT/_range);
    
}

void timeline::update(float _loc, float _scrub)
{
    timeLoc = _loc;
    
    
}

void timeline::draw(float _scale, float _offset)
{

    int _locTimelineY = ofGetHeight()-tHeight;
    int _offsetX =  - tWidth * _offset * _scale;

    int _locPosX = tWidth * timeLoc * _scale + _offsetX;
    int _locActionLine = _locTimelineY + .5 * tHeight;
    
    ofPushStyle();
    
    ofFill();
    ofSetColor(200);
    ofDrawRectangle(0, _locTimelineY, tWidth,tHeight);
    
    ofNoFill();
    ofSetLineWidth(3);
    ofSetColor(0);
    ofDrawRectangle(0, _locTimelineY, tWidth,tHeight);

    ofSetLineWidth(1);
    ofDrawLine(0, _locActionLine, tWidth, _locActionLine);
    
    //Draw all actions
    int _loc = 0;
    int _t = TIME_SEGMENT;

    int tBracket = bracketSize;

    while (tBracket < tWidth) {
        
        float _scaledBracket = _scale * tBracket + _offsetX;
        
        ofDrawLine(_scaledBracket, _locTimelineY, _scaledBracket, _locTimelineY + 5 );
        ttf.drawString(ofToString(_t), _scaledBracket - TIME_SIZE, _locTimelineY + 2*TIME_SIZE );

        tBracket += bracketSize;
        _t += TIME_SEGMENT;
    }

    for (int i=0; i<tArrCmd.size(); i++)
    {
        _loc = tWidth * (tArrCmd[i].timecode / timeRange) * _scale + _offsetX;
        
        ofDrawRectangle(_loc-.5*TIME_SIZE, _locActionLine-TIME_SIZE, TIME_SIZE, TIME_SIZE);
        ttfCmd.drawString(ofToString(tArrCmd[i].cmd), _loc - .3*TIME_SIZE, _locActionLine - CMD_SIZE*.2);

        ofDrawLine(_loc, _locActionLine, _loc, _locActionLine + 20);
    }

    ofSetLineWidth(1);
    ofSetColor(255,0,0);
    ofDrawLine(_locPosX,_locTimelineY,_locPosX,ofGetHeight());

    ofPopStyle();

}
