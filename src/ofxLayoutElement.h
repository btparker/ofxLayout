#pragma once
#include "ofMain.h"
#include "ofxOSS.h"
#include "ofxXmlSettings.h"
#include "ofxProgressiveTextureLoad.h"
#include "ofxLoaderSpool.h"
#include "ofxMask.h"

namespace TAG {
    enum ENUM{BODY, ELEMENT, TEXT, INVALID};
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
    
    void setAssets(ofxLoaderSpool* assetsPtr);
    void setFonts(map<string, ofxFontStash>* fontsPtr);
    void setData(map<string,string>* dataPtr);
    
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
    string getInlineStyle();
    ofxOSS getInlineStyles();
    void setInlineStyle(string style);
    
    void overrideStyles(ofxOSS* styleObject);
    void addChild(ofxLayoutElement* child);
    
    ofxOSS styles;
    
    vector<ofxLayoutElement*> children;
    ofxLayoutElement* parent;
    
    ofFbo* getFbo();
    
    void pushTransforms();
    void popTransforms();
    
    void drawBackgroundImage();
    void drawBackgroundVideo();
    void drawBackgroundColor();
    
    bool beginBackgroundBlendMode();
    void endBackgroundBlendMode();
    
    void setLayout(ofxLayout* layout);

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
    
    map<string,string>* dataPtr;
    ofxLoaderSpool* assetsPtr;
    map<string, ofxFontStash>* fontsPtr;
    
    ofVideoPlayer* video;
};