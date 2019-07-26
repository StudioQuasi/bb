#pragma once

#include "ofMain.h"

const int STATE_WAIT = 0;
const int STATE_PLAY = 1;

const int CMD_MOUTH_OPEN = 0;
const int CMD_MOUTH_CLOSE = 1;
const int CMD_TAIL_ON = 2;
const int CMD_HEAD_ON = 3;
const int CMD_BODY_OFF = 4;

struct cmd {

    int cmdID;
    long int timecode;
    
    cmd(
        int _cmdID,
        long int _timecode
    )
    {
        cmdID = _cmdID;
        timecode = _timecode;
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
		

    int state;
    ofSoundPlayer   playerSound;
    long int timeCode;
    bool isPaused;

    vector<cmd> arrCmds;

    ofJson stroke;
    ofTrueTypeFont ttf;
    ofPath path;

    string soundFileName;
    
    float 
};
