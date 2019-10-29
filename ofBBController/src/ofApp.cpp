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

    isPaused = false;
    
    ttf.load("mono.ttf", 35);
    ttf_side.load("mono.ttf", 14);

    path.setStrokeColor(0);
    path.setFilled(false);
    path.setStrokeWidth(1);

    animMouth.reset(0);
    animMouth.setCurve(LINEAR);
    animMouth.setDuration(.1);
    
    animFadeOut.reset(0);
    animFadeOut.setCurve(LINEAR);
    animFadeOut.setDuration(3);

    _keyOff = true;

    const int CMD_MOUTH_OPEN = 0;
    const int CMD_MOUTH_CLOSE = 1;
    const int CMD_TAIL_ON = 2;
    const int CMD_HEAD_ON = 3;
    const int CMD_BODY_OFF = 4;
    const int CMD_TAIL_OFF = 5;
    const int CMD_BODY_RELEASE = 7;
    const int CMD_TAIL_RELEASE = 8;
    
    arrCmdNames[0] = "MOUTH_OPEN";
    arrCmdNames[1] = "MOUTH_CLOSE";
    arrCmdNames[2] = "TAIL_ON";
    arrCmdNames[3] = "HEAD_ON";
    arrCmdNames[4] = "BODY_OFF";
    arrCmdNames[5] = "TAIL_OFF";
    arrCmdNames[6] = "Program";
    arrCmdNames[7] = "BODY_RELEASE";
    arrCmdNames[8] = "TAIL_RELEASE";
    
    arrStateNames[0] = "WAIT";
    arrStateNames[1] = "RECORD";
    arrStateNames[2] = "PLAYBACK";
    arrStateNames[3] = "PAUSED";
    arrStateNames[4] = "JUKEBOX";
    
    nextCmdIndex = 0;
    
    vector<string> _tracksChoices = {"choices_chorus.json","choices_lead1.json","choices_lead2.json", "choices_beat.json", "choices_accenta.json", "choices_test.json"};
    arrSongs.push_back(song("choices.mp3","",_tracksChoices, 165));
    
    vector<string> _tracksLou = {"walk_lead.json","walk_chorus.json"};
    arrSongs.push_back(song("mobydick.mp3","",_tracksLou, 222));
    //arrSongs.push_back(song("walkonthewildside.mp3","",_tracksLou, 222));
    
    //Add Songs to Queue
    vector<string> _tracksTH = {"th_chrous.json","th_lead1.json","th_lead2.json", "th_beat.json"};
    arrSongs.push_back(song("talking_heads_once.mp3","talkingheads.json",_tracksTH, 240));
    
    //vector<string> _tracksS = {"chorus.json","guy.json","lead1.json","lead2.json"};
    //arrSongs.push_back(song("shoop.mp3","out.json",_tracksS));
    
    vector<string> _tracksSA = {"sa_beat_2.json", "sa_lead1.json", "sa_lead2.json", "sa_lead3.json", "sa_chorus_new.json"};
    arrSongs.push_back(song("stayingalive.mp3","out.json",_tracksSA, 101));

    //vector<string> _mt_se = {"mt_se_lead.json", "mt_se_beat.json"};
    //arrSongs.push_back(song("straight_edge.mp3","out.json",_mt_se, 0));
    songIndex = 0;

    //Load First Song
    playerSound.load(ofToDataPath(arrSongs[songIndex].songFile));
    songLength = getSongLength();

    //Set up Serial
    serial.listDevices();
    vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();

    int baud = 57600; //19200;
    serial.setup(0, baud); //open the first device
    
    bTailOn = false;
    nextTail = 0;
    
    isFlipping = false;
    
    //Open stairs image
    imgStairs.load("images/royalPalms_67Fish.jpg");

    //Add All Gui elements to set
    panelGroup.setName("Bass Layout Params");

    panelGroup.add(masterScale.set("Master Scale",1,.1,10));
    panelGroup.add(masterX.set("Master X",0,-2*ofGetWidth(),2*ofGetWidth()));
    panelGroup.add(masterY.set("Master Y",0,-2*ofGetHeight(),2*ofGetHeight()));
    panelGroup.add(bbScale.set("Bass Scale",.1,.05,5));

    panelGroup.add(bbFlipSpeed.set("Flip Speed",1,.1,3));

    panelGroup.add(bShowDisplay.set("Show display", false));

    panelGroup.add(bbShowDebug.set("Show Debug",false));

    panelGroup.add(autoSleep.set("Auto sleep lead",false));
    panelGroup.add(showTimeline.set("Show timeline", true));
    panelGroup.add(isDrawBang.set("Draw Bang",true));
    panelGroup.add(isRecording.set("Is recording",false));
    
    panelGroup.add(isCreateGroups.set("Create Groups",false));

    panelGroup.add(maxDiff.set("Max diff",50,0,200));
    panelGroup.add(songVolume.set("Song volume",1.0,0,1.0));
    panelGroup.add(songTweak.set("Song Tweak",0,-500,500));
    panelGroup.add(bbSongMute.set("Mute Song",false));
    
    panelGroup.add(bbTimelineScale.set("Timeline Scale",1,1,25));

    panelGroup.add(bbTimelineScale.set("Timeline Scale",1,1,25));
    panelGroup.add(bbTimelineSlide.set("Timeline Slide",0,0,1));
    panelGroup.add(bbTimelineScrub.set("Timeline Scrub",0,0,1));

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

    vector<int> _idsSALead1 = {5,19,27,40,51};
    vector<int> _idsSALead2 = {4,18,26,39,50};
    vector<int> _idsSALead3 = {3,17,25,38,49};
    vector<int> _idsSALead2and3 = {4,18,26,39,50,3,17,25,38,49};

    vector<int> _idsAccentsA = {9,20,30,43};

    vector<int> _mt1 = {4};
    vector<int> _mt2 = {4,3,9,10,0,1,5};
    vector<int> _mt3 = {4,3,9,10,0,1,5,2,8,14,7,15};
    vector<int> _mt4 = {4,3,9,10,0,1,5,2,8,14,15,6,7,12,13,18,19,17,11,16,20};

    arrGroups.push_back(new group('A', _idsSALead1));
    arrGroups.push_back(new group('B', _idsSALead2));
    arrGroups.push_back(new group('C', _idsSALead3));

    vector<int> idsAllBut = {
        0,1,
        2,4,
        6,7,8,9,10,
        11,12,13,14,15,
        16,18,
        20,
        21,22,23,
        24,26,
        28,29,30,31,
        32,33,34,35,
        36,37,39,
        41,42,43,44,
        45,46,47,48,
        50,
        52,53,56,
        54,55
    };
    
    vector<int> _idsChorus1 = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    vector<int> _idsChorus3 = {16,17,18,19,20,21,22,23,24,25,26,27};
    vector<int> _idsChorus5 = {28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44};
    vector<int> _idsChorus7 = {45,46,47,48,49,50,51,52,53,54,55,56};

    vector<int> _idsChorusA = {0,1,
        6,7,8,9,10,
        16,17,18,19,
        21,22,23,
        28,29,30,31,
        36,37,38,39,40,
        45,46,47,48,
        52,53,56};

    vector<int> _idsChorusB = {2,3,4,5,
        11,12,13,14,15,
        20,
        24,25,26,27,
        32,33,34,35,
        41,42,43,44,
        49,50,51,
        54,55};

    arrGroups.push_back(new group('D', _idsChorus1));
    arrGroups.push_back(new group('F', _idsChorus3));
    arrGroups.push_back(new group('H', _idsChorus5));
    arrGroups.push_back(new group('J', _idsChorus7));

    arrGroups.push_back(new group('E', _idsSALead2and3));

    arrGroups.push_back(new group('N', idsAllBut));
    arrGroups.push_back(new group('O', _idsAccentsA));
    
    arrGroups.push_back(new group('L', _idsChorusA));
    arrGroups.push_back(new group('M', _idsChorusB));
    
    vector<int> _clearArr = {
        0,1,2,3,4,5,6,7,8,9,10,
        11,12,13,14,15,16,17,18,19,20,
        21,22,23,24,25,26,27,28,29,30,
        31,32,33,34,35,36,37,38,39,40,
        41,42,43,44,45,46,47,48,49,50,
        51,52,53,54,55
    };

    //assignGroup(_clearGroup);

    //Create the groups
    bool _initGroup = true;
    for (int i=0; i<arrGroups.size(); i++) {

        assignGroup(arrGroups[i], _initGroup);
        _initGroup = false;
    }

    //Create the timeline
    mainTimeline = new timeline(ofGetWidth(), 100, scene);

    mainTimeline->setRange(songLength, arrCmds);

    //Output Log
    ofLogToFile("log.txt", true);

}

