#include "ofApp.h"

using namespace ofxInterface;

//--------------------------------------------------------------
void ofApp::setup(){

    //
    scene = new Node();
    scene->setSize(ofGetWidth(), ofGetHeight());
    scene->setName("Scene");

    TouchManager::one().setup(scene);

    state = STATE_WAIT;
    
    //soundFileName = "smooth_operator.mp3"; //hallandoats.mp3";
    //cmdFileName = "smooth_operator.json";
    
    //Create Universal Groups
    
    //vector<fish*> _arrFish;

    /*
    _arrFish.push_back();
    struct group {
        char groupID;
        vector<fish*> arrFish;
        
        group(
              char _groupID,
              vector<fish*> _arrFish
              )
        {
            groupID = _groupID;
            arrFish = _arrFish;
        }
    };*/
    
    arrSongs.push_back(song("hallandoats.mp3","hallandouts_good1.json"));
    arrSongs.push_back(song("smooth_operator.mp3","smooth_operator.json"));
    songIndex = 0;

    layoutFile = "new_layout.json";

    ofLog() << ofToDataPath(arrSongs[songIndex].songFile);
    
    //playerSound.load(ofToDataPath(arrSongs[songIndex].songFile));
    
    isPaused = false;
    
    ttf.load("mono.ttf", 35);
    ttf_side.load("mono.ttf", 14);

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

    int baud = 9600;
    serial.setup(0, baud); //open the first device
    
    bTailOn = false;
    nextTail = 0;
    
    isFlipping = false;
    
    //Open stairs image
    imgStairs.load("images/royalPalms_67Fish.jpg");

    //Add All Gui elements to set
    panelGroup.setName("Bass Layout Params");
    
    /*
    panelGroup.add(bbCols.set("Columns",10,1,50));
    panelGroup.add(bbRows.set("Rows",10,1,50));
    panelGroup.add(bbColSpacing.set("Col Spacing",20,0,300));
    panelGroup.add(bbRowSpacing.set("Row Spacing",20,0,300));
    */
    
    panelGroup.add(masterScale.set("Master Scale",1,.1,10));
    panelGroup.add(masterX.set("Master X",0,-ofGetWidth(),ofGetWidth()));
    panelGroup.add(masterY.set("Master Y",0,-ofGetHeight(),ofGetHeight()));
    panelGroup.add(bbScale.set("Bass Scale",.1,.05,5));

    //panelGroup.add(bbOriginX.set("Origin X",0,0,ofGetWidth()));
    //panelGroup.add(bbOriginY.set("Origin Y",0,0,ofGetHeight()));
    //panelGroup.add(bbSlope.set("Slope",0,0,200));
    
    panelGroup.add(bbBGX.set("Background X",0,-ofGetWidth(),ofGetWidth()));
    panelGroup.add(bbBGY.set("Background Y",0,-ofGetHeight(),ofGetHeight()));
    panelGroup.add(bbBGScale.set("Background Scale",1,0,10));
    panelGroup.add(bbShowDebug.set("Show Debug"));

    //panelGroup.add(bbFlatIndex.set("Flat Index", 0, 0, 21));
    //panelGroup.add(bbFlatOffset.set("Flat Offset", 0, -100, 100));

    /*
    panelFish.setup();
    panelFish.add(fishID.setup("Bass ID",0,0,63));
    panelFish.add(fishControllerID.setup("Controller ID",0,0,21));
    panelFish.add(fishControllerIndex.setup("Motor Index",0,0,2));
    panelFish.add(fishGroupID.setup("Group",0,0,2));
    */
    
    //panelGroup.set
    panelFish = new ofxPanel();
    panelFish->add(fishID.setup("Bass ID",0,0,63));
    panelFish->add(fishControllerID.setup("Controller ID",0,0,21));
    panelFish->add(fishControllerIndex.setup("Motor Index",0,0,2));
    panelFish->add(fishGroupID.setup("Group",0,0,2));

    panel.setDefaultHeight(25);
    panel.setDefaultWidth(ofGetWidth()/5);
    panel.setup(panelGroup, "bbwall_settings.xml");
    panel.setPosition(0, 200);
    panel.loadFromFile("bbwall_settings.xml");

    panelFish->setDefaultHeight(25);
    panelFish->setDefaultWidth(ofGetWidth()/25);
    //panelFish.setup(panelFishGroup, "bbwall_fish_settings.xml");
    panelFish->setPosition(150, 200);
    //panelFish.loadFromFile("bbwall_fish_settings.xml");
    
    //Read Layout File
    readLayoutJsonFile();
    
    //Create All the groups - Move this to the individual song
    vector<int> _idsX = {0,14,22,33,45,53};
    group * _groupX = new group('x', _idsX);
    assignGroup(_groupX);
    
    vector<int> _idsY = {4,18,26,37,48};
    group * _groupY = new group('x', _idsX);
    assignGroup(_groupY);

    //createLayoutByParam();
}

