#include "ofxLayoutElement.h"
#include "ofxLayout.h"

/// |   Constructor/Destructor   | ///
/// | -------------------------- | ///
ofxLayoutElement::ofxLayoutElement(){
    parent = NULL;
    video = NULL;
    boundary = ofRectangle();
    styles.setDefaults();
}

void ofxLayoutElement::setAssets(ofxLoaderSpool* assetsPtr){
    this->assetsPtr = assetsPtr;
}

void ofxLayoutElement::setFonts(map<string, ofxFontStash>* fontsPtr){
    this->fontsPtr = fontsPtr;
}

void ofxLayoutElement::setData(map<string, string>* dataPtr){
    this->dataPtr = dataPtr;
}

ofxLayoutElement::~ofxLayoutElement(){
    if(video != NULL){
        video->closeMovie();
        video = NULL;
    }
    for(int i = 0 ; i < children.size(); i++){
        delete children[i];
    }
}

/// |   Cycle Functions  | ///
/// | ------------------ | ///

void ofxLayoutElement::update(){
    if(parent == NULL){
        boundary.x = 0;
        boundary.y = 0;
        boundary.width = ofGetViewportWidth();
        boundary.height = ofGetViewportHeight();
    }
    else{
        boundary = styles.computeElementTransform(parent->boundary);
    }
    if(elementMask.getWidth() != boundary.width || elementMask.getHeight() != boundary.height){
        elementMask.setup(boundary.width, boundary.height,ofxMask::LUMINANCE);
    }

    ofPushMatrix();
    ofTranslate(boundary.x, boundary.y, 0);
    elementMask.beginMask();
    ofSetColor(ofToFloat(getStyle(OSS_KEY::OPACITY))*255);
    ofRect(0,0,boundary.width, boundary.height);
    if(getStyle(OSS_KEY::MASK) != ""){
        vector<string> filters;
        filters.push_back(getStyle(OSS_KEY::MASK));
        layout->computeFbo(elementMask.getMasker(),&filters);
    }
    elementMask.endMask();
    elementMask.begin();
    ofClear(0,0,0,0);
    ofEnableAlphaBlending();
    drawStyles();
    // For subclasses
    drawTag();
    ofDisableAlphaBlending();
    elementMask.end();
    for(int i = 0 ; i < children.size(); i++){
        children[i]->update();
    }
    ofPopMatrix();
}

void ofxLayoutElement::pushTransforms(){
    ofPushMatrix();
    ofTranslate(boundary.x, boundary.y, 0);
}

void ofxLayoutElement::popTransforms(){
    ofPopMatrix();
}

void ofxLayoutElement::addChild(ofxLayoutElement* child){
    child->parent = this;
    child->setLayout(this->layout);
    child->setAssets(this->assetsPtr);
    child->setFonts(this->fontsPtr);
    child->setData(this->dataPtr);
    children.push_back(child);
}

void ofxLayoutElement::draw(){
    ofPushMatrix();
    ofTranslate(boundary.x, boundary.y, 0);
    ofEnableAlphaBlending();
    
    elementMask.draw();
    ofDisableAlphaBlending();
    for(int i = 0 ; i < children.size(); i++){
        children[i]->draw();
    }
    ofSetColor(255);
    ofPopMatrix();
}

/// |   Setters/Getters   | ///
/// | ------------------- | ///
string ofxLayoutElement::getValue(){
    return this->elementValue;
}

void ofxLayoutElement::setValue(string value){
    if(ofStringTimesInString(value, "{{") > 0){
        string dataKey = value.substr(2,value.size()-4);
        
        if(dataPtr->count(dataKey) > 0){
            value = dataPtr->at(dataKey);
        }
    }
    this->elementValue = value;
}


TAG::ENUM ofxLayoutElement::getTag(){
    return this->tag;
}

void ofxLayoutElement::setTag(string tag){
    setTag(getTagEnum(tag));
}

void ofxLayoutElement::setTag(TAG::ENUM tag){
    this->tag = tag;
}


