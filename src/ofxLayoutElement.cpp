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
    if(styles.hasStyle(OSS_KEY::POSITION)){
        ofPoint pos = styles.getPosition(boundary, parentBoundary);
        boundary.x = pos.x;
        boundary.y = pos.y;
    }
}

void ofxLayoutElement::draw(){
    // Saving the scissor state
    glPushAttrib(GL_SCISSOR_BIT);
    
    ofPushMatrix();
    ofTranslate(boundary.x, boundary.y);
    applyStyles();
    glDisable(GL_SCISSOR_TEST);
    glPopAttrib();
    
    for(int i = 0; i < childNodes.size(); i++){
        childNodes[i]->draw();
    }
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

void ofxLayoutElement::addChildElement(ofxLayoutElement* childElement){
    childElement->parentNode = this;
    childNodes.push_back(childElement);
}