#include "ofxLayoutElement.h"
#include "ofxLayout.h"

/// |   Constructor/Destructor   | ///
/// | -------------------------- | ///
ofxLayoutElement::ofxLayoutElement(){
    stateTransitioning = false;
    opacity = 1.0;
    parent = NULL;
    video = NULL;
    position = ofPoint(0,0);
    dimensions = ofRectangle(0,0,ofGetViewportWidth(),ofGetViewportHeight());
//    fbo.allocate(dimensions.getWidth(),dimensions.getHeight(), GL_RGBA);
//    fbo.begin();
//    ofClear(0,0,0,0);
//    fbo.end();
    
    styles.setDefaults();
    mouseState = MOUSE_STATE::NONE;
    
    state = "default";
    ofAddListener(ofEvents().mouseMoved, this, &ofxLayoutElement::mouseMoved);
    ofAddListener(ofEvents().mousePressed, this, &ofxLayoutElement::mousePressed);
    ofAddListener(ofEvents().mouseReleased, this, &ofxLayoutElement::mouseReleased);
}

void ofxLayoutElement::mouseMoved(ofMouseEventArgs &args){
    if(dimensions.inside(args.x, args.y)){
        
    }
}

void ofxLayoutElement::mouseReleased(ofMouseEventArgs &args){
}

void ofxLayoutElement::mousePressed(ofMouseEventArgs &args){
}

void ofxLayoutElement::setMouseState(MOUSE_STATE::ENUM mouseState){
    this->mouseState = mouseState;
}

void ofxLayoutElement::setParent(ofxLayoutElement *parent){
    this->parent = parent;
    this->layout = parent->layout;
}

ofxLayoutElement* ofxLayoutElement::getParent(){
    return this->parent;
}

MOUSE_STATE::ENUM ofxLayoutElement::getMouseState(){
    return this->mouseState;
}

ofxLayoutElement::~ofxLayoutElement(){
    ofRemoveListener(ofEvents().mouseMoved, this, &ofxLayoutElement::mouseMoved);
    ofRemoveListener(ofEvents().mousePressed, this, &ofxLayoutElement::mousePressed);
    ofRemoveListener(ofEvents().mouseReleased, this, &ofxLayoutElement::mouseReleased);
    
    if(video != NULL){
        video->closeMovie();
        video = NULL;
    }
    for(int i = 0 ; i < children.size(); i++){
        delete children[i];
    }
}

void ofxLayoutElement::show(){
    styles.setStyle(OSS_KEY::DISPLAY, OSS_VALUE::BLOCK);
}

void ofxLayoutElement::hide(){
    styles.setStyle(OSS_KEY::DISPLAY, OSS_VALUE::NONE);
}

bool ofxLayoutElement::visible(){
    bool displayNone = hasStyle(OSS_KEY::DISPLAY) && getOssValueStyle(OSS_KEY::DISPLAY) == OSS_VALUE::NONE;
    bool opacityZero = hasStyle(OSS_KEY::OPACITY) && getFloatStyle(OSS_KEY::OPACITY) == 0.0f;
    return !displayNone && !opacityZero;
}

/// |   Cycle Functions  | ///
/// | ------------------ | ///

