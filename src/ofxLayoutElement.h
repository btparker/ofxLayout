#include "ofMain.h"
#include "ofxOSS.h"
#include "ofxXmlSettings.h"
#include "ofxProgressiveTextureLoad.h"

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

    string getID();
    void setID(string _ID);
    
    string getStyle(OSS_KEY::ENUM styleKey);
    string getStyle(string styleKey);
    
    bool hasStyle(OSS_KEY::ENUM styleKey);
    bool hasStyle(string styleKey);
    
    void setStyle(OSS_KEY::ENUM styleKey, string styleValue);
    void setStyle(string styleKey, string styleValue);

    ofxOSS* getOverridingStylesheet(OSS_KEY::ENUM styleKey);
    ofxOSS* getOverridingStylesheet(string styleKey);
    
    void setStylesheet(ofxOSS* _stylesheet);
    
    /// |   Utilities   | ///
    /// | ------------- | ///
    
    /// \brief Sets this element as the parent to the given pointer of another element (along with parentBoundary and such as well)
    ///
    /// On update/draw of this element, will updat/draw first then update/draw all children.
    ///
    /// \param ofxLayoutElement* Pointer to an element
    void addChildElement(ofxLayoutElement* childElement);

    

protected:
    void drawStyles();
    void updateDimensions();
    void updatePosition();
    void updateBackgroundImage();
    void updateBackgroundVideo();
    
    ofRectangle boundary;
    ofRectangle parentBoundary;
    ofxLayoutElement* parentNode;
    vector<ofxLayoutElement*> childNodes;
    
    ofxOSS elementStyles;
    ofxOSS* stylesheet;
    
    string ID;
    
    ofFbo* elementFbo;
    
    ofTexture * backgroundImage;
    string backgroundImageName;
    bool backgroundImageReadyToDraw;
    ofxProgressiveTextureLoad progressiveTextureLoader;
    
    ofVideoPlayer backgroundVideo;
    bool backgroundVideoReadyToDraw;
    string backgroundVideoName;
    
    void backgroundImageReady(ofxProgressiveTextureLoad::textureEvent& arg);
    void backgroundImageDrawable(ofxProgressiveTextureLoad::textureEvent& arg);
};