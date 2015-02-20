#pragma once
#include "ofMain.h"
#include "ofxOSS.h"
#include "ofxXmlSettings.h"
#include "ofxProgressiveTextureLoad.h"
#include "ofxLoaderSpool.h"
#include "ofxMask.h"

namespace TAG {
    enum ENUM{BODY, ELEMENT, SVG, G, POLYGON, PATH, INVALID};
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
    
    void setShape(ofPath shape);
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

protected:
    ofFbo fbo;
    ofxLayout* layout;
    virtual void drawTag(){};
    ofPoint position;
    
    ofRectangle dimensions;
    
    ofVec4f margins;
    
    string ID;
    TAG::ENUM tag;
    string classes;
    string elementValue;
    
    string inlineStyle;
    
    MOUSE_STATE::ENUM mouseState;
    
    ofVideoPlayer* video;
    ofPath shape;
};