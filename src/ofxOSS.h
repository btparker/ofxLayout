#include "ofMain.h"
// Types of styles
namespace OSS_TYPE{
    enum ENUM{INVALID, COLOR, NUMBER, POSITION};
};

// Style keys, in order to enforce string input
namespace OSS_KEY{
    enum ENUM{INVALID, BACKGROUND_COLOR, WIDTH, HEIGHT, POSITION};
};

class ofxOSS{

public:
    
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
    
    /// \brief Returns TYPE enum of key provided
    ///
    /// Example: 'background-color' yields TYPE::COLOR. Returns warning and INVALID type if none found.
    ///
    /// \param key
    static OSS_TYPE::ENUM getType(string key);
    static OSS_TYPE::ENUM getType(OSS_KEY::ENUM key);
    
    
    
    
    /// |   Basic Styles   | ///
    /// | ---------------- | ///
    
    string getStyle(OSS_KEY::ENUM key);
    string getStyle(string key);
    
    bool hasStyle(string key);
    bool hasStyle(OSS_KEY::ENUM key);
    
    void setStyle(OSS_KEY::ENUM key, string value);
    void setStyle(string key, string value);
    
    
    
    
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
    /// \param dimensionKey WIDTH, HEIGHT, etc
    /// \param parentDimension Bounding box width, height, etc of the parent

    float getDimensionStyleValue(OSS_KEY::ENUM dimensionKey, float parentDimension);
    
    /// \brief Based on the boundary and current display modes, returns the absolute value
    /// for the [x,y] position given.
    ///
    /// \param ofRectangle boundary
    
    ofPoint getPosition(ofRectangle boundary, ofRectangle parentBoundary);
    
    
private:
    // Where all of the styles are stored
    map<OSS_KEY::ENUM, string> stylesMap;
    
    //
    float computeLeftPosition(string xStr, ofRectangle boundary, ofRectangle parentBoundary);
    
    //
    float computeTopPosition(string yStr, ofRectangle boundary, ofRectangle parentBoundary);
};