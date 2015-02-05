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

    for(int i = 0 ; i < children.size(); i++){
        children[i]->update();
    }
    ofPopMatrix();
}

void ofxLayoutElement::addChild(ofxLayoutElement* child){
    child->parent = this;
    child->setLayout(this->layout);
    child->setAssets(this->assetsPtr);
    child->setFonts(this->fontsPtr);
    children.push_back(child);
}

void ofxLayoutElement::draw(){
    if(hasStyle(OSS_KEY::DISPLAY) && ofxOSS::getOssValueFromString(getStyle(OSS_KEY::DISPLAY)) == OSS_VALUE::NONE){
        return;
    }
    ofPushMatrix();
    ofTranslate(boundary.x, boundary.y, 0);
//    ofRotate(ofGetFrameNum() * .01, 0, 0, 1);
    if(hasStyle(OSS_KEY::SCALE)){
        ofScale(getFloatStyle(OSS_KEY::SCALE),getFloatStyle(OSS_KEY::SCALE));
    }
    ofEnableAlphaBlending();
    drawStyles();
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
    this->layout->idElementMap["#"+ID] = this;
}

bool ofxLayoutElement::hasStyle(OSS_KEY::ENUM styleKey){
    return this->styles.rules.count(styleKey) > 0;
}

string ofxLayoutElement::getStyle(OSS_KEY::ENUM styleKey){
    return this->styles.rules[styleKey]->getString();
}

float ofxLayoutElement::getFloatStyle(OSS_KEY::ENUM styleKey){
    return this->styles.rules[styleKey]->getFloat();
}

ofColor ofxLayoutElement::getColorStyle(OSS_KEY::ENUM styleKey){
    return this->styles.rules[styleKey]->getColor();
}

OSS_VALUE::ENUM ofxLayoutElement::getOssValueStyle(OSS_KEY::ENUM styleKey){
    return this->styles.rules[styleKey]->getOssValue();
}

/// |   Utilities   | ///
/// | ------------- | ///

void ofxLayoutElement::drawStyles(){
    drawBackground();
    drawText();
}

void ofxLayoutElement::drawBackground(){
    bool blendModeActive = beginBackgroundBlendMode();
    // I'm sure there is a clever blending order for this, but for now I switch the order of color and image
    // based on whether blend mode is enabled or disabled
    if(!blendModeActive){
        drawBackgroundColor();
    }
    
    drawBackgroundGradient();
    drawBackgroundImage();
    drawBackgroundVideo();
    
    if(blendModeActive){
        drawBackgroundColor();
    }
    endBackgroundBlendMode();
}

