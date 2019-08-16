#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    state = STATE_WAIT;
    
    soundFileName = "hallandoats.mp3"; //hallandoats.mp3";
    cmdFileName = "hallandouts_good1.json";

    ofLog() << ofToDataPath(soundFileName);
    
    playerSound.load(ofToDataPath(soundFileName));
    
    isPaused = false;
    
    ttf.load("mono.ttf", 25);
    ttf_side.load("mono.ttf", 15);

    path.setStrokeColor(0);
    path.setFilled(false);
    path.setStrokeWidth(1);

    animMouth.reset(0);
    animMouth.setCurve(LINEAR);
    animMouth.setDuration(.1);
    
    _keyOff = true;
    
    arrCmdNames[0] = "MOUTH_OPEN";
    arrCmdNames[1] = "MOUTH_CLOSE";
    arrCmdNames[2] = "TAIL_ON";
    arrCmdNames[3] = "HEAD_ON";
    arrCmdNames[4] = "BODY_OFF";
    
    arrStateNames[0] = "WAIT";
    arrStateNames[1] = "RECORD";
    arrStateNames[2] = "PLAYBACK";
    
    nextCmdIndex = 0;

    //Set up Serial
    serial.listDevices();
    vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();

    int baud = 57600;
    serial.setup(0, baud); //open the first device
    
    bTailOn = false;
    nextTail = 0;
    
    isFlipping = false;
}

//--------------------------------------------------------------
void ofApp::update(){
    
    float dt = 1.0f / 60.0f;
    animMouth.update(dt);
    
    if (state == STATE_PLAYBACK && nextCmdIndex < arrCmds.size()) {
        
        float _n = arrCmds[nextCmdIndex].timecode;
        
        float _t = ofGetElapsedTimef() - timeCode;

        if (_t > _n) {
            
            ofLog() << _t << " " << _n;

            //Handle commands
            //bool b1 = false;
            //bool b2 = false;

            //Serial.println(arrCmds[nextCmdIndex].cmd);
            //Serial.println(arrCmds[nextCmdIndex].b1);
            arrPlayedCmds.push_back(arrCmds[nextCmdIndex]);

            serial.writeBytes(arrCmds[nextCmdIndex].sCmd.c_str(), arrCmds[nextCmdIndex].sCmd.length());

            nextCmdIndex++;

            //Start over song
            if (arrCmds.size() == nextCmdIndex) {

                readJsonFile();
            }

        }
    }

    if (state != STATE_PLAYBACK) {
        
        if (ofGetElapsedTimef() > nextTail && isFlipping) {
            
            float _t = ofGetElapsedTimef() - timeCode;
            
            if (bTailOn) {
                
                string _scmd = buildCommandString(CMD_TAIL_ON, 1);
                
                if (state == STATE_RECORD)
                    arrCmds.push_back(bbcmd(CMD_TAIL_ON, _t, _scmd));
                
                serial.writeBytes(_scmd.c_str(), _scmd.length());
                ofLog() << _scmd;
                
                nextTail = ofGetElapsedTimef() + 1.0;
            }
            else
            {
                string _scmd = buildCommandString(CMD_TAIL_OFF, 1);
                
                if (state == STATE_RECORD)
                    arrCmds.push_back(bbcmd(CMD_TAIL_OFF, _t, _scmd));
                
                serial.writeBytes(_scmd.c_str(), _scmd.length());
                ofLog() << _scmd;
                
                nextTail = ofGetElapsedTimef() + 1.2;
            }

            bTailOn = !bTailOn;
        }
    }
}

