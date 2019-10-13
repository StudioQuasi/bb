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

const int TIME_SEGMENT = 30;
const int TIME_SIZE = 12;
const int CMD_SIZE = 8;

using namespace ofxInterface;

struct bbcmd {
    
    int cmd;
    float timecode;
    string sCmd;
    Byte arrIndex[5];
    Byte group;
    
    bbcmd(
          int _cmdID,
          float _timecode,
          string _cmdString,
          Byte _group
          )
    {
        
        cmd = _cmdID;
        timecode = _timecode;
        sCmd = _cmdString;
        group = _group;
    }
};

class timeline  {
    
public:

    timeline(int _x, int _y);

    void draw(float _scale, float _offset);

    void update(float _t, float _scrub);
    void setRange(float _range, vector<bbcmd> _arrCmd);

protected:
    
    int tWidth;
    int tHeight;

    int timeSegment;
    
    float timeRange;
    float timeLoc;

    int pixelLoc;
    int bracketSize;
    
    vector<bbcmd> tArrCmd;

    ofTrueTypeFont ttf, ttfCmd;

};

#endif /* fish_hpp */