void ofxLayoutElement::update(){
    // If root element, boundary is initially set to the current viewport dimensions
    if(this->getTag() == TAG::BODY){
        setPosition(ofPoint(0,0));
        setDimensions(ofGetViewportWidth(), ofGetViewportHeight());
    }
    

    // *** COMPUTE WIDTH *** //
    if(hasStyle(OSS_KEY::WIDTH)){
        // If width is zero, sets to auto (expands to fit children)
//        if(getStyle(OSS_KEY::WIDTH)->asFloat() == 0){
//            dimensions.width = 0;
//            getStyle(OSS_KEY::WIDTH)->setOssValue(OSS_VALUE::AUTO);
//        }
        // Percent width
        if(hasParent() && getStyle(OSS_KEY::WIDTH)->getType() == OSS_TYPE::PERCENT){
            float percentWidth = getStyle(OSS_KEY::WIDTH)->asFloat()/100.0f;
            dimensions.width = percentWidth * parent->getBoundary().getWidth();
        }
        // Fixed size (px)
        else if (getStyle(OSS_KEY::WIDTH)->getType() == OSS_TYPE::NUMBER){
            dimensions.width = getStyle(OSS_KEY::WIDTH)->asFloat();
        }
    }
    
    // *** COMPUTING CHILDREN *** //
    
    float childrenHeight;
    
    
    float relX = 0;
    float relY = 0;
    float childRowHeight = 0;
    
    bool isWidthAuto = hasStyle(OSS_KEY::WIDTH) && getStyle(OSS_KEY::WIDTH)->asOssValue() == OSS_VALUE::AUTO;

    float minWidth = 0;
    if(hasStyle(OSS_KEY::MIN_WIDTH)){
        minWidth = getStyle(OSS_KEY::MIN_WIDTH)->asFloat();
        dimensions.width = max(dimensions.width, minWidth);
    }
    
    float maxWidth = hasParent() ? parent->getBoundary().getWidth() : INFINITY;
    if(hasStyle(OSS_KEY::MAX_WIDTH)){
        maxWidth = getStyle(OSS_KEY::MAX_WIDTH)->asFloat();
        dimensions.width = max(dimensions.width, maxWidth);
    }
    
    float expandingWidth = minWidth;
    float maxExpandedWidth = expandingWidth;
    
    for(int i = 0 ; i < children.size(); i++){
        children[i]->update();
        
        float cW = children[i]->getDimensions().getWidth();
        float cH = children[i]->getDimensions().getHeight();
        
        // Expanding div to contain children
        if((isWidthAuto && (relX+cW) <= maxWidth)){
            expandingWidth += cW;
        }
        else if(relX+cW > maxWidth){
            relX = 0;
            relY += childRowHeight;
            childRowHeight = 0;
            if(isWidthAuto){
                expandingWidth = 0;
            }
        }
        maxExpandedWidth = max(maxExpandedWidth, cW);
        maxExpandedWidth = max(expandingWidth,maxExpandedWidth);
 
        // Setting child position
        ofPoint childPos = ofPoint(relX,relY);
        children[i]->setPosition(childPos);
        children[i]->setDimensions(cW, cH);
        
        relX += cW;
        childRowHeight =  cH > childRowHeight ? cH : childRowHeight;
    }
    
    if(isWidthAuto){
        dimensions.width = maxExpandedWidth;
    }
    
    // Only variable to escape this scope
    childrenHeight = relY+childRowHeight;
    
    
    
    
    // *** COMPUTE HEIGHT *** //
    if(hasStyle(OSS_KEY::HEIGHT)){
        if(getStyle(OSS_KEY::HEIGHT)->asOssValue() == OSS_VALUE::AUTO || getStyle(OSS_KEY::HEIGHT)->asFloat() == 0){
            dimensions.height = childrenHeight;
        }
        else if(hasParent() && getStyle(OSS_KEY::HEIGHT)->getType() == OSS_TYPE::PERCENT){
            float percentHeight = getStyle(OSS_KEY::HEIGHT)->asFloat()/100.0f;
            dimensions.height = percentHeight * parent->getBoundary().getHeight();
        }
        // Fixed size (px)
        else if(getStyle(OSS_KEY::HEIGHT)->getType() == OSS_TYPE::NUMBER){
            dimensions.height = getStyle(OSS_KEY::HEIGHT)->asFloat();
        }
    }
    
    // Positioning
    for(int i = 0 ; i < children.size(); i++){
        ofPoint relativePos = children[i]->getPosition();
        
        ofPoint offset = ofPoint(0,0);
        
        
        // If positioning type is fixed, need to compute offset to the viewport, not parent element
        ofRectangle containingDimensions;
        if(hasStyle(OSS_KEY::POSITION) && getStyle(OSS_KEY::POSITION)->asOssValue() == OSS_VALUE::FIXED){
            containingDimensions = ofGetCurrentViewport();
        }
        else{
            containingDimensions = getDimensions();
        }
        
        
        if(children[i]->hasStyle(OSS_KEY::TOP)){
            if(children[i]->getStyle(OSS_KEY::TOP)->getType() == OSS_TYPE::PERCENT){
                float percentTop = children[i]->getStyle(OSS_KEY::TOP)->asFloat()/100.0f;
                offset.y = percentTop * containingDimensions.getHeight();
            }
            // Fixed size (px)
            else if(children[i]->getStyle(OSS_KEY::TOP)->getType() == OSS_TYPE::NUMBER){
                offset.y = children[i]->getStyle(OSS_KEY::TOP)->asFloat();
            }
        }
        if(children[i]->hasStyle(OSS_KEY::BOTTOM)){
            if(children[i]->getStyle(OSS_KEY::BOTTOM)->getType() == OSS_TYPE::PERCENT){
                // Inverse
                float percentTop = 1.0f-(children[i]->getStyle(OSS_KEY::BOTTOM)->asFloat()/100.0f);
                offset.y = percentTop * containingDimensions.getHeight();
            }
            // Fixed size (px)
            else if(children[i]->getStyle(OSS_KEY::BOTTOM)->getType() == OSS_TYPE::NUMBER){
                offset.y = containingDimensions.getHeight() - children[i]->getHeight() - children[i]->getStyle(OSS_KEY::BOTTOM)->asFloat();
            }
        }

        if(children[i]->hasStyle(OSS_KEY::LEFT)){
            if(children[i]->getStyle(OSS_KEY::LEFT)->getType() == OSS_TYPE::PERCENT){
                float percentLeft = children[i]->getStyle(OSS_KEY::LEFT)->asFloat()/100.0f;
                offset.x = percentLeft * containingDimensions.getWidth();
            }
            // Fixed size (px)
            else if(children[i]->getStyle(OSS_KEY::LEFT)->getType() == OSS_TYPE::NUMBER){
                offset.x = children[i]->getStyle(OSS_KEY::LEFT)->asFloat();
            }
        }
        if(children[i]->hasStyle(OSS_KEY::RIGHT)){
            if(children[i]->getStyle(OSS_KEY::RIGHT)->getType() == OSS_TYPE::PERCENT){
                // Inverse
                float percentTop = 1.0f-(children[i]->getStyle(OSS_KEY::RIGHT)->asFloat()/100.0f);
                offset.x = percentTop * containingDimensions.getWidth();
            }
            // Fixed size (px)
            else if(children[i]->getStyle(OSS_KEY::RIGHT)->getType() == OSS_TYPE::NUMBER){
                offset.x = containingDimensions.getWidth() - children[i]->getWidth() - children[i]->getStyle(OSS_KEY::RIGHT)->asFloat();
            }
        }
        ofPoint childPos = offset;
        
        if(hasStyle(OSS_KEY::POSITION)){
            switch(children[i]->getStyle(OSS_KEY::POSITION)->asOssValue()){
                case OSS_VALUE::RELATIVE:
                    childPos = relativePos+offset;
                    break;
                case OSS_VALUE::ABSOLUTE:
                    childPos = offset;
                    break;
                case OSS_VALUE::FIXED:
                    // Note that offset is computed differently for fixed
                    childPos = offset - getGlobalPosition();
                    break;
                case OSS_VALUE::STATIC:
                default:
                    childPos = relativePos;
            }
        }
        children[i]->setPosition(childPos);
    }
    
    
    // Setting child position
    
//    if((int)fbo.getWidth() != (int)dimensions.getWidth() || (int)fbo.getHeight() != (int)dimensions.getHeight()){
//        cout << "CHANGES" << endl;
//        fbo.allocate((int)dimensions.getWidth(),(int)dimensions.getHeight(), GL_RGBA);
//        fbo.begin();
//        ofClear(0,0,0,0);
//        fbo.end();
//    }
}

