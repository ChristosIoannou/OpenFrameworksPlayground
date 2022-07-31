#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    //Set up sound sample
    sound.loadSound( "112 Balam - Yucatan.mp3" );
    sound.setLoop( true );
    sound.play();
    spectrum.resize(N, 0.0f);
    p.resize(n);

    for (int i = 0; i < n; ++i){
        tx.push_back(ofRandom(0, 1000));
        ty.push_back(ofRandom(0, 1000));
    }

    show_spectrum = false;

    //Set spectrum values to 0
    //for (size_t i=0; i<spectrum.size(); i++) {
    //    spectrum[i] = 0.0f;
    //}

    //Initialize points offsets by random numbers
    //for ( int j=0; j<n; j++ ) {
    //    tx[j] = ofRandom( 0, 1000 );
    //    ty[j] = ofRandom( 0, 1000 );
    //}

}

//--------------------------------------------------------------
void ofApp::update(){
    ofSoundUpdate();
    //Get current spectrum with N bands
    float *val = ofSoundGetSpectrum( N );

    //We should not release memory of val,
    //because it is managed by sound engine
    //Update our smoothed spectrum,
    //by slowly decreasing its values and getting maximum with val
    //So we will have slowly falling peaks in spectrum
    for ( int i=0; i<N; i++ ) {
        spectrum[i] *= 0.97;	//Slow decreasing
        spectrum[i] = max( spectrum[i], val[i] );
    }

    //Update particles using spectrum values
    //Computing dt as a time between the last
    //and the current calling of update()
    float time = ofGetElapsedTimef();
    float dt = time - time0;
    dt = ofClamp( dt, 0.0, 0.1 );
    time0 = time; //Store the current time

    //Update Rad and Vel from spectrum
    //Note, the parameters in ofMap's were tuned for best result
    //just for current music track
    Rad = ofMap( spectrum[ bandRad ], 1, 3, 400, 1000, true );
    Vel = ofMap( spectrum[ bandVel ], 0, 0.1, 0.05, 0.5 );
    //Update particles positions
    for (int j=0; j<n; j++) {
        tx[j] += Vel * dt;	//move offset
        ty[j] += Vel * dt;	//move offset
        //Calculate Perlin's noise in [-1, 1] and
        //multiply on Rad
        p[j].x = ofSignedNoise( tx[j] ) * Rad;
        p[j].y = ofSignedNoise( ty[j] ) * Rad;
    }

    //Calculate color based on range
    std::vector<float>::iterator rg_it = std::next(spectrum.begin(), 10);
    std::vector<float>::iterator gb_it = std::next(spectrum.begin(), 100);

    bass = std::accumulate(spectrum.begin(), rg_it, 0.0f);
    mids = std::accumulate(rg_it, gb_it, 0.0f);
    highs = std::accumulate(gb_it, spectrum.end(), 0.0f);
    totals = bass + mids + highs;
    red = static_cast<int>(std::min(ofMap(bass, 0, 6, 0, 255), 255.0f));
    green = static_cast<int>(std::min(ofMap(mids, 0, 6, 0, 255), 255.0f));
    blue = static_cast<int>(std::min(ofMap(highs, 0, 6, 0, 255), 255.0f));

    std::cout << red << ", " << green << ", " << blue << std::endl;

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground( ofColor::black);	//Set up the background
    ofEnableAlphaBlending();


    if (show_spectrum){
        //Draw background rect for spectrum
        ofSetColor( ofColor::black );
        ofFill();
        ofRect( 10, 700, N * 6, -100 );
        //Draw spectrum
        ofSetColor( ofColor::ghostWhite );
        for (int i=0; i<N; i++) {
            //Draw bandRad and bandVel by black color,
            //and other by gray color
            if ( i == bandRad || i == bandVel ) {
                ofSetColor( ofColor::orange ); //Black color
            }
            else {
                ofSetColor( ofColor::lightGoldenRodYellow ); //Gray color
            }
            ofRect( 10 + i * 5, 700, 3, -spectrum[i] * 100 );
        }
    }

    //Draw cloud

    //Move center of coordinate system to the screen center
    ofPushMatrix();
    ofTranslate( ofGetWidth() / 2, ofGetHeight() / 2 );

    //Draw points
    ofEnableAlphaBlending();
    ofSetColor( ofColor(red, green, blue, 120 )); // higher alpha is more opaque
    ofFill();
    for (int i = 0; i < n; ++i) {
        ofCircle( p[i], 2 );
    }


    //Draw lines between near points
    float dist = 40;	//Threshold parameter of distance
    for (int j=0; j<n; ++j) {
        for (int k=j+1; k<n; ++k) {
            if ( ofDist( p[j].x, p[j].y, p[k].x, p[k].y )
                 < dist ) {
                ofLine( p[j], p[k] );
            }
        }
    }


    //Restore coordinate system
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case 'g':
            show_spectrum = !show_spectrum;
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

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