void ofApp::readJsonFile() {

    ofFile file(ofToDataPath(cmdFileName));
    ofJson js;

    if(file.exists()){
        
        file >> js;
        arrCmds.clear();

        int _count = 0;
        for(auto & _cmd: js){

            //ofLog() << _cmd;
            arrCmds.push_back(bbcmd(_cmd["cmd"], _cmd["timecode"], _cmd["set"]));
            _count++;
        }
        
        ofLog() << "* Read " << _count;

    }
    
    //Set state to Playback
    state = STATE_PLAYBACK;
    
    timeCode = ofGetElapsedTimef();
    
    playerSound.stop();
    playerSound.play();
    playerSound.setPosition(OFFSET_POSITION);
    
    nextCmdIndex = 0;
    arrPlayedCmds.clear();

}

void ofApp::writeJsonFile() {

    Json::Value arrCommands(Json::arrayValue);
    
    for (int i=0; i<arrCmds.size(); i++) {

        Json::Value _cmd;
        Json::Value _arrBB(Json::arrayValue);

        _cmd["cmd"] = arrCmds[i].cmd;
        _cmd["timecode"] = arrCmds[i].timecode;

        /*
        if (arrCmds[i].b1)
            _arrBB.append(Json::Value(0));

        if (arrCmds[i].b2)
            _arrBB.append(Json::Value(1));

        _cmd["set"] = _arrBB;
         */

        _cmd["set"] = arrCmds[i].sCmd;

        ofLog() << arrCmds[i].cmd << "\t" << arrCmds[i].timecode;

        arrCommands.append(_cmd);
    }

    ofLog() << arrCommands;
    ofLog() << "* Write " << arrCmds.size();

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
    ttf.drawString("Billy Bass Wall", 20, 40);
    ttf.drawString("\"" + soundFileName + "\"", 20, 90);
    
    float _t = 0;
    
    if (state == STATE_RECORD || state == STATE_PLAYBACK) {
        _t = ofGetElapsedTimef() - timeCode;
    }
    
    ttf.drawString(ofToString(_t), 20, 140);

    ofPushStyle();
    switch (state) {
    
        case STATE_WAIT:
            ofSetColor(0);
            break;
            
        case STATE_RECORD:
            ofSetColor(255,0,0);
            break;
            
        case STATE_PLAYBACK:
            ofSetColor(50,250,50);
            break;
    }
    ttf.drawString(arrStateNames[state], 20, 190);
    ofPopStyle();
    
    int _r = ofGetWidth() * (.05 + .1 * animMouth.getCurrentValue());
    
    ofNoFill();
    ofSetLineWidth(3);
    ofDrawCircle(ofGetWidth()*.5, ofGetHeight()*.5, _r);
    
    int _offset = 40;
    string _s = "";

    if (state == STATE_RECORD)
    {
        for (int i=arrCmds.size()-1; i>=0; i--) {

            _s = ofToString(arrCmdNames[arrCmds[i].cmd]) + "  " + ofToString(arrCmds[i].timecode);

            ttf_side.drawString(_s, ofGetWidth()*.8, _offset);
            _offset += 25;
        }
    }
    
    if (state == STATE_PLAYBACK)
    {
        for (int i=arrPlayedCmds.size()-1; i>=0; i--) {
            
            _s = ofToString(arrCmdNames[arrPlayedCmds[i].cmd]) + "  " + ofToString(arrPlayedCmds[i].timecode);
            
            ttf_side.drawString(_s, ofGetWidth()*.8, _offset);
            _offset += 25;
        }
    }

}

