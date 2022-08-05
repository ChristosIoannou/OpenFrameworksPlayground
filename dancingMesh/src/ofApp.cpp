#include "ofApp.h"

// DANCING MESH

//--------------------------------------------------------------
void ofApp::setup(){
    //Set up sound sample
    sound.load( "112 Balam - Yucatan.mp3" );
    sound.setLoop( true );

    ofBackground(ofColor::black);	//Set up the background
    ofEnableAlphaBlending();

    spectrum.resize(N, 0.0f);
    p.resize(n);

    for (int i = 0; i < n; ++i){
        tx.push_back(ofRandom(0, 1000));
        ty.push_back(ofRandom(0, 1000));
        tz.push_back(ofRandom(0, 1000));
    }

    mesh.setMode(OF_PRIMITIVE_LINES);
    mesh.enableColors();

    createMesh();

    // Set up initial position for cam
    //cam.setAutoDistance(false);
    //cam.setPosition(0.0, 0.0, 1500.0);

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
        spectrum[i] *= 0.98;	//Slow decreasing
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
    Rad = ofMap( spectrum[ bandRad ], 1, 3, 500, 1200, true );
    Vel = ofMap( spectrum[ bandVel ], 0, 0.1, 0.05, 0.5 );

    //Update particles positions

    for (int j=0; j<n; j++) {
        tx[j] += Vel * dt;	//move offset
        ty[j] += Vel * dt;	//move offset
        tz[j] += Vel * dt;
        //Calculate Perlin's noise in [-1, 1] and
        //multiply on Rad
        ofVec3f newPosition = mesh.getVertex(j);
        newPosition.x = ofSignedNoise( tx[j] ) * Rad;
        newPosition.y = ofSignedNoise( ty[j] ) * Rad;
        newPosition.z = ofSignedNoise( tz[j] ) * Rad;
        mesh.setVertex(j, newPosition);
    }

    // Let's add some lines!
    mesh.clearIndices();
    float connectionDistance = 80;
    int numVerts = mesh.getNumVertices();
    for (int a=0; a<numVerts; ++a) {
        ofVec3f verta = mesh.getVertex(a);
        for (int b=a+1; b<numVerts; ++b) {
            ofVec3f vertb = mesh.getVertex(b);
            float distance = verta.distance(vertb);
            if (distance <= connectionDistance) {
                // In OF_PRIMITIVE_LINES, every pair of vertices or indices will be
                // connected to form a line
                mesh.addIndex(a);
                mesh.addIndex(b);
            }
        }
    }

    if (earthOrbit) {
        earthNode.setPosition(mesh.getVertex(0));

        cam.lookAt(earthNode);
        finalDistanceToTarget = 5; // + sin(time);
        ofVec3f camPosition = cam.getPosition();
        ofVec3f earthPosition = earthNode.getPosition();
        ofVec3f camToEarth = camPosition.interpolate(earthPosition, 0.5*(cos(time/100) + 1));

        if (camPosition.distance(earthPosition) > finalDistanceToTarget && !earthLocked){
            //float x_time =
            cam.setPosition(camToEarth);
            if (camPosition.distance(earthPosition) == finalDistanceToTarget){
                earthLocked = true;
            }
        }

        if (earthLocked) {
            angleH += 1.f;
            if (angleH > 360.f)
                angleH = 0.0f;

            angleV += 0.25f;
            if (angleV > 360.f)
                angleV = 0.0f;

            //if (bRoll) roll += 0.5f;

            // convert angleV to range [-90,90] for latitude
            float vFac = sin(angleV * M_PI / 180.f) * 90.f;
            cam.orbit(angleH, vFac, finalDistanceToTarget, earthNode);
            cam.lookAt(earthNode);
        }
    }


    //Calculate color based on range
    analyseFFT();

}

//--------------------------------------------------------------
void ofApp::draw(){

    if (showSpectrum){
        drawSpectrum();
    }

    //Move center of coordinate system to the screen center
    ofPushMatrix();
    ofTranslate( ofGetWidth() / 2, ofGetHeight() / 2 );

    cam.begin();

    //Draw points
    drawPoints();
    mesh.draw();

    cam.end();
    //Restore coordinate system
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
    case 'g':
    case 'G':
        showSpectrum = !showSpectrum;
        break;
    case 'p':
    case 'P':
        sound.play();
        break;
    case 'e':
    case 'E':
        earthOrbit = !earthOrbit;
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


//--------------------------------------------------------------
void ofApp::analyseFFT(){

    //Calculate color based on range
    std::vector<float>::iterator rg_it = std::next(spectrum.begin(), 10);
    std::vector<float>::iterator gb_it = std::next(spectrum.begin(), 100);

    bass = std::accumulate(spectrum.begin(), rg_it, 0.0f);
    mids = std::accumulate(rg_it, gb_it, 0.0f);
    highs = std::accumulate(gb_it, spectrum.end(), 0.0f);
    totals = std::accumulate(spectrum.begin(), spectrum.end(), 0.0f);


    red = static_cast<int>(std::min(ofMap(bass, 0, 6, 0, 255), 255.0f));
    green = static_cast<int>(std::min(ofMap(mids, 0, 6, 0, 255), 255.0f));
    blue = static_cast<int>(std::min(ofMap(highs, 0, 6, 0, 255), 255.0f));
    brightness = std::min(ofMap(totals, 0, 10, 100, 255), 255.0f);

}

void ofApp::drawPoints(){
    ofEnableAlphaBlending();
    ofColor drawColor;
    ofNoFill();
    brightness = 255;
    for (int i = 0; i < n; ++i) {
        if (i == earthId)
            continue;
        drawPoint(i);
    }
    ofFill();
    drawPoint(earthId);
}

void ofApp::drawPoint(int i){
    float hue = ofMap(static_cast<float>(i), 0.0, static_cast<float>(n), 170.0f, 255.0f);
    ofColor drawColor;
    drawColor.setHsb(hue, 255, brightness, 120);
    ofSetColor( drawColor ); // higher alpha is more opaque
    ofDrawSphere( mesh.getVertex(i), 2 );
}

void ofApp::createMesh(){
    for (int i = 0; i < n; ++i){
        // assign a colour between blue and red
        float hue = ofMap(static_cast<float>(i), 0.0, static_cast<float>(n), 0.66f, 1.0f);
        ofFloatColor drawColor;
        drawColor.setHsb(hue, 1.0, 1.0, 0.47);
        // create point and push to mesh
        ofVec3f temp(ofRandom(-500.0, 500.0), ofRandom(-500.0, 500.0), ofRandom(-500.0, 500.0));
        points.push_back(temp);
        mesh.addVertex(temp);
        mesh.addColor(drawColor);
    }
}

void ofApp::moveCamToPosition(ofNode node){

}

void ofApp::drawSpectrum(){
    for (int i=0; i<N; i++) {
        //Draw bandRad and bandVel by black color,
        //and other by gray color
        if ( i == bandRad || i == bandVel ) {
            ofSetColor( ofColor::orange ); //Black color
        }
        else {
            ofSetColor( ofColor::lightGoldenRodYellow ); //Gray color
        }
        ofDrawRectangle( 10 + i * 5, 700, 3, -spectrum[i] * 100 );
    }
}
