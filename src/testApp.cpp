#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
    
    ofSetFrameRate(30);
    ofSetVerticalSync(true);
    ofSetBackgroundAuto(true);
    ofBackground(28, 28, 28);
	ofSetLogLevel(OF_LOG_VERBOSE);
	
    // --- OSC
    
    _sender.setup("127.0.0.1", 10000);
    
    // --- MIDI
    
	midiIn.listPorts(); // via instance
	midiIn.openPort("nanoKONTROL2 SLIDER/KNOB");
	midiIn.ignoreTypes(false, false, false);
	midiIn.addListener(this);
	midiIn.setVerbose(true);
    
	midiIn2.openPort("nanoPAD2 PAD");
	midiIn2.ignoreTypes(false, false, false);
	midiIn2.addListener(this);
	midiIn2.setVerbose(true);
    
    // ---
    
    for( int i = 0; i < 8; i++ ){
        _knob[i]  = 0;
        _fader[i] = 0;
    }
    
    for( int i = 0; i < 16; i++ ){
        _pad[i]  = FALSE;
    }
    
}

//--------------------------------------------------------------
void testApp::update() {
}

//--------------------------------------------------------------
void testApp::draw() {
    
    int h = ofGetHeight();
    float faderSize = 98;
    float thumbSize = 8;
    
    ofSetColor(0);
    ofLine( 0, h / 2, ofGetWidth(), h / 2 );
    ofSetColor(32);
    ofLine( 0, h / 2 + 1, ofGetWidth(), h / 2 + 1 );
    
    for( int i = 0; i < 8; i++ ){
        
        float size = ( faderSize - thumbSize ) * _fader[i] / 127.0f;
        float x = 24+29*i;
        
        ofSetColor(20);
        ofRect(   x, h - faderSize - 16, 1, faderSize );
        /*
         ofRect( x-2, h - 18, 5, 5 );
         ofRect( x-2, h - faderSize - 18, 5, 5 );
         ofRect( x-2, h - faderSize/2 - 18, 5, 5 );
         //*/
        ofSetColor(180);
        ofRect( x-5, h - 16 - thumbSize - size, 11, thumbSize );
        
        size = ( faderSize - thumbSize ) * _knob[i] / 127.0f;
        
        ofSetColor(20);
        ofRect(   x, h - faderSize - 16 - 128, 1, faderSize );
        /*
         ofRect( x-2, h - 18 - 128, 5, 5 );
         ofRect( x-2, h - faderSize - 18 - 128, 5, 5 );
         ofRect( x-2, h - faderSize/2 - 18 - 128, 5, 5 );
         //*/
        ofSetColor(180);
        ofRect( x-5, h - 16 - thumbSize - size - 128, 11, thumbSize );
        
    }
    
    int i, j;
    
    for( i = 0; i < 8; i++ ){
        if( _pad[i] == TRUE ){
            ofSetColor(255);
        }else{
            ofSetColor(64);
        }
        ofRect( 256 + 13 + 60 * floor(i/2), 73 - 60 * floor(i%2), 50, 50 );
    }
    
    for( j = 8; j < 16; j++ ){
        if( _pad[j] == TRUE ){
            ofSetColor(255);
        }else{
            ofSetColor(64);
        }
        i = j-8;
        ofRect( 256 + 13 + 60 * floor(i/2), 193 - 60 * floor(i%2), 50, 50 );
    }
    
    return;
    
    //*
	// draw the last recieved message contents to the screen
	text << "Received: " << ofxMidiMessage::getStatusString(midiMessage.status);
	ofDrawBitmapString(text.str(), 20, 20);
	text.str("");
	
	text << "channel: " << midiMessage.channel;
	ofDrawBitmapString(text.str(), 20, 34);
	text.str("");
	
	text << "pitch: " << midiMessage.pitch;
	ofDrawBitmapString(text.str(), 20, 48);
	text.str("");
	ofRect(20, 58, ofMap(midiMessage.pitch, 0, 127, 0, ofGetWidth()-40), 20);
	
	text << "velocity: " << midiMessage.velocity;
	ofDrawBitmapString(text.str(), 20, 96);
	text.str("");
	ofRect(20, 105, ofMap(midiMessage.velocity, 0, 127, 0, ofGetWidth()-40), 20);
	
	text << "control: " << midiMessage.control;
	ofDrawBitmapString(text.str(), 20, 144);
	text.str("");
	ofRect(20, 154, ofMap(midiMessage.control, 0, 127, 0, ofGetWidth()-40), 20);
	
	text << "value: " << midiMessage.value;
	ofDrawBitmapString(text.str(), 20, 192);
	text.str("");
    
    
	if(midiMessage.status == MIDI_PITCH_BEND) {
		ofRect(20, 202, ofMap(midiMessage.value, 0, MIDI_MAX_BEND, 0, ofGetWidth()-40), 20);
	}else {
		ofRect(20, 202, ofMap(midiMessage.value, 0, 127, 0, ofGetWidth()-40), 20);
	}
	
	text << "delta: " << midiMessage.deltatime;
	ofDrawBitmapString(text.str(), 20, 240);
	text.str(""); // clear
    //*/
    
}

//--------------------------------------------------------------
void testApp::exit() {
	midiIn.closePort();
	midiIn.removeListener(this);
}

//--------------------------------------------------------------
void testApp::newMidiMessage(ofxMidiMessage& msg) {
    
	midiMessage = msg;
    
    ofxOscMessage m;
    int control = midiMessage.control;
    int pitch   = midiMessage.pitch;
    int value   = midiMessage.value;
    
    if( midiMessage.portName == "nanoKONTROL2 SLIDER/KNOB" ){
        
        if( control < 8 ){
            // --- fader.
            _fader[control] = value;
        }
        
        if( 16 <= control && control < 24 ){
            // --- knob.
            _knob[control-16] = value;
        }
        
        m.setAddress("/nanokontrol");
        for( int i = 0; i < 8; i++ ){
            m.addIntArg( _fader[i] );
        }
        for( int i = 0; i < 8; i++ ){
            m.addIntArg( _knob[i] );
        }
        _sender.sendMessage(m);
        
        return;
        
    }else{
        
        if( control == 1 ){
            
            _x = value;
            
            m.setAddress("/nanopad/xy");
            m.addIntArg( _x );
            m.addIntArg( _y );
            _sender.sendMessage(m);
            
            return;
            
        }else if( control == 2 ){
            
            _y = value;
            
            m.setAddress("/nanopad/xy");
            m.addIntArg( _x );
            m.addIntArg( _y );
            _sender.sendMessage(m);
            
            return;
            
        }
        
        string status = ofxMidiMessage::getStatusString(midiMessage.status);
        int pitch = midiMessage.pitch;
        
        if( status == "Note On" ){
            
            cout << "on" << endl;
            
            m.setAddress("/nanopad/on");
            m.addIntArg( pitch );
            _sender.sendMessage(m);
            
            _pad[pitch-36] = TRUE;
            
            return;
            
        }else if( status == "Note Off" ){
            
            m.setAddress("/nanopad/off");
            m.addIntArg( pitch );
            _sender.sendMessage(m);
            
            _pad[pitch-36] = FALSE;
            
            return;
            
        }

    }
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key) {
	switch(key) {
		case 'l':
			midiIn.listPorts();
			break;
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key) {
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y) {
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {
}

//--------------------------------------------------------------
void testApp::mouseReleased() {
}
