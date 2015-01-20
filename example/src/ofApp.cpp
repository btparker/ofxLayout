#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    fbo.allocate(50,500);
    loadExternalLayout();
}

//--------------------------------------------------------------
void ofApp::update(){
    fbo.begin();
    layout.update();
    fbo.end();
    layout.computeFbo(&activeFbo, &filters);
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    fbo.begin();
    layout.draw();
    fbo.end();
    ofPushMatrix();
    ofScale(ofGetWidth()/50.0f, ofGetHeight()/500.0f);
    fbo.draw(0,0);
    ofPopMatrix();
}

void ofApp::loadExternalLayout(){
    layout.loadDataFromFile("data.json");
    layout.loadOfmlFromFile("index.ofml");
    layout.loadOssFromFile("styles.oss");
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
void ofApp::windowResized(int w, int h){
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
