#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofEnableAlphaBlending();
    container.loadFromFile("index.ofml");
//    ofxLayoutElement* parent = &container;
//    for(int i = 0; i < 5; i++){
//        ofxLayoutElement* child = new ofxLayoutElement();
//        string channel = ofToString(int(((i+1)/6.0)*255.0));
//        child->setStyle(OSS_KEY::BACKGROUND_COLOR, "rgb(0, "+channel+", 0)");
//        child->setStyle(OSS_KEY::WIDTH, "75%");
//        child->setStyle(OSS_KEY::HEIGHT, "50%");
//        child->setStyle(OSS_KEY::POSITION, "center 0px");
//        parent->addChildElement(child);
//        parent = child;
//    }
}

//--------------------------------------------------------------
void ofApp::update(){
    container.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    container.draw();
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
