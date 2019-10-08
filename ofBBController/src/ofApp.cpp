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

    layoutFile = "new_layout.json";
    
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

    //Add Songs to Queue
    vector<string> _tracksHall = {};
    arrSongs.push_back(song("hallandoats.mp3","hallandouts_good1.json",_tracksHall));
    
    vector<string> _tracksSade = {};
    arrSongs.push_back(song("smooth_operator.mp3","smooth_operator.json",_tracksSade));
    
    vector<string> _tracks = {"chorus.json","guy.json","lead1.json","lead2.json"};
    arrSongs.push_back(song("shoop.mp3","out.json",_tracks));
    
    songIndex = 2;

    //Load First Song
    playerSound.load(ofToDataPath(arrSongs[songIndex].songFile));

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
    panelGroup.add(masterX.set("Master X",0,-2*ofGetWidth(),2*ofGetWidth()));
    panelGroup.add(masterY.set("Master Y",0,-2*ofGetHeight(),2*ofGetHeight()));
    panelGroup.add(bbScale.set("Bass Scale",.1,.05,5));

    panelGroup.add(bbFlipSpeed.set("Flip Speed",1,.1,3));
    
    //panelGroup.add(bbOriginX.set("Origin X",0,0,ofGetWidth()));
    //panelGroup.add(bbOriginY.set("Origin Y",0,0,ofGetHeight()));
    //panelGroup.add(bbSlope.set("Slope",0,0,200));
    
    panelGroup.add(bbBGX.set("Background X",0,-ofGetWidth(),ofGetWidth()));
    panelGroup.add(bbBGY.set("Background Y",0,-ofGetHeight(),ofGetHeight()));
    panelGroup.add(bbBGScale.set("Background Scale",1,0,10));
    panelGroup.add(bbShowDebug.set("Show Debug"));

    //panelGroup.add(bbFlatIndex.set("Flat Index", 0, 0, 21));
    //panelGroup.add(bbFlatOffset.set("Flat Offset", 0, -100, 100));
    
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

    vector<int> _idsLead1 = {9,20,30,42}; //14,22,33,45,53};
    arrGroups.push_back(new group('v', _idsLead1));
    
    vector<int> _idsLead2 = {0,14,22,34};
    arrGroups.push_back(new group('y', _idsLead2));
    
    //Create All the groups - Move this to the individual song
    vector<int> _idsFlip1 = {0,1,6,7,8,9,10,16,17,18,19,21,22,23,28,29,30,31,36,37,38,39,40,45,46,47,48,53,52};
    arrGroups.push_back(new group('w', _idsFlip1));

    vector<int> _idsFlip2 = {2,3,4,5,11,12,13,14,15,20,24,25,26,27,32,33,34,35,41,42,43,44,49,50,51,54,55};
    arrGroups.push_back(new group('z', _idsFlip2));


    //Create the groups
    if (bCreateGroups) {
    
        for (int i=1; i>=0; i--) {
            assignGroup(arrGroups[i]);
        }
    }

    //assignGroup(_groupLead1);
    //assignGroup(_groupLead2);
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
            writeCommand(arrCmds[nextCmdIndex].cmd, false, arrCmds[nextCmdIndex].group);

            nextCmdIndex++;

            //Start over song
            if (arrCmds.size() == nextCmdIndex && isLoop) {

                loadAndPlaySong();
            }

        }
    }

    if (state != STATE_PLAYBACK) {
        
        if (isFlipping)
        {
            
            if (isTailUp && ofGetElapsedTimef() > nextTailOff)
            {
                writeCommand(CMD_TAIL_OFF, true, arrGroups[tailGroup]->groupID);
                isTailUp = false;
            }
        
            if (ofGetElapsedTimef() > nextTail) {
            
                float _t = ofGetElapsedTimef() - timeCode;
            
                if (bTailOn) {
                
                    //writeCommand(CMD_TAIL_ON, true, arrGroups[2]->groupID);
                    writeCommand(CMD_TAIL_ON, true, arrGroups[2]->groupID);
                
                    //writeCommand(CMD_TAIL_OFF, true, arrGroups[3]->groupID);

                    tailGroup = 2;
                    nextTail = ofGetElapsedTimef() + bbFlipSpeed.get();
                }
                else
                {
                    writeCommand(CMD_TAIL_ON, true, arrGroups[3]->groupID);
                
                    tailGroup = 3;
                    nextTail = ofGetElapsedTimef() + bbFlipSpeed.get();
                }
                nextTailOff = ofGetElapsedTimef() + .2;

                isTailUp = true;
                bTailOn = !bTailOn;
            }
            
        }
    }
    
    //Update each fish
    for (int i=0; i<arrFish.size(); i++)
    {
        if (arrFish[i]->update() == true) {

            writeCommand(CMD_BODY_OFF, true, arrFish[i]->groupID);
            //arrGroups[_groupIndex]->bodyState = CMD_HEAD_ON;
        }
    }

}

