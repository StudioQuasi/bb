//
//  timeline.hpp
//  ofBBController
//
//  Created by Adam Lassy on 10/13/19.
//

#ifndef timeline_h
#define timeline_h

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxInterface.h"
#include "bang.h"

using namespace ofxInterface;

class timeline  {
    
public:

    timeline(int _x, int _y, ofxInterface::Node* scene);

    void draw(float _scale, float _offset, bool _showBang);

    void update(float _t, float _scrub);
    void setRange(float _range, vector<bbcmd*> &_arrCmd);

protected:
    
    int tWidth;
    int tHeight;

    int timeSegment;
    
    float timeRange;
    float timeLoc;

    int pixelLoc;
    int bracketSize;
    
    vector<bbcmd*> tArrCmd;
    vector<bang*> tArrBang;
    
    ofTrueTypeFont ttf, ttfCmd;

    ofxInterface::Node* scene;

};

#endif /* fish_hpp */

