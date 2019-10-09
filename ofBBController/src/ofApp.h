#pragma once

#include "ofMain.h"
#include "ofxAnimatableFloat.h"
#include "ofxJSON.h"
#include "ofxGui.h"

#include "controller.h"
#include "fish.h"
#include "ofxInterface.h"

const int STATE_WAIT = 0;
const int STATE_RECORD = 1;
const int STATE_PLAYBACK = 2;

const int CMD_MOUTH_OPEN = 0;
const int CMD_MOUTH_CLOSE = 1;
const int CMD_TAIL_ON = 2;
const int CMD_HEAD_ON = 3;
const int CMD_BODY_OFF = 4;
const int CMD_TAIL_OFF = 7;

const int CMD_TYPE_ALL = 1;
const int CMD_TYPE_LEAD = 0;
const int CMD_TYPE_OTHERS = 2;

const int LEAD_BASS = 0;

const int LAST_MOUTH_LEAD = 0;
const int LAST_MOUTH_ALL = 1;
const int LAST_MOUTH_OTHERS = 2;

const float OFFSET_POSITION = 0; //.303;

const int NUM_FISH = 63;
const int NUM_CONTROLLER = 21;

const bool isLoop = false;

struct group {

    char groupID;
    vector<int> arrFishID;
    int bodyState;

    group(
        char _groupID,
        vector<int> _arrFishID
    )
    {
        groupID = _groupID;
        
        arrFishID = _arrFishID;
        //for (int i=0; i<5; i++) {
        //    arrFish[i] = _arrFish[i];
        //}
    }
};

struct song {

    string songFile;
    string cmdFile;
    vector<group*> arrGroup;
    vector<string> arrTracks;
    int trackIndex;

    song(
        string _songFile,
        string _cmdFile,
        vector<string> _arrTracks
    )
    {
        songFile = _songFile;
        cmdFile = _cmdFile;
        arrTracks = _arrTracks;
        
        trackIndex = 0;
    }

};

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
		
        string buildCommandString(int _cmd, char _type);
    
    void buttonPress();
    
    void setAllBodyState(int _mouthState, int _bodyState, int _cmdID, char _groupID=NULL);

    ofxInterface::Node* scene;
    
    //Gui Panel
    ofxPanel panel;
    ofxPanel * panelFish;

    ofParameterGroup panelGroup;

    ofParameter<int> bbCols;
    ofParameter<int> bbRows;
    ofParameter<float> bbScale;
    ofParameter<float> masterScale;
    ofParameter<int> masterX;
    ofParameter<int> masterY;
    
    ofParameter<int> bbColSpacing;
    ofParameter<int> bbRowSpacing;
    ofParameter<int> bbOriginX;
    ofParameter<int> bbOriginY;

    ofParameter<float> bbFlipSpeed;
    
    ofParameter<int> bbBGX;
    ofParameter<int> bbBGY;
    ofParameter<float> bbBGScale;
    ofParameter<int> bbSlope;

    ofParameter<bool> bbShowDebug;
    ofParameter<int> bbFlatIndex;
    ofParameter<int> bbFlatOffset;
    //ofParameterGroup panelFishGroup;

    ofxIntField fishID;
    ofxIntField fishControllerID;
    ofxIntSlider fishControllerIndex;
    ofxIntField fishGroupID;


    bool bShowGui = true;

    ofxJSONElement result;

    void assignGroup(group * _group);

    void writeCommand(int _cmdID, bool _record, char _groupID=NULL);

    void writeJsonFile();
    void loadAndPlaySong();
    
    void createLayoutByParam();
    void readLayoutJsonFile();
    void writeLayoutJsonFile();

    void drawStairs();

    int getKeyIndex(char _key);

    void testAllTails();

    //bool sortCmds(bbcmd & a, bbcmd & b);

    int state;
    ofSoundPlayer playerSound;
    float timeCode;
    bool isPaused;

    vector<bbcmd> arrCmds;
    vector<bbcmd> arrPlayedCmds;
    vector<fish*> arrFish;
    vector<song> arrSongs;
    vector<group*> arrGroups;

    int songIndex;

    ofJson stroke;
    ofTrueTypeFont ttf, ttf_side, ttf_small;
    ofPath path;

    //string soundFileName;
    //string cmdFileName;
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
    
    float nextTailOff;
    bool isTailUp = false;
    int tailGroup = 0;

    //Background stairs image
    ofImage imgStairs;

    bool bCreateGroups = false;

};
