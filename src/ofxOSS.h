#include "ofMain.h"
#include "ofxJSON.h"
#include "ofxFontStash.h"

// Types of styles
namespace OSS_TYPE{
    enum ENUM{INVALID, COLOR, NUMBER, POSITION, IMAGE};
};

// Style keys, in order to enforce string input
namespace OSS_KEY{
    enum ENUM{INVALID, BACKGROUND_COLOR, WIDTH, HEIGHT, POSITION, BACKGROUND_IMAGE, BACKGROUND_VIDEO, BACKGROUND_SIZE, BACKGROUND_POSITION};
};

namespace OSS_VALUE{
    enum ENUM{CENTER, COVER, CONTAIN, AUTO};
};

struct ofxOssRule{
    OSS_TYPE::ENUM type;
    string value;
};

class ofxOSS{

public:
    ofxOSS();
    ~ofxOSS();
    
    /// |   Setters/Getters   | ///
    /// | ------------------- | ///
    
    string getStyle(OSS_KEY::ENUM key);
    string getStyle(string key);
    
    static bool validKey(string key);
    static bool validKey(OSS_KEY::ENUM key);
    
    void setDefaults();
    
    /// \brief Returns TYPE enum of key provided
    ///
    /// Example: 'background-color' yields TYPE::COLOR. Returns warning and INVALID type if none found.
    ///
    /// \param key
    static OSS_TYPE::ENUM getType(OSS_KEY::ENUM key);
    static OSS_TYPE::ENUM getType(string key);
    
    /// |   Utilities   | ///
    /// | ------------- | ///
    
    
    /// \brief Lookup key enum from provided key string
    ///
    /// Example: BACKGROUND_COLOR from 'background-color'. Returns warning and INVALID enum if none found.
    ///
    /// \param key The string of the css-like key
    static OSS_KEY::ENUM getEnumFromString(string key);
    
    /// \brief Lookup key string from provided key enum
    ///
    /// Example: 'background-color' from BACKGROUND_COLOR. Returns warning and empty string if none found.
    ///
    /// \param key The enum of the css-like key
    static string getStringFromEnum(OSS_KEY::ENUM key);
    
    /// \brief Loads and parses an OSS file, stores in relevant styles
    ///
    /// \param string filename
    void loadFromFile(string filename);
    
    
    
    
    /// |   Color Styles   | ///
    /// | ---------------- | ///
    
    /// \brief Create an ofColor from a css-like string value input, supports alpha ( rgba() ).
    ///
    /// Color hex (#FF0000), rgb(255,0,0), or rgba(255,0,0,128) type of inputs work.
    /// Currently, there is no 'red' or 'green' string type input.
    /// If input cannot be parsed, throws warning and returns ofColor::black
    ///
    /// \param colorValue The string of the css-like color input
    static ofColor parseColor(string colorValue);

    /// \brief Given a comma deliminated string of color channels ( '255, 0, 0' ), return the ofColor
    ///
    /// Useful because color is stored this way in the stylesMap. Can handle rgba.
    ///
    /// \param colorChannels The string of color channels ( '255, 0, 0' )
    static ofColor parseColorChannels(string colorChannels);
    
    
    
    
    
    /// |   Transformation Styles   | ///
    /// | ------------------------- | ///
    
    
    /// \brief Based on the parent dimension and current display modes, returns the absolute value
    /// for the dimension given.
    ///
    /// Accepts percent ( '100%' ), and absolute values ( '50px', '50')
    ///
    /// \param parentDimension Bounding box width, height, etc of the parent
    float getDimensionStyleValue(string dimensionValue, float parentDimension);
    float getDimensionStyleValue(OSS_KEY::ENUM dimensionKey, float parentDimension);
    
    /// \brief Based on the boundary and current display modes, returns the absolute value
    /// for the [x,y] position given.
    ///
    /// \param ofRectangle boundary
    ofPoint getPosition(ofRectangle boundary, ofRectangle parentBoundary);
    
    /// \brief Based on the background size style, image dimensions, and the element boundary, returns the computed draw dimensions of the image.
    ///
    /// \param ofRectangle dimensions
    /// \param ofRectangle boundary
    ofRectangle computeBackgroundTransform(ofRectangle dimensions, ofRectangle boundary);
    
    /// \brief Based on the background position style, (update) image dimensions, and the element boundary, returns the computed background transform of the image
    ///
    /// \param ofRectangle imageDimensions
    /// \param ofRectangle elementBoundary
//    ofRectangle getBackgroundPosition(ofRectangle imageTransform, ofRectangle elementBoundary);
    
    static ofxOssRule* generateRule(string key, string value);
    
//    bool isBackgroundSizeDynamic();
    
    ofxOSS* parent = NULL;
    
    map<OSS_KEY::ENUM, ofxOssRule*> rules;
    
    map<string,ofxOSS*> idMap;
    map<string,ofxOSS*> classMap;
    map<string,ofxOSS*> tagMap;
    
    
private:
    /// \brief Calculates the x position (from the left) given a style string and relevant boundary information
    ///
    /// \param string xStr Describes the x position ('center', 50px, 25%)
    /// \param ofRectangle boundary The boundary of the element which the style is to be applied to
    /// \param ofRectangle parentBoundary The boundary of the parent element of the element which the style is to be applied
    float computeLeftPosition(string xStr, ofRectangle boundary, ofRectangle parentBoundary);
    
    /// \brief Calculates the y position (from the top) given a style string and relevant boundary information
    ///
    /// \param string yStr Describes the y position ('center', 50px, 25%)
    /// \param ofRectangle boundary The boundary of the element which the style is to be applied to
    /// \param ofRectangle parentBoundary The boundary of the parent element of the element which the style is to be applied
    float computeTopPosition(string yStr, ofRectangle boundary, ofRectangle parentBoundary);
    
    map<string,ofxFontStash*> fonts;
};