void ofxLayoutElement::addChild(ofxLayoutElement* child){
    child->setParent(this);
    children.push_back(child);
}

void ofxLayoutElement::draw(){
//    update();
    if(visible()){
        ofPushStyle();
        ofPushMatrix();
        ofTranslate(getPosition());
        if(hasStyle(OSS_KEY::SCALE)){
            ofScale(getFloatStyle(OSS_KEY::SCALE),getFloatStyle(OSS_KEY::SCALE));
        }
        if(hasStyle(OSS_KEY::OSS_OVERFLOW) && getOssValueStyle(OSS_KEY::OSS_OVERFLOW) == OSS_VALUE::HIDDEN){
            glPushAttrib(GL_SCISSOR_BIT);
            
            //Silly lower left origin of glScissor
            glScissor(0,ofGetViewportHeight() - getHeight(),getWidth(),getHeight());
            
            glEnable(GL_SCISSOR_TEST);
        }
        
        glEnable(GL_BLEND);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        if(hasStyle(OSS_KEY::OPACITY)){
            opacity *= getStyle(OSS_KEY::OPACITY)->asFloat();
        }
        
        ofSetColor(255*opacity);
        drawBackground();
        drawShape();
        drawText();
        
        glDisable(GL_BLEND);

        for(int i = 0 ; i < children.size(); i++){
            children[i]->setOpacity(opacity);
            children[i]->draw();
        }
        
        if(hasStyle(OSS_KEY::OSS_OVERFLOW) && getOssValueStyle(OSS_KEY::OSS_OVERFLOW)== OSS_VALUE::HIDDEN){
            glDisable(GL_SCISSOR_TEST);
            glPopAttrib();
        }
        
        ofPopMatrix();
        ofPopStyle();
    }
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
        case TAG::DIV:
            tag = "div";
            break;
        case TAG::SVG:
            tag = "svg";
            break;
        case TAG::G:
            tag = "g";
            break;
        case TAG::POLYGON:
            tag = "polygon";
            break;
        case TAG::PATH:
            tag = "path";
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
    else if(tagString == "div") {
        return TAG::DIV;
    }
    else if(tagString == "svg") {
        return TAG::SVG;
    }
    else if(tagString == "g") {
        return TAG::G;
    }
    else if(tagString == "polygon") {
        return TAG::POLYGON;
    }
    else if(tagString == "path") {
        return TAG::PATH;
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

ofxOssRule* ofxLayoutElement::getStyle(OSS_KEY::ENUM styleKey){
    return this->styles.rules[styleKey];
}

string ofxLayoutElement::getStringStyle(OSS_KEY::ENUM styleKey){
    return getStyle(styleKey)->asString();
}

float ofxLayoutElement::getFloatStyle(OSS_KEY::ENUM styleKey){
    return getStyle(styleKey)->asFloat();
}

ofColor ofxLayoutElement::getColorStyle(OSS_KEY::ENUM styleKey){
    return getStyle(styleKey)->asColor();
}

OSS_VALUE::ENUM ofxLayoutElement::getOssValueStyle(OSS_KEY::ENUM styleKey){
    return getStyle(styleKey)->asOssValue();
}

/// |   Utilities   | ///
/// | ------------- | ///
void ofxLayoutElement::drawShape(){
    ofPushStyle();
    ofColor fill;
    ofColor stroke;
    if(hasStyle(OSS_KEY::FILL)){
        fill = getColorStyle(OSS_KEY::FILL);
        fill.a *= opacity;
        shape.setFillColor(fill);
    }

    if(hasStyle(OSS_KEY::STROKE)){
        stroke = getColorStyle(OSS_KEY::STROKE);
        stroke.a *= opacity;
        shape.setStrokeColor(stroke);
    }
    
    if(hasStyle(OSS_KEY::STROKE_MITERLIMIT)){
        shape.setStrokeWidth(getFloatStyle(OSS_KEY::STROKE_MITERLIMIT)/10);
    }
    
    ofSetColor(255,255, 255, 255);
    shape.draw();
    ofPopStyle();
}

void ofxLayoutElement::drawBackground(){
    ofPushStyle();
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
    ofPopStyle();
}

void ofxLayoutElement::drawText(){
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    if(hasStyle(OSS_KEY::FONT_FAMILY)){
        ofRectangle drawBox;
        
        string fontFilename = getStringStyle(OSS_KEY::FONT_FAMILY);
        if(layout->getFonts()->count(fontFilename) > 0){
            string text = getValue();
            if(hasStyle(OSS_KEY::TEXT_TRANSFORM)){
                vector<string> words = ofSplitString(text, " ",true,true);
                OSS_VALUE::ENUM textTransform = getOssValueStyle(OSS_KEY::TEXT_TRANSFORM);
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
                fontSize = getFloatStyle(OSS_KEY::FONT_SIZE);
            }
            
            float lineHeight;
            if(hasStyle(OSS_KEY::LINE_HEIGHT)){
                lineHeight = getFloatStyle(OSS_KEY::LINE_HEIGHT)/100.0f;
            }
            else{
                lineHeight = 1.0f;
            }
            
            layout->getFonts()->at(fontFilename).setLineHeight(lineHeight);
            
            float textMaxWidth = dimensions.width;
            if(hasStyle(OSS_KEY::TEXT_MAX_WIDTH)){
                textMaxWidth = getFloatStyle(OSS_KEY::TEXT_MAX_WIDTH);
            }
            
            
            
            
            int numLines;
            ofRectangle fontBBox = layout->getFonts()->at(fontFilename).drawMultiLineColumn(text, fontSize, 0, 0, textMaxWidth,numLines, true, 0, true);
            drawBox.width = fontBBox.width;
            drawBox.height = fontBBox.height;
            
            
            float x;
            float y = fontSize*layout->getFonts()->at(fontFilename).getLineHeight();
            
            if(hasStyle(OSS_KEY::TEXT_ALIGN)){
                string textAlign = getStringStyle(OSS_KEY::TEXT_ALIGN);
                if(textAlign == "left"){
                    x = 0.0f;
                }
                else if(textAlign == "center"){
                    x = dimensions.width/2-fontBBox.width/2;
                }
                else if(textAlign == "right"){
                    x = dimensions.width - fontBBox.width;
                }
            }
            
            drawBox.x = x;
            drawBox.y = 0;
            
            if(hasStyle(OSS_KEY::TEXT_PADDING)){
                float paddingTop, paddingRight, paddingBottom, paddingLeft = 0.0f;
                vector<string> paddings = ofSplitString(getStringStyle(OSS_KEY::TEXT_PADDING), " ");
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
                string colorStr = getStringStyle(OSS_KEY::COLOR);
                ofColor fontColor = ofxOSS::parseColor(colorStr);
                ofSetColor(fontColor);
            }

            layout->getFonts()->at(fontFilename).drawMultiLineColumn(text, fontSize, x, y, textMaxWidth,numLines, false, 0, true);
        }
    }
    ofDisableAlphaBlending();
}
void ofxLayoutElement::drawBackgroundGradient(){
    if(hasStyle(OSS_KEY::BACKGROUND_GRADIENT)){
        
        bool vertical;// = true;
        ofColor firstColor;//(1.0f, 0.0f, 0.0f, 0.0f);
        ofColor secondColor;//(0.0f, 0.0f, 1.0f, 1.0f  );
        ofxOSS::parseBackgroundGradient(getStringStyle(OSS_KEY::BACKGROUND_GRADIENT), &firstColor, &secondColor, &vertical);
        firstColor.a *= opacity;
        secondColor.a *= opacity;
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
        glVertex3f( dimensions.width, 0.0f, 0.0f );
        
        
        //SECOND COLOR (FOR BR)
        glColor4f( secondColorF.r, secondColorF.g, secondColorF.b, secondColorF.a  );
        
        //BR
        glVertex3f( dimensions.width, dimensions.height, 0.0f );
        
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
        glVertex3f( 0.0f, dimensions.height, 0.0f );
        
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
    ofPushStyle();
    if(hasStyle(OSS_KEY::BACKGROUND_IMAGE)){
        ofSetColor(255);
        ofxLoaderBatch* imagesBatch = layout->getAssets()->getBatch("images");
        string imageID = getStringStyle(OSS_KEY::BACKGROUND_IMAGE);
        if(imagesBatch->hasTexture(imageID) && imagesBatch->isTextureReady(imageID)){
            drawBackgroundTexture(imagesBatch->getTexture(imageID));
        }
    }
    ofPopStyle();
}

void ofxLayoutElement::drawBackgroundVideo(){
    ofPushStyle();
    if(hasStyle(OSS_KEY::BACKGROUND_VIDEO)){
        ofSetColor(255,255,255,255);
        string videoPath = getStringStyle(OSS_KEY::BACKGROUND_VIDEO);
        
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
    ofPopStyle();
}


void ofxLayoutElement::drawBackgroundTexture(ofTexture *texture){
    ofPushStyle();
    texture->setTextureMinMagFilter(GL_NEAREST,GL_NEAREST); 
    ofRectangle bgTextureTransform = ofRectangle();
    bgTextureTransform.setWidth(texture->getWidth());
    bgTextureTransform.setHeight(texture->getHeight());
    bgTextureTransform.setX(0);
    bgTextureTransform.setY(0);
//    bgTextureTransform = styles.computeBackgroundTransform(bgTextureTransform, getBoundary());
//    if(hasStyle(OSS_KEY::BACKGROUND_POSITION)){
//        bgTextureTransform.setPosition(styles.getBackgroundPosition(bgTextureTransform, dimensions));
//    }
    
    float bgX = bgTextureTransform.x;
    float bgY = bgTextureTransform.y;
    int numRepeatX = 0;
    int numRepeatY = 0;
    
    if(hasStyle(OSS_KEY::BACKGROUND_REPEAT)){
        OSS_VALUE::ENUM repeatValue = getStyle(OSS_KEY::BACKGROUND_REPEAT)->asOssValue();
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
            numRepeatX = ceil((float)dimensions.width/(float)bgTextureTransform.width);
        }
        if(repeatY){
            while(bgY > 0){
                bgY -= bgTextureTransform.height;
            }
            numRepeatY = ceil((float)dimensions.height/(float)bgTextureTransform.height);
        }
    }
    ofSetColor(255, 255, 255,floor(255*opacity));
    for(int x = 0; x <= numRepeatX; x++){
        for(int y = 0; y <= numRepeatY; y++){
            texture->draw(bgX+bgTextureTransform.width*x, bgY+bgTextureTransform.height*y, bgTextureTransform.width, bgTextureTransform.height);
        }
    }
    ofPopStyle();
}

void ofxLayoutElement::drawBackgroundColor(){
    ofPushStyle();
    if(hasStyle(OSS_KEY::BACKGROUND_COLOR)){
        ofColor color = getColorStyle(OSS_KEY::BACKGROUND_COLOR);
        color.a *= opacity;
        ofSetColor(color);
        ofFill();
        ofDrawRectangle(0,0,dimensions.width,dimensions.height);
    }
    ofPopStyle();
}

void ofxLayoutElement::overrideStyles(ofxOSS *styleObject){
    for(auto iterator = styleObject->rules.begin(); iterator != styleObject->rules.end(); iterator++){
        this->styles.rules[iterator->first] = iterator->second;
    }
}


void ofxLayoutElement::copyStyles(ofxOSS *styleObject){
    for(auto iterator = styleObject->rules.begin(); iterator != styleObject->rules.end(); iterator++){
        if(this->styles.rules.count(iterator->first) == 0){
            this->styles.rules[iterator->first] = iterator->second;
        }
        else{
            this->styles.rules[iterator->first]->setValue(iterator->second->asString());
        }
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

void ofxLayoutElement::appendInlineStyle(string style){
    this->inlineStyle += (" "+style);
}

ofRectangle ofxLayoutElement::getBoundary(){
    ofPoint absPos = getGlobalPosition();
    return ofRectangle(absPos.x,absPos.y,dimensions.width, dimensions.height);
}

ofPoint ofxLayoutElement::getGlobalPosition(){
    ofPoint pos(0,0);
    if(hasParent()){
        pos.set(parent->getGlobalPosition());
    }
    pos.set(pos.x+getPosition().x, pos.y+getPosition().y);
    return pos;
}

ofPoint ofxLayoutElement::getPosition(){
    return position;
}

void ofxLayoutElement::setDimensions(float width, float height){
    this->dimensions.set(0, 0, width, height);
}

void ofxLayoutElement::setLayout(ofxLayout *layout){
    this->layout = layout;
}

ofPath* ofxLayoutElement::getShape(){
    return &this->shape;
}

ofFbo* ofxLayoutElement::getFbo(){
    return &fbo;
}

bool ofxLayoutElement::hasParent(){
    return parent != NULL;
}

void ofxLayoutElement::setPosition(ofPoint pos){
    this->position.set(pos);
}

ofRectangle ofxLayoutElement::getDimensions(){
    return this->dimensions;
}

float ofxLayoutElement::getWidth(){
    return this->dimensions.getWidth();
}

float ofxLayoutElement::getHeight(){
    return this->dimensions.getHeight();
}

string ofxLayoutElement::getState(){
    return this->state;
}

bool ofxLayoutElement::isStateTransitioning(){
    return stateTransitioning;
}

void ofxLayoutElement::stateTransFinished(ofxAnimatable::AnimationEvent &evt){
    stateTransitioning = false;
    this->state = ((ofxAnimationInstance*)evt.who)->getStateID();
}

void ofxLayoutElement::setState(string state){
    if(animationStates.count(state) > 0){
        animationStates[state]->reset();
        animationStates[state]->play();
        stateTransitioning = true;
        ofAddListener(animationStates[state]->animFinished, this, &ofxLayoutElement::stateTransFinished);
    }
    else{
        this->state = state;
    }
}

void ofxLayoutElement::addAnimationState(string stateName, ofxAnimationInstance* animInst){
    animationStates[stateName] = animInst;
}

bool ofxLayoutElement::hasAnimations(){
    return animationStates.size() > 0;
}

void ofxLayoutElement::setOpacity(float opacity){
    this->opacity = opacity;
}

float ofxLayoutElement::getOpacity(){
    return opacity;
}

void ofxLayoutElement::setBorders(float borderWidth){
    borders.top = borderWidth;
    borders.right = borderWidth;
    borders.bottom = borderWidth;
    borders.left = borderWidth;
}

void ofxLayoutElement::setBorders(SideDimensions borders){
    this->borders = borders;
}

SideDimensions ofxLayoutElement::getBorders(){
    return borders;
}