//--------------------------------------------------------------
void ofApp::resetCmdIndex() {

    int _curIndex = 0;
    float _t = playerSound.getPositionMS();
    ofLog() << "* Orig Index : " << nextCmdIndex;

    while (_curIndex < arrCmds.size() && _t > arrCmds[_curIndex]->timecodeL) {
            _curIndex++;
    }

    nextCmdIndex = _curIndex;
    ofLog() << "Post Index : " << nextCmdIndex;
}


void ofApp::update() {

    ofSetWindowTitle(ofToString(ofGetFrameRate()));

    //Update the touch manager
    //TouchManager::one().update();

    //float dt = 1.0f / 60.0f;
    //animMouth.update(dt);

    float dt = 1.0f / 60.0f;
    animFadeOut.update(dt);

    int _t = playerSound.getPositionMS() + songTweak.get();

    if (_t<0) _t=0;

    if ((state == STATE_JUKEBOX || state == STATE_PLAYBACK) && nextCmdIndex < arrCmds.size()) {

        if (playerSound.isPlaying())
        {

            int _nextIndex = arrCmds.size() > (nextCmdIndex + 1) ? nextCmdIndex + 1 : nextCmdIndex;

            int _n = arrCmds[nextCmdIndex]->timecodeL;
            int _nn = arrCmds[_nextIndex]->timecodeL;

            if (_t > _n) {

                string _cmd;

                if (_nn-_n < maxDiff.get()) {

                    arrPlayedCmds.push_back(arrCmds[nextCmdIndex]);
                    displayCommand(arrCmds[nextCmdIndex]->cmd, arrCmds[nextCmdIndex]->group);
                    _cmd = buildCommandString(arrCmds[nextCmdIndex]->cmd, arrCmds[nextCmdIndex]->group);
                
                    nextCmdIndex++;
                    if (nextCmdIndex < arrCmds.size())
                    {
                        arrPlayedCmds.push_back(arrCmds[nextCmdIndex]);
                        displayCommand(arrCmds[nextCmdIndex]->cmd, arrCmds[nextCmdIndex]->group);
                        _cmd += buildCommandString(arrCmds[nextCmdIndex]->cmd, arrCmds[nextCmdIndex]->group);
                    }
                    ofLog() << "DBL " << _cmd;

                } else {

                    arrPlayedCmds.push_back(arrCmds[nextCmdIndex]);
                    displayCommand(arrCmds[nextCmdIndex]->cmd, arrCmds[nextCmdIndex]->group);
                    _cmd = buildCommandString(arrCmds[nextCmdIndex]->cmd, arrCmds[nextCmdIndex]->group);
                
                    ofLog() << _cmd;
                }

                ofLog() << _cmd.c_str();

                serial.writeBytes(_cmd.c_str(), _cmd.length());
                nextCmdIndex++;
            }

            //
            // If Fadeout
            //
            if (arrSongs[songIndex].isFade && (_t > arrSongs[songIndex].fadePoint || nextCmdIndex >= arrCmds.size())) {
            
                if (!animFadeOut.isAnimating()) {
                
                    animFadeOut.reset(1);
                    animFadeOut.animateTo(0);
                    ofLog() << "START ANIMATING";
    
                } else if (animFadeOut.getCurrentValue() <= .1){
                
                    animFadeOut.reset(1);
                    nextSong();
                }
                playerSound.setVolume(animFadeOut.getCurrentValue());
            
                ofLog() << animFadeOut.getCurrentValue();
            }
        }
        else
        {
            ofLog() << "Ass";
        }

    } else {

        playerSound.setVolume(songVolume.get());
    }

    if (state == STATE_PLAYBACK || state == STATE_PAUSED || state == STATE_WAIT) {

        if (isFlipping)
        {

            if (ofGetElapsedTimef() > nextTail) {

                if (bTailOn) {
                    
                    writeCommand(CMD_TAIL_ON, true, NULL);
                    //writeCommand(CMD_TAIL_OFF, true, 'L');
                    
                    tailGroup = 2;
                    nextTail = ofGetElapsedTimef() + bbFlipSpeed.get();
                }
                else
                {

                    //writeCommand(CMD_TAIL_ON, true, 'L');
                    writeCommand(CMD_TAIL_RELEASE, true, NULL);

                    tailGroup = 3;
                    nextTail = ofGetElapsedTimef() + bbFlipSpeed.get();
                }
                nextTailOff = ofGetElapsedTimef() + .2;
                
                isTailUp = true;
                bTailOn = !bTailOn;
            }
        }
    }

    if ( serial.available() > 0) {

        char _in = serial.readByte();
        
        if (_in == '1') {
            ofLog() << "READ BUTTON";
            ofLog() << _in;
            buttonPress();
        }

    }

    //Mute spongs
    if (bbSongMute.get() != bSongMute) {

        playerSound.setVolume(bSongMute);
        bSongMute = bbSongMute.get();
    }

    //Update Timeline
    if (showTimeline.get() && (state == STATE_PLAYBACK || state == STATE_JUKEBOX)) {

        bbTimelineScrub.set(playerSound.getPosition());

    } else if (state == STATE_PAUSED) {

        playerSound.setPosition(bbTimelineScrub.get());
    }

    //Update Groups
    if (autoSleep.get())

        for (int i=0; i<arrGroups.size(); i++)
        {
            if (ofGetElapsedTimef() > arrGroups[i]->lastSpeak + 1 && arrGroups[i]->bodyState == STATE_BODY_HEAD) {
                arrGroups[i]->bodyState = STATE_BODY_OFF;
                writeCommand(CMD_BODY_OFF, true, arrGroups[i]->groupID);
            }
        }

    if (showTimeline.get())
        mainTimeline->update(playerSound.getPosition(), bbTimelineSlide.get());
}

