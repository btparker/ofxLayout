#pragma once
#include "ofMain.h"
#include "ofxJSON.h"
#include "ofxFontStash.h"
#include "ofxAnimatableManager.h"

namespace MOUSE_STATE{
    enum ENUM{NONE, HOVER, ACTIVE};
}

// Types of styles
namespace OSS_TYPE{
    enum ENUM{NONE, COLOR, NUMBER, OSS_VALUE, PERCENT, INVALID};
};

// Style keys, in order to enforce string input
namespace OSS_KEY{
    enum ENUM{
        // DRAWING
        OPACITY, MASK, DISPLAY, OSS_OVERFLOW,
        
        // BORDERS
        BORDER_WIDTH, BORDER_COLOR, BORDER_RADIUS,
        
        // TRANSFORMS
        MARGIN_TOP, MARGIN_RIGHT, MARGIN_BOTTOM, MARGIN_LEFT,
        WIDTH, MIN_WIDTH, MAX_WIDTH,
        HEIGHT, POSITION, TOP, LEFT, RIGHT, BOTTOM, SCALE,
        
        // BACKGROUND
        BACKGROUND_COLOR, BACKGROUND_IMAGE, BACKGROUND_VIDEO, BACKGROUND_SIZE,
        BACKGROUND_POSITION, BACKGROUND_BLEND_MODE, BACKGROUND_GRADIENT, BACKGROUND_REPEAT,
        
        // TEXT
        FONT_FAMILY, COLOR, TEXT_ALIGN, FONT_SIZE, TEXT_TRANSFORM, TEXT_BACKGROUND_COLOR,
        TEXT_PADDING, TEXT_MAX_WIDTH, LINE_HEIGHT, VERTICAL_ALIGN,
        
        BLUR,
        
        // SHAPE
        FILL, STROKE, STROKE_MITERLIMIT,
        
        POINTER_EVENTS,
        
        // Invalid is last in case we want to extend the enum
        INVALID
    };
};

namespace OSS_VALUE{
    enum ENUM{
        // GENERAL
        NONE, AUTO,
        
        // DISPLAY
        BLOCK, VISIBLE, HIDDEN,
        
        // POSITION / TEXT ALIGN
        CENTER, LEFT, RIGHT, TOP, BOTTOM,
        FIT,
        
        // POSITIONING TYPES
        STATIC, RELATIVE, ABSOLUTE, FIXED,
        
        // SIZE
        COVER, CONTAIN, FILL,
        
        // BLEND MODES
        ALPHA, ADD, SUBTRACT, SCREEN, MULTIPLY, DISABLED,
        
        // TEXT TRANSFORM
        UPPERCASE, LOWERCASE, CAPITALIZE,
        
        // BACKGROUND REPEAT
        REPEAT, REPEAT_X, REPEAT_Y,
        
        
        // Invalid is last in case we want to extend the enum
        INVALID
    };
};

class ofxOssRule;

class ofxOSS{
public:
    ofxOSS();
    ~ofxOSS();
    
    /// |   Setters/Getters   | ///
    /// | ------------------- | ///
    
    ofxOssRule* getStyle(OSS_KEY::ENUM key);
    ofxOssRule* getStyle(string key);
    
    void setStyle(OSS_KEY::ENUM key, OSS_VALUE::ENUM value);
    void setStyle(OSS_KEY::ENUM key, string value);
    void setStyle(OSS_KEY::ENUM key, float value);
    void setStyle(OSS_KEY::ENUM key, ofColor value);
    
    static bool validKey(string key);
    static bool validKey(OSS_KEY::ENUM key);
    
    void setDefaults();
    
    /// |   Utilities   | ///
    /// | ------------- | ///
    
    
    /// \brief Lookup key enum from provided key string
    ///
    /// Example: BACKGROUND_COLOR from 'background-color'. Returns warning and INVALID enum if none found.
    ///
    /// \param key The string of the css-like key
    static OSS_KEY::ENUM getOssKeyFromString(string key);
    
    /// \brief Lookup key string from provided key enum
    ///
    /// Example: 'background-color' from BACKGROUND_COLOR. Returns warning and empty string if none found.
    ///
    /// \param key The enum of the css-like key
    static string getStringFromOssKey(OSS_KEY::ENUM key);
    
    static string getStringFromOssValue(OSS_VALUE::ENUM value);
    static OSS_VALUE::ENUM getOssValueFromString(string value);
    static OSS_TYPE::ENUM getOssTypeFromOssKey(OSS_KEY::ENUM key);
    static OSS_TYPE::ENUM getOssTypeFromOssKey(string key);
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
    static string stringifyColor(ofColor color);
    
    
    
