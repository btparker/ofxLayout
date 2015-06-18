#include "ofxLayoutElement.h"
#include "ofxLayout.h"
#ifdef USE_OFX_TIME_MEASUREMENTS
  #include "ofxTimeMeasurements.h"
#endif


/// |   Constructor/Destructor   | ///
/// | -------------------------- | ///
ofxLayoutElement::ofxLayoutElement(){
    opacity = 1.0;
    path = NULL;
    pathFillHack = false;
    isSVG = false;
    parent = NULL;
    video = NULL;
    position = ofPoint(0,0);
    dimensions = ofRectangle(0,0,0,0);
    tag = TAG::DIV;
    level = 0;
    styles.setDefaults();
    mouseState = MOUSE_STATE::NONE;
    stateLocked = false;
    overlayFbo = NULL;
#define _S(a) #a
    string shader_src = _S(
                           uniform float opacity;
                           uniform sampler2DRect tex0;
                           void main(){
                               gl_FragColor = texture2DRect(tex0, gl_TexCoord[0].st);
                               gl_FragColor.a = texture2DRect(tex0, gl_TexCoord[0].st).a;
                               gl_FragColor.rgb /= gl_FragColor.a;
                               gl_FragColor.a *= min(1.0,opacity);
                           }
                           );
    
#undef _S
    blendShader.setupShaderFromSource(GL_FRAGMENT_SHADER, shader_src);
    blendShader.linkProgram();
}

ofxLayoutElement::~ofxLayoutElement(){
    if(path){
        path->clear();
    }
    
    if(video != NULL){
#ifdef USE_OFX_TIME_MEASUREMENTS
        TS_START("ofxLayout close video");
#endif
        video->close();
        delete video;
        video = NULL;
#ifdef USE_OFX_TIME_MEASUREMENTS
        TS_STOP("ofxLayout close video");
#endif
    }
    if(overlayFbo){
        overlayFbo->clear();
        delete overlayFbo;
    }
    
    for(auto state : states){
        for(ofxAnimationInstance* anim : states[state.first]){
            layout->am.removeAnimationInstance(anim->getID());
        }
        states[state.first].clear();
    }
    
    for(int i = 0 ; i < childElements.size(); i++){
        if(childElements[i]){
            delete childElements[i];
            childElements[i] = NULL;
        }
    }
    
    
}

void ofxLayoutElement::mouseMoved(ofMouseEventArgs &args){
    mouseMovedPt = getLocalPoint(args);
    string evtStr = "mouseMoved";
    ofNotifyEvent(mouseMovedEvt, evtStr, this);
}

void ofxLayoutElement::mouseReleased(ofMouseEventArgs &args){
    mouseDraggedPt.set(ofPoint());
    mouseReleasedPt = getLocalPoint(args);
    string evtStr = "mouseReleased";
    ofNotifyEvent(mouseReleasedEvt, evtStr, this);
}

void ofxLayoutElement::fingerReleased(ofPoint _finger)
{
    mouseDraggedPt.set(ofPoint());
    mouseReleasedPt = getLocalPoint(_finger);
    string evtStr = "mouseReleased";
    ofNotifyEvent(mouseReleasedEvt, evtStr, this);
}


void ofxLayoutElement::mousePressed(ofMouseEventArgs &args){
    mousePressedPt = getLocalPoint(args);
    string evtStr = "mousePressed";
    ofNotifyEvent(mousePressedEvt, evtStr, this);
}

void ofxLayoutElement::fingerPressed(ofPoint _finger)
{
    mousePressedPt = getLocalPoint(_finger);
    string evtStr = "mousePressed";
    ofNotifyEvent(mousePressedEvt, evtStr, this);
}


void ofxLayoutElement::mouseDragged(ofMouseEventArgs &args){
    mouseDraggedPt = getLocalPoint(args);
    string evtStr = "mouseDragged";
    ofNotifyEvent(mouseDraggedEvt, evtStr, this);
}

void ofxLayoutElement::fingerDragged(ofPoint _finger){
    mouseDraggedPt = getLocalPoint(_finger);
    string evtStr = "mouseDragged";
    ofNotifyEvent(mouseDraggedEvt, evtStr, this);
    mouseMovedPt = getLocalPoint(_finger);
    string evtStr2 = "mouseMoved";
    ofNotifyEvent(mouseMovedEvt, evtStr2, this);
    
}

ofPoint ofxLayoutElement::getLocalPoint(ofPoint pt){
    
    ofMatrix4x4 mat = layout->getMouseTransformation();
    ofPoint gp = getGlobalPosition();
    ofPoint lp = pt*mat-gp;
    
    return lp;
}

void ofxLayoutElement::setMouseState(MOUSE_STATE::ENUM mouseState){
    this->mouseState = mouseState;
}

void ofxLayoutElement::setParent(ofxLayoutElement *parent){
    this->parent = parent;
    this->level = parent->getLevel()+1;
    this->layout = parent->layout;
}

ofxLayoutElement* ofxLayoutElement::getParent(){
    return this->parent;
}

MOUSE_STATE::ENUM ofxLayoutElement::getMouseState(){
    return this->mouseState;
}

void ofxLayoutElement::setStyle(OSS_KEY::ENUM key, OSS_VALUE::ENUM value){
    styles.setStyle(key, value);
}

void ofxLayoutElement::setStyle(OSS_KEY::ENUM key, string value){
    if(key == OSS_KEY::BACKGROUND_IMAGE){
        string oldImage = getStringStyle(OSS_KEY::BACKGROUND_IMAGE);
        if(layout->getAssets()->getBatch("images")->hasTexture(oldImage) && value != oldImage){
            layout->getAssets()->getBatch("images")->removeTexture(oldImage);
        }
    }
    styles.setStyle(key, value);
    if(key == OSS_KEY::BACKGROUND_IMAGE){
        layout->updateAssets(this);
    }
}

void ofxLayoutElement::setStyle(OSS_KEY::ENUM key, float value){
    styles.setStyle(key, value);
}

void ofxLayoutElement::setStyle(OSS_KEY::ENUM key, ofColor value){
    styles.setStyle(key, value);
}