void ofApp::buttonPress() {

    nextSong();
    
    /*
    if (state == STATE_JUKEBOX) {
        
        playerSound.setPaused(true);
        state = STATE_PAUSED;
        
    } else if (state == STATE_PAUSED) {
        
        playerSound.setPaused(false);
        mainTimeline->setRange(songLength, arrCmds);
        
        resetCmdIndex();
        state = STATE_JUKEBOX;
        
    } else if (state == STATE_WAIT) {
        
        mainTimeline->setRange(songLength, arrCmds);
        loadAndPlaySong(true);
    }*/
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
            
            //scene->addChild(_f);
            arrFish.push_back(_f);
        }
    }
}

float ofApp::getSongLength() {

    playerSound.play();
    playerSound.setPosition(0.9999999f);
    float ms = playerSound.getPositionMS() * .001;
    playerSound.setPosition(0);
    playerSound.stop();
    
    return ms;
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

            //scene->addChild(_f);
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

bool sortCmds(bbcmd * a, bbcmd * b) {

    if (a->timecodeL < b->timecodeL) {
        return true;
    } else {
        return false;
    }
}

void ofApp::loadTrack(string _file)
{
    ofFile file(ofToDataPath(_file));
    ofJson js;

    if(file.exists()) {
        
        file >> js;
        
        int _count = 0;
        for(auto & _cmd: js){
            
            char _group = ofToChar(_cmd["group"]);

            if (_group == '\x01') {
                _group = NULL;
            }

            int _tc;
            try {
                _tc = _cmd["timecodeL"];
            }
            catch (exception& e)
            {
                float _f = _cmd["timecode"];
                _tc = 1000 * _f;
            }

            bbcmd * _bbcmd = new bbcmd(
                                       _cmd["cmd"],
                                       _cmd["timecode"],
                                       _tc,
                                       _cmd["set"],
                                       _group
                                       );

            arrCmds.push_back(_bbcmd);
            _count++;
        }
        
        ofLog() << "* Read " << _count;
    }
}

void ofApp::loadAndPlaySong(bool _allTracks) {

    //Load this song
    ofLog() << "Start Load";
    playerSound.load(ofToDataPath(arrSongs[songIndex].songFile));
    ofLog() << "End Load";

    songLength = getSongLength();
    
    //Clear the commands
    arrCmds.clear();
    //arrTracks.clear();

    //Clear all the fish
    string _cmd = buildCommandString(CMD_BODY_OFF, NULL);
    serial.writeBytes(_cmd.c_str(), _cmd.length());
    
    //Load the each command file
    if (_allTracks)
    {
        state = STATE_JUKEBOX;

        for (int i=0; i<arrSongs[songIndex].arrTrackPaths.size(); i++)
        {
            loadTrack(arrSongs[songIndex].arrTrackPaths[i]);
        }
    
    } else {

        //Set state to Playback
        state = STATE_PLAYBACK;

        int _trackIndex = arrSongs[songIndex].trackIndex;
        
        loadTrack(arrSongs[songIndex].arrTrackPaths[_trackIndex]);
        
    }
    
    //Sort the Array of cmds
    ofSort(arrCmds, &sortCmds);

    mainTimeline->setRange(songLength, arrCmds);

    playerSound.stop();
    
    ofLog() << "Start Play";

    playerSound.play();
    playerSound.setPosition(OFFSET_POSITION);

    nextCmdIndex = 0;
    arrPlayedCmds.clear();

}

void ofApp::writeJsonFile() {

    song _s = arrSongs[songIndex];
    
    string _track = _s.arrTrackPaths[_s.trackIndex];

    Json::Value arrCommands(Json::arrayValue);
    
    for (int i=0; i<arrCmds.size(); i++) {

        Json::Value _cmd;
        _cmd["cmd"] = arrCmds[i]->cmd;
        _cmd["timecode"] = arrCmds[i]->timecode;
        _cmd["timecodeL"] = arrCmds[i]->timecodeL;
        _cmd["set"] = arrCmds[i]->sCmd;
        _cmd["group"] = ofToString(arrCmds[i]->group);

        ofLog() << arrCmds[i]->cmd << "\t" << arrCmds[i]->timecodeL;

        arrCommands.append(_cmd);
    }

    ofLog() << arrCommands;
    ofLog() << "* Write " << arrCmds.size();
    
    std::ofstream file_id;
    file_id.open(ofToDataPath(_track));
    
    Json::Value value_obj;
    //populate 'value_obj' with the objects, arrays etc.
    
    Json::StyledWriter styledWriter;
    file_id << styledWriter.write(arrCommands);
    
    file_id.close();}

//--------------------------------------------------------------
void ofApp::draw(){

    //ofSetColor(0);
    if (bShowDisplay.get())
    {
        ofPushMatrix();
    
        ofTranslate(masterX.get(), masterY.get());
    
        ofPushMatrix();
    
        ofScale(masterScale.get());
    
        //Draw backgtround image
        imgStairs.draw(-10, -105, 1.457*1102, 1.457*702);

        for (int i=0; i<arrFish.size(); i++)
        {
            arrFish[i]->draw(bbScale.get(), bbShowDebug.get() );
        }
    
        ofPopMatrix();
        ofPopMatrix();
    }

    float _t = playerSound.getPositionMS() * .001;
    ofPushStyle();

    if (isRecording.get()) {
        ofSetColor(255,0,0);
    } else {
        ofSetColor(0);
    }

    ttf.drawString(ofToString(_t), 20, 250);

    switch (state) {

        case STATE_WAIT:
            ofSetColor(0);
            break;

        case STATE_RECORD:
            ofSetColor(255,0,0);
            break;

        case STATE_PLAYBACK:
        case STATE_JUKEBOX:
            ofSetColor(50,250,50);
            break;
    }

    ttf.drawString(arrStateNames[state], 20, 300);
    ofPopStyle();

    int _r = ofGetWidth() * (.05 + .1 * animMouth.getCurrentValue());

    //ofNoFill();
    //ofSetLineWidth(3);
    //ofDrawCircle(ofGetWidth()*.5, ofGetHeight()*.5, _r);

    int _offset = 40;
    string _s = "";

    ofPushStyle();
    
    ofSetColor(50);

    if (false) //state == STATE_JUKEBOX || state == STATE_PLAYBACK)
    {
        for (int i=arrCmds.size()-1; i>=0; i--) {

            _s = ofToString(arrCmdNames[arrCmds[i]->cmd]) + " " + ofToString(arrCmds[i]->timecodeL);

            ttf_side.drawString(_s, ofGetWidth()*.8, _offset);
            _offset += 25;
        }
    }

    if (bShowDisplay.get()) //state == STATE_JUKEBOX || state == STATE_PLAYBACK)
    {
        int _low = arrPlayedCmds.size() > 50 ? arrPlayedCmds.size() - 50 : 0;

        for (int i=arrPlayedCmds.size()-1; i>=_low; i--) {

            int _l = i>0 ? i-1 : 0;
            int _d = arrPlayedCmds[i]->timecodeL - arrPlayedCmds[_l]->timecodeL;
            
            _s = ofToString(arrCmdNames[arrPlayedCmds[i]->cmd]) + " " +
                ofToString(arrPlayedCmds[i]->cmd) + " " +
                ofToString(arrPlayedCmds[i]->timecodeL) + " d: " +
                ofToString(_d);

            if (_d < maxDiff.get())
                ofSetColor(255,0,0);
            else
                ofSetColor(0);
    
            ttf_side.drawString(_s, ofGetWidth()*.6, _offset);
            _offset += 25;
        }
    }

    ofPopStyle();

    //Draw each fish
    //ofPushMatrix();
    //ofTranslate(bbOriginX.get(), bbOriginY.get());

    ofPushStyle();
    ofSetColor(0,255,0);
    ttf.drawString("Billy Bass Wall v0.1", 20, 40);
    ttf.drawString("\"" + arrSongs[songIndex].songFile + "\"", 20, 90);

    for (int i=0; i<arrSongs[songIndex].arrTrackPaths.size(); i++)
    {

        if (i == arrSongs[songIndex].trackIndex)
            ofSetColor(255,0,0);
        else
            ofSetColor(0,0,0);

        ttf_side.drawString("\"" + arrSongs[songIndex].arrTrackPaths[i] + "\"", 20, 120 + i*20);
    }

    ofPopStyle();
    
    //Draw
    if (bShowGui)
    {
        panel.draw();
        //panelFish->draw();
    }

    //Draw Timeline
    if (showTimeline.get()) {
        mainTimeline->draw(bbTimelineScale.get(), bbTimelineSlide.get(), isDrawBang.get() );
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

    int setNum = 0;

    vector<int> _columns = {2,4,5,6,4,1,3,4,4,4,5,4,4,3,3,2};
    int _columnIndex = 0;

    while (fishIndex < arrFish.size())
    {

        if (currFish >= 0 && isUp && ofGetElapsedTimef() > _nextTimeOff)
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
            string _cmd = "2";
        
            for (int i=0; i<_columns[_columnIndex]; i++)
            {
                
                _cmd += ofToString(arrFish[fishIndex]->controllerIndex) +
                        ofToString(arrFish[fishIndex]->driverIndex);
    
                fishIndex++;
            }

            _columnIndex++;
            _cmd += "\n";
            
            ofLog() << _cmd;

            //Write the Bytes
            serial.writeBytes(_cmd.c_str(), _cmd.length());

            isUp = true;
            currFish = fishIndex;

            _nextTimeOn = ofGetElapsedTimef() + .15;

            _nextTimeOff = ofGetElapsedTimef() + .10;
            //fishIndex++;

        }
    }
}

void ofApp::assignGroup(group * _group, bool _init)
{
    ofLog() << "* Assign Group";

    //Write to the hardware
    int assignIndex = 0;
    int fishIndex = 0;
    float _nextTime = 0;
    
    for (int i=0; i<_group->arrFishID.size(); i++)
    {
        //Set Group ID
        arrFish[_group->arrFishID[i]]->addGroup(_group->groupID);
    }

    char _g = _init ? CMD_INIT_GROUP : CMD_SET_GROUP;

    if (isCreateGroups.get())
    {
        while (fishIndex < _group->arrFishID.size())
        {

            if (ofGetElapsedTimef() > _nextTime)
            {
                int _cmdID = CMD_SET_GROUP;
                if (arrFish[_group->arrFishID[fishIndex]]->initGroup == false) {
                    _cmdID = CMD_INIT_GROUP;
                    arrFish[_group->arrFishID[fishIndex]]->initGroup = true;
                }
                
            string _cmd = ofToString(_cmdID) +
                ofToString(arrFish[_group->arrFishID[fishIndex]]->controllerIndex) +
                ofToString(arrFish[_group->arrFishID[fishIndex]]->driverIndex) +
                ofToString(_group->groupID) +
                "\n";

            ofLog() << _cmd;

            //Write the Bytes
            serial.writeBytes(_cmd.c_str(), _cmd.length());
            
            _nextTime = ofGetElapsedTimef() + .5;
            fishIndex++;
            }
        }
    }
}

void ofApp::displayCommand(int _cmdID, char _groupID)
{
    
    int _mouth = -1;
    int _body = -1;
    
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
            
        case CMD_BODY_RELEASE:
        case CMD_BODY_OFF:
        case CMD_TAIL_RELEASE:
        case CMD_TAIL_OFF:
            _body = STATE_BODY_OFF;
            break;
            
    }
    
    //Set visuals for all fish
    setAllBodyState(_mouth, _body, _cmdID, _groupID);
}

