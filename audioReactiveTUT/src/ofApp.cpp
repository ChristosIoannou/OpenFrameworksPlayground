#include "ofApp.h"
#include <algorithm>

//--------------------------------------------------------------
void ofApp::setup(){
    // load vocal samples
    // loadVocal();
    loadPiano();

    tempo = 5000; // starting with five seconds tempo
    b_autoPlay = false;
    currTime = ofGetSystemTimeMillis();

    ofSetBackgroundColor(0);
    ofSetBackgroundAuto(false);
    b_Gui = true;
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

    ofSetColor(0, 0, 0, 10);
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetWidth());

    ofSetColor(255);
    if (b_autoPlay){
        if (ofGetSystemTimeMillis() > currTime + tempo){
            int num = ofRandom(totalVoices);
            voices[num].play();
            currTime = ofGetSystemTimeMillis();
            std::cout << "playing voice " << num << std::endl;
        }
    }

    // toggle showing Gui
    if(b_Gui){
        // onscreen display of voices that are playing
        for (int i = 0; i < totalVoices; i++){
            if (voices[i].isPlaying()){
                // draw a while progress bar
                ofDrawRectangle(90, ((i)*20) + 7 , ofMap(voices[i].getPosition(),
                                                         0, 1, 20, (ofGetWidth() - 110)), 18);
                ofDrawBitmapStringHighlight("Voice " + ofToString(i+1) + ": ", 10, (i+1)*20);
            } else {
                ofDrawBitmapString("Voice " + ofToString(i+1) + ": ", 10, (i+1)*20);
            }
        }

        // onscreen instructions
        ofDrawBitmapString("auto play is " + ofToString(b_autoPlay), 10, 200);
        ofDrawBitmapString("tempo is " + ofToString(tempo) + " ms", 10, 220);
        ofDrawBitmapString("press keys 1-7 to play voices\n"
        "'a' to autoplay \n"
        "+/- to increase/decrease tempo \n"
        "'p' to load Piano Samples \n"
        "'v' to load Vocal samples", 10, 240);
    }

    // Audio visualisation here
    ofColor drawColor;
    int length = 800;
    float bandsToGet = 512;
    float width = (float)(5*120) / bandsToGet;
    float * val = ofSoundGetSpectrum(bandsToGet);
    for (int i = 0; i < bandsToGet; i++){
        ofPushMatrix();
        ofTranslate(ofGetWidth()/2 , ofGetHeight()/2);
        ofRotateDeg(360.0 / bandsToGet * i);
        drawColor.setHsb(255/bandsToGet * i, 255, 255, 50);
        ofSetColor(drawColor);
        int barWidth = std::fmax(3, 40 - val[i]*100);
        ofDrawRectangle(100, -barWidth/2, val[i]*length, barWidth/2);
        ofPopMatrix();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case '1':
            voices[0].play();
            break;
        case '2':
            voices[1].play();
            break;
        case '3':
            voices[2].play();
            break;
        case '4':
            voices[3].play();
            break;
        case '5':
            voices[4].play();
            break;
        case '6':
            voices[5].play();
            break;
        case '7':
            voices[6].play();
            break;
        case '8':
            if(voices.size()>7){
                voices[7].play();
            }
            break;
        case 'a':
            b_autoPlay = !b_autoPlay;
            break;
        case 'v':
            loadVocal();
            break;

        case '=':
        case '+':
            tempo += 500;
            break;
        case '-':
        case '_':
            if (tempo > 1000){
                tempo -= 500;
            }
            break;
        case 'g':
            b_Gui = !b_Gui;
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

void ofApp::loadVocal(){
    // load piano samples
    totalVoices = 7;
    voices.clear();
    for (int i = 0; i < totalVoices; i++){
        ofSoundPlayer voice;
        string path = "Choir-Cut/Eno-Choir-0" + ofToString( i+1 ) + ".wav";
        std::cout << "loading " << path << std::endl;
        voice.load(path);
        voice.setMultiPlay(true);
        voices.push_back(voice);
    }
}

void ofApp::loadPiano(){
    // load piano samples
    totalVoices = 8;
    voices.clear();
    for (int i = 0; i < totalVoices; i++){
        ofSoundPlayer voice;
        string path = "Piano-Cut/Eno-Piano-0" + ofToString( i+1 ) + ".wav";
        cout << "loading " << path << endl;
        voice.load(path);
        voice.setMultiPlay(true);
        voices.push_back(voice);
    }
}
