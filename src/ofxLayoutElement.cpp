#include "ofxLayoutElement.h"


void ofxLayoutElement::update(){
    if(!parentNode){
        parentBoundary = ofRectangle(0.0f, 0.0f, ofGetWidth(), ofGetHeight());
    }
    else{
        parentBoundary = ofRectangle(parentNode->boundary);
    }
    
    updateDimensions();
    updatePosition();
    
    for(int i = 0; i < childNodes.size(); i++){
        childNodes[i]->update();
    }
}

void ofxLayoutElement::updateDimensions(){
    boundary.width = styles.getDimensionStyleValue(OSS_KEY::WIDTH,parentBoundary.width);
    boundary.height = styles.getDimensionStyleValue(OSS_KEY::HEIGHT,parentBoundary.height);
}

void ofxLayoutElement::updatePosition(){
    if(styles.hasStyle("position")){
        ofPoint pos = styles.getPosition(boundary, parentBoundary);
        boundary.x = pos.x;
        boundary.y = pos.y;
    }
}

void ofxLayoutElement::draw(){
    glPushAttrib(GL_SCISSOR_BIT);
    //glEnable(GL_SCISSOR_TEST);
    ofPushMatrix();
    ofTranslate(boundary.x, boundary.y);
    //glScissor(boundary.x, boundary.y+(ofGetHeight()-boundary.height), boundary.width, boundary.height);
    applyStyles();
//    ofPopMatrix();
    glDisable(GL_SCISSOR_TEST);
    glPopAttrib();
}

void ofxLayoutElement::applyStyles(){
    if(styles.hasStyle(OSS_KEY::BACKGROUND_COLOR)){
        ofSetColor(styles.getColorStyle(OSS_KEY::BACKGROUND_COLOR));
        ofFill();
        ofRect(0,0,boundary.width,boundary.height);
    }
}

void ofxLayoutElement::setStyle(string styleKey, string styleValue){
    return setStyle(ofxOSS::getEnumFromString(styleKey), styleValue);
}

void ofxLayoutElement::setStyle(OSS_KEY::ENUM styleKey, string styleValue){
    styles.setStyle(styleKey, styleValue);
}

string ofxLayoutElement::getStyle(OSS_KEY::ENUM styleKey){
    if(!styles.hasStyle(styleKey)){
        return "";
    }
    return styles.getStyle(styleKey);
}

string ofxLayoutElement::getStyle(string key){
    return getStyle(ofxOSS::getEnumFromString(key));
}