//--------------------------------------------------------------
void ofApp::update(){

    //Update the touch manager
    TouchManager::one().update();

    float dt = 1.0f / 60.0f;
    animMouth.update(dt);
    
    if (state == STATE_PLAYBACK && nextCmdIndex < arrCmds.size()) {
        
        float _n = arrCmds[nextCmdIndex].timecode;
        
        float _t = ofGetElapsedTimef() - timeCode;

        if (_t > _n) {
            
            ofLog() << _t << " " << _n;

            arrPlayedCmds.push_back(arrCmds[nextCmdIndex]);
            writeCommand(arrCmds[nextCmdIndex].cmd, arrCmds[nextCmdIndex].cmdType, false);

            nextCmdIndex++;

            //Start over song
            if (arrCmds.size() == nextCmdIndex && isLoop) {

                loadAndPlaySong();
            }

        }
    }

    if (state != STATE_PLAYBACK) {
        
        if (ofGetElapsedTimef() > nextTail && isFlipping) {
            
            float _t = ofGetElapsedTimef() - timeCode;
            
            if (bTailOn) {
                
                writeCommand(CMD_TAIL_ON, 1, true);
                
                nextTail = ofGetElapsedTimef() + 1.0;
            }
            else
            {
                writeCommand(CMD_TAIL_OFF, 1, true);
                
                nextTail = ofGetElapsedTimef() + 1.2;
            }

            bTailOn = !bTailOn;
        }
    }
    
    //Update each fish
    for (int i=0; i<arrFish.size(); i++)
    {
        arrFish[i]->update();
    }

}

void ofApp::drawStairs()
{

    
}

void ofApp::createLayoutByParam() {
    
    int _id = 0;
    char _controllerIndex;
    int _driverIndex = 0;

    int _x, _y;

    ofLog() << ">>> Create Layout by Param";

    ofLog() << "Cols : " << bbCols.get();
    ofLog() << "Rows : " << bbRows.get();

    for (int i=0; i<bbCols.get(); i++) {
        
        for (int j=0; j<bbRows.get(); j++) {

            _x = i * bbColSpacing.get();
            _y = j * bbRowSpacing.get();

            //ofLog() << _x << "," << _y;

            _id++;
            bool isLead = false;
            
            if (_id % 4 == 1)
                isLead = true;
            
            if (_id % 3 == 0) {
                _controllerIndex++;
            }

            fish * _f = new fish();
            
            _f->setup(
                                     _id,
                                     _controllerIndex,
                                     _driverIndex,
                                     ofVec2f(_x, _y),
                                     isLead,
                                     1,
                                    panelFish
                                     );
            
            scene->addChild(_f);
            arrFish.push_back(_f);

        }
    }
}

void ofApp::readLayoutJsonFile() {
    
    ofFile file(ofToDataPath(layoutFile));
    ofJson js;
    
    ofLog() << "* Read layout file : " << layoutFile;
    
    if(file.exists()){
        
        file >> js;
        arrFish.clear();
        
        int _count = 0;
        bool _isLead = false;

        for(auto & _fish: js){

            ofLog() << _fish;
            _isLead = false;

            if (!_fish["groups"].empty()) {
                ofLog() << "*** IS EMPTY";
                _isLead = true;
            }

            fish * _f = new fish();
            
            ofLog() << _fish["pos"][0] << " " << _fish["pos"][1];
            
            ofLog() << "Controller Index : " << _fish["controllerIndex"] << " " << ofToChar(_fish["controllerIndex"]);
            
            _f->setup(
                                _count,
                                ofToChar(_fish["controllerIndex"]),
                                _fish["driverIndex"],
                                ofVec2f(_fish["x"],_fish["y"]),
                                _isLead,
                                1,
                                panelFish
            );

            scene->addChild(_f);
            arrFish.push_back(
                _f
            );

            _count++;
        }

        ofLog() << "* Read Fish In" << _count;

    }
}

