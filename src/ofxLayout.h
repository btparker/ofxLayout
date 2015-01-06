#include "ofMain.h"
#include "ofxLayoutElement.h"
#include "ofxXmlSettings.h"
#include "ofxLoaderSpool.h"

namespace TAG {
    enum ENUM{BODY, ELEMENT, TEXT, INVALID};
};

class ofxLayout{
    
public:
    /// |   Constructor/Destructor   | ///
    /// | -------------------------- | ///
    
    ofxLayout();
    ~ofxLayout();

    /// |   Cycle Functions  | ///
    /// | ------------------ | ///
    
    void update();
    void draw();
    
    
    /// |   Utilities   | ///
    /// | ------------- | ///
    void init();
    void applyChanges();
    static string getTagString(TAG::ENUM tagEnum);
    static TAG::ENUM getTagEnum(string tagString);
    /// \brief Loads an external OFML file (basically, xml or html), ties this element to the root element described in the file,
    /// and generates all child elements as well
    ///
    /// \param string ofmlFilename Location of the OFML file
    /// \param string ossFilename (optional) Location of the OSS file
    void loadOfmlFromFile(string ofmlFilename);
    
    void loadOssFromFile(string ossFilename);
    void applyStyles(ofxLayoutElement* element = NULL, ofxOSS* styleObject = NULL);
    
    ofxLoaderSpool assets;
    
    
protected:
    ofxLayoutElement* contextTreeRoot;
    ofxOSS* styleRulesRoot;
    
    /// \brief Using an ofxXmlSettings object (likely loaded from file), recursively constructs elements based on current depth.
    ///
    /// \param ofxXmlSettings* layout
    /// \param int which (optional) If there are multiple elements at the current level, specifies which to recurse into
    void loadFromXmlLayout(ofxXmlSettings* xmlLayout, ofxLayoutElement*, TAG::ENUM tagEnum, int which = 0);
    
    void loadTags(ofxXmlSettings* xmlLayout, ofxLayoutElement* element);
    
    /// \brief
    ///
    /// \param
    /// \param
    void loadFromOss(ofxJSONElement* jsonElement, ofxOSS* styleObject);

};