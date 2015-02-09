#pragma once
#include "ofMain.h"
#include "ofxOSS.h"
#include "ofxXmlSettings.h"
#include "ofxProgressiveTextureLoad.h"
#include "ofxLoaderSpool.h"
#include "ofxMask.h"

namespace TAG {
    enum ENUM{BODY, ELEMENT, INVALID};
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
    
    static string getTagString(TAG::ENUM tagEnum);
    static TAG::ENUM getTagEnum(string tagString);

    string getID();
    void setID(string ID);
    
    string getValue();
    void setValue(string value);
    
    string getClasses();
    void setClasses(string classes);
    
    ofRectangle getBoundary();
    
    bool hasStyle(OSS_KEY::ENUM styleKey);
    string getStyle(OSS_KEY::ENUM styleKey);
    float getFloatStyle(OSS_KEY::ENUM styleKey);
    ofColor getColorStyle(OSS_KEY::ENUM styleKey);
    OSS_VALUE::ENUM getOssValueStyle(OSS_KEY::ENUM styleKey);

    string getInlineStyle();
    ofxOSS getInlineStyles();
    void setInlineStyle(string style);
    
    void overrideStyles(ofxOSS* styleObject);
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
    
    bool beginBackgroundBlendMode();
    void endBackgroundBlendMode();
    
    void setLayout(ofxLayout* layout);
    
    MOUSE_STATE::ENUM getMouseState();
    void setMouseState(MOUSE_STATE::ENUM mouseState);
    
    void mouseMoved(ofMouseEventArgs& args);
    void mousePressed(ofMouseEventArgs& args);
    void mouseReleased(ofMouseEventArgs& args);

protected:
    ofxLayout* layout;
    void drawStyles();
    virtual void drawTag(){};
    
    ofRectangle boundary;
    
    string ID;
    TAG::ENUM tag;
    string classes;
    string elementValue;
    
    ofxMask elementMask;
    
    string inlineStyle;
    
    MOUSE_STATE::ENUM mouseState;
    
    ofVideoPlayer* video;
};