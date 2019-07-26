#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    state = STATE_WAIT;

    soundFileName = "hallandoats.mp3";
    playerSound.load(soundFileName);
    
    isPaused = false;
    
    ttf.load("mono.ttf", 25);
    path.setStrokeColor(0);
    path.setFilled(false);
    path.setStrokeWidth(1);
    
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

    ofSetColor(0);
    ttf.drawString(soundFileName, 20, 40);
    
    float _t = 0;
    
    if (state == STATE_PLAY) {
        _t = (1.0*ofGetElapsedTimeMillis() - timeCode)/1000;
    }
    
    ttf.drawString(ofToString(_t), 20, 100);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    switch (key) {
            
        case 's':
            
            if (state == STATE_WAIT) {
                
                state = STATE_PLAY;
                playerSound.play();

            } else if (state == STATE_PLAY) {
                
                isPaused = !isPaused;
                playerSound.setPaused(isPaused);
            }

            timeCode = ofGetElapsedTimeMillis();

            break;
    
        case 'r':
            
            playerSound.stop();
            state = STATE_WAIT;
            arrCmds.clear();
            
            break;
            
        case 'j':

            if (state == STATE_PLAY) {
                
                arrCmds.push_back(cmd(CMD_MOUTH_OPEN,ofGetElapsedTimeMillis()));
            }
            break;

        case 'd':

            ofLog() << "Commands:";

            for (int i=0; i<arrCmds.size(); i++) {
                ofLog() << arrCmds[i].cmdID << "\t" << arrCmds[i].timecode;
            }
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

    switch (key) {
            
        case 'j':
            
            if (state == STATE_PLAY) {
                
                arrCmds.push_back(cmd(CMD_MOUTH_CLOSE, ofGetElapsedTimeMillis()));
            }
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
