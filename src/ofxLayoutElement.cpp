#include "ofxLayoutElement.h"

/// |   Constructor/Destructor   | ///
/// | -------------------------- | ///
ofxLayoutElement::ofxLayoutElement(){
    boundary = ofRectangle(0,0,ofGetWidth(),ofGetHeight());
    elementFbo = new ofFbo();
    elementFbo->allocate();
    
    styles = new ofxOSS();
}

ofxLayoutElement::~ofxLayoutElement(){
    delete elementFbo;
}

/// |   Cycle Functions  | ///
/// | ------------------ | ///

void ofxLayoutElement::update(){
    for(int i = 0 ; i < children.size(); i++){
        children[i]->update();
    }
}

void ofxLayoutElement::draw(){
    // Saving the scissor state
    glPushAttrib(GL_SCISSOR_BIT);
    ofPushMatrix();
    ofTranslate(boundary.x, boundary.y, 0);
    elementFbo->begin();
    ofClear(ofColor(0,0,0,0));
    drawStyles();
    elementFbo->end();
    elementFbo->draw(0,0,boundary.width,boundary.height);
    
    ofPopMatrix();
    glPopAttrib();
    for(int i = 0 ; i < children.size(); i++){
        children[i]->draw();
    }
}

/// |   Setters/Getters   | ///
/// | ------------------- | ///

string ofxLayoutElement::getTag(){
    return this->tag;
}

void ofxLayoutElement::setTag(string tag){
    this->tag = tag;
}

string ofxLayoutElement::getClasses(){
    return this->classes;
}

void ofxLayoutElement::setClasses(string classes){
    this->classes = classes;
}

string ofxLayoutElement::getID(){
    return this->ID;
}

void ofxLayoutElement::setID(string ID){
    this->ID = ID;
}

bool ofxLayoutElement::hasStyle(OSS_KEY::ENUM styleKey){
    return this->styles->rules.count(styleKey) > 0;
}

string ofxLayoutElement::getStyle(OSS_KEY::ENUM styleKey){
    return this->styles->rules[styleKey]->value;
}

/// |   Utilities   | ///
/// | ------------- | ///

void ofxLayoutElement::drawStyles(){
    if(hasStyle(OSS_KEY::BACKGROUND_COLOR)){
        ofSetColor(ofxOSS::parseColor(getStyle(OSS_KEY::BACKGROUND_COLOR)));
        ofFill();
        ofRect(0,0,boundary.width,boundary.height);
    }

    
}

void ofxLayoutElement::overrideStyles(ofxOSS *styleObject){
    for(auto iterator = styleObject->rules.begin(); iterator != styleObject->rules.end(); iterator++){
        this->styles->rules[iterator->first] = iterator->second;
    }
}

string ofxLayoutElement::getInlineStyle(){
    return this->inlineStyle;
}

void ofxLayoutElement::setInlineStyle(string style){
    this->inlineStyle = style;
}