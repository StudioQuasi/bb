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
    int timecodeL;
    string sCmd;
    Byte arrIndex[5];
    Byte group;
    bool isDelete;
    int groupIndex;
    
    bbcmd(
          int _cmdID,
          float _timecode,
          int _timecodeL,
          string _cmdString,
          Byte _group
          )
    {
        
        cmd = _cmdID;
        timecode = _timecode;
        sCmd = _cmdString;
        group = _group;
        isDelete = false;
        timecodeL = _timecodeL;
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
    int getTimecode();
    void setTimecode(int _t);
    
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


