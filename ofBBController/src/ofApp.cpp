#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    state = STATE_WAIT;
    
    soundFileName = "hallandoats.mp3";
    
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
    
    if (state == STATE_PLAYBACK) {
        
        float _n = arrCmds[nextCmdIndex].timecode;
        
        float _t = ofGetElapsedTimef() - timeCode;
        if (_t > _n) {
            
            ofLog() << _t << " " << _n;

            //Handle commands
            bool b1 = false;
            bool b2 = false;

            //Serial.println(arrCmds[nextCmdIndex].cmd);
            //Serial.println(arrCmds[nextCmdIndex].b1);
            arrPlayedCmds.push_back(arrCmds[nextCmdIndex]);
            
            
            switch (arrCmds[nextCmdIndex].cmd) {
                    
                case CMD_MOUTH_OPEN:
                    
                    if (arrCmds[nextCmdIndex].b1)
                        serial.writeByte('1');
                    if (arrCmds[nextCmdIndex].b2)
                        serial.writeByte('2');
                    break;
                    
                case CMD_MOUTH_CLOSE:
                    
                    if (arrCmds[nextCmdIndex].b1)
                        serial.writeByte('3');
                    if (arrCmds[nextCmdIndex].b2)
                        serial.writeByte('4');
                    break;
                    
                    /*
                 case CMD_TAIL_ON:
                    
                    if (arrCmd[nextCmdIndex].b1)
                        runMotor(BASS_1_TAIL, BACKWARD, 255);
                    if (arrCmd[nextCmdIndex].b2)
                        runMotor(BASS_2_TAIL, BACKWARD, 255);
                    break;
                    
                case CMD_HEAD_ON:
                    
                    if (arrCmd[nextCmdIndex].b1)
                        runMotor(BASS_1_TAIL, FORWARD, 255);
                    if (arrCmd[nextCmdIndex].b2)
                        runMotor(BASS_2_TAIL, FORWARD, 255);
                    break;
                    
                case CMD_BODY_OFF:
                    
                    if (arrCmd[nextCmdIndex].b1)
                        runMotor(BASS_1_TAIL, RELEASE, 0);
                    if (arrCmd[nextCmdIndex].b2)
                        runMotor(BASS_2_TAIL, RELEASE, 0);
                    break;
                    
                */
            }
        
            nextCmdIndex++;
        }
    }
    
    if (state == STATE_RECORD) {
        
        if (ofGetElapsedTimef() > nextTail && isFlipping) {
            
            float _t = ofGetElapsedTimef() - timeCode;
            
            if (bTailOn) {
                arrCmds.push_back(bbcmd(CMD_TAIL_ON, _t, true, true));
                serial.writeByte('5');
                serial.writeByte('6');
                
                nextTail = ofGetElapsedTimef() + .1;
            }
            else
            {
                arrCmds.push_back(bbcmd(CMD_BODY_OFF, _t, true, true));
                serial.writeByte('9');
                serial.writeByte('0');
                
                nextTail = ofGetElapsedTimef() + 1.2;
            }
            
            
            bTailOn = !bTailOn;
        }
    }
}

void ofApp::readJsonFile() {

    ofFile file(ofToDataPath("out.json"));
    ofJson js;

    if(file.exists()){
        
        file >> js;
        arrCmds.clear();

        int _count = 0;
        for(auto & _cmd: js){

            //ofLog() << _cmd;
            arrCmds.push_back(bbcmd(_cmd["cmd"],_cmd["timecode"],true,false));
            _count++;
        }
        
        ofLog() << "* Read " << _count;

    }
    
    //Set state to Playback
    state = STATE_PLAYBACK;
    
    timeCode = ofGetElapsedTimef();
    
    playerSound.stop();
    playerSound.play();
    playerSound.setPosition(.17);
    
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

        if (arrCmds[i].b1)
            _arrBB.append(Json::Value(0));

        if (arrCmds[i].b2)
            _arrBB.append(Json::Value(1));

        _cmd["set"] = _arrBB;

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

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    if (_keyOff == true) {
        
        _keyOff = false;
        
    switch (key) {
            
        case 's':
            
            if (state == STATE_WAIT) {
                
                state = STATE_RECORD;
                
                playerSound.play();
                playerSound.setPosition(.17);

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

            if (state == STATE_RECORD) {

                bool b1 = true;
                bool b2 = false;
                
                if (key == 'k') {
                    b2 = true;
                }
                
                animMouth.animateTo(1);
                
                float _t = ofGetElapsedTimef() - timeCode;
                
                serial.writeByte('1');
                if (key == 'k') {
                    serial.writeByte('2');
                }

                arrCmds.push_back(bbcmd(CMD_MOUTH_OPEN, _t, b1, b2));
            }
            break;

        case 'm':
        case ',':

            if (state == STATE_RECORD) {

                bool b1 = true;
                bool b2 = false;

                if (key == ',') {
                    b2 = true;
                }

                float _t = ofGetElapsedTimef() - timeCode;
            
                serial.writeByte('7');
                if (key == ',') {
                    serial.writeByte('8');
                }
            
                arrCmds.push_back(bbcmd(CMD_HEAD_ON, _t, b1, b2));
            }
            break;

        case '.':
            
            if (state == STATE_RECORD) {
                
                float _t = ofGetElapsedTimef() - timeCode;
                
                bool b1 = true;
                bool b2 = true;
                
                serial.writeByte('9');
                serial.writeByte('0');
                
                arrCmds.push_back(bbcmd(CMD_BODY_OFF, _t, b1, b2));
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
            
            if (state == STATE_RECORD) {
                
                bool b1 = true;
                bool b2 = false;
                
                if (key == 'k') {
                    b2 = true;
                }

                animMouth.animateTo(0);
                float _t = ofGetElapsedTimef() - timeCode;
                arrCmds.push_back(bbcmd(CMD_MOUTH_CLOSE, _t, b1, b2));
                
                serial.writeByte('3');
                if (key == 'k') {
                    serial.writeByte('4');
                }
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
