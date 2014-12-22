#include "ofxLayoutElement.h"

/// |   Constructor/Destructor   | ///
/// | -------------------------- | ///
ofxLayoutElement::ofxLayoutElement(){
    backgroundVideoReadyToDraw = false;
    
    stylesheet = new ofxOSS();
    backgroundImageReadyToDraw = false;
    backgroundImage = new ofTexture(); //create your own texture, it will be cleared so be sure its empty
    
    //setup the loader by giving it a texture to load into, and a resizing quality
    //CV_INTER_LINEAR, CV_INTER_NN, CV_INTER_CUBIC, CV_INTER_AREA
    progressiveTextureLoader.setup(backgroundImage, CV_INTER_CUBIC);
    
    //these 2 settings control how long it takes for the tex to load
    progressiveTextureLoader.setScanlinesPerLoop(512);//see header for explanation
    progressiveTextureLoader.setTargetTimePerFrame(10.0f); //how long to spend uploading data per frame, in ms
    
    //add a listener to get notified when tex is fully loaded
    //and one to let you know when the texture is drawable
    ofAddListener(progressiveTextureLoader.textureReady, this, &ofxLayoutElement::backgroundImageReady);
    ofAddListener(progressiveTextureLoader.textureDrawable, this, &ofxLayoutElement::backgroundImageDrawable);
    
    elementFbo = new ofFbo();
    elementFbo->allocate();
    
    parentNode = NULL;
}

ofxLayoutElement::~ofxLayoutElement(){
    delete backgroundImage;
    delete elementFbo;
    for(int i = 0; i < childNodes.size(); i++){
        delete childNodes[i];
    }
}

/// |   Cycle Functions  | ///
/// | ------------------ | ///

void ofxLayoutElement::update(){
    if(!parentNode){
        parentBoundary = ofRectangle(0.0f, 0.0f, ofGetWidth(), ofGetHeight());
    }
    else{
        parentBoundary = ofRectangle(parentNode->boundary);
    }
    
    updateDimensions();
    updatePosition();
    updateBackgroundImage();
    updateBackgroundVideo();
    
    for(int i = 0; i < childNodes.size(); i++){
        childNodes[i]->update();
    }
}

void ofxLayoutElement::draw(){
    // Saving the scissor state
    glPushAttrib(GL_SCISSOR_BIT);
    ofPushMatrix();
    ofTranslate(boundary.x, boundary.y, 0);
    elementFbo->begin();
    drawStyles();
    elementFbo->end();
    elementFbo->draw(0,0,boundary.width,boundary.height);
    for(int i = 0; i < childNodes.size(); i++){
        childNodes[i]->draw();
    }
    
    ofPopMatrix();
    glPopAttrib();
}

/// |   Setters/Getters   | ///
/// | ------------------- | ///

string ofxLayoutElement::getID(){
    return ID;
}

