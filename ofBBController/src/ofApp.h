#pragma once

#include "ofMain.h"
#include "ofxAnimatableFloat.h"
#include "ofxJSON.h"
#include "ofxGui.h"

#include "controller.h"
#include "fish.h"

const int STATE_WAIT = 0;
const int STATE_RECORD = 1;
const int STATE_PLAYBACK = 2;

const int CMD_MOUTH_OPEN = 0;
const int CMD_MOUTH_CLOSE = 1;
const int CMD_TAIL_ON = 2;
const int CMD_HEAD_ON = 3;
const int CMD_BODY_OFF = 4;
const int CMD_TAIL_OFF = 5;

const int CMD_TYPE_ALL = 1;
const int CMD_TYPE_LEAD = 0;
const int CMD_TYPE_OTHERS = 2;

const int LEAD_BASS = 0;

const int LAST_MOUTH_LEAD = 0;
const int LAST_MOUTH_ALL = 1;
const int LAST_MOUTH_OTHERS = 2;

const float OFFSET_POSITION = .303;

const int NUM_FISH = 63;
const int NUM_CONTROLLER = 21;

const bool isLoop = false;

struct bbcmd {

    int cmd;
    float timecode;
    string sCmd;
    Byte arrIndex[5];
    int cmdType;
    
    bbcmd(
        int _cmdID,
        float _timecode,
        string _cmdString,
        int _cmdType
    )
    {

        cmd = _cmdID;
        timecode = _timecode;
        sCmd = _cmdString;
        cmdType = _cmdType;
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
    
    void setAllBodyState(int _mouthState, int _bodyState, int _cmdID, int _cmdType);

    //Gui Panel
    ofxPanel panel, panelFish;
    
    ofParameterGroup panelGroup;
    
    ofParameter<int> bbCols;
    ofParameter<int> bbRows;
    ofParameter<float> bbScale;
    ofParameter<int> bbColSpacing;
    ofParameter<int> bbRowSpacing;
    ofParameter<int> bbOriginX;
    ofParameter<int> bbOriginY;

    ofParameter<int> bbBGX;
    ofParameter<int> bbBGY;
    ofParameter<float> bbBGScale;
    ofParameter<int> bbSlope;

    /*
    ofParameterGroup panelFishGroup;
    ofParameter<int> fishID;
    ofParameter<int> fishControllerID;
    ofParameter<int> fishControllerIndex;
    ofParameter<int> fishGroupID;
    */
    
    bool bShowGui = true;

    ofxJSONElement result;

    void writeCommand(int _cmdID, int _cmdType, bool _record);

    void writeJsonFile();
    void readJsonFile();
    
    void createLayoutByParam();
    void readLayoutJsonFile();

    void drawStairs();

    int state;
    ofSoundPlayer playerSound;
    float timeCode;
    bool isPaused;

    vector<bbcmd> arrCmds;
    vector<bbcmd> arrPlayedCmds;

    vector<fish> arrFish;

    ofJson stroke;
    ofTrueTypeFont ttf, ttf_side;
    ofPath path;

    string soundFileName;
    string cmdFileName;
    string layoutFile;

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
    
    //Background stairs image
    ofImage imgStairs;
};