    static void parseBackgroundGradient(string bgGradientStr, ofColor* firstColor, ofColor* secondColor, bool* vertical);
    
    
    
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
    ofPoint getBackgroundPosition(ofRectangle boundary, ofRectangle parentBoundary);
    
    ofPoint computePosition(string posString, ofRectangle boundary, ofRectangle parentBoundary);
    
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
    
    void generateRule(string key, string value);
    void generateRule(OSS_KEY::ENUM key, string value);
    void generateRule(OSS_KEY::ENUM key, float value);
    void generateRule(OSS_KEY::ENUM key, ofColor value);
    void generateRule(OSS_KEY::ENUM key, OSS_VALUE::ENUM value);
    
    ofRectangle computeElementTransform(ofRectangle parentBoundary);
    
    //    bool isBackgroundSizeDynamic();
    
    ofxOSS* parent = NULL;
    
    map<OSS_KEY::ENUM, ofxOssRule*> rules;
    
    map<string,ofxOSS> idMap;
    map<string,ofxOSS> classMap;
    map<string,ofxOSS> tagMap;
    
    
private:
    /// \brief Given a comma deliminated string of color channels ( '255, 0, 0' ), return the ofColor
    ///
    /// Useful because color is stored this way in the stylesMap. Can handle rgba.
    ///
    /// \param colorChannels The string of color channels ( '255, 0, 0' )
    static ofColor parseColorChannels(string colorChannels);
    
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

class ofxOssRule{
public:
    ofxOssRule(){
        setType(OSS_TYPE::NONE);
    }
    
    ofxOssRule(string value){
        setType(OSS_TYPE::NONE);
        setValue(value);
    }
    
    ofxOssRule(ofColor color){
        setType(OSS_TYPE::COLOR);
        setColor(color);
    }
    
    ofxOssRule(float number){
        setType(OSS_TYPE::NUMBER);
        setFloat(number);
    }
    
    string asString(){
        switch(getType()){
            case OSS_TYPE::COLOR:
                this->stringValue = ofxOSS::stringifyColor(this->asColor());
                break;
            case OSS_TYPE::OSS_VALUE:
                this->stringValue = ofxOSS::getStringFromOssValue(asOssValue());
                break;
            case OSS_TYPE::NUMBER:
                this->stringValue = ofToString(numberValue.getCurrentValue());
                break;
            case OSS_TYPE::PERCENT:
                this->stringValue = ofToString(numberValue.getCurrentValue())+"%";
                break;
            default:;
        }
        return this->stringValue;
    }
    
    void setValue(string value){
        this->ossValue = ofxOSS::getOssValueFromString(value);
        if(this->ossValue != OSS_VALUE::INVALID){
            setType(OSS_TYPE::OSS_VALUE);
        }
        switch(getType()){
            case OSS_TYPE::COLOR:
                this->colorValue.setColor(ofxOSS::parseColor(value));
                break;
            case OSS_TYPE::NUMBER:
            case OSS_TYPE::PERCENT:
                if(ofStringTimesInString(value, "%") > 0){
                    setType(OSS_TYPE::PERCENT);
                }
                if(ofStringTimesInString(value, "pt")){
                    value = ofToString((4.0f/3.0f)*ofToFloat(value));
                }
                this->numberValue.reset(ofToFloat(value));
                break;
            default:;
        }
        this->stringValue = value;
    }
    
    void setColor(ofColor color){
        this->colorValue.setColor(color);
    }
    
    ofColor asColor(){
        return colorValue.getCurrentColor();
    }
    
    void setFloat(float number){
        this->numberValue.reset(number);
    }
    
    float asFloat(){
        return numberValue.getCurrentValue();
    }
    
    void setOssValue(OSS_VALUE::ENUM ossValue){
        this->ossValue = ossValue;
    }
    
    OSS_VALUE::ENUM asOssValue(){
        return this->ossValue;
    }
    
    ofxAnimatableOfColor* getAnimatableColor(){
        return &colorValue;
    }
    ofxAnimatableFloat* getAnimatableFloat(){
        return &numberValue;
    }
    
    OSS_TYPE::ENUM getType(){
        return this->type;
    }
    
    void setType(OSS_TYPE::ENUM type){
        this->type = type;
    }
    
protected:
    OSS_TYPE::ENUM type = OSS_TYPE::NONE;
    OSS_VALUE::ENUM ossValue = OSS_VALUE::NONE;
    string stringValue;
    ofxAnimatableOfColor colorValue;
    ofxAnimatableFloat numberValue;
};