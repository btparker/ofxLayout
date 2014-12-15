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

void ofxLayoutElement::loadFromFile(string filename){
    ofxXmlSettings layout;
    layout.loadFile(filename);
    loadFromLayout(&layout);
}

void ofxLayoutElement::loadFromLayout(ofxXmlSettings *layout, int which){
    string id = layout->getAttribute("element","id", "", which);
    ID = id;
    string style = layout->getAttribute("element","style", "", which);
    vector<string> styles;
    if(style != ""){
        styles = ofSplitString(style, ";", true, false);
    }
    for(int i = 0; i < styles.size(); i++){
        vector<string> stylePair = ofSplitString(styles[i], ":", true, true);
        this->setStyle(stylePair[0], stylePair[1]);
    }
    
    layout->pushTag("element",which);
    int numChildren = layout->getNumTags("element");
    for(int i = 0; i < numChildren; i++){
        ofxLayoutElement* child = new ofxLayoutElement();
        addChildElement(child);
        child->loadFromLayout(layout,i);
        cout << child->ID << endl;
        cout << child->parentNode->ID << endl;
        
    }
    layout->popTag();
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
    
    for(int i = 0; i < childNodes.size(); i++){
        childNodes[i]->draw();
    }
    
    ofPopMatrix();
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

void ofxLayoutElement::addChildElement(ofxLayoutElement* childElement){
    childElement->parentNode = this;
    childNodes.push_back(childElement);
}