string ofxLayoutElement::getTagString(TAG::ENUM tagEnum){
    string tag = "";
    switch (tagEnum) {
        case TAG::BODY:
            tag = "body";
            break;
        case TAG::ELEMENT:
            tag = "element";
            break;
        case TAG::TEXT:
            tag = "text";
            break;
        default:
            ofLogWarning("ofxLayout::getTagString","Can't find corresponding string for enum");
            break;
    }
    return tag;
}

TAG::ENUM ofxLayoutElement::getTagEnum(string tagString){
    TAG::ENUM tag = TAG::INVALID;
    if(tagString == "body"){
        return TAG::BODY;
    }
    else if(tagString == "element") {
        return TAG::ELEMENT;
    }
    else if(tagString == "text") {
        return TAG::TEXT;
    }
    else{
        ofLogWarning("ofxLayout::getTagString","Can't find corresponding enum for tag string '"+tagString+"'");
        return TAG::INVALID;
    }
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
    return this->styles.rules.count(styleKey) > 0;
}

string ofxLayoutElement::getStyle(OSS_KEY::ENUM styleKey){
    return this->styles.rules[styleKey].getStringValue();
}

/// |   Utilities   | ///
/// | ------------- | ///

void ofxLayoutElement::drawStyles(){
    bool blendModeActive = beginBackgroundBlendMode();
    
    // I'm sure there is a clever blending order for this, but for now I switch the order of color and image
    // based on whether blend mode is enabled or disabled
    if(blendModeActive){
        drawBackgroundImage();
        drawBackgroundVideo();
        drawBackgroundColor();
    }
    else{
        drawBackgroundColor();
        drawBackgroundVideo();
        drawBackgroundImage();
    }
    endBackgroundBlendMode();
}

bool ofxLayoutElement::beginBackgroundBlendMode(){
    bool blendModeActive = true;
    
    if(hasStyle(OSS_KEY::BACKGROUND_BLEND_MODE)){
        OSS_VALUE::ENUM bgBlendMode = ofxOSS::getOssValueFromString(getStyle(OSS_KEY::BACKGROUND_BLEND_MODE));
        switch (bgBlendMode) {
            case OSS_VALUE::DISABLED:
                blendModeActive = false;
                ofEnableBlendMode(OF_BLENDMODE_DISABLED);
                break;
            case OSS_VALUE::ALPHA:
                ofEnableBlendMode(OF_BLENDMODE_ALPHA);
                break;
            case OSS_VALUE::ADD:
                ofEnableBlendMode(OF_BLENDMODE_ADD);
                break;
            case OSS_VALUE::SUBTRACT:
                ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
                break;
            case OSS_VALUE::SCREEN:
                ofEnableBlendMode(OF_BLENDMODE_SCREEN);
                break;
            case OSS_VALUE::MULTIPLY:
                ofEnableBlendMode(OF_BLENDMODE_MULTIPLY);
                break;
            default:
                blendModeActive = false;
                ofEnableBlendMode(OF_BLENDMODE_DISABLED);
        }
        if(blendModeActive){
            ofBackground(0);
        }
    }
    
    return blendModeActive;
}

void ofxLayoutElement::endBackgroundBlendMode(){
    if(hasStyle(OSS_KEY::BACKGROUND_BLEND_MODE)){
        ofDisableBlendMode();
    }
}

void ofxLayoutElement::drawBackgroundImage(){
    if(hasStyle(OSS_KEY::BACKGROUND_IMAGE)){
        ofSetColor(255);
        ofxLoaderBatch* imagesBatch = assetsPtr->getBatch("images");
        string imageID = getStyle(OSS_KEY::BACKGROUND_IMAGE);
        if(imagesBatch->hasTexture(imageID) && imagesBatch->isTextureDrawable(imageID)){
            drawBackgroundTexture(imagesBatch->getTexture(imageID));
        }
    }
}

void ofxLayoutElement::drawBackgroundVideo(){
    if(hasStyle(OSS_KEY::BACKGROUND_VIDEO)){
        ofSetColor(255);
        string videoPath = getStyle(OSS_KEY::BACKGROUND_VIDEO);
        
        if(video == NULL){
            video = new ofVideoPlayer();
            video->loadMovie(videoPath);
            video->setVolume(0.0f);
            video->play();
        }
        else{
            video->update();
            drawBackgroundTexture(&video->getTextureReference());
        }
        
    }
}


