#pragma once

#include "ofMain.h"
#include "ofxAnimatableFloat.h"
#include "ofxJSON.h"

const int STATE_WAIT = 0;
const int STATE_RECORD = 1;
const int STATE_PLAYBACK = 2;

const int CMD_MOUTH_OPEN = 0;
const int CMD_MOUTH_CLOSE = 1;
const int CMD_TAIL_ON = 2;
const int CMD_HEAD_ON = 3;
const int CMD_BODY_OFF = 4;
const int CMD_TAIL_OFF = 5;

const int LEAD_BASS = 3;

const int LAST_MOUTH_LEAD = 0;
const int LAST_MOUTH_ALL = 1;
const int LAST_MOUTH_OTHERS = 2;

const float OFFSET_POSITION = 0;

struct bbcmd {

    int cmd;
    float timecode;
    string sCmd;
    Byte arrIndex[5];
    
    bbcmd(
        int _cmdID,
        float _timecode,
        string _cmdString
    )
    {

        cmd = _cmdID;
        timecode = _timecode;
        sCmd = _cmdString;

        /*
        for (int i=0; i<5; i++) {
            arrIndex[i] = _arrIndex[i];
        }*/

    }
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
        string buildCommandString(int _cmd, int _type);
    

    ofxJSONElement result;

    void writeJsonFile();
    void readJsonFile();

    int state;
    ofSoundPlayer   playerSound;
    float timeCode;
    bool isPaused;

    vector<bbcmd> arrCmds;
    vector<bbcmd> arrPlayedCmds;

    ofJson stroke;
    ofTrueTypeFont ttf, ttf_side;
    ofPath path;

    string soundFileName;
    string cmdFileName;
    
    ofxAnimatableFloat animMouth;
    bool _keyOff = true;
    
    string arrCmdNames[5];
    string arrStateNames[3];
    
    int nextCmdIndex;
    ofSerial    serial;

    bool bTailOn;
    float nextTail;
    
    bool isFlipping;
    
    int _lastCmd;

    
};