void ofxLayoutElement::show(){
    if(!states["show"].empty()){
        setState("show");
    }
    else{
        styles.setStyle(OSS_KEY::DISPLAY, OSS_VALUE::BLOCK);
    }
    
}

void ofxLayoutElement::hide(){
    if(!states["hide"].empty()){
        setState("hide");
    }
    else{
        styles.setStyle(OSS_KEY::DISPLAY, OSS_VALUE::NONE);
    }
}

bool ofxLayoutElement::visible(){
    bool displayNone = hasStyle(OSS_KEY::DISPLAY) && getOssValueStyle(OSS_KEY::DISPLAY) == OSS_VALUE::NONE;
    bool opacityZero = opacity != 0 && hasStyle(OSS_KEY::OPACITY) && getFloatStyle(OSS_KEY::OPACITY) == 0.0f;
    return !displayNone;
}

/// |   Cycle Functions  | ///
/// | ------------------ | ///

void ofxLayoutElement::update(float dt){
    
    globalTransformations = ofMatrix4x4::newIdentityMatrix();
    // If root element, boundary is initially set to the current viewport dimensions
    if(this->getTag() == TAG::BODY){
        setPosition(layout->getPosition());
        setDimensions(layout->getWidth(),layout->getHeight());
        this->getStyle(OSS_KEY::WIDTH)->setValue("100%");
        this->getStyle(OSS_KEY::HEIGHT)->setValue("100%");
    }
    
    // *** COMPUTE WIDTH *** //
    // Percent width
    if(hasParent() && getStyle(OSS_KEY::WIDTH)->getType() == OSS_TYPE::PERCENT){
        float percentWidth = getStyle(OSS_KEY::WIDTH)->asFloat()/100.0f;
        dimensions.width = percentWidth * parent->getBoundary().getWidth();
    }
    // Fixed size (px)
    else if (getStyle(OSS_KEY::WIDTH)->getType() == OSS_TYPE::NUMBER){
        dimensions.width = getStyle(OSS_KEY::WIDTH)->asFloat();
    }
    
    // *** COMPUTING CHILDREN *** //
    
    float childrenHeight;
    
    
    float relX = 0;
    float relY = 0;
    float childRowHeight = 0;
    
    bool isWidthAuto = getStyle(OSS_KEY::WIDTH)->asOssValue() == OSS_VALUE::AUTO;
    bool isHeightAuto = getStyle(OSS_KEY::HEIGHT)->asOssValue() == OSS_VALUE::AUTO;
    
    float minWidth = 0;
    if(hasStyle(OSS_KEY::MIN_WIDTH)){
        minWidth = getStyle(OSS_KEY::MIN_WIDTH)->asFloat();
        dimensions.width = max(dimensions.width, minWidth);
    }
    
    if(hasStyle(OSS_KEY::BACKGROUND_IMAGE) && isWidthAuto){
        if(getBackgroundImageTexture()){
            dimensions.width = max(getBackgroundImageTexture()->getWidth(), minWidth);
        }
        else if(isSVG){
            dimensions.width = max(svg.getWidth(), minWidth);
        }
    }
    
    if(hasStyle(OSS_KEY::BACKGROUND_IMAGE) && isHeightAuto){
        if(getBackgroundImageTexture()){
            dimensions.height = getBackgroundImageTexture()->getHeight();
        }
        else if(isSVG){
            dimensions.height = svg.getHeight();
        }
    }
    
    updateText();
    
    if(isWidthAuto && hasStyle(OSS_KEY::FONT_FAMILY)){
        minWidth = max(minWidth,fontData.drawBox.width);
    }
    
    float maxWidth = isWidthAuto ? parent->getBoundary().getWidth() : dimensions.width;
    if(hasStyle(OSS_KEY::MAX_WIDTH)){
        maxWidth = getStyle(OSS_KEY::MAX_WIDTH)->asFloat();
        dimensions.width = max(dimensions.width, maxWidth);
    }
    
    float expandingWidth = minWidth;
    float maxExpandedWidth = expandingWidth;
    
    for(ofxLayoutElement* child : childElements){
        child->update(dt);
        float cW = child->getDimensions().getWidth();
        float cH = child->getDimensions().getHeight();
        float mL = child->getFloatStyle(OSS_KEY::MARGIN_LEFT);
        float mR = child->getFloatStyle(OSS_KEY::MARGIN_RIGHT);
        float mT = child->getFloatStyle(OSS_KEY::MARGIN_TOP);
        float mB = child->getFloatStyle(OSS_KEY::MARGIN_BOTTOM);
        
        float bW = cW + mL + mR;
        float bH = cH + mT + mB;
        
        // Expanding div to contain children
        if((isWidthAuto && (relX+bW) <= maxWidth)){
            expandingWidth += bW;
        }
        else if(relX+bW > maxWidth){
            relX = 0;
            relY += childRowHeight;
            childRowHeight = 0;
            if(isWidthAuto){
                expandingWidth = 0;
            }
        }
        
        maxExpandedWidth = max(maxExpandedWidth, bW);
        maxExpandedWidth = max(expandingWidth,maxExpandedWidth);
        
        // Setting child position
        ofPoint childPos = ofPoint(relX+mL,relY+mT);
        child->setPosition(childPos);
        child->setDimensions(cW, cH);
        
        relX += bW;
        childRowHeight =  bH > childRowHeight ? bH : childRowHeight;
    }
    
    if(isWidthAuto){
        dimensions.width = max(maxExpandedWidth,dimensions.width);
    }
    
    // Only variable to escape this scope
    childrenHeight = relY+childRowHeight;
    
    
    // *** COMPUTE HEIGHT *** //
    if(isHeightAuto){
        if(hasStyle(OSS_KEY::FONT_FAMILY)){
            dimensions.height = fontData.drawBox.getHeight();
        }
        else if(!childElements.empty()){
            dimensions.height = childrenHeight;
        }
        
    }
    else if(hasParent() && getStyle(OSS_KEY::HEIGHT)->getType() == OSS_TYPE::PERCENT){
        float percentHeight = getStyle(OSS_KEY::HEIGHT)->asFloat()/100.0f;
        dimensions.height = percentHeight * parent->getBoundary().getHeight();
    }
    // Fixed size (px)
    else if(getStyle(OSS_KEY::HEIGHT)->getType() == OSS_TYPE::NUMBER){
        dimensions.height = getStyle(OSS_KEY::HEIGHT)->asFloat();
    }
    
    if(hasStyle(OSS_KEY::MAX_HEIGHT)){
        dimensions.height = min(dimensions.height, getFloatStyle(OSS_KEY::MAX_HEIGHT));
    }
    
    // Positioning
    for(ofxLayoutElement* child : childElements){
        ofPoint relativePos = child->getPosition();
        
        ofPoint offset = ofPoint(0,0);

        // If positioning type is fixed, need to compute offset to the viewport, not parent element
        ofRectangle containingDimensions;
        if(getStyle(OSS_KEY::POSITION)->asOssValue() == OSS_VALUE::FIXED){
            containingDimensions.setPosition(layout->getPosition());
            containingDimensions = layout->getBody()->getDimensions();
        }
        else{
            containingDimensions = getDimensions();
        }
        
        if(child->getStyle(OSS_KEY::TOP)->getType() == OSS_TYPE::PERCENT){
            float percentTop = child->getStyle(OSS_KEY::TOP)->asFloat()/100.0f;
            offset.y = percentTop * containingDimensions.getHeight();
        }
        // Fixed size (px)
        else if(child->getStyle(OSS_KEY::TOP)->getType() == OSS_TYPE::NUMBER){
            offset.y = child->getStyle(OSS_KEY::TOP)->asFloat();
        }
        if(child->hasStyle(OSS_KEY::BOTTOM)){
            if(child->getStyle(OSS_KEY::BOTTOM)->getType() == OSS_TYPE::PERCENT){
                // Inverse
                float percentTop = 1.0f-(child->getStyle(OSS_KEY::BOTTOM)->asFloat()/100.0f);
                offset.y = percentTop * containingDimensions.getHeight();
            }
            // Fixed size (px)
            else if(child->getStyle(OSS_KEY::BOTTOM)->getType() == OSS_TYPE::NUMBER){
                
                offset.y = containingDimensions.getHeight() - child->getHeight() - child->getStyle(OSS_KEY::BOTTOM)->asFloat();
            }
        }
        
        
        if(child->getStyle(OSS_KEY::LEFT)->getType() == OSS_TYPE::PERCENT){
            float percentLeft = child->getStyle(OSS_KEY::LEFT)->asFloat()/100.0f;
            offset.x = percentLeft * containingDimensions.getWidth();
        }
        // Fixed size (px)
        else if(child->getStyle(OSS_KEY::LEFT)->getType() == OSS_TYPE::NUMBER){
            offset.x = child->getStyle(OSS_KEY::LEFT)->asFloat();
        }
        if(child->hasStyle(OSS_KEY::RIGHT)){
            if(child->getStyle(OSS_KEY::RIGHT)->getType() == OSS_TYPE::PERCENT){
                // Inverse
                float percentTop = 1.0f-(child->getStyle(OSS_KEY::RIGHT)->asFloat()/100.0f);
                offset.x = percentTop * containingDimensions.getWidth();
            }
            // Fixed size (px)
            else if(child->getStyle(OSS_KEY::RIGHT)->getType() == OSS_TYPE::NUMBER){
                offset.x = containingDimensions.getWidth() - child->getWidth() - child->getStyle(OSS_KEY::RIGHT)->asFloat();
            }
        }
        
        
        if(child->getOssValueStyle(OSS_KEY::MARGIN_LEFT) == OSS_VALUE::AUTO){
            offset.x = dimensions.width/2.0-child->getDimensions().getWidth()/2.0;
        }
        if(child->getOssValueStyle(OSS_KEY::MARGIN_TOP) == OSS_VALUE::AUTO){
            offset.y = dimensions.height/2.0-child->getDimensions().getHeight()/2.0;
        }
        
        ofPoint childPos = offset;
        
        switch(child->getStyle(OSS_KEY::POSITION)->asOssValue()){
            case OSS_VALUE::RELATIVE:
                childPos = relativePos+offset;
                break;
            case OSS_VALUE::ABSOLUTE:
                childPos = offset;
                break;
            case OSS_VALUE::FIXED:
                // Note that offset is computed differently for fixed
                childPos = offset - getGlobalPosition() + layout->getPosition();
                break;
            case OSS_VALUE::STATIC:
            default:
                childPos = relativePos+offset;
        }
        
        child->setPosition(childPos);
    }
    if(hasStyle(OSS_KEY::BACKGROUND_VIDEO)){
        string videoPath = getStringStyle(OSS_KEY::BACKGROUND_VIDEO);
        
        if(video == NULL){
#ifdef USE_OFX_TIME_MEASUREMENTS
            TS_START("ofxLayout open video");
#endif
            video = new ofxHapPlayer();
            video->loadAsync(videoPath);
            video->setVolume(0.0f);
            video->setLoopState(OF_LOOP_NORMAL);
            video->play();
            video->setPaused(true);
#ifdef USE_OFX_TIME_MEASUREMENTS
            TS_STOP("ofxLayout open video");
#endif
        }
        else{
            video->update();
        }
        
    }
}

