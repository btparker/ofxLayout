#include "ofMain.h"
#include "ofxJSON.h"

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

class ofxOSS{

public:
    
    /// |   Setters/Getters   | ///
    /// | ------------------- | ///
    
    string getStyle(OSS_KEY::ENUM key);
    string getStyle(string key);
    
    bool hasStyle(string key);
    bool hasStyle(OSS_KEY::ENUM key);
    
    void setStyle(OSS_KEY::ENUM key, string value);
    void setStyle(string key, string value);
    
    /// \brief Returns TYPE enum of key provided
    ///
    /// Example: 'background-color' yields TYPE::COLOR. Returns warning and INVALID type if none found.
    ///
    /// \param key
    static OSS_TYPE::ENUM getType(OSS_KEY::ENUM key);
    static OSS_TYPE::ENUM getType(string key);
    
    /// \brief Returns a pointer to the styles associated with a given ID
    ///
    /// \param string _ID Element ID
    ofxOSS* getStylesByID(string _ID);
    
    
    
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
    
    /// \brief Get the ofColor from the corresponding style key (example: 'background-color')
    ///
    /// \param key The key to get a value
    ofColor getColorStyle(string key);
    ofColor getColorStyle(OSS_KEY::ENUM key);
    
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
    /// \param ofRectangle imageDimensions
    /// \param ofRectangle elementBoundary
    ofRectangle getBackgroundSizeDimensions(ofRectangle imageTransform, ofRectangle elementBoundary);
    
    /// \brief Based on the background position style, (update) image dimensions, and the element boundary, returns the computed background transform of the image
    ///
    /// \param ofRectangle imageDimensions
    /// \param ofRectangle elementBoundary
    ofRectangle getBackgroundPosition(ofRectangle imageTransform, ofRectangle elementBoundary);
    
    bool isBackgroundSizeDynamic();
    
    
    
private:
    /// \brief Where the computational styles (by element) are stored
    ///
    /// \key OSS_KEY (POSITION, WIDTH, etc)
    /// \value string Style text rule ('center 50%', '#FF00FF', etc)
    map<OSS_KEY::ENUM, string> stylesMap;
    
    /// \brief Where the styles by id (say from an external OSS file) are stored
    ///
    /// \key id
    /// \value ofxOSS stylesheet
    map<string, ofxOSS*> idStyles;
    
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
};