void ofxLayoutElement::drawText(){
    ofEnableAlphaBlending();
    if(hasStyle(OSS_KEY::FONT_FAMILY)){
        ofRectangle drawBox;
        
        string fontFilename = getStyle(OSS_KEY::FONT_FAMILY);
        if(fontsPtr->count(fontFilename) > 0){
            string text = getValue();
            if(hasStyle(OSS_KEY::TEXT_TRANSFORM)){
                vector<string> words = ofSplitString(text, " ",true,true);
                OSS_VALUE::ENUM textTransform = ofxOSS::getOssValueFromString(getStyle(OSS_KEY::TEXT_TRANSFORM));
                switch(textTransform){
                    case OSS_VALUE::NONE:
                        //do nothing
                        break;
                    case OSS_VALUE::UPPERCASE:
                        text = ofToUpper(text);
                        break;
                    case OSS_VALUE::LOWERCASE:
                        text = ofToLower(text);
                        break;
                    case OSS_VALUE::CAPITALIZE:
                        for(int i = 0; i < words.size(); i++) {
                            words[i] = ofToLower(words[i]);
                            words[i] = ofToUpper(words[i].substr(0,1))+words[i].substr(1,words[i].size());
                        }
                        text = ofJoinString(words, " ");
                        break;
                    default:
                        text = text;
                }
            }
            
            float fontSize;
            if(hasStyle(OSS_KEY::FONT_SIZE)){
                fontSize = ofToFloat(getStyle(OSS_KEY::FONT_SIZE));
            }
            
            float lineHeight;
            if(hasStyle(OSS_KEY::LINE_HEIGHT)){
                lineHeight = ofToFloat(getStyle(OSS_KEY::LINE_HEIGHT))/100.0f;
            }
            else{
                lineHeight = 1.0f;
            }
            
            fontsPtr->at(fontFilename).setLineHeight(lineHeight);
            
            float textMaxWidth = boundary.width;
            if(hasStyle(OSS_KEY::TEXT_MAX_WIDTH)){
                textMaxWidth = ofToFloat(getStyle(OSS_KEY::TEXT_MAX_WIDTH));
            }
            
            
            
            
            int numLines;
            ofRectangle fontBBox = fontsPtr->at(fontFilename).drawMultiLineColumn(text, fontSize, 0, 0, textMaxWidth,numLines, true, 0, true);
            drawBox.width = fontBBox.width;
            drawBox.height = fontBBox.height;
            
            
            float x;
            float y = fontSize*fontsPtr->at(fontFilename).getLineHeight();
            
            if(hasStyle(OSS_KEY::TEXT_ALIGN)){
                string textAlign = getStyle(OSS_KEY::TEXT_ALIGN);
                if(textAlign == "left"){
                    x = 0.0f;
                }
                else if(textAlign == "center"){
                    x = boundary.width/2-fontBBox.width/2;
                }
                else if(textAlign == "right"){
                    x = boundary.width - fontBBox.width;
                }
            }
            
            drawBox.x = x;
            drawBox.y = 0;
            
            if(hasStyle(OSS_KEY::TEXT_PADDING)){
                float paddingTop, paddingRight, paddingBottom, paddingLeft = 0.0f;
                vector<string> paddings = ofSplitString(getStyle(OSS_KEY::TEXT_PADDING), " ");
                if(paddings.size() == 1){
                    float padding = ofToFloat(paddings[0]);
                    paddingTop = padding;
                    paddingRight = padding;
                    paddingBottom = padding;
                    paddingLeft = padding;
                }
                else if(paddings.size() == 2){
                    float paddingV = ofToFloat(paddings[0]);
                    float paddingH = ofToFloat(paddings[1]);
                    paddingTop = paddingV;
                    paddingRight = paddingH;
                    paddingBottom = paddingV;
                    paddingLeft = paddingH;
                }
                
                else if(paddings.size() == 4){
                    paddingTop = ofToFloat(paddings[0]);
                    paddingRight = ofToFloat(paddings[1]);
                    paddingBottom = ofToFloat(paddings[2]);
                    paddingLeft = ofToFloat(paddings[3]);
                }
                
                x += paddingLeft;
                
                drawBox.width += paddingLeft+paddingRight;
                drawBox.height += paddingTop+paddingBottom;
                
            }
            
            ofFill();
            if(hasStyle(OSS_KEY::TEXT_BACKGROUND_COLOR)){
                ofSetColor(getColorStyle(OSS_KEY::TEXT_BACKGROUND_COLOR));
                ofDrawRectangle(drawBox);
            }
            
            ofFill();
            if(hasStyle(OSS_KEY::COLOR)){
                string colorStr = getStyle(OSS_KEY::COLOR);
                ofColor fontColor = ofxOSS::parseColor(colorStr);
                ofSetColor(fontColor);
            }
            fontsPtr->at(fontFilename).drawMultiLineColumn(text, fontSize, x, y, textMaxWidth,numLines, false, 0, true);
        }
    }
    ofDisableAlphaBlending();
}
void ofxLayoutElement::drawBackgroundGradient(){
    if(hasStyle(OSS_KEY::BACKGROUND_GRADIENT)){
        
        bool vertical;// = true;
        ofColor firstColor;//(1.0f, 0.0f, 0.0f, 0.0f);
        ofColor secondColor;//(0.0f, 0.0f, 1.0f, 1.0f  );
        ofxOSS::parseBackgroundGradient(getStyle(OSS_KEY::BACKGROUND_GRADIENT), &firstColor, &secondColor, &vertical);
        
        ofFloatColor firstColorF = firstColor;
        if(firstColor.limit() == 255){
            firstColorF.set(firstColor.r/255.0f, firstColor.g/255.0f, firstColor.b/255.0f, firstColor.a/255.0f);
        }
        
        ofFloatColor secondColorF = secondColor;
        if(secondColor.limit() == 255){
            secondColorF.set(secondColor.r/255.0f, secondColor.g/255.0f, secondColor.b/255.0f, secondColor.a/255.0f);
        }
        // Maybe there is a simpler OF way, but I was having issues with
        // boundary using ofBackgroundGradient
        
        glDepthMask(false);
        glEnable(GL_BLEND);
        glBegin(GL_QUADS);
        
        //FIRST COLOR
        glColor4f( firstColorF.r, firstColorF.g, firstColorF.b, firstColorF.a );
        
        // TL
        glVertex3f( 0.0f, 0.0f, 0.0f );
        
        // FOR TR
        if(vertical){
            //FIRST COLOR
            glColor4f( firstColorF.r, firstColorF.g, firstColorF.b, firstColorF.a  );
        }
        else{
            //SECOND COLOR
            glColor4f( secondColorF.r, secondColorF.g, secondColorF.b, secondColorF.a  );
        }
        
        //TR
        glVertex3f( boundary.width, 0.0f, 0.0f );
        
        
        //SECOND COLOR (FOR BR)
        glColor4f( secondColorF.r, secondColorF.g, secondColorF.b, secondColorF.a  );
        
        //BR
        glVertex3f( boundary.width, boundary.height, 0.0f );
        
        // FOR BL
        if(vertical){
            //SECOND COLOR
            glColor4f( secondColorF.r, secondColorF.g, secondColorF.b, secondColorF.a  );
        }
        else{
            //FIRST COLOR
            glColor4f( firstColorF.r, firstColorF.g, firstColorF.b, firstColorF.a  );
        }
        
        //BL
        glVertex3f( 0.0f, boundary.height, 0.0f );
        
        glDisable(GL_BLEND);
        glEnd();
        glDepthMask(true);
    }
    
}

