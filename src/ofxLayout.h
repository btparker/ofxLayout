#pragma once
#include "ofMain.h"
#include "ofxLayoutElement.h"
#include "ofxXmlSettings.h"
#include "ofxLoaderSpool.h"
#include "ofxFontStash.h"
#include "ofxSVGPathParser.h"

class ofxLayout{
    
public:
    /// |   Constructor/Destructor   | ///
    /// | -------------------------- | ///
    
    ofxLayout();
    ofxLayout(int width, int height);
    ~ofxLayout();

    /// |   Cycle Functions  | ///
    /// | ------------------ | ///
    
    void update();
    void draw();
    map<string, ofxFontStash>* getFonts();
    ofxLoaderSpool* getAssets();
    
    /// |   Utilities   | ///
    /// | ------------- | ///
    bool ready();
    bool drawable();
    void init();
    void applyChanges();
    
    string getData(string key);
    void setData(string key, string value);
    
    void loadFromFile(string filename);
    
    /// \brief Loads an external OFML file (basically, xml or html), ties this element to the root element described in the file,
    /// and generates all child elements as well
    ///
    /// \param string ofmlFilename Location of the OFML file
    /// \param string ossFilename (optional) Location of the OSS file
    void loadOfmlFromFile(string ofmlFilename);
    
    void loadOssFromFile(string ossFilename);
    void loadDataFromFile(string dataFilename);
    void applyStyles(ofxLayoutElement* element = NULL, ofxOSS* styleObject = NULL);
    
    ofxLoaderSpool assets;
    map<string, ofxFontStash> fonts;
    map<string, string> data;
    
    void unload();
    
    ofxLayoutElement* getElementById(string ID);
    map<string, ofxLayoutElement*> idElementMap;
    
    string populateExpressions(string value);

    ofxAnimatableManager animatableManager;
    
    ofxLayoutElement* hittest(ofPoint pt, vector<ofxLayoutElement*>* returnedElements = NULL, ofxLayoutElement* startElement = NULL);
    
    void mouseMoved(ofMouseEventArgs& args);
    void mousePressed(ofMouseEventArgs& args);
    void mouseReleased(ofMouseEventArgs& args);
    
    /// \brief Using an ofxXmlSettings object (likely loaded from file), recursively constructs elements based on current depth.
    ///
    /// \param ofxXmlSettings* layout
    /// \param int which (optional) If there are multiple elements at the current level, specifies which to recurse into
    void loadFromXmlLayout(ofxXmlSettings* xmlLayout, ofxLayoutElement*, TAG::ENUM tagEnum, int which = 0);
    
    ofxLayoutElement* getBody();
protected:
    ofxLayoutElement contextTreeRoot;
    ofxOSS styleRulesRoot;
    
    
    
    
    
    void loadTags(ofxXmlSettings* xmlLayout, ofxLayoutElement* element);
    
    /// \brief
    ///
    /// \param
    /// \param
    void loadFromOss(ofxJSONElement* jsonElement, ofxOSS* styleObject);
    
    void loadAnimationsFromOss(ofxJSONElement* jsonElement, ofxOSS* styleObject);
    
    void loadAnimationInstancesFromOss(ofxJSONElement* jsonElement, ofxOSS* styleObject, ofxLayoutElement* element = NULL);
    
    int pixelWidth, pixelHeight;
    
    ofFbo fbo;
};