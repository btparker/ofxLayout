#include "ofxLayoutElement.h"

/// |   Constructor/Destructor   | ///
/// | -------------------------- | ///
ofxLayoutElement::ofxLayoutElement(){
    parent = NULL;
    
    boundary = ofRectangle();
    elementFbo = new ofFbo();
    elementFbo->allocate();
    
    styles = new ofxOSS();
}

ofxLayoutElement::ofxLayoutElement(ofxLoaderSpool* assetsPtr){
    this->assetsPtr = assetsPtr;
    
    parent = NULL;
    
    boundary = ofRectangle();
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
    if(parent == NULL){
        boundary.x = 0;
        boundary.y = 0;
        boundary.width = ofGetWidth();
        boundary.height = ofGetHeight();
    }
    else{
        boundary = ofRectangle(parent->boundary);
        boundary.setPosition(styles->getPosition(boundary, parent->boundary));
    }
    
    if(elementFbo->getWidth() != boundary.width || elementFbo->getHeight() != boundary.height){
        elementFbo->allocate(boundary.width, boundary.height);
    }

    for(int i = 0 ; i < children.size(); i++){
        children[i]->update();
    }
}

void ofxLayoutElement::addChild(ofxLayoutElement* childElement){
    childElement->parent = this;
    children.push_back(childElement);
}

void ofxLayoutElement::draw(){
    // Saving the scissor state
    glPushAttrib(GL_SCISSOR_BIT);
    ofPushMatrix();
    ofScale(1.0,1.0);
    ofTranslate(boundary.x, boundary.y, 0);
    elementFbo->begin();
    ofClear(ofColor(0,0,0,0));
    drawStyles();
    elementFbo->end();
    elementFbo->draw(0,0);
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
    if(hasStyle(OSS_KEY::BACKGROUND_IMAGE)){
        ofSetColor(255);
        ofxLoaderBatch* imagesBatch = assetsPtr->getBatch("images");
        string imageID = getStyle(OSS_KEY::BACKGROUND_IMAGE);
        if(imagesBatch->hasTexture(imageID) && imagesBatch->isTextureDrawable(imageID)){
            ofRectangle imageTransform = ofRectangle();
            imageTransform.setWidth(imagesBatch->getTexture(imageID)->getWidth());
            imageTransform.setHeight(imagesBatch->getTexture(imageID)->getHeight());
            
            imageTransform = styles->computeBackgroundTransform(imageTransform, boundary);
            
            imagesBatch->getTexture(imageID)->draw(imageTransform);
        }
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

ofxOSS* ofxLayoutElement::getInlineStyles(){
    ofxOSS* inlineStyles = new ofxOSS();
    vector<string> stylesVec = ofSplitString(this->inlineStyle, ";", true, true);
    for(int i = 0; i < stylesVec.size(); i++){
        vector<string> styleKeyValueVec = ofSplitString(stylesVec[i], ":", true, true);
        string styleKey = styleKeyValueVec[0];
        string styleValue = styleKeyValueVec[1];
        if(ofxOSS::validKey(styleKey)){
            ofxOssRule* rule = ofxOSS::generateRule(styleKey, styleValue);
            inlineStyles->rules[ofxOSS::getEnumFromString(styleKey)] = rule;
        }
    }
    return inlineStyles;
}

void ofxLayoutElement::setInlineStyle(string style){
    this->inlineStyle = style;
}