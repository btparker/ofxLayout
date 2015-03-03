#pragma once
#include "ofMain.h"
#include "ofxOSS.h"
#include "ofxXmlSettings.h"
#include "ofxProgressiveTextureLoad.h"
#include "ofxLoaderSpool.h"
#include "ofxMask.h"

struct SideDimensions{
    float top = 0.0f;
    float right = 0.0f;
    float bottom = 0.0f;
    float left = 0.0f;
};

namespace TAG {
    enum ENUM{BODY, DIV, SVG, G, POLYGON, PATH, CIRCLE, INVALID};
};

class ofxLayout;
class ofxLayoutElement{
    
public:
    /// |   Constructor/Destructor   | ///
    /// | -------------------------- | ///
    
    ofxLayoutElement();
    ~ofxLayoutElement();
    
    
    /// |   Cycle Functions  | ///
    /// | ------------------ | ///
    
    void update();
    void draw();
    
    /// |   Setters/Getters   | ///
    /// | ------------------- | ///
    void setParent(ofxLayoutElement* parent);
    ofxLayoutElement* getParent();
    
    TAG::ENUM getTag();
    void setTag(TAG::ENUM tag);
    void setTag(string tag);
    
    ofPath* getShape();
    
    static string getTagString(TAG::ENUM tagEnum);
    static TAG::ENUM getTagEnum(string tagString);

    string getID();
    void setID(string ID);
    
    string getValue();
    void setValue(string value);
    
    string getClasses();
    void setClasses(string classes);
    
    void setDimensions(float width, float height);
    void setPosition(ofPoint pos);
    ofRectangle getDimensions();
    float getHeight();
    float getWidth();
    ofRectangle getBoundary();
    
    bool hasStyle(OSS_KEY::ENUM styleKey);
    ofxOssRule* getStyle(OSS_KEY::ENUM styleKey);
    string getStringStyle(OSS_KEY::ENUM styleKey);
    float getFloatStyle(OSS_KEY::ENUM styleKey);
    ofColor getColorStyle(OSS_KEY::ENUM styleKey);
    OSS_VALUE::ENUM getOssValueStyle(OSS_KEY::ENUM styleKey);

    string getInlineStyle();
    ofxOSS getInlineStyles();
    void setInlineStyle(string style);
    void appendInlineStyle(string style);
    
    void overrideStyles(ofxOSS* styleObject);
    void copyStyles(ofxOSS* styleObject);
    
    void addChild(ofxLayoutElement* child);
    
    ofxOSS styles;
    
    vector<ofxLayoutElement*> children;
    ofxLayoutElement* parent;
    
    void drawBackground();
    void drawBackgroundImage();
    void drawBackgroundVideo();
    void drawBackgroundColor();
    void drawBackgroundGradient();
    void drawBackgroundTexture(ofTexture* texture);
    void drawText();
    void drawShape();
    
    bool beginBackgroundBlendMode();
    void endBackgroundBlendMode();
    
    void setLayout(ofxLayout* layout);
    
    MOUSE_STATE::ENUM getMouseState();
    void setMouseState(MOUSE_STATE::ENUM mouseState);
    
    void mouseMoved(ofMouseEventArgs& args);
    void mousePressed(ofMouseEventArgs& args);
    void mouseReleased(ofMouseEventArgs& args);
    
    // Display methods
    void show();
    void hide();
    
    bool visible();
    
    ofFbo* getFbo();
    
    bool hasParent();
    
    ofPoint getPosition();
    ofPoint getGlobalPosition();
    float getGlobalScale();
    string getState();
    
//    void setState(string state);

    //    void addAnimationState(string stateName, ofxAnimationInstance* animInst);
    
//    void stateTransFinished(ofxAnimatable::AnimationEvent &evt);
//    bool isStateTransitioning();
    
    void setBorders(float borderWidth);
    void setBorders(SideDimensions borders);
    
    SideDimensions getBorders();
    
    void setOpacity(float opacity);
    float getOpacity();
    void updateGlobalTransformations();
//    bool hasAnimations();
    void drawBorder();
    ofRectangle getGlobalClippingRegion();
    ofRectangle getClippingRegion();
    
    ofPath* initShape();
    bool isShape();
    void drawContent();
    
    void attachAnimationInstance(ofxAnimationInstance* animationInstance);
    
protected:

    ofMatrix4x4 globalTransformations;
//    bool stateTransitioning;
    ofFbo fbo;
    ofxLayout* layout;
//    map<string, ofxAnimationInstance*> animationStates;
    string state;
    
    virtual void drawTag(){};
    ofPoint position;
    
    ofRectangle dimensions;
    SideDimensions borders;
    
    string ID;
    TAG::ENUM tag;
    string classes;
    string elementValue;
    
    float opacity;
    
    string inlineStyle;
    
    MOUSE_STATE::ENUM mouseState;
    
    ofVideoPlayer* video;
    ofPath* shape;
};