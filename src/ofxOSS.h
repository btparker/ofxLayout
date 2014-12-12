#include "ofMain.h"

namespace OSS_TYPE{
    enum ENUM{INVALID, COLOR};
};

namespace OSS_KEY{
    enum ENUM{INVALID, BACKGROUND_COLOR};
};

class ofxOSS{

public:
    
    /// \brief Get the ofColor from the corresponding style key (example: 'background-color')
    ///
    /// \param key The key to get a value
    ofColor getColorStyle(string key);
    ofColor getColorStyle(OSS_KEY::ENUM key);
    
    
    string getStyle(OSS_KEY::ENUM key);
    string getStyle(string key);
    
    void setStyle(OSS_KEY::ENUM key, string value);
    void setStyle(string key, string value);
    
    /// \brief Create an ofColor from a css-like string value input, supports alpha ( rgba() ).
    ///
    /// Color hex (#FF0000), rgb(255,0,0), or rgba(255,0,0,128) type of inputs work.
    /// Currently, there is no 'red' or 'green' string type input.
    /// If input cannot be parsed, throws warning and returns ofColor::black
    ///
    /// \param colorValue The string of the css-like color input
    static ofColor getColorFromString(string colorValue);
    
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
    
    bool has(string key);
    bool has(OSS_KEY::ENUM key);
    
    static ofColor parseColor(string colorChannels);
    
private:
    map<OSS_KEY::ENUM, string> stylesMap;
};