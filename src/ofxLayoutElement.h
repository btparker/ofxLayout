#pragma once
#include "ofMain.h"
#include "ofxOSS.h"
#include "ofxXmlSettings.h"
#include "ofxProgressiveTextureLoad.h"
#include "ofxLoaderSpool.h"

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
    string getTag();
    void setTag(string tag);

    string getID();
    void setID(string ID);
    
    string getValue();
    void setValue(string value);
    
    string getClasses();
    void setClasses(string classes);
    
    bool hasStyle(OSS_KEY::ENUM styleKey);
    string getStyle(OSS_KEY::ENUM styleKey);
    string getInlineStyle();
    ofxOSS* getInlineStyles();
    void setInlineStyle(string style);
    
    void overrideStyles(ofxOSS* styleObject);
    void addChild(ofxLayoutElement* childElement);
    
    ofxOSS* styles;
    
    vector<ofxLayoutElement*> children;
    ofxLayoutElement* parent;

protected:
    void drawStyles();
    ofRectangle boundary;
    
    string ID;
    string tag;
    string classes;
    string value;
    
    ofFbo* elementFbo;
    
    string inlineStyle;
    
    ofxLoaderSpool* assetsPtr;
};