void ofApp::writeLayoutJsonFile() {
    
    Json::Value arrFishJson(Json::arrayValue);
    
    for (int i=0; i<arrFish.size(); i++) {
        
        Json::Value _fish;
        Json::Value _arrBB(Json::arrayValue);
        
        _fish["id"] = arrFish[i]->id;
        _fish["controllerIndex"] = ofToString(arrFish[i]->controllerIndex);
        _fish["driverIndex"] = arrFish[i]->driverIndex;
        _fish["x"] = arrFish[i]->getPosition().x;
        _fish["y"] = arrFish[i]->getPosition().y;

        //_fish["pos"] = {0,1};

        arrFishJson.append(_fish);
    }
    
    ofLog() << arrFishJson;
    ofLog() << "* Write " << arrFishJson.size();
    
    std::ofstream file_id;
    file_id.open(ofToDataPath(layoutFile));
    
    Json::Value value_obj;
    //populate 'value_obj' with the objects, arrays etc.
    
    Json::StyledWriter styledWriter;
    file_id << styledWriter.write(arrFishJson);

    file_id.close();
}

void ofApp::loadAndPlaySong() {

    //Assign Groups
    //for (int i=0; i<arrSongs[songIndex].arrGroup.size(); i++)
    //{
    //    assignGroup(arrSongs[songIndex].arrGroup[i]);
    //}

    //Load this song
    playerSound.load(ofToDataPath(arrSongs[songIndex].songFile));
    
    //Load the commands
    ofFile file(ofToDataPath(arrSongs[songIndex].cmdFile));
    ofJson js;

    if(file.exists()){
        
        file >> js;
        arrCmds.clear();

        int _count = 0;
        for(auto & _cmd: js){

            //ofLog() << _cmd;
            int _cmdType = CMD_TYPE_ALL;

            if (!_cmd["group"].empty()) {
                _cmdType = CMD_TYPE_LEAD;
            }

            arrCmds.push_back(
                bbcmd(
                      _cmd["cmd"],
                      _cmd["timecode"],
                      _cmd["set"],
                      _cmdType
                )
            );
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
        _cmd["set"] = arrCmds[i].sCmd;

        if (arrCmds[i].cmdType == CMD_TYPE_LEAD) {
            _cmd["group"] = {0};
        }

        ofLog() << arrCmds[i].cmd << "\t" << arrCmds[i].timecode;

        arrCommands.append(_cmd);
    }

    ofLog() << arrCommands;
    ofLog() << "* Write " << arrCmds.size();
    
    std::ofstream file_id;
    file_id.open(ofToDataPath("out.json"));
    
    Json::Value value_obj;
    //populate 'value_obj' with the objects, arrays etc.
    
    Json::StyledWriter styledWriter;
    file_id << styledWriter.write(arrCommands);
    
    file_id.close();}

//--------------------------------------------------------------
void ofApp::draw(){

    //ofSetColor(0);
    ofPushMatrix();
    
        ofTranslate(masterX.get(), masterY.get());
    
        ofPushMatrix();
    
        ofScale(masterScale.get());
    
        //Draw backgtround image
        imgStairs.draw(bbBGX.get(), bbBGY.get(), bbBGScale.get()*1102, bbBGScale.get()*702);

        for (int i=0; i<arrFish.size(); i++)
        {
            arrFish[i]->draw(bbScale.get(), bbShowDebug.get() );
        }
    
        ofPopMatrix();
    ofPopMatrix();
    
    float _t = 0;
    if (state == STATE_RECORD || state == STATE_PLAYBACK) {
        _t = ofGetElapsedTimef() - timeCode;
    }

    ofPushStyle();
    
    ofSetColor(0);
    ttf.drawString(ofToString(_t), 20, 140);
    
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

    //ofNoFill();
    //ofSetLineWidth(3);
    //ofDrawCircle(ofGetWidth()*.5, ofGetHeight()*.5, _r);

    int _offset = 40;
    string _s = "";

    ofPushStyle();
    
    ofSetColor(50);

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
    ofPopStyle();

    //Draw each fish
    //ofPushMatrix();
    //ofTranslate(bbOriginX.get(), bbOriginY.get());

    ofPushStyle();
    ofSetColor(0,255,0);
    ttf.drawString("Billy Bass Wall", 20, 40);
    ttf.drawString("\"" + arrSongs[songIndex].songFile + "\"", 20, 90);
    ofPopStyle();
    
    //Draw
    if (bShowGui)
    {
        panel.draw();
        //panelFish->draw();
    }

}

string ofApp::buildCommandString(int _cmd, int _type)
{
    string _scmd = ofToString(_cmd);

    if (_type == 0) {

        _scmd += "x"; //ofToString(LEAD_BASS);
    } else if (_type == 2) {

        _scmd += "0124";
    }

    _scmd += "\n";

    return _scmd;
}

void ofApp::assignGroup(group * _group)
{

    //Write to the hardware
    for (int i=0; i<_group->arrFishID.size(); i++)
    {
        
         string _cmd = "6" +
            ofToString(arrFish[_group->arrFishID[i]]->controllerIndex) +
            ofToString(arrFish[_group->arrFishID[i]]->driverIndex) +
            ofToString(_group->groupID);

        ofLog() << _cmd;
        
        //Set Group ID
        arrFish[_group->arrFishID[i]]->groupID = _group->groupID;
        
        ofLog() << arrFish[_group->arrFishID[i]]->groupID;

        //Write the Bytes
        serial.writeBytes(_cmd.c_str(), _cmd.length());

    }

}

void ofApp::writeCommand(int _cmdID, int _cmdType, bool _record, char _groupID)
{

    int _mouth = -1;
    int _body = -1;

    string _cmd = buildCommandString(_cmdID, _cmdType);

    float _t = ofGetElapsedTimef() - timeCode;

    switch (_cmdID) {

        case CMD_MOUTH_OPEN:
            _mouth = STATE_MOUTH_OPEN;
            break;
        
        case CMD_MOUTH_CLOSE:
            _mouth = STATE_MOUTH_CLOSE;
            break;

        case CMD_TAIL_ON:
            _body = STATE_BODY_TAIL;
            break;

        case CMD_HEAD_ON:
            _body = STATE_BODY_HEAD;
            break;
            
        case CMD_BODY_OFF:
        case CMD_TAIL_OFF:
            _body = STATE_BODY_OFF;
            break;

    }

    //Set visuals for all fish
    setAllBodyState(_mouth, _body, _cmdID, _cmdType, _groupID);

    //Write to the hardware
    serial.writeBytes(_cmd.c_str(), _cmd.length());

    //If is a subgroup
    bool _isGroup = false;
    /*
    if (_cmdType == CMD_TYPE_LEAD) {
        _isGroup = true;
    }*/

    if (_record && state == STATE_RECORD)
        arrCmds.push_back(bbcmd(_cmdID, _t, _cmd, _isGroup));

}

void ofApp::setAllBodyState(int _mouthState, int _bodyState, int _cmdID, int _cmdType, char _groupID)
{

    for (int i=0; i<arrFish.size(); i++)
    {
        if (
            _cmdType == CMD_TYPE_ALL
            ||
            _groupID == arrFish[i]->groupID
        )
        {
        //    (_cmdType == CMD_TYPE_LEAD && arrFish[i]->isLead)
        //    ||
        //    (_cmdType == CMD_TYPE_OTHERS && !arrFish[i]->isLead)
        //)
        //{
            if (arrFish[i]->getBodyState() == STATE_BODY_HEAD && (_cmdID == CMD_TAIL_ON || _cmdID == CMD_TAIL_OFF)) {
                arrFish[i]->setBodyState(_mouthState, -1);
            } else {
                arrFish[i]->setBodyState(_mouthState, _bodyState);
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    char _groupID = NULL;

    if (_keyOff == true) {
        
        _keyOff = false;
        
    switch (key) {
            
        case 's':
            
            if (state == STATE_WAIT) {
                
                state = STATE_RECORD;
                isFlipping = !isFlipping;

                playerSound.play();
                playerSound.setPosition(OFFSET_POSITION);

            } else if (state == STATE_RECORD) {
                
                
                isPaused = !isPaused;
                playerSound.setPaused(isPaused);
                
            } else if (state == STATE_PLAYBACK) {

                playerSound.stop();
                state = STATE_WAIT;
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
                
                //float _t = ofGetElapsedTimef() - timeCode;

                int _cmd = CMD_MOUTH_OPEN;
                int _cmdType;
                
                if (key == 'k') {
                    
                    _lastCmd = LAST_MOUTH_ALL;
                    _cmdType = CMD_TYPE_ALL;

                } else if (key == 'j' ) {
                    
                    _lastCmd = LAST_MOUTH_LEAD;
                    _cmdType = CMD_TYPE_LEAD;
                    
                    _groupID = 'x';

                } else {

                    _lastCmd = LAST_MOUTH_OTHERS;
                    _cmdType = CMD_TYPE_OTHERS;
                }

                writeCommand(_cmd, _cmdType, true, _groupID);
            }
            break;

        case 'u':
        case 'i':

            if (state != STATE_PLAYBACK) {


                int _cmd = CMD_HEAD_ON;
                int _cmdType;
                
                if (key == 'i') {
                    _cmdType = 1;
                } else {
                    _cmdType = 0;
                }
                
                writeCommand(_cmd, _cmdType, true);
            }
            break;

        case '7':
        case '8':
            
            if (state != STATE_PLAYBACK) {
                
                int _cmd = CMD_TAIL_ON;
                int _cmdType;
    
                if (key == '8') {
                    _cmdType = 1;
                } else {
                    _cmdType = 0;
                }
                
                writeCommand(_cmd, _cmdType, true);
            }
            break;

        case 'm':
        case ',':
            
            if (state != STATE_PLAYBACK) {

                int _cmdType;

                if (key == ',') {
                    _cmdType = 1;
                } else {
                    _cmdType = 0;
                }
                
                writeCommand(CMD_BODY_OFF, _cmdType, true);

            }
            break;
            
        case 'w':

            ofLog() << "Commands:";

            writeJsonFile();
            break;
            
        case 'a':
            
            if (state == STATE_WAIT) {
            
                ofLog() << "Advance File";
                songIndex++;
                if (songIndex >= arrSongs.size()) {
                    songIndex = 0;
                }
            }

            break;

        case 'o':
            
            ofLog() << "Read layout json file.";
            loadAndPlaySong();
            break;
            
        case 'p':
            
            isFlipping = !isFlipping;
            break;
            
        case 'x':
            
            for (int i=0; i<arrFish.size(); i++)
            {
                arrFish[i]->setBodyState(STATE_MOUTH_OPEN, -1);
            }
            break;
        
        case '1':

            writeLayoutJsonFile();
            break;
        }
    }
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key){

    _keyOff = true;
    char _groupID = NULL;

    switch (key) {

        case 'j':
        case 'k':
        case 'l':
            
            if (state != STATE_PLAYBACK) {
                
                //float _t = ofGetElapsedTimef() - timeCode;
                
                int _cmdType;

                if (key == 'k')
                    _cmdType = 1;
                else if (key == 'j') {
                    _cmdType = 0;
                    _groupID = 'x';
                }
                else
                    _cmdType = 2;
                    //_cmd = buildCommandString(CMD_MOUTH_CLOSE,2);
                    
                animMouth.animateTo(0);

                writeCommand(CMD_MOUTH_CLOSE, _cmdType, true, _groupID);

                //serial.writeBytes(_cmd.c_str(), _cmd.length());
                //if (state == STATE_RECORD)
                //   arrCmds.push_back(bbcmd(CMD_MOUTH_CLOSE, _t, _cmd));
            }
            break;
            
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

void ofApp::mouseDragged(int x, int y, int button){
    
    /******
     * pass touch/mouse events to the touch manager
     */
    TouchManager::one().touchMove(button, ofVec2f(x, y));
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    /******
     * pass touch/mouse events to the touch manager
     */
    TouchManager::one().touchDown(button, ofVec2f(x, y));
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
    /******
     * pass touch/mouse events to the touch manager
     */
    TouchManager::one().touchUp(button, ofVec2f(x, y));
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
