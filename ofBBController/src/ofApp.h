#pragma once

#include "ofMain.h"
#include "ofxAnimatableFloat.h"
#include "ofxJSON.h"
#include "ofxGui.h"

#include "controller.h"
#include "fish.h"
#include "ofxInterface.h"
#include "timeline.h"

const int STATE_WAIT = 0;
const int STATE_RECORD = 1;
const int STATE_PLAYBACK = 2;
const int STATE_PAUSED = 3;
const int STATE_JUKEBOX = 4;

const int CMD_MOUTH_OPEN = 0;
const int CMD_MOUTH_CLOSE = 1;
const int CMD_TAIL_ON = 2;
const int CMD_HEAD_ON = 3;
const int CMD_BODY_OFF = 4;
const int CMD_TAIL_OFF = 5;
const int CMD_BODY_RELEASE = 7;
const int CMD_TAIL_RELEASE = 8;

const int CMD_SET_GROUP = 6;
const int CMD_INIT_GROUP = 9;

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

const int KEY_OFFSET = 0;

const bool isLoop = false;

//const int MAX_DIFF = 50;

struct group {

    char groupID;
    vector<int> arrFishID;
    int bodyState;
    
    float lastSpeak;
    
    int lastCmd;
    int lastBody;

    group(
        char _groupID,
        vector<int> _arrFishID
    )
    {

        groupID = _groupID;
        arrFishID = _arrFishID;
    }
};

struct song {

    string songFile;
    string cmdFile;
    vector<group*> arrGroup;
    vector<string> arrTrackPaths;
    int trackIndex;
    int fadePoint;
    bool isFade;

    song(
        string _songFile,
        string _cmdFile,
        vector<string> _arrTracks,
        float _fadePoint
    )
    {
        songFile = _songFile;
        cmdFile = _cmdFile;
        arrTrackPaths = _arrTracks;
        
        if (_fadePoint > 0) {
            fadePoint = _fadePoint * 1000;
            isFade = true;
        } else {
            isFade = false;
        }

        trackIndex = 0;
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
		
        void resetCmdIndex();
    
    string buildCommandString(int _cmd, char _type);
    string buildFullCmdString(int _cmd, int _board, int _index);

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
    
    ofParameter<bool> bShowDisplay;

    ofParameter<bool> isCreateGroups;

    ofParameter<bool> showTimeline;
    ofParameter<bool> isDrawBang;
    ofParameter<bool> isRecording;
    ofParameter<bool> bbSongMute;
    ofParameter<int> songTweak;
    ofParameter<float> songVolume;
    ofParameter<int> maxDiff;

    ofParameter<bool> autoSleep;
    ofParameter<bool> bbTimelineActive;
    ofParameter<float> bbTimelineScale;
    ofParameter<float> bbTimelineSlide;
    ofParameter<float> bbTimelineScrub;

    ofxIntField fishID;
    ofxIntField fishControllerID;
    ofxIntSlider fishControllerIndex;
    ofxIntField fishGroupID;

    bool bSongMute = false;
    bool bShowGui = true;

    ofxJSONElement result;

    void assignGroup(group * _group, bool _init);

    void playbackCommand();
    void displayCommand(int _cmdID, char _groupID);
    void writeCommand(int _cmdID, bool _record, char _groupID=NULL);
    void writeCommandNew(int _cmdID, bool _record, char _groupID);

    void writeJsonFile();
    void loadTrack(string _file);
    void loadAndPlaySong(bool allTracks);
    
    float getSongPosition();

    void createLayoutByParam();
    void readLayoutJsonFile();
    void writeLayoutJsonFile();

    void drawStairs();
    int getKeyIndex(char _key);

    void testAllTails();

    float getSongLength();
    //bool sortCmds(bbcmd & a, bbcmd & b);

    int state;
    ofSoundPlayer playerSound;
    float songLength;
    //float timeCode;
    bool isPaused;

    vector<bbcmd*> arrCmds;
    vector<bbcmd*> arrPlayedCmds;
    vector<fish*> arrFish;
    vector<song> arrSongs;
    vector<group*> arrGroups;
    vector<vector<bbcmd*>> arrTracks;

    int songIndex;

    ofJson stroke;
    ofTrueTypeFont ttf, ttf_side, ttf_small;
    ofPath path;

    string layoutFile;
    ofxAnimatableFloat animMouth;
    ofxAnimatableFloat animFadeOut;

    bool _keyOff = true;

    string arrCmdNames[9];
    string arrStateNames[5];

    int nextCmdIndex;
    ofSerial    serial;

    bool bTailOn;
    float nextTail;
    bool isFlipping;
    int _lastCmd;

    float nextTailOff;
    bool isTailUp = false;
    int tailGroup = 0;

    bool isSerial = false;

    //Background stairs image
    ofImage imgStairs;

    //bool bCreateGroups = false;

    timeline * mainTimeline;
    void nextSong();
    
    float tmrSong = 0;
    bool isPlaying = false;
};