void ofApp::buttonPress() {

    if (state == STATE_WAIT) {
        
        ofLog() << "Advance File";
        songIndex++;
        if (songIndex >= arrSongs.size()) {
            songIndex = 0;
        }
        
        loadAndPlaySong();
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

            char _group = ofToChar(_cmd["group"]);

            if (_group == '\x01') {
                _group = NULL;
            }

            arrCmds.push_back(
                bbcmd(
                      _cmd["cmd"],
                      _cmd["timecode"],
                      _cmd["set"],
                      _group
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
        _cmd["group"] = ofToString(arrCmds[i].group);

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

string ofApp::buildCommandString(int _cmd, char _group)
{
    string _scmd = ofToString(_cmd);

    if (_group != NULL) {

        _scmd += _group; //ofToString(LEAD_BASS);
    }

    _scmd += "\n";

    return _scmd;
}

void ofApp::testAllTails() {
    
    ofLog() << "* Test All Tails";
    
    //Write to the hardware
    int assignIndex = 0;
    int fishIndex = 0;
    int currFish = 0;

    float _nextTimeOn = 0;
    float _nextTimeOff = 0;
    bool isUp = false;

    while (fishIndex < arrFish.size())
    {

        if (currFish > 0 && isUp && ofGetElapsedTimef() > _nextTimeOff)
        {
            isUp = false;
            
            string _cmd = "4" +
            ofToString(arrFish[currFish]->controllerIndex) +
            ofToString(arrFish[currFish]->driverIndex) +
            "\n";
         
            //Write the Bytes
            serial.writeBytes(_cmd.c_str(), _cmd.length());
        }

        if (ofGetElapsedTimef() > _nextTimeOn)
        {
            string _cmd = "2" +
            ofToString(arrFish[fishIndex]->controllerIndex) +
            ofToString(arrFish[fishIndex]->driverIndex) +
            "\n";
            
            ofLog() << _cmd;

            //Write the Bytes
            serial.writeBytes(_cmd.c_str(), _cmd.length());

            isUp = true;
            currFish = fishIndex;
            
            /*
            _cmd = "0" +
            ofToString(arrFish[fishIndex]->controllerIndex) +
            ofToString(arrFish[fishIndex]->driverIndex) +
            "\n";
            
            ofLog() << _cmd;
            
            //Write the Bytes
            serial.writeBytes(_cmd.c_str(), _cmd.length());
            */
            
            /*
            if (currFish >= 0) {
                _cmd = "4" +
                ofToString(arrFish[currFish]->controllerIndex) +
                ofToString(arrFish[currFish]->driverIndex) +
                "\n";
            
                //Write the Bytes
                serial.writeBytes(_cmd.c_str(), _cmd.length());
             
                _cmd = "1" +
                ofToString(arrFish[currFish]->controllerIndex) +
                ofToString(arrFish[currFish]->driverIndex) +
                "\n";
                
                //Write the Bytes
                serial.writeBytes(_cmd.c_str(), _cmd.length());
            }
            */

            _nextTimeOn = ofGetElapsedTimef() + .15;

            _nextTimeOff = ofGetElapsedTimef() + .10;
            fishIndex++;
            
        }
        
        /*
        if (fishIndex > 0 && ofGetElapsedTimef() > _nextTimeOff)
        {
            string _cmd = "4" +
            ofToString(arrFish[fishIndex - 1]->controllerIndex) +
            ofToString(arrFish[fishIndex - 1]->driverIndex) +
            "\n";
                
            //Write the Bytes
            serial.writeBytes(_cmd.c_str(), _cmd.length());
        }*/
    }
    
}

void ofApp::assignGroup(group * _group)
{
    ofLog() << "* Assign Group";

    //Write to the hardware
    int assignIndex = 0;
    int fishIndex = 0;
    float _nextTime = 0;

    while (fishIndex < _group->arrFishID.size())
    {
        
        if (ofGetElapsedTimef() > _nextTime)
        {
            string _cmd = "6" +
                ofToString(arrFish[_group->arrFishID[fishIndex]]->controllerIndex) +
                ofToString(arrFish[_group->arrFishID[fishIndex]]->driverIndex) +
                ofToString(_group->groupID) +
                "\n";

            ofLog() << _cmd;

            //Set Group ID
            arrFish[_group->arrFishID[fishIndex]]->groupID = _group->groupID;

            //Write the Bytes
            serial.writeBytes(_cmd.c_str(), _cmd.length());
            
            _nextTime = ofGetElapsedTimef() + .5;
            fishIndex++;
        }
    }

}

void ofApp::writeCommand(int _cmdID, bool _record, char _groupID)
{

    int _mouth = -1;
    int _body = -1;

    string _cmd = buildCommandString(_cmdID, _groupID);

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
    setAllBodyState(_mouth, _body, _cmdID, _groupID);

    //Write to the hardware
    serial.writeBytes(_cmd.c_str(), _cmd.length());

    ofLog() << ">>>" << _cmd.c_str();

    //If is a subgroup
    bool _isGroup = false;
    /*
    if (_cmdType == CMD_TYPE_LEAD) {
        _isGroup = true;
    }*/

    if (_record && state == STATE_RECORD)
        arrCmds.push_back(bbcmd(_cmdID, _t, _cmd, _groupID));

}

void ofApp::setAllBodyState(int _mouthState, int _bodyState, int _cmdID, char _groupID)
{

    for (int i=0; i<arrFish.size(); i++)
    {
        if (
            _groupID == NULL
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
int ofApp::getKeyIndex(char _key)
{

    switch(_key) {
        
        case '6':
        case 'y':
        case 'h':
        case 'n':
            return -1;
        
        case '7':
        case 'u':
        case 'j':
        case 'm':
            return 0;
            
        case '8':
        case 'i':
        case 'k':
        case ',':
            return 1;
            
        case '9':
        case 'o':
        case 'l':
        case '.':
            return 2;
    }

    return NULL;
}

void ofApp::keyPressed(int key){

    char _groupID = NULL;

    if (_keyOff == true) {
        
        _keyOff = false;
    
        int _groupIndex = getKeyIndex(key);
        if (_groupIndex >= 0) {
            _groupID = arrGroups[_groupIndex]->groupID;
        }
    switch (key) {
            
        case 's':
            
            if (state == STATE_WAIT) {
                
                state = STATE_RECORD;
                //isFlipping = !isFlipping;

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

        case '6':
        case '7':
        case '8':
        case '9':
            
            if (state != STATE_PLAYBACK) {

                animMouth.animateTo(1);
                
                //float _t = ofGetElapsedTimef() - timeCode;

                int _cmd = CMD_MOUTH_OPEN;
                
                /*
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
                 */
                
                if (key == '6') {
                    isFlipping = true;
                }

                if ((key == '7' || key == '8') && arrGroups[_groupIndex]->bodyState != CMD_HEAD_ON) {

                    writeCommand(CMD_HEAD_ON, true, _groupID);
                    arrGroups[_groupIndex]->bodyState = CMD_HEAD_ON;
                    //setBodyState(-1, STATE_BODY_HEAD);
                }

                writeCommand(_cmd, true, _groupID);
            }
            break;

        case 'h':
        case 'j':
        case 'k':
        case 'l':
            
            if (state != STATE_PLAYBACK) {


                int _cmd = CMD_HEAD_ON;
                
                /*
                int _cmdType;
                
                if (key == 'i') {
                    _cmdType = 1;
                } else {
                    _cmdType = 0;
                }*/
                
                if (_groupIndex >= 0) {
                    arrGroups[_groupIndex]->bodyState = CMD_HEAD_ON;
                }

                writeCommand(_cmd, true, _groupID);
            }
            break;


        case 'y':
        case 'u':
        case 'i':
        case 'o':
    
            if (state != STATE_PLAYBACK) {

                int _cmd = CMD_TAIL_ON;
                writeCommand(_cmd, true, _groupID);
            }
            break;

        case 'n':
        case 'm':
        case ',':
        case '.':
            
            if (state != STATE_PLAYBACK) {

                /*int _cmdType;

                if (key == ',') {
                    _cmdType = 1;
                } else {
                    _cmdType = 0;
                }*/
                
                if (_groupIndex >= 0) {
                    arrGroups[_groupIndex]->bodyState = CMD_BODY_OFF;
                }
                writeCommand(CMD_HEAD_ON, true, _groupID);
                writeCommand(CMD_BODY_OFF, true, _groupID);
            }
            break;

        case 'w':
            ofLog() << "Commands:";

            writeJsonFile();
            break;

        case 'a':
            
            buttonPress();

            break;

        case 'p':
            
            ofLog() << "Read layout json file.";
            loadAndPlaySong();
            break;
            
        case 't':
            
            isFlipping = !isFlipping;
            break;
            
        //case 'x':
            
        //    for (int i=0; i<arrFish.size(); i++)
        //    {
        //        arrFish[i]->setBodyState(STATE_MOUTH_OPEN, -1);
        //    }
        //    break;
        
        case '1':

            writeLayoutJsonFile();
            break;
        
        case '2':

            testAllTails();
            break;
        }
    
    }
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key){

    _keyOff = true;
    char _groupID = NULL;

    int _groupIndex = getKeyIndex(key);
    if (_groupIndex >= 0) {
        _groupID = arrGroups[_groupIndex]->groupID;
    }
    
    switch (key) {

        case '6':
        case '7':
        case '8':
        case '9':
            
            if (state != STATE_PLAYBACK) {
                
                //float _t = ofGetElapsedTimef() - timeCode;
                
                /*
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
                */
                
                animMouth.animateTo(0);

                writeCommand(CMD_MOUTH_CLOSE, true, _groupID);

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
