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
    ofxLayoutElement(ofxLoaderSpool* assetsPtr);
    ~ofxLayoutElement();
    
    
    /// |   Cycle Functions  | ///
    /// | ------------------ | ///
    
    void update();
    void draw();
    
    
    /// |   Setters/Getters   | ///
    /// | ------------------- | ///
    
    string getTag();
    void setTag(string tag);

    string getID();
    void setID(string ID);
    
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
    
    ofFbo* elementFbo;
    
    string inlineStyle;
    
    ofxLoaderSpool* assetsPtr;
};