bool ofxLayoutElement::beginBackgroundBlendMode(){
    bool blendModeActive = true;
    
    if(hasStyle(OSS_KEY::BACKGROUND_BLEND_MODE)){
        OSS_VALUE::ENUM bgBlendMode = getOssValueStyle(OSS_KEY::BACKGROUND_BLEND_MODE);
        switch (bgBlendMode) {
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
            video->load(videoPath);
            video->setVolume(0.0f);
            video->play();
            video->setLoopState(OF_LOOP_NORMAL);
        }
        else{
            video->update();
            drawBackgroundTexture(&video->getTexture());
        }
        
    }
}


void ofxLayoutElement::drawBackgroundTexture(ofTexture *texture){
    ofRectangle bgTextureTransform = ofRectangle();
    bgTextureTransform.setWidth(texture->getWidth());
    bgTextureTransform.setHeight(texture->getHeight());
    bgTextureTransform = styles.computeBackgroundTransform(bgTextureTransform, boundary);
    if(hasStyle(OSS_KEY::BACKGROUND_POSITION)){
        bgTextureTransform.setPosition(styles.getBackgroundPosition(bgTextureTransform, boundary));
    }
    
    float bgX = bgTextureTransform.x;
    float bgY = bgTextureTransform.y;
    int numRepeatX = 0;
    int numRepeatY = 0;
    
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
                repeatY = false;
                break;
            case OSS_VALUE::REPEAT_Y:
                repeatX = false;
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
            numRepeatX = ceil((float)boundary.width/(float)bgTextureTransform.width);
        }
        if(repeatY){
            while(bgY > 0){
                bgY -= bgTextureTransform.height;
            }
            numRepeatY = ceil((float)boundary.height/(float)bgTextureTransform.height);
        }
        
    }
    for(int x = 0; x <= numRepeatX; x++){
        for(int y = 0; y <= numRepeatY; y++){
            texture->draw(bgX+bgTextureTransform.width*x, bgY+bgTextureTransform.height*y, bgTextureTransform.width, bgTextureTransform.height);
        }
    }
}

void ofxLayoutElement::drawBackgroundColor(){
    if(hasStyle(OSS_KEY::BACKGROUND_COLOR)){
        ofSetColor(getColorStyle(OSS_KEY::BACKGROUND_COLOR));
        ofFill();
        ofDrawRectangle(0,0,boundary.width,boundary.height);
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
            inlineStyles.generateRule(ofxOSS::getOssKeyFromString(styleKey), styleValue);
        }
    }
    return inlineStyles;
}

void ofxLayoutElement::setInlineStyle(string style){
    this->inlineStyle = style;
}

ofRectangle ofxLayoutElement::getBoundary(){
    return boundary;
}

void ofxLayoutElement::setLayout(ofxLayout *layout){
    this->layout = layout;
}