void ofApp::playbackCommand()
{
    
}

void ofApp::writeCommand(int _cmdID, bool _record, char _groupID)
{

    displayCommand(_cmdID, _groupID);

    string _cmd = buildCommandString(_cmdID, _groupID);

    //Write to the hardware
    serial.writeBytes(_cmd.c_str(), _cmd.length());
    ofLog() << _cmd.c_str();

    if (_record && isRecording.get()) { // state == STATE_RECORD)

        long _l = playerSound.getPositionMS();
        float _t = playerSound.getPositionMS() * .001;
        
        ofLog() << _l << "," << _t;

        
        arrCmds.push_back(new bbcmd(_cmdID, _t, _l, _cmd, _groupID));
    }

}

void ofApp::setAllBodyState(int _mouthState, int _bodyState, int _cmdID, char _groupID)
{

    for (int i=0; i<arrFish.size(); i++)
    {
        if (
            _groupID == NULL
            ||
            arrFish[i]->inGroup(_groupID)
        )
        {
        //    (_cmdType == CMD_TYPE_LEAD && arrFish[i]->isLead)
        //    ||
        //    (_cmdType == CMD_TYPE_OTHERS && !arrFish[i]->isLead)
        //)
        //{
            if (arrFish[i]->getBodyState() == STATE_BODY_HEAD &&
                (_cmdID == CMD_TAIL_ON || _cmdID == CMD_TAIL_OFF || _cmdID == CMD_TAIL_RELEASE)
            ) {

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
    int _out = -1;

    switch(_key) {

        case '4':
        case 'r':
        case 'f':
        case 'v':
            _out = -1;
            break;

        case '5':
        case 't':
        case 'g':
        case 'b':
            _out = 0;
            break;

        case '6':
        case 'y':
        case 'h':
        case 'n':
            _out = 1;
            break;
        
        case '7':
        case 'u':
        case 'j':
        case 'm':
            _out = 2;
            break;
            
        case '8':
        case 'i':
        case 'k':
        case ',':
            _out = 3;
            break;
            
        case '9':
        case 'o':
        case 'l':
        case '.':
            _out = 4;
            break;
            
        case '0':
        case 'p':
        case ';':
        case '/':
            _out = 5;
            break;
            
        case '-':
        case '[':
        case '\'':
        case ']':
            _out = 6;
            break;
    }

    if (_out >= 0) {
        _out += KEY_OFFSET;
    }

    return _out;
}

void ofApp::nextSong() {

    //Load First Song
    //playerSound.load(ofToDataPath(arrSongs[songIndex].songFile));
    //songLength = getSongLength();

    
    if (playerSound.isPlaying()) {
    
        playerSound.stop();
        playerSound.setPosition(0);
        playerSound.unload();
        
        state = STATE_WAIT;
        
        writeCommand(CMD_BODY_OFF, false, NULL);

        arrCmds.clear();
    } else {
        
        songIndex++;
        if (songIndex >= arrSongs.size()) {
            
            songIndex = 0;
        }

        loadAndPlaySong(true);
    }
    

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

        case '4':

            if (state != STATE_PLAYBACK || isRecording.get()) {
                
                animMouth.animateTo(1);

                int _cmd = CMD_MOUTH_OPEN;
                
                writeCommand(_cmd, true, NULL);

                /*
                writeCommand(_cmd, true, 'N');
                isTailUp = !isTailUp;
                if (isTailUp) {
                    writeCommand(CMD_TAIL_ON, true, 'N');
                } else {
                    writeCommand(CMD_TAIL_OFF, true, 'N');
                }
                 */
            }
            break;

        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '0':
        case '-':
            
            if (state != STATE_PLAYBACK || isRecording.get()) {

                animMouth.animateTo(1);
                
                int _cmd = CMD_MOUTH_OPEN;
            
                if (
                    (
                     (key >= '1' && key <= '9') || key == '0' || key == '-'
                    )
                    && arrGroups[_groupIndex]->bodyState != CMD_HEAD_ON
                )
                {

                    writeCommand(CMD_HEAD_ON, true, _groupID);
                    if (_groupID == 'L') {
                        
                        arrGroups[4]->bodyState = CMD_BODY_RELEASE;
                        writeCommand(CMD_BODY_RELEASE, true, 'M');
                    } else if (_groupID == 'M') {
                        
                        arrGroups[3]->bodyState = CMD_BODY_RELEASE;
                        writeCommand(CMD_BODY_RELEASE, true, 'L');
                    }

                    arrGroups[_groupIndex]->bodyState = CMD_HEAD_ON;
                    arrGroups[_groupIndex]->lastSpeak = ofGetElapsedTimef();
                }

                writeCommand(_cmd, true, _groupID);
            }
            break;

        case 'f':
        case 'g':
        case 'h':
        case 'j':
        case 'k':
        case 'l':
        case ';':
        case ']':
            
            if (state != STATE_PLAYBACK || isRecording.get()) {

                int _cmd = CMD_HEAD_ON;
                
                if (_groupIndex >= 0) {
                    arrGroups[_groupIndex]->bodyState = CMD_HEAD_ON;
                }

                writeCommand(_cmd, true, _groupID);
            }
            break;

        case 'r':
        case 't':
        case 'y':
        case 'u':
        case 'i':
        case 'o':
        case 'p':
        case '[':
    
            if (state != STATE_PLAYBACK || isRecording.get()) {

                int _cmd = CMD_TAIL_ON;
                writeCommand(_cmd, true, _groupID);
            }
            break;

        case 'v':
        case 'b':
        case 'n':
        case 'm':
        case ',':
        case '.':
        case '/':
        case '\'':
            
            if (state != STATE_PLAYBACK || isRecording.get()) {
                
                if (_groupIndex >= 0) {
                    arrGroups[_groupIndex]->bodyState = CMD_BODY_OFF;
                }
                //writeCommand(CMD_HEAD_ON, true, _groupID);
                writeCommand(CMD_BODY_OFF, true, _groupID);
            }
            break;

        //Stop video
        case 'e':
            
            playerSound.stop();
            playerSound.setPosition(0);
            playerSound.unload();
            
            state = STATE_WAIT;
            arrCmds.clear();
            
            break;
        
        //Write a track
        case 'w':
            ofLog() << "Commands:";
            writeJsonFile();
            break;

        //Press Button
        case 'c':
            
            buttonPress();
            break;

        //Load or pause a track
        case 'q':
            
            ofLog() << "Read layout json file.";
            
            if (state == STATE_PLAYBACK) {
                
                playerSound.setPaused(true);
                state = STATE_PAUSED;

            } else if (state == STATE_PAUSED) {
                
                playerSound.setPaused(false);
                mainTimeline->setRange(songLength, arrCmds);

                resetCmdIndex();
                state = STATE_PLAYBACK;

            } else if (state == STATE_WAIT) {
                
                loadAndPlaySong(false);
            }
            break;
            
        //Toggle flipping
        case 'd':
            arrCmds.clear();
            break;
            
        case 'z':
            testAllTails();
            break;
            
        case 'x':
            
            isFlipping = !isFlipping;
            break;
        
        //Play jukebox
        case 'a':
            
            if (state == STATE_JUKEBOX) {
                
                playerSound.setPaused(true);
                state = STATE_PAUSED;
                
            } else if (state == STATE_PAUSED) {
                
                playerSound.setPaused(false);
                mainTimeline->setRange(songLength, arrCmds);
                
                resetCmdIndex();
                state = STATE_JUKEBOX;
                
            } else if (state == STATE_WAIT) {
                
                mainTimeline->setRange(songLength, arrCmds);
                loadAndPlaySong(true);
            }
            
            break;
            
        case '3':

            arrSongs[songIndex].trackIndex++;
            if (arrSongs[songIndex].trackIndex >= arrSongs[songIndex].arrTrackPaths.size()) {
                arrSongs[songIndex].trackIndex = 0;
            }
            
            //loadAndPlaySong(false);
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
        arrGroups[_groupIndex]->lastSpeak = ofGetElapsedTimef();
    }
    
    if (state != STATE_JUKEBOX)
    {
        switch (key) {

            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '0':
            case '-':
            case '=':
            
                animMouth.animateTo(0);

                writeCommand(CMD_MOUTH_CLOSE, isRecording.get(), _groupID);
                break;
        }
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
