#include "ofxLayoutElement.h"


void ofxLayoutElement::update(){
    if(!parentNode){
        boundary = ofRectangle(0.0f, 0.0f, ofGetWidth(), ofGetHeight());
    }
    else{
        boundary = ofRectangle(parentNode->boundary);
    }
    
    // DIMENSIONS
    boundary.width = styles.getDimensionStyleValue(OSS_KEY::WIDTH,boundary.width);
    boundary.height = styles.getDimensionStyleValue(OSS_KEY::HEIGHT,boundary.height);
  
    for(int i = 0; i < childNodes.size(); i++){
        childNodes[i]->update();
    }
}

void ofxLayoutElement::draw(){
    glPushAttrib(GL_SCISSOR_BIT);
    glEnable(GL_SCISSOR_TEST);
    glScissor(boundary.x, boundary.y+(ofGetHeight()-boundary.height), boundary.width, boundary.height);
    
    applyStyles();
    
    glDisable(GL_SCISSOR_TEST);
    glPopAttrib();
}

void ofxLayoutElement::applyStyles(){
    if(styles.hasStyle(OSS_KEY::BACKGROUND_COLOR)){
        ofSetColor(styles.getColorStyle(OSS_KEY::BACKGROUND_COLOR));
        ofFill();
        ofRect(boundary);
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