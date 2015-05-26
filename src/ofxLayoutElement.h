#pragma once
#include "ofMain.h"
#include "ofxOSS.h"
#include "ofxXmlSettings.h"
#include "ofxProgressiveTextureLoad.h"
#include "ofxLoaderSpool.h"
#include "ofxMask.h"
#include "ofxSvg.h"
#include "ofxHapPlayer.h"

struct SideDimensions{
    float top = 0.0f;
    float right = 0.0f;
    float bottom = 0.0f;
    float left = 0.0f;
};

struct FontData{
    string fontFilename;
    string text;
    int fontSize;
    int x;
    int y;
    int fontHeight;
    int textMaxWidth;
    int numLines;
    ofColor color;
    string ta;
    ofRectangle drawBox;
    ofRectangle bounding;
    TextAlign::ENUM textAlign;
};

namespace TAG {
    enum ENUM{BODY, DIV, INVALID};
};

const string PLAY_STATE = "play";
const string PAUSE_STATE = "pause";

class ofxLayout;
class ofxLayoutElement{
    
public:
    /// |   Constructor/Destructor   | ///
    /// | -------------------------- | ///
    
    ofxLayoutElement();
    ~ofxLayoutElement();
    
    
    /// |   Cycle Functions  | ///
    /// | ------------------ | ///
    
    void update(float dt = 1.0f/60.0f);
    void draw(ofFbo* fbo = NULL);
    
    /// |   Setters/Getters   | ///
    /// | ------------------- | ///
    void setParent(ofxLayoutElement* parent);
    ofxLayoutElement* getParent();
    
    TAG::ENUM getTag();
    void setTag(TAG::ENUM tag);
    void setTag(string tag);
    
    ofPath* getPath();
    
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
    void setStyle(OSS_KEY::ENUM key, OSS_VALUE::ENUM value);
    void setStyle(OSS_KEY::ENUM key, string value);
    void setStyle(OSS_KEY::ENUM key, float value);
    void setStyle(OSS_KEY::ENUM key, ofColor value);
    string getInlineStyle();
    ofxOSS getInlineStyles();
    void setInlineStyle(string style);
    void appendInlineStyle(string style);
    
    void overrideStyles(ofxOSS* styleObject);
    
    void addChild(ofxLayoutElement* child);
    
    ofxOSS styles;
    ofxLayoutElement* parent;
    
    void drawBackground();
    void drawBackgroundImage();
    void drawBackgroundVideo();
    void drawBackgroundColor();
    void drawBackgroundTexture(ofTexture* texture);
    void drawText();
    void drawPath();
    
    bool beginBackgroundBlendMode();
    void endBackgroundBlendMode();
    
    void setLayout(ofxLayout* layout);
    
    MOUSE_STATE::ENUM getMouseState();
    void setMouseState(MOUSE_STATE::ENUM mouseState);
    
    void mouseMoved(ofMouseEventArgs& args);
    void mousePressed(ofMouseEventArgs& args);
    void mouseReleased(ofMouseEventArgs& args);
    void mouseDragged(ofMouseEventArgs& args);
    void fingerPressed(ofPoint _finger);
    void fingerDragged(ofPoint _finger);
    void fingerReleased(ofPoint _finger);
    
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
    ofEvent<string> stateEvt;
    string state;
    void setState(string state, bool recursive = false, bool reset = false);
    
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
    
    bool isPath();
    void drawContent();
    bool clickable();
    void attachAnimationInstance(ofxAnimationInstance* animationInstance);
    map<string, vector<ofxAnimationInstance*> >* getStates();
    void addState(string state, ofxAnimationInstance* animationInstance);
    ofEvent<string>* getStateEvent();
    bool hasState(string state);
    void addClass(string className);
    ofTexture* getBackgroundImageTexture();
    void loadSvg(string imageFilename);
    bool hittest(ofPoint pt);
    ofPoint getLocalPoint(ofPoint pt);
    
    ofPoint getMouseMoved();
    ofPoint getMouseReleased();
    ofPoint getMousePressed();
    ofPoint getMouseDragged();
    
    
    ofEvent<string> mouseMovedEvt;
    ofEvent<string> mouseReleasedEvt;
    ofEvent<string> mousePressedEvt;
    ofEvent<string> mouseDraggedEvt;
    ofxLayout* getLayout();
    void lockState(bool stateLocked);
    bool isStateLocked();
    
    void beginOverlay();
    void endOverlay();
    
    bool isSVG;
    ofxSVG* getSvg();
    void drawPath(ofPath* path);
    int getLevel();
    void setPath(ofPath* path, bool pathFillHack = false, ofPoint startPoint = ofPoint(), ofPoint endPoint = ofPoint());
    vector<ofxLayoutElement*> children(string selector = "");
    void remove(ofxLayoutElement* element);
    void setChildren(vector<ofxLayoutElement*> childs);
    
    void playBackgroundVideo();
    void pauseBackgroundVideo();
    
    void setPathPercent(float percent);
    
protected:
    ofMatrix4x4 globalTransformations;
    ofxLayout* layout;
    virtual void drawTag(){};
    ofPoint position;
    ofPoint startPt;
    ofPoint endPt;
    float pathStartFi;
    float pathEndFi;
    
    ofRectangle dimensions;
    SideDimensions borders;
    void updateText();
    string ID;
    TAG::ENUM tag;
    string classes;
    string elementValue;
    void beginBackgroundSize();
    void endBackgroundSize();
    float opacity;
    
    ofxSVG svg;
    string inlineStyle;
    
    MOUSE_STATE::ENUM mouseState;
    
//    ofVideoPlayer* video;
    ofxHapPlayer* video;
    ofPath* path;
    bool pathFillHack;
    float pathPercent;
    map<string, vector<ofxAnimationInstance*> > states;
    
    ofPoint mouseMovedPt;
    ofPoint mouseReleasedPt;
    ofPoint mousePressedPt;
    ofPoint mouseDraggedPt;
    FontData fontData;
    ofRectangle scissorStack;
    bool stateLocked;
    ofFbo* overlayFbo;
    int level;
    ofMesh mesh;
    vector<ofxLayoutElement*> childElements;
};