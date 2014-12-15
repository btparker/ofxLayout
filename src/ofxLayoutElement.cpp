#include "ofxLayoutElement.h"

ofxLayoutElement::ofxLayoutElement(){
    stylesheet = new ofxOSS();
}

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

void ofxLayoutElement::setStylesheet(ofxOSS *_stylesheet){
    stylesheet = _stylesheet;
}

string ofxLayoutElement::getID(){
    return ID;
}

void ofxLayoutElement::setID(string _ID){
    ID = _ID;
}

void ofxLayoutElement::loadFromLayout(ofxXmlSettings *layout, int which){
    string id = layout->getAttribute("element","id", "", which);
    setID(id);
    
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
    }
    layout->popTag();
}

void ofxLayoutElement::updateDimensions(){
    ofxOSS* widthOSS = getOverridingStylesheet(OSS_KEY::WIDTH);
    boundary.width = widthOSS->getDimensionStyleValue(OSS_KEY::WIDTH,parentBoundary.width);
    
    ofxOSS* heightOSS = getOverridingStylesheet(OSS_KEY::HEIGHT);
    boundary.height = heightOSS->getDimensionStyleValue(OSS_KEY::HEIGHT,parentBoundary.height);
}

void ofxLayoutElement::updatePosition(){
    if(hasStyle(OSS_KEY::POSITION)){
        ofxOSS* positionOSS = getOverridingStylesheet(OSS_KEY::POSITION);
        ofPoint pos = positionOSS->getPosition(boundary, parentBoundary);
        boundary.x = pos.x;
        boundary.y = pos.y;
    }
}

void ofxLayoutElement::draw(){
    // Saving the scissor state
    glPushAttrib(GL_SCISSOR_BIT);
    ofPushMatrix();
    ofTranslate(boundary.x, boundary.y, 0);
    
    applyStyles();
    
    for(int i = 0; i < childNodes.size(); i++){
        childNodes[i]->draw();
    }
    
    ofPopMatrix();
    glPopAttrib();
}

void ofxLayoutElement::applyStyles(){
    if(hasStyle(OSS_KEY::BACKGROUND_COLOR)){
        ofxOSS* backgroundColorOSS = getOverridingStylesheet(OSS_KEY::BACKGROUND_COLOR);
        ofSetColor(backgroundColorOSS->getColorStyle(OSS_KEY::BACKGROUND_COLOR));
        ofFill();
        ofRect(0,0,boundary.width,boundary.height);
    }
}

void ofxLayoutElement::setStyle(string styleKey, string styleValue){
    return setStyle(ofxOSS::getEnumFromString(styleKey), styleValue);
}

void ofxLayoutElement::setStyle(OSS_KEY::ENUM styleKey, string styleValue){
    elementStyles.setStyle(styleKey, styleValue);
}

string ofxLayoutElement::getStyle(OSS_KEY::ENUM styleKey){
    ofxOSS* oss = getOverridingStylesheet(styleKey);
    if(oss != NULL){
        return oss->getStyle(styleKey);
    }
    else{
        return "";
    }
    
}

string ofxLayoutElement::getStyle(string key){
    return getStyle(ofxOSS::getEnumFromString(key));
}

ofxOSS* ofxLayoutElement::getOverridingStylesheet(OSS_KEY::ENUM styleKey){
    ofxOSS* idStyles = stylesheet->getStylesByID(getID());
    if(elementStyles.hasStyle(styleKey)){
        return &elementStyles;
    }
    else if(idStyles != NULL && idStyles->hasStyle(styleKey)){
        return idStyles;
    }
    else{
        return NULL;
    }
}

bool ofxLayoutElement::hasStyle(OSS_KEY::ENUM styleKey){
    ofxOSS* idStyles = stylesheet->getStylesByID(getID());
    return elementStyles.hasStyle(styleKey) || (idStyles != NULL && idStyles->hasStyle(styleKey));
}

bool ofxLayoutElement::hasStyle(string styleKey){
    return hasStyle(ofxOSS::getEnumFromString(styleKey));
}

ofxOSS* ofxLayoutElement::getOverridingStylesheet(string styleKey){
    return getOverridingStylesheet(ofxOSS::getEnumFromString(styleKey));
}

void ofxLayoutElement::addChildElement(ofxLayoutElement* childElement){
    childElement->parentNode = this;
    childElement->stylesheet = this->stylesheet;
    childNodes.push_back(childElement);
}