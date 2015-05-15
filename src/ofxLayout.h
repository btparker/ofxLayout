#pragma once
#include "ofMain.h"
#include "ofxLayoutElement.h"
#include "ofxXmlSettings.h"
#include "ofxLoaderSpool.h"
#include "ofxFontStash.h"
#include "ofShader.h"
#include "ofxMultiFboBlur.h"
#include "ofxTuio.h"

const string IMAGES_BATCH = "images";
class ofxLayout{
    
public:
    /// |   Constructor/Destructor   | ///
    /// | -------------------------- | ///
    
    ofxLayout();
    ofxLayout(ofPoint pos, ofRectangle dimensions);
    ofxLayout(int x, int y, int w, int h);
    void init(int x, int y, int w, int h);
    ~ofxLayout();

    /// |   Cycle Functions  | ///
    /// | ------------------ | ///
    
    void update(float dt=1.0f/60.0f);
    void draw();
    map<string, ofxFontStash*>* getFonts();
    ofxLoaderSpool* getAssets();
    
    /// |   Utilities   | ///
    /// | ------------- | ///
    bool ready();
    bool drawable();
    void applyChanges();
    void applyAnimations();
    int getWidth();
    int getHeight();
    ofPoint getPosition();
    string getData(string key);
    void setData(string key, string value);
    
    void loadFromFile(string filename);
    void loadFromTemplate(string templateFolder, ofxJSONElement data);
    
    /// \brief Loads an external OFML file (basically, xml or html), ties this element to the root element described in the file,
    /// and generates all child elements as well
    ///
    /// \param string ofmlFilename Location of the OFML file
    /// \param string ossFilename (optional) Location of the OSS file
    void loadOfmlFromFile(string ofmlFilename);
    void loadAnimationsFromFile(string animationsFilename);
    void loadOssFromFile(string ossFilename);
    void loadDataFromFile(string dataFilename);
    void loadData(ofxJSONElement data);
    void applyStyles(ofxLayoutElement* element = NULL);
    
    void removeElement(ofxLayoutElement* element);
    
    ofxLoaderSpool assets;
    map<string, ofxFontStash*> fonts;
    map<string, string> data;
    
    void unload();
    
    ofxLayoutElement* getElementById(string ID);
    set<ofxLayoutElement*> getElementsByClass(string classname);
    map<string, ofxLayoutElement*> idElementMap;
    map<string, set<ofxLayoutElement*> > classElementMap;
    
    void populateExpressions(string& value);

//    ofxAnimatableManager animatableManager;
    
    void mouseMoved(ofMouseEventArgs& args);
    void mousePressed(ofMouseEventArgs& args);
    void mouseReleased(ofMouseEventArgs& args);
    void mouseDragged(ofMouseEventArgs& args);
    
    ofEvent<string> mouseMovedEvt;
    ofEvent<string> mouseReleasedEvt;
    ofEvent<string> mousePressedEvt;
    ofEvent<string> mouseDraggedEvt;
    
    void tuioPressed(ofxTuioCursor &tuioCursor);
    void tuioRemoved(ofxTuioCursor &tuioCursor);
    void tuioUpdated(ofxTuioCursor &tuioCursor);

    ofEvent<ofxTuioCursor> tuioCursorAddedEvt;
    ofEvent<ofxTuioCursor> tuioCursorRemovedEvt;
    ofEvent<ofxTuioCursor> tuioCursorUpdatedEvt;
    
    
    /// \brief Using an ofxXmlSettings object (likely loaded from file), recursively constructs elements based on current depth.
    ///
    /// \param ofxXmlSettings* layout
    /// \param int which (optional) If there are multiple elements at the current level, specifies which to recurse into
    void loadFromXmlLayout(ofxXmlSettings* xmlLayout, ofxLayoutElement*, TAG::ENUM tagEnum, int which = 0);
    
    ofxLayoutElement* getBody();
    
    void loadTagElements(TAG::ENUM tag, ofxXmlSettings *xmlLayout, ofxLayoutElement* element);
    int width, height;
    ofxAnimatableManager am;
    ofxMultiFboBlur mFboBlur;
    ofxLayoutElement* hittest(ofPoint pt);
    void setMouseTransformation(ofMatrix4x4 mouseTransformation);
    ofMatrix4x4 getMouseTransformation();
    void enableMouseEvents();
    void disableMouseEvents();
    
    void populateXML(ofxXmlSettings* xml);
    void populateJSON(ofxJSONElement* json);
    void updateAssets(ofxLayoutElement* element);
    

    ofxTuioClient tuioClient;
    void enableTuioEvents(int port);
    void disableTuioEvents();
    bool tuioEnabled;
    
protected:
    ofxLayoutElement contextTreeRoot;
    ofxOSS styleRulesRoot;
    
    
    void loadTags(ofxXmlSettings* xmlLayout, ofxLayoutElement* element);
    
    /// \brief
    ///
    /// \param
    /// \param
    void loadFromOss(ofxJSONElement* jsonElement, ofxOSS* styleObject);
//    
    int pixelWidth, pixelHeight;
    
    int x,y;
    
    void allocateBlurFbo(int w, int h);
    
    ofMatrix4x4 mouseTransformation;
};