void ofxLayoutElement::drawBackgroundTexture(ofTexture *texture){
    ofRectangle bgTextureTransform = ofRectangle();
    bgTextureTransform.setWidth(texture->getWidth());
    bgTextureTransform.setHeight(texture->getHeight());
    bgTextureTransform = styles.computeBackgroundTransform(bgTextureTransform, boundary);
    if(hasStyle(OSS_KEY::BACKGROUND_POSITION)){
        bgTextureTransform.setPosition(styles.getPosition(bgTextureTransform, parent->boundary));
    }
    
    float bgX = bgTextureTransform.x;
    float bgY = bgTextureTransform.y;
    int numRepeatX = 1;
    int numRepeatY = 1;
    
    if(hasStyle(OSS_KEY::BACKGROUND_REPEAT)){
        OSS_VALUE::ENUM repeatValue = ofxOSS::getOssValueFromString(getStyle(OSS_KEY::BACKGROUND_REPEAT));
        bool repeatX, repeatY;
        switch(repeatValue){
            case OSS_VALUE::REPEAT:
                repeatX = true;
                repeatY = true;
                break;
            case OSS_VALUE::REPEAT_X:
                repeatX = true;
                break;
            case OSS_VALUE::REPEAT_Y:
                repeatY = true;
                break;
            default:
                repeatX = false;
                repeatY = false;
        }
        
        if(repeatX){
            while(bgX > 0){
                bgX -= bgTextureTransform.width;
            }
            numRepeatX = ceil((float)boundary.width/(float)bgTextureTransform.width)+1;
        }
        if(repeatY){
            while(bgY > 0){
                bgY -= bgTextureTransform.height;
            }
            numRepeatY = ceil((float)boundary.height/(float)bgTextureTransform.height)+1;
        }
        
    }
    
    for(int x = 0; x < numRepeatX; x++){
        for(int y = 0; y < numRepeatY; y++){
            texture->draw(bgX+bgTextureTransform.width*x, bgY+bgTextureTransform.height*y, bgTextureTransform.width, bgTextureTransform.height);
        }
    }
}

void ofxLayoutElement::drawBackgroundColor(){
    if(hasStyle(OSS_KEY::BACKGROUND_COLOR)){
        ofSetColor(ofxOSS::parseColor(getStyle(OSS_KEY::BACKGROUND_COLOR)));
        ofFill();
        ofRect(0,0,boundary.width,boundary.height);
    }
}

void ofxLayoutElement::overrideStyles(ofxOSS *styleObject){
    for(auto iterator = styleObject->rules.begin(); iterator != styleObject->rules.end(); iterator++){
        this->styles.rules[iterator->first] = iterator->second;
    }
}

string ofxLayoutElement::getInlineStyle(){
    return this->inlineStyle;
}

ofxOSS ofxLayoutElement::getInlineStyles(){
    ofxOSS inlineStyles;
    vector<string> stylesVec = ofSplitString(this->inlineStyle, ";", true, true);
    for(int i = 0; i < stylesVec.size(); i++){
        vector<string> styleKeyValueVec = ofSplitString(stylesVec[i], ":", true, true);
        string styleKey = styleKeyValueVec[0];
        string styleValue = styleKeyValueVec[1];
        if(ofxOSS::validKey(styleKey)){
            ofxOssRule rule = ofxOSS::generateRule(styleKey, styleValue);
            inlineStyles.rules[ofxOSS::getOssKeyFromString(styleKey)] = rule;
        }
    }
    return inlineStyles;
}

void ofxLayoutElement::setInlineStyle(string style){
    this->inlineStyle = style;
}

ofFbo* ofxLayoutElement::getFbo(){
    return elementMask.getMaskee();
}

ofRectangle ofxLayoutElement::getBoundary(){
    return boundary;
}

void ofxLayoutElement::setLayout(ofxLayout *layout){
    this->layout = layout;
}