void ofxLayoutElement::addChild(ofxLayoutElement* child){
    child->setParent(this);
    if(!child->getID().empty()){
        this->layout->idElementMap[child->getID()] = child;
    }
    for(string className : ofSplitString(child->getClasses()," ")){
        this->layout->classElementMap[className].insert(child);
    }
    childElements.push_back(child);
    layout->applyStyles(child);
    layout->applyAnimations();
    child->setState("default");
}

void ofxLayoutElement::draw(ofFbo* fbo){
    if(fbo){
        fbo->begin();
    }
    if(visible()){
        ofPushStyle();
        ofPushMatrix();
        
        updateGlobalTransformations();
        
        ofTranslate(getPosition());
        if(hasStyle(OSS_KEY::SCALE)){
            ofPushMatrix();
            float scale = getFloatStyle(OSS_KEY::SCALE);
            float bgTransX = getFloatStyle(OSS_KEY::ORIGIN_X);
            float bgTransY = getFloatStyle(OSS_KEY::ORIGIN_Y);
            ofTranslate((1-scale)*dimensions.width*bgTransX, (1-scale)*dimensions.height*bgTransY);
            ofScale(scale,scale, 1.0);
        }
        if(hasStyle(OSS_KEY::BORDER_WIDTH)){
            setBorders(getFloatStyle(OSS_KEY::BORDER_WIDTH));
        }
        
        if(hasStyle(OSS_KEY::OSS_OVERFLOW) && getOssValueStyle(OSS_KEY::OSS_OVERFLOW) == OSS_VALUE::HIDDEN){
            glPushAttrib(GL_SCISSOR_BIT);
            glEnable(GL_SCISSOR_TEST);
            ofRectangle glScissorRect = getGlobalClippingRegion();
            ofRectangle viewport = layout->getBody()->getGlobalClippingRegion();
            glScissor(glScissorRect.getX(), glScissorRect.getY(), glScissorRect.width, glScissorRect.height);
        }

        if(hasStyle(OSS_KEY::OPACITY)){
            opacity *= getStyle(OSS_KEY::OPACITY)->asFloat();
        }

#ifndef OFXLAYOUT_DISABLE_MULTIBLUR
		bool isBlurring = hasStyle(OSS_KEY::BLUR) && getFloatStyle(OSS_KEY::BLUR) > 0 && layout->mFboBlur.isAllocated();
#else
		bool isBlurring = false;
#endif
        if(isBlurring){
            layout->mFboBlur.setBlurOffset(getFloatStyle(OSS_KEY::BLUR));
            layout->mFboBlur.beginDrawScene();
            ofClear(0.0f, 0.0f, 0.0f, 0.0f);
        }
        
        drawContent();
        
        if(isBlurring){
            layout->mFboBlur.endDrawScene();
            glPushAttrib(GL_BLEND);
            glDisable(GL_BLEND);
            layout->mFboBlur.performBlur();
            ofSetColor(ofColor::white);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            layout->mFboBlur.drawBlurFbo();
            glDisable(GL_BLEND);
            glPopAttrib();
        }
        
        
        
        ofPopStyle();
        
        for(ofxLayoutElement* child : childElements){
            child->setOpacity(opacity);
            child->draw();
        }
        
        
        if(overlayFbo && overlayFbo->isAllocated()){
            ofPushStyle();
            glPushAttrib(GL_BLEND);
            glEnable(GL_BLEND);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            ofSetColor(255*opacity);
            overlayFbo->draw(0,0);
            glDisable(GL_BLEND);
            glPopAttrib();
            ofPopStyle();
        }
        
        if(hasStyle(OSS_KEY::OSS_OVERFLOW) && getOssValueStyle(OSS_KEY::OSS_OVERFLOW)== OSS_VALUE::HIDDEN){
            glDisable(GL_SCISSOR_TEST);
            glPopAttrib();
        }
        
        if(hasStyle(OSS_KEY::SCALE)){
            ofPopMatrix();
        }
        ofPopMatrix();
    }
    if(fbo){
        fbo->end();
    }
}