string ofApp::buildCommandString(int _cmd, int _type)
{
    string _scmd = ofToString(_cmd);

    if (_type == 0) {

        _scmd += ofToString(LEAD_BASS);
    } else if (_type == 2) {

        _scmd += "0124";
    }

    _scmd += "\n";

    return _scmd;
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
                playerSound.setPosition(OFFSET_POSITION);

            } else if (state == STATE_RECORD) {
                
                isPaused = !isPaused;
                playerSound.setPaused(isPaused);
            }

            timeCode = ofGetElapsedTimef();
            break;

        case 'r':
            
            playerSound.stop();
            state = STATE_WAIT;
            arrCmds.clear();
            
            break;

        case 'j':
        case 'k':
        case 'l':

            if (state != STATE_PLAYBACK) {

                animMouth.animateTo(1);
                
                float _t = ofGetElapsedTimef() - timeCode;

                string _cmd;

                if (key == 'k') {
                    
                    _lastCmd = LAST_MOUTH_ALL;
                    _cmd = buildCommandString(CMD_MOUTH_OPEN,1);
                } else if (key == 'j' ) {
                    
                    _lastCmd = LAST_MOUTH_LEAD;
                    _cmd = buildCommandString(CMD_MOUTH_OPEN,0);
                } else {

                    _lastCmd = LAST_MOUTH_OTHERS;
                    _cmd = buildCommandString(CMD_MOUTH_OPEN,2);
                }

                serial.writeBytes(_cmd.c_str(), _cmd.length());

                ofLog() << _cmd;
    
                if (state == STATE_RECORD)
                    arrCmds.push_back(bbcmd(CMD_MOUTH_OPEN, _t, _cmd));
            }
            break;

        case 'u':
        case 'i':

            if (state != STATE_PLAYBACK) {

                float _t = ofGetElapsedTimef() - timeCode;

                string _cmd;
                
                if (key == 'i')
                    _cmd = buildCommandString(CMD_HEAD_ON,1);
                else
                    _cmd = buildCommandString(CMD_HEAD_ON,0);
                
                serial.writeBytes(_cmd.c_str(), _cmd.length());

                if (state == STATE_RECORD)
                    arrCmds.push_back(bbcmd(CMD_HEAD_ON, _t, _cmd));
            }
            break;

        case '7':
        case '8':
            
            if (state != STATE_PLAYBACK) {
                
                float _t = ofGetElapsedTimef() - timeCode;
                
                string _cmd;
                
                if (key == '8')
                    _cmd = buildCommandString(CMD_TAIL_ON,1);
                else
                    _cmd = buildCommandString(CMD_TAIL_ON,0);
                
                serial.writeBytes(_cmd.c_str(), _cmd.length());
                
                if (state == STATE_RECORD)
                    arrCmds.push_back(bbcmd(CMD_TAIL_ON, _t, _cmd));
            }
            break;

        case 'm':
        case ',':
            
            if (state != STATE_PLAYBACK) {
                
                float _t = ofGetElapsedTimef() - timeCode;
                
                string _cmd;

                if (key == ',')
                    _cmd = buildCommandString(CMD_BODY_OFF,1);
                else
                    _cmd = buildCommandString(CMD_BODY_OFF,0);
                
                serial.writeBytes(_cmd.c_str(), _cmd.length());
                
                if (state == STATE_RECORD)
                    arrCmds.push_back(bbcmd(CMD_BODY_OFF, _t, _cmd));

            }
            break;
            
        case 'w':

            ofLog() << "Commands:";

            writeJsonFile();
            break;
            
        case 'o':
            
            readJsonFile();
            break;
            
        case 'p':
            
            isFlipping = !isFlipping;
            break;
        }
    }
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key){

    _keyOff = true;
    
    switch (key) {

        case 'j':
        case 'k':
        case 'l':
            
            if (state != STATE_PLAYBACK) {
                
                float _t = ofGetElapsedTimef() - timeCode;
                
                string _cmd;

                if (key == 'k')
                    _cmd = buildCommandString(CMD_MOUTH_CLOSE,1);
                else if (key == 'j')
                    _cmd = buildCommandString(CMD_MOUTH_CLOSE,0);
                else
                    _cmd = buildCommandString(CMD_MOUTH_CLOSE,2);
                    
                animMouth.animateTo(0);
                serial.writeBytes(_cmd.c_str(), _cmd.length());

                if (state == STATE_RECORD)
                   arrCmds.push_back(bbcmd(CMD_MOUTH_CLOSE, _t, _cmd));
            }
            break;
            
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
