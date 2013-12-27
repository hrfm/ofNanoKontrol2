#pragma once

#include "ofMain.h"

#include "ofxMidi.h"
#include "ofxOsc.h"

class testApp : public ofBaseApp, public ofxMidiListener {
	
public:
	
	void setup();
	void update();
	void draw();
	void exit();
	
	void keyPressed(int key);
	void keyReleased(int key);
	
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased();
	
	void newMidiMessage(ofxMidiMessage& eventArgs);
	
	stringstream text;
	
	ofxMidiIn midiIn;
	ofxMidiIn midiIn2;
	ofxMidiMessage midiMessage;
    
    ofxOscSender _sender;
    
    int _knob[8];
    int _fader[8];
    
    int _x;
    int _y;
    bool _pad[16];
    
};