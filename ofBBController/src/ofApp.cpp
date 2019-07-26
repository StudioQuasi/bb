#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    state = STATE_WAIT;

    soundFileName = "hallandoats_icant.mp3";
    playerSound.load(ofToDataPath(soundFileName));
    
    isPaused = false;
    
    ttf.load("mono.ttf", 25);
    ttf_side.load("mono.ttf", 15);

    path.setStrokeColor(0);
    path.setFilled(false);
    path.setStrokeWidth(1);

    animMouth.reset(0);
    animMouth.setCurve(LINEAR);
    animMouth.setDuration(1);
    
    _keyOff = true;
    
    arrCmdNames[0] = "MOUTH_OPEN";
    arrCmdNames[1] = "MOUTH_CLOSE";
    arrCmdNames[2] = "TAIL_ON";
    arrCmdNames[3] = "HEAD_ON";
    arrCmdNames[4] = "BODY_OFF";
    
    arrStateNames[0] = "WAIT";
    arrStateNames[1] = "RECORD";
    arrStateNames[2] = "PLAYBACK";
}

//--------------------------------------------------------------
void ofApp::update(){

    float dt = 1.0f / 60.0f;

    animMouth.update(dt);
}

void ofApp::readJsonFile() {

    ofFile file(ofToDataPath("out.json"));
    ofJson js;

    if(file.exists()){
        
        file >> js;
        arrCmds.clear();

        for(auto & _cmd: js){

            ofLog() << _cmd;

            arrCmds.push_back(bbcmd(_cmd["cmd"],_cmd["timecode"],true,false));

        }
        //calculateText();
    }
}

void ofApp::writeJsonFile() {

    Json::Value arrCommands(Json::arrayValue);
    
    for (int i=0; i<arrCmds.size(); i++) {

        Json::Value _cmd;
        Json::Value _arrBB(Json::arrayValue);

        _cmd["cmd"] = arrCmds[i].cmd;
        _cmd["timecode"] = arrCmds[i].timecode;
        
        _arrBB.append(Json::Value(0));
        _arrBB.append(Json::Value(1));
        _cmd["set"] = _arrBB;
        
        ofLog() << arrCmds[i].cmd << "\t" << arrCmds[i].timecode;
        
        arrCommands.append(_cmd);
    }
    
    ofLog() << arrCommands;
    
    /*
    //Write the file
    ofFile _file;
    _file.open(ofToDataPath("out.json"), ofFile::ReadWrite, false);
    
    bool _written = _file.writeFromBuffer(arrCommands);
    _file.close();
    */
    
    std::ofstream file_id;
    file_id.open(ofToDataPath("out.json"));
    
    Json::Value value_obj;
    //populate 'value_obj' with the objects, arrays etc.
    
    Json::StyledWriter styledWriter;
    file_id << styledWriter.write(arrCommands);
    
    file_id.close();}

//--------------------------------------------------------------
void ofApp::draw(){

    ofSetColor(0);
    ttf.drawString(soundFileName, 20, 40);
    
    float _t = 0;
    
    if (state == STATE_RECORD) {
        _t = (1.0*ofGetElapsedTimeMillis() - timeCode)/1000;
    }
    
    ttf.drawString(ofToString(_t), 20, 90);

    ofPushStyle();
    switch (state) {
    
        case STATE_WAIT:
            ofSetColor(0);
            break;
            
        case STATE_RECORD:
            ofSetColor(255,0,0);
            break;
            
        case STATE_PLAYBACK:
            ofSetColor(0,255,0);
            break;
    }
    ttf.drawString(arrStateNames[state], 20, 140);
    ofPopStyle();
    
    int _r = ofGetWidth() * (.05 + .1 * animMouth.getCurrentValue());
    
    ofNoFill();
    ofSetLineWidth(3);
    ofDrawCircle(ofGetWidth()*.5, ofGetHeight()*.5, _r);
    
    int _offset = 40;
    string _s = "";

    for (int i=arrCmds.size()-1; i>=0; i--) {

        _s = ofToString(arrCmdNames[arrCmds[i].cmd]) + "  " + ofToString(arrCmds[i].timecode);

        ttf_side.drawString(_s, ofGetWidth()*.8, _offset);
        _offset += 25;
    }

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    if (_keyOff == true) {
        
        _keyOff = false;
        
    switch (key) {
            
        case 's':
            
            if (state == STATE_WAIT) {
                
                state = STATE_RECORD;
                playerSound.play();

            } else if (state == STATE_RECORD) {
                
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

            if (state == STATE_RECORD) {

                animMouth.animateTo(1);
                
                float _t = (1.0*ofGetElapsedTimeMillis() - timeCode)/1000;
                arrCmds.push_back(bbcmd(CMD_MOUTH_OPEN,_t,true,false));
            }
            break;

        case 'w':

            ofLog() << "Commands:";

            writeJsonFile();
            break;
            
        case 'o':
            
            readJsonFile();
            break;
    }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

    _keyOff = true;
    
    switch (key) {

        case 'j':
            
            if (state == STATE_RECORD) {
                
                animMouth.animateTo(0);
                float _t = (1.0*ofGetElapsedTimeMillis() - timeCode)/1000;
                arrCmds.push_back(bbcmd(CMD_MOUTH_CLOSE, _t, true, false));
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