void ofxLayoutElement::setID(string _ID){
    ID = _ID;
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

bool ofxLayoutElement::hasStyle(OSS_KEY::ENUM styleKey){
    ofxOSS* idStyles = stylesheet->getStylesByID(getID());
    return elementStyles.hasStyle(styleKey) || (idStyles != NULL && idStyles->hasStyle(styleKey));
}

bool ofxLayoutElement::hasStyle(string styleKey){
    return hasStyle(ofxOSS::getEnumFromString(styleKey));
}

void ofxLayoutElement::setStyle(OSS_KEY::ENUM styleKey, string styleValue){
    elementStyles.setStyle(styleKey, styleValue);
}

void ofxLayoutElement::setStyle(string styleKey, string styleValue){
    return setStyle(ofxOSS::getEnumFromString(styleKey), styleValue);
}



void ofxLayoutElement::setStylesheet(ofxOSS *stylesheet){
    this->stylesheet = stylesheet;
}

/// |   Utilities   | ///
/// | ------------- | ///

void ofxLayoutElement::addChildElement(ofxLayoutElement* childElement){
    childElement->parentNode = this;
    childElement->stylesheet = this->stylesheet;
    childNodes.push_back(childElement);
}

void ofxLayoutElement::updateDimensions(){
    ofxOSS* widthOSS = getOverridingStylesheet(OSS_KEY::WIDTH);
    boundary.width = widthOSS->getDimensionStyleValue(OSS_KEY::WIDTH,parentBoundary.width);
    
    ofxOSS* heightOSS = getOverridingStylesheet(OSS_KEY::HEIGHT);
    boundary.height = heightOSS->getDimensionStyleValue(OSS_KEY::HEIGHT,parentBoundary.height);
    
    if(boundary.height != elementFbo->getHeight() || boundary.width != elementFbo->getWidth()){
        elementFbo->allocate(boundary.width, boundary.height);
    }
}

void ofxLayoutElement::updatePosition(){
    if(hasStyle(OSS_KEY::POSITION)){
        ofxOSS* positionOSS = getOverridingStylesheet(OSS_KEY::POSITION);
        ofPoint pos = positionOSS->getPosition(boundary, parentBoundary);
        boundary.x = pos.x;
        boundary.y = pos.y;
    }
}

void ofxLayoutElement::updateBackgroundImage(){
    if(hasStyle(OSS_KEY::BACKGROUND_IMAGE)){
        ofxOSS* backgroundImageOSS = getOverridingStylesheet(OSS_KEY::BACKGROUND_IMAGE);
        string src = backgroundImageOSS->getStyle(OSS_KEY::BACKGROUND_IMAGE);
        if(src != backgroundImageName){
            backgroundImageName = src;
            
            //start loading the texture!
            progressiveTextureLoader.loadTexture(backgroundImageName, true /*create mipmaps*/);
        }
        

    }
}

void ofxLayoutElement::updateBackgroundVideo(){
    if(hasStyle(OSS_KEY::BACKGROUND_VIDEO)){
        ofxOSS* backgroundVideoOSS = getOverridingStylesheet(OSS_KEY::BACKGROUND_VIDEO);
        string src = backgroundVideoOSS->getStyle(OSS_KEY::BACKGROUND_VIDEO);
        if(src != backgroundVideoName){
            backgroundVideoName = src;
            backgroundVideo.loadMovie(backgroundVideoName);
            backgroundVideo.play();
            backgroundVideo.setVolume(0.0);
        }
        
        backgroundVideo.update();
    }
}

void ofxLayoutElement::drawStyles(){
    if(hasStyle(OSS_KEY::BACKGROUND_COLOR)){
        ofxOSS* backgroundColorOSS = getOverridingStylesheet(OSS_KEY::BACKGROUND_COLOR);
        ofSetColor(backgroundColorOSS->getColorStyle(OSS_KEY::BACKGROUND_COLOR));
        ofFill();
        ofRect(0,0,boundary.width,boundary.height);
    }
    
    if((hasStyle(OSS_KEY::BACKGROUND_IMAGE) && backgroundImageReadyToDraw) || (hasStyle(OSS_KEY::BACKGROUND_VIDEO))){
        ofSetColor(255);
        
        ofRectangle backgroundTransform;
        if(hasStyle(OSS_KEY::BACKGROUND_IMAGE)){
            backgroundTransform = ofRectangle(0,0,backgroundImage->getWidth(), backgroundImage->getHeight());
        }
        else if (hasStyle(OSS_KEY::BACKGROUND_VIDEO)) {
            backgroundTransform = ofRectangle(0,0,backgroundVideo.getWidth(), backgroundVideo.getHeight());
        }
        
        
        
        if(hasStyle(OSS_KEY::BACKGROUND_SIZE)){
            ofxOSS* backgroundSizeOSS = getOverridingStylesheet(OSS_KEY::BACKGROUND_SIZE);
            backgroundTransform = backgroundSizeOSS->getBackgroundSizeDimensions(backgroundTransform, boundary);
        }
        
        if(hasStyle(OSS_KEY::BACKGROUND_POSITION)){
            ofxOSS* backgroundPositionOSS = getOverridingStylesheet(OSS_KEY::BACKGROUND_POSITION);
            backgroundTransform = backgroundPositionOSS->getBackgroundPosition(backgroundTransform, boundary);
        }
        if(hasStyle(OSS_KEY::BACKGROUND_IMAGE)){
            backgroundImage->draw(backgroundTransform.x, backgroundTransform.y, 0, backgroundTransform.width,backgroundTransform.height);
        }
        else if (hasStyle(OSS_KEY::BACKGROUND_VIDEO)){
            backgroundVideo.draw(backgroundTransform.x, backgroundTransform.y, backgroundTransform.width, backgroundTransform.height);
        }
    }
    
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

ofxOSS* ofxLayoutElement::getOverridingStylesheet(string styleKey){
    return getOverridingStylesheet(ofxOSS::getEnumFromString(styleKey));
}


/// |   Background Image   | ///
/// | -------------------- | ///

void ofxLayoutElement::backgroundImageDrawable(ofxProgressiveTextureLoad::textureEvent& arg){
    ofLogNotice() << "background image drawable!";
    backgroundImageReadyToDraw = true;
}

void ofxLayoutElement::backgroundImageReady(ofxProgressiveTextureLoad::textureEvent& arg){
    if (arg.loaded){
        ofLogNotice() << "background image ready!";
        backgroundImageReadyToDraw = true;
    }else{
        ofLogError() << "background image load failed!" << arg.texturePath;
    }
}

/// |   Background Video   | ///
/// | -------------------- | ///