void ofxLayoutElement::drawContent(){
    ofSetColor(min(255,int(255*opacity)));
    drawBorder();
    drawBackground();
    drawPath();
    drawText();
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

void ofxLayoutElement::addClass(string className){
    this->classes += " "+className;
}

string ofxLayoutElement::getID(){
    return this->ID;
}

void ofxLayoutElement::setID(string ID){
    this->ID = ID;
}

bool ofxLayoutElement::hasStyle(OSS_KEY::ENUM styleKey){
    return styles.hasStyle(styleKey);
}

ofxOssRule* ofxLayoutElement::getStyle(OSS_KEY::ENUM styleKey){
    return styles.getStyle(styleKey);
}

string ofxLayoutElement::getStringStyle(OSS_KEY::ENUM styleKey){
    if(hasStyle(styleKey)){
        return getStyle(styleKey)->asString();
    }
    else{
        return "";
    }
}

float ofxLayoutElement::getFloatStyle(OSS_KEY::ENUM styleKey){
    if(hasStyle(styleKey)){
        return getStyle(styleKey)->asFloat();
    }
    else{
        return 0.0;
    }
    
}

ofColor ofxLayoutElement::getColorStyle(OSS_KEY::ENUM styleKey){
    if(hasStyle(styleKey)){
        return getStyle(styleKey)->asColor();
    }
    else{
        return ofColor::black;
    }
}

OSS_VALUE::ENUM ofxLayoutElement::getOssValueStyle(OSS_KEY::ENUM styleKey){
    if(hasStyle(styleKey)){
        return getStyle(styleKey)->asOssValue();
    }
    else{
        return OSS_VALUE::INVALID;
    }
}

/// |   Utilities   | ///
/// | ------------- | ///
void ofxLayoutElement::drawPath(){
    if(path != NULL){
        ofPushStyle();
        ofColor fill;
        ofColor stroke;

        if(hasStyle(OSS_KEY::FILL)){
            fill = getColorStyle(OSS_KEY::FILL);    
            fill.a *= opacity;
            
            ofFill();
            path->setFilled(true);
            path->setFillColor(fill);
        }

        if(hasStyle(OSS_KEY::STROKE)){
            stroke = getColorStyle(OSS_KEY::STROKE);
            stroke.a *= opacity;
            
            ofNoFill();
            path->setFilled(false);
            path->setStrokeColor(stroke);
        }

        if(hasStyle(OSS_KEY::STROKE_MITERLIMIT)){
            path->setStrokeWidth(getFloatStyle(OSS_KEY::STROKE_MITERLIMIT));
        }

        ofSetColor(255,255, 255, 255);
        
        path->setCurveResolution(100);
        if(pathFillHack){
            if(hasStyle(OSS_KEY::STROKE)){
                ofFill();
                float radius = getFloatStyle(OSS_KEY::STROKE_MITERLIMIT);
                for(ofPolyline& poly : path->getOutline()){
                    int numSamples = 0.5*round(poly.getPerimeter());
                    if(numSamples == 0){
                        break;
                    }
                    float startDist = INFINITY;
                    float endDist = INFINITY;
                    for(int i = 0; i <= numSamples; i++){
                        float percent = (1.0f*i)/(1.0f*numSamples);
                        if(i == 0){
                            percent = FLT_EPSILON;
                        }
                        else if(i == numSamples){
                            percent = 1.0-FLT_EPSILON;
                        }
                        float findex = poly.getIndexAtPercent(percent);
                        ofPoint pt = poly.getPointAtIndexInterpolated(findex);
                        if(pt.distance(startPt) < startDist){
                            startDist = pt.distance(startPt);
                            pathStartP = percent;
                        }
                        if(pt.distance(endPt) < endDist){
                            endDist = pt.distance(endPt);
                            pathEndP = percent;
                        }
                    }
                    
                    float pDiff = pathEndP - pathStartP;
                    mesh.clear();
                    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
                    for(int j = 0; j < numSamples; j++){
                        float percentC = (float(j))/(float(numSamples));
                        
                        if(percentC > pathPercent){
                            break;
                        }
                        
                        float findexC = pathStartP+percentC*pDiff;

                        ofPoint cpt = poly.getPointAtPercent(findexC);
                        
                        float findex = poly.getIndexAtPercent(findexC);
                        
                        ofPoint cnPtA = cpt+radius*poly.getNormalAtIndexInterpolated(findex);
                        
                        ofPoint cnPtB = cpt-radius*poly.getNormalAtIndexInterpolated(findex);
                        
                        mesh.addVertex(cnPtA);
                        mesh.addVertex(cnPtB);
                    }
                    
                    ofSetColor(stroke);
                    mesh.draw();

                }
            }
        }
        else{
            path->draw();
        }
        
        ofPopStyle();
    }
}


void ofxLayoutElement::drawBorder(){
    float bw = 0;
    float br = 0;
    ofColor borderColor;
    
    if(hasStyle(OSS_KEY::BORDER_WIDTH)){
        bw = getFloatStyle(OSS_KEY::BORDER_WIDTH);
    }
    
    if(hasStyle(OSS_KEY::BORDER_RADIUS)){
        br = getFloatStyle(OSS_KEY::BORDER_RADIUS);
    }
    
    ofRectangle rect = getGlobalClippingRegion();
    
    ofPushStyle();
    if(hasStyle(OSS_KEY::BORDER_COLOR)){
        borderColor = getColorStyle(OSS_KEY::BORDER_COLOR);
        borderColor.a *= opacity;
        
        ofSetColor(borderColor);
    }
    ofFill();
    ofDrawRectangle( rect.x, rect.y,bw,rect.height);
    ofDrawRectangle( rect.x+rect.width-bw, rect.y,bw,rect.height);
    ofDrawRectangle( rect.x+bw, rect.y,rect.width-2*bw,bw);
    ofDrawRectangle( rect.x+bw, rect.y+rect.height-bw,rect.width-2*bw,bw);
    ofPopStyle();
    
}

void ofxLayoutElement::drawBackground(){
    ofPushStyle();
    if(hasStyle(OSS_KEY::BACKGROUND_SIZE)){
        beginBackgroundSize();
    }
    drawBackgroundImage();
    drawBackgroundVideo();
    if(hasStyle(OSS_KEY::BACKGROUND_SIZE)){
        endBackgroundSize();
    }
    beginBackgroundBlendMode();
    drawBackgroundColor();
    endBackgroundBlendMode();
    
    ofPopStyle();
}

void ofxLayoutElement::beginBackgroundSize(){
    float bgWidth, bgHeight;
    if(getBackgroundImageTexture()){
        bgWidth = getBackgroundImageTexture()->getWidth();
        bgHeight = getBackgroundImageTexture()->getHeight();
    }
    else if(isSVG){
        bgWidth = svg.getWidth();
        bgHeight = svg.getHeight();
    }
	else if (video != NULL) {
		bgWidth = video->getWidth();
		bgHeight = video->getHeight();
	}
    
    float wRatio = bgWidth/getWidth();
    float hRatio = bgHeight/getHeight();
    
    float scale;
    
    if(getOssValueStyle(OSS_KEY::BACKGROUND_SIZE) == OSS_VALUE::COVER){
        scale = 1.0/min(wRatio,hRatio);
    }
    else if(getOssValueStyle(OSS_KEY::BACKGROUND_SIZE) == OSS_VALUE::CONTAIN){
        scale = 1.0/max(wRatio,hRatio);
    }
    else{
        scale = 1.0;
    }
    
    ofPushMatrix();
    float xTrans = (getWidth()-bgWidth*scale)/2;
    float yTrans = (getHeight()-bgHeight*scale)/2;
    ofTranslate(xTrans, yTrans);
    ofScale(scale, scale, 1.0);
}

void ofxLayoutElement::endBackgroundSize(){
    ofPopMatrix();
}

void ofxLayoutElement::updateText(){
    if(hasStyle(OSS_KEY::FONT_FAMILY)){
        
        // FONTNAME
        {
            fontData.fontFilename = getStringStyle(OSS_KEY::FONT_FAMILY);
            if(layout->getFonts()->count(fontData.fontFilename) == 0){
                fontData.fontFilename.clear();
                return;
            }
        }
        
        //FONT TEXT
        {
            fontData.text = getValue();
            if(hasStyle(OSS_KEY::TEXT_TRANSFORM)){
                vector<string> words = ofSplitString(fontData.text, " ",true,true);
                OSS_VALUE::ENUM textTransform = getOssValueStyle(OSS_KEY::TEXT_TRANSFORM);
                switch(textTransform){
                    case OSS_VALUE::UPPERCASE:
                        fontData.text = ofToUpper(fontData.text);
                        break;
                    case OSS_VALUE::LOWERCASE:
                        fontData.text = ofToLower(fontData.text);
                        break;
                    case OSS_VALUE::CAPITALIZE:
                        for(int i = 0; i < words.size(); i++) {
                            words[i] = ofToLower(words[i]);
                            words[i] = ofToUpper(words[i].substr(0,1))+words[i].substr(1,words[i].size());
                        }
                        fontData.text = ofJoinString(words, " ");
                        break;
                    default:
                        fontData.text = fontData.text;
                }
            }
        }
        
        // FONT SIZE
        {
            int fontSize = 0;
            bool fitText = false;
            string textToFit = fontData.text;
            if(hasStyle(OSS_KEY::TEXT_BREAKPOINTS)){
                vector<string> breakpoints = ofSplitString(getStringStyle(OSS_KEY::TEXT_BREAKPOINTS), " ");
                for(int i = 0; i < breakpoints.size(); i++){
                    vector<string> bp = ofSplitString(breakpoints[i], "/");
                    int cc = ofToInt(bp[0]);
                    int fs = ofToInt(bp[1]);
                    if(fontData.text.size() >= cc){
                        fontSize = fs;
                    }
                }
            }
            else if(getOssValueStyle(OSS_KEY::FONT_SIZE) == OSS_VALUE::FIT){
                fontSize = 5.0;
                fitText = true;
            }
            else if(getOssValueStyle(OSS_KEY::FONT_SIZE) == OSS_VALUE::FIT_WORD){
                fontSize = 5.0;
                fitText = true;
                textToFit = "";
                for(string word : ofSplitString(fontData.text, " ")){
                    if(word.length() > textToFit.length()){
                        textToFit = word;
                    }
                }
            }
            else if(hasStyle(OSS_KEY::FONT_SIZE)){
                fontSize = getFloatStyle(OSS_KEY::FONT_SIZE);
            }
            
            if(fitText){
                ofRectangle fontBBox = layout->getFonts()->at(fontData.fontFilename)->getBBox(textToFit,fontSize,0,0);
                fontSize *= (int)(0.95*(dimensions.width)/(fontBBox.width+1));
            }
            
            if(hasStyle(OSS_KEY::FONT_SIZE_MIN)){
                fontSize = max(getFloatStyle(OSS_KEY::FONT_SIZE_MIN),(float)fontSize);
            }
            
            if(hasStyle(OSS_KEY::FONT_SIZE_MAX)){
                fontSize = min(getFloatStyle(OSS_KEY::FONT_SIZE_MAX),(float)fontSize);
            }
            fontData.fontSize = fontSize;
        }
        
        // LETTER SPACING
        {
            layout->getFonts()->at(fontData.fontFilename)->setCharacterSpacing(getFloatStyle(OSS_KEY::LETTER_SPACING));
        }
        
        
        
        // AUTO WIDTH
        if(getOssValueStyle(OSS_KEY::WIDTH) == OSS_VALUE::AUTO){
            dimensions.width = fontData.drawBox.width;
        }
    }
}


void ofxLayoutElement::drawText(){
    if(hasStyle(OSS_KEY::FONT_FAMILY)){
        string fontFilename = fontData.fontFilename;
        if(layout->getFonts()->count(fontFilename) > 0){
            string text = fontData.text;
            int fontSize = fontData.fontSize;
            float textMaxWidth = layout->getWidth();
            if(hasStyle(OSS_KEY::TEXT_MAX_WIDTH)){
                textMaxWidth = getFloatStyle(OSS_KEY::TEXT_MAX_WIDTH);
            }
            
            int numLines;
            
            fontData.fontHeight = layout->getFonts()->at(fontFilename)->getBBox("A", fontSize, 0, 0).height;
            float x = 0.0f;
            float y = 0.0f;
            
            
            TextAlign::ENUM ta = TextAlign::LEFT;
            
            if(hasStyle(OSS_KEY::TEXT_ALIGN)){
                string textAlign = getStringStyle(OSS_KEY::TEXT_ALIGN);
                if(textAlign == "left"){
                    x = 0.0f;
                }
                else if(textAlign == "center"){
                    x = dimensions.width/2-fontData.drawBox.width/2;
                    ta = TextAlign::MIDDLE;
                }
                else if(textAlign == "right"){
                    x = dimensions.width - fontData.drawBox.width*1.01f;
                    ta = TextAlign::RIGHT;
                }
            }
            
            if(hasStyle(OSS_KEY::VERTICAL_ALIGN)){
                string verticalAlign = getStringStyle(OSS_KEY::VERTICAL_ALIGN);
                
                if(verticalAlign == "top"){
                    y = 0;
                }
                else if(verticalAlign == "center"){
                    y = dimensions.height/2-fontData.drawBox.height/2;
                }
                else if(verticalAlign == "bottom"){
                    y = dimensions.height-fontData.drawBox.height*1.05f;
                }
            }
            ofFill();
            if(hasStyle(OSS_KEY::COLOR)){
                string colorStr = getStringStyle(OSS_KEY::COLOR);
                ofColor fontColor = ofxOSS::parseColor(colorStr);
                fontColor.a *= opacity;
                ofSetColor(fontColor);
            }
            
            // LINE HEIGHT
            {
                float lh = getFloatStyle(OSS_KEY::LINE_HEIGHT);
                layout->getFonts()->at(fontData.fontFilename)->setLineHeight(lh);
            }
            
            layout->getFonts()->at(fontFilename)->setKerning(true);
            glPushAttrib(GL_BLEND);
            glEnable(GL_BLEND);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            fontData.drawBox = layout->getFonts()->at(fontFilename)->drawMultiLineColumn(fontData.text, fontData.fontSize, x, y+fontData.fontHeight, textMaxWidth,numLines, false, 0, true, ta);
            glDisable(GL_BLEND);
            glPopAttrib();
        }
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

void ofxLayoutElement::beginOverlay(){
    if(overlayFbo == NULL || !overlayFbo->isAllocated() || overlayFbo->getWidth() != (int)getWidth() || overlayFbo->getHeight() != (int)getHeight()){
        if(overlayFbo != NULL){
            overlayFbo->clear();
            delete overlayFbo;
        }
        
        overlayFbo = new ofFbo();
        
        ofFbo::Settings fboS;
        fboS.width = getWidth();
        fboS.height = getHeight();
        fboS.internalformat = GL_RGBA;
        fboS.minFilter = GL_LINEAR;
        fboS.maxFilter = GL_LINEAR;
        fboS.numSamples = 8;
        fboS.useStencil = true;
        overlayFbo->allocate(fboS);
    }
    overlayFbo->begin();
    ofSetVerticalSync(true);
    ofSetCurveResolution(100);
    ofSetCircleResolution(100);
    ofEnableSmoothing();
    ofEnableAlphaBlending();
    ofEnableArbTex();
    ofClear(0, 0, 0, 0);
}

void ofxLayoutElement::endOverlay(){
    overlayFbo->end();
}

void ofxLayoutElement::endBackgroundBlendMode(){
    if(hasStyle(OSS_KEY::BACKGROUND_BLEND_MODE)){
        ofDisableBlendMode();
    }
}

void ofxLayoutElement::drawBackgroundImage(){
    ofPushStyle();
    if(hasStyle(OSS_KEY::BACKGROUND_IMAGE)){
        // Get filename
        string imageFilename = getStringStyle(OSS_KEY::BACKGROUND_IMAGE);
        ofFile file(ofToDataPath(imageFilename));
        string bgImgExt = ofToLower(file.getExtension());
        if(bgImgExt == "svg" && isSVG){
            svg.draw(opacity);
        }
        else{
            drawBackgroundTexture(getBackgroundImageTexture());
        }
    }
    ofPopStyle();
}

ofTexture* ofxLayoutElement::getBackgroundImageTexture(){
    if(hasStyle(OSS_KEY::BACKGROUND_IMAGE)){
        // Get filename
        string imageFilename = getStringStyle(OSS_KEY::BACKGROUND_IMAGE);
        ofFile file(ofToDataPath(imageFilename));
        string bgImgExt = ofToLower(file.getExtension());
        if(bgImgExt == "svg"){
            return NULL;
        }
        ofxLoaderBatch* imagesBatch = layout->getAssets()->getBatch(IMAGES_BATCH);
        if(ofStringTimesInString(imageFilename, ":") > 0){
            vector<string> ids = ofSplitString(imageFilename,":");
            imagesBatch = imagesBatch->getBatch(ids[0]);
            imageFilename = ids[1];
        }
        if(imagesBatch->hasTexture(imageFilename) && imagesBatch->isTextureReady(imageFilename)){
            return imagesBatch->getTexture(imageFilename);
        }
        
    }
    return NULL;
}

void ofxLayoutElement::drawBackgroundVideo(){
    ofPushStyle();
    if(hasStyle(OSS_KEY::BACKGROUND_VIDEO)){
        ofSetColor(255,255,255,255);
        string videoPath = getStringStyle(OSS_KEY::BACKGROUND_VIDEO);
        
        if(video != NULL){
            drawBackgroundTexture(video->getTexture());
        }
        
    }
    ofPopStyle();
}

void ofxLayoutElement::playBackgroundVideo(){
    if(hasStyle(OSS_KEY::BACKGROUND_VIDEO) && video != NULL){
		video->setPaused(false);
    }
}

void ofxLayoutElement::pauseBackgroundVideo(){
    if(hasStyle(OSS_KEY::BACKGROUND_VIDEO) && video != NULL){
		video->setPaused(true);
    }
}


void ofxLayoutElement::drawBackgroundTexture(ofTexture *texture){
    if(!texture){
        return;
    }
    ofPushStyle();
    texture->setTextureMinMagFilter(GL_LINEAR,GL_LINEAR);
    ofRectangle bgTextureTransform = ofRectangle();
    bgTextureTransform.setWidth(texture->getWidth());
    bgTextureTransform.setHeight(texture->getHeight());
    bgTextureTransform.setX(0);
    bgTextureTransform.setY(0);
    
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
    
    glPushAttrib(GL_BLEND);
    glEnable(GL_BLEND);
    if(getOssValueStyle(OSS_KEY::PREMULTIPLY) == OSS_VALUE::DIVIDE){
        ofSetColor(255, 255, 255, 255*opacity);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else if(getOssValueStyle(OSS_KEY::PREMULTIPLY) == OSS_VALUE::CUSTOM){
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    }
    else{
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        ofSetColor(255, 255, 255,floor(255*opacity));
    }
    
    makeTexCoords(tex_coords_, texture->getTextureData());
    makeVertices(vertices_, texture->getTextureData());
        
    for(int x = 0; x <= numRepeatX; x++){
        for(int y = 0; y <= numRepeatY; y++){
            ofRectangle textureSize(bgX+bgTextureTransform.width*x, bgY+bgTextureTransform.height*y,bgTextureTransform.width, bgTextureTransform.height);
            texture->draw(textureSize);
        }
    }
    
    glDisable(GL_BLEND);
    glPopAttrib();
    ofPopStyle();
}

void ofxLayoutElement::drawBackgroundColor(){
    ofPushStyle();
    if(hasStyle(OSS_KEY::BACKGROUND_COLOR)){
        ofColor color = getColorStyle(OSS_KEY::BACKGROUND_COLOR);
        color.a *= opacity;
        ofSetColor(color);
        ofFill();
        if(getFloatStyle(OSS_KEY::BORDER_RADIUS) > 0.0){
            float r = 0.0;
            if(getStyle(OSS_KEY::BORDER_RADIUS)->getType() == OSS_TYPE::PERCENT){
                r = (getFloatStyle(OSS_KEY::BORDER_RADIUS)/100.0)*min(getWidth(),getHeight());
            }
            else{
                r = getFloatStyle(OSS_KEY::BORDER_RADIUS);
            }
            ofSetCircleResolution(100);
            ofDrawRectRounded(0, 0, getWidth(), getHeight(), r);
        }
        else{
            ofDrawRectangle(0,0,dimensions.width,dimensions.height);
        }
    }
    ofPopStyle();
}

void ofxLayoutElement::overrideStyles(ofxOSS *styleObject){
    for(auto iterator = styleObject->rules.begin(); iterator != styleObject->rules.end(); iterator++){
        OSS_KEY::ENUM key = iterator->first;
        string value = iterator->second->asString();
        setStyle(key, value);
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
    return globalTransformations.getTranslation();
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

ofPath* ofxLayoutElement::getPath(){
    return this->path;
}

void ofxLayoutElement::setPath(ofPath* path, bool pathFillHack, ofPoint startPoint, ofPoint endPoint){
    this->startPt = startPoint;
    this->endPt = endPoint;
    
    this->pathFillHack = pathFillHack;
    this->path = path;
    
    if(pathFillHack){
        pathPercent = 0.0;
    }
}

void ofxLayoutElement::setPathPercent(float percent){
    this->pathPercent = percent;
}

ofFbo* ofxLayoutElement::getFbo(){
    return NULL;
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
    return state;
}

ofEvent<string>* ofxLayoutElement::getStateEvent(){
    return &stateEvt;
}

void ofxLayoutElement::setState(string stateChange, bool recursive, bool reset){
    if((reset || stateChange != this->state) && !isStateLocked()){
        if(states.count(stateChange) > 0){
            for(ofxAnimationInstance* anim : states[stateChange]){
                anim->trigger();
            }
        }
        if(hasStyle(OSS_KEY::BACKGROUND_VIDEO)){
            if(stateChange == PLAY_STATE){
                playBackgroundVideo();
            }
            else if(stateChange == PAUSE_STATE){
                pauseBackgroundVideo();
            }
        }
        this->state = stateChange;
        ofNotifyEvent(stateEvt, stateChange, this);
    }
    
    if(recursive){
        for(ofxLayoutElement* child : childElements){
            child->setState(stateChange, recursive, reset);
        }
    }
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

void ofxLayoutElement::updateGlobalTransformations(){
    if(hasParent()){
        globalTransformations *= parent->globalTransformations;
    }
    globalTransformations.translate(getPosition());
    
    if(hasStyle(OSS_KEY::SCALE)){
        //globalTransformations.scale(getFloatStyle(OSS_KEY::SCALE),getFloatStyle(OSS_KEY::SCALE),1.0);
    }
}


ofRectangle ofxLayoutElement::getGlobalClippingRegion(){
    
    ofRectangle globalClippingRegion = getClippingRegion();
    if(hasParent()){
        globalClippingRegion.translate(parent->getGlobalPosition());
        globalClippingRegion.translate(getPosition());
        
        globalClippingRegion = parent->getGlobalClippingRegion().getIntersection(globalClippingRegion);
    }
    return globalClippingRegion;
}

ofRectangle ofxLayoutElement::getClippingRegion(){
    ofRectangle clippingRegion(0,0,0,0);
    clippingRegion.x = -borders.left;
    clippingRegion.y = -borders.top;
    clippingRegion.width = getWidth() + borders.left+borders.right;
    clippingRegion.height = getHeight() + borders.top+borders.bottom;
    return clippingRegion;
}

bool ofxLayoutElement::isPath(){
    return path != NULL;
}

float ofxLayoutElement::getGlobalScale(){
    return globalTransformations.getScale().x;
}

bool ofxLayoutElement::clickable(){
    return hasStyle(OSS_KEY::POINTER_EVENTS) && getOssValueStyle(OSS_KEY::POINTER_EVENTS) != OSS_VALUE::NONE;
}

void ofxLayoutElement::attachAnimationInstance(ofxAnimationInstance *animationInstance){
    set<string> keys = animationInstance->getKeys();
    for(string key : keys){
        if(ofxOSS::validKey(key)){
            OSS_KEY::ENUM ossKey = ofxOSS::getOssKeyFromString(key);
            if(this->hasStyle(ossKey)){
                ofxOssRule* style = getStyle(ossKey);
                OSS_TYPE::ENUM ossType = style->getType();
                switch (ossType) {
                    case OSS_TYPE::COLOR:
                        animationInstance->setAnimatable(key, style->getAnimatableColor());
                        break;
                    case OSS_TYPE::NUMBER:
                    case OSS_TYPE::PERCENT:
                        animationInstance->setAnimatable(key, style->getAnimatableFloat());
                        break;
                    default:
                        ofLogWarning("ofxLayoutElement::attachAnimationInstance","Can't attach to key '"+key+"', not an animatable type");
                }
            }
        }
    }
}

map<string, vector<ofxAnimationInstance*> >* ofxLayoutElement::getStates(){
    return &states;
}

void ofxLayoutElement::addState(string state, ofxAnimationInstance *animationInstance){
    states[state].push_back(animationInstance);
    attachAnimationInstance(states[state].back());
}

bool ofxLayoutElement::hasState(string state){
    return states.count(state) > 0;
}

void ofxLayoutElement::loadSvg(string imageFilename){
    isSVG = true;
    svg.clear();
    svg.load(imageFilename);
    getStyle(OSS_KEY::BACKGROUND_IMAGE)->setValue(imageFilename);
}

bool ofxLayoutElement::hittest(ofPoint pt){
    return getGlobalClippingRegion().inside(pt);
}

ofPoint ofxLayoutElement::getMouseMoved(){
    return mouseMovedPt;
}
ofPoint ofxLayoutElement::getMouseReleased(){
    return mouseReleasedPt;
}
ofPoint ofxLayoutElement::getMousePressed(){
    return mousePressedPt;
}
ofPoint ofxLayoutElement::getMouseDragged(){
    return mouseDraggedPt;
}

ofxLayout* ofxLayoutElement::getLayout(){
    return layout;
}

void ofxLayoutElement::lockState(bool stateLocked){
    this->stateLocked = stateLocked;
}

bool ofxLayoutElement::isStateLocked(){
    return this->stateLocked;
}

ofxSVG* ofxLayoutElement::getSvg(){
    return &svg;
}

int ofxLayoutElement::getLevel(){
    return level;
}

vector<ofxLayoutElement*> ofxLayoutElement::children(string selector){
    if(selector.empty()){
        return childElements;
    }
    else{
        vector<ofxLayoutElement*> selectedChildren;
        vector<string> selectors = ofSplitString(selector, " ");
        for(ofxLayoutElement* child : childElements){
            for(string& selector : selectors){
                if(selector.substr(0,1) == "#"){
                    string ID = selector.substr(1);
                    if(child->getID() == ID){
                        selectedChildren.push_back(child);
                        break;
                    }
                }
                else if(selector.substr(0,1) == "."){
                    string className = selector.substr(1);
                    bool hasClass = false;
                    for(string childClassName : ofSplitString(child->getClasses(), " ")){
                        if(childClassName == className){
                            hasClass = true;
                            break;
                        }
                    }
                    if(hasClass){
                        selectedChildren.push_back(child);
                        break;
                    }
                }
            }
        }
        return selectedChildren;
    }
}

void ofxLayoutElement::remove(ofxLayoutElement *element){
    for(int i = 0; i < childElements.size(); i++){
        if(childElements[i] == element){
            delete childElements[i];
            childElements.erase(childElements.begin()+i);
        }
    }
}

void ofxLayoutElement::setChildren(vector<ofxLayoutElement *> childs){
    childElements.clear();
    childElements = childs;
}