#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    void analyseFFT();
    void drawPoint(int i);
    void drawPoints();
    void drawSpectrum();
    void createMesh();
    void moveCamToPosition(ofNode node);

    ofSoundPlayer sound;	//Sound samplevoidkeyPressed(int key);
    bool play;
    const int N = 256;		//Number of bands in spectrum
    std::vector<float> spectrum;      //Smoothed spectrum values
    float Rad = 600;		//Cloud raduis parameter
    float Vel = 0.1;		//Cloud points velocity parameter
    int bandRad = 1;		//Band index in spectrum, affecting Rad value
    int bandVel = 100;		//Band index in spectrum, affecting Vel value
    const int n = 500;		//Number of cloud points
    std::vector<float> tx, ty, tz;		//Offsets for Perlin noise calculation for points
    std::vector<ofPoint> p;			//Cloud's points positions
    float time0 = 0;		//Time value, used for dt computing
    bool showSpectrum = false;
    float bass, mids, highs, totals;
    int red, green, blue;
    float brightness;
    float distanceToTarget;

    bool earthOrbit = false;
    bool earthLocked = false;
    int earthId = 0;
    ofNode earthNode;
    float angle;
    float angleH, angleV, roll, finalDistanceToTarget;

    ofEasyCam cam;
    ofMesh mesh;
    ofLight light;
    std::vector<ofVec3f> points;
};
