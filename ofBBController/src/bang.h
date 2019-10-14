//
//  bang.hpp
//  ofBBController
//
//  Created by Adam Lassy on 10/13/19.
//

#ifndef bang_h
#define bang_h

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxInterface.h"

const int TIME_SEGMENT = 10;
const int TIME_SIZE = 12;
const int CMD_SIZE = 8;

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

using namespace ofxInterface;

class bang : public ofxInterface::Node {
    
public:

    bang(bbcmd * _bcmd);
    
    void draw();
    
    void onTouchDown(TouchEvent &event);
    void onTouchMove(TouchEvent &event);
    void onTouchUp(TouchEvent &event);

    int getCmd();
    float getTimecode();
    void setTimecode(float _t);
    
    float scale;

protected:

    ofVec2f loc;

    int anchorX;
    float anchorTimecodeX;
    
    ofTrueTypeFont ttf;
    
    bbcmd * tcmd;
    bool bMoved;

};

#endif /* fish_hpp */


