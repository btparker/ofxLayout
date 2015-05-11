#include "ofxOSS.h"

ofxOSS::ofxOSS(){
}

void ofxOSS::setDefaults(){
    // Create defaults
    generateRule(OSS_KEY::TOP, "0px");
    
    generateRule(OSS_KEY::LEFT, "0px");
    
    generateRule(OSS_KEY::BACKGROUND_POSITION, OSS_VALUE::AUTO);
    
    generateRule(OSS_KEY::WIDTH, OSS_VALUE::AUTO);
    
    generateRule(OSS_KEY::HEIGHT, OSS_VALUE::AUTO);
    
    generateRule(OSS_KEY::MARGIN_LEFT, "0px");
    
    generateRule(OSS_KEY::MARGIN_RIGHT, "0px");
    
    generateRule(OSS_KEY::MARGIN_TOP, "0px");
    
    generateRule(OSS_KEY::MARGIN_BOTTOM, "0px");
    
    generateRule(OSS_KEY::TEXT_ALIGN, OSS_VALUE::LEFT);
    
    generateRule(OSS_KEY::VERTICAL_ALIGN, OSS_VALUE::TOP);
    
    generateRule(OSS_KEY::FONT_SIZE, 50);
    
    generateRule(OSS_KEY::LETTER_SPACING, 0);
    
    generateRule(OSS_KEY::BACKGROUND_SIZE, OSS_VALUE::AUTO);
    generateRule(OSS_KEY::ORIGIN_X, 0.5f);
    generateRule(OSS_KEY::ORIGIN_Y, 0.5f);
    generateRule(OSS_KEY::BACKGROUND_BLEND_MODE, OSS_VALUE::ALPHA);
    
    generateRule(OSS_KEY::OPACITY, "1.0");
    
    generateRule(OSS_KEY::LINE_HEIGHT, "1.0");

//    generateRule(OSS_KEY::SCALE, "1.0");
    
    generateRule(OSS_KEY::POSITION, OSS_VALUE::STATIC);
    
    generateRule(OSS_KEY::TEXT_TRANSFORM, OSS_VALUE::NONE);
    
    generateRule(OSS_KEY::COLOR, ofColor::black);
    generateRule(OSS_KEY::BACKGROUND_COLOR, ofColor(0,0,0,0));
    
    generateRule(OSS_KEY::OSS_OVERFLOW, OSS_VALUE::VISIBLE);
    
    generateRule(OSS_KEY::BORDER_WIDTH, 0);
    
    generateRule(OSS_KEY::BLUR, 0);
    
    generateRule(OSS_KEY::BORDER_COLOR, ofColor::black);
    
    generateRule(OSS_KEY::BORDER_RADIUS, 0);
    
    generateRule(OSS_KEY::DISPLAY, OSS_VALUE::BLOCK);
    
    generateRule(OSS_KEY::POINTER_EVENTS, OSS_VALUE::AUTO);
}

void ofxOSS::setStyle(OSS_KEY::ENUM key, OSS_VALUE::ENUM value){
    generateRule(key, value);
}

void ofxOSS::setStyle(OSS_KEY::ENUM key, string value){
    generateRule(key, value);
}

void ofxOSS::setStyle(OSS_KEY::ENUM key, float value){
    generateRule(key, value);
}

void ofxOSS::setStyle(OSS_KEY::ENUM key, ofColor value){
    generateRule(key, value);
}

ofxOSS::~ofxOSS(){
    rules.clear();
    idMap.clear();
    classMap.clear();
    tagMap.clear();
}

/// |   Setters/Getters   | ///
/// | ------------------- | ///

ofxOssRule* ofxOSS::getStyle(string key){
    return getStyle(getOssKeyFromString(key));
}

ofxOssRule* ofxOSS::getStyle(OSS_KEY::ENUM styleKey){
    return &rules[styleKey];
}

bool ofxOSS::validKey(string key){
    return getOssKeyFromString(key) != OSS_KEY::INVALID;
}


/// |   Utilities   | ///
/// | ------------- | ///

OSS_KEY::ENUM ofxOSS::getOssKeyFromString(string key){
    OSS_KEY::ENUM keyEnum;
    if(key == "background-color"){
        return OSS_KEY::BACKGROUND_COLOR;
    }
    else if(key == "background-image"){
        return OSS_KEY::BACKGROUND_IMAGE;
    }
    else if(key == "background-video"){
        return OSS_KEY::BACKGROUND_VIDEO;
    }
    else if(key == "background-size"){
        return OSS_KEY::BACKGROUND_SIZE;
    }
    else if(key == "origin-x"){
        return OSS_KEY::ORIGIN_X;
    }
    else if(key == "origin-y"){
        return OSS_KEY::ORIGIN_Y;
    }
    else if(key == "background-position"){
        return OSS_KEY::BACKGROUND_POSITION;
    }
    else if(key == "background-blend-mode"){
        return OSS_KEY::BACKGROUND_BLEND_MODE;
    }
    else if(key == "width"){
        return OSS_KEY::WIDTH;
    }
    else if(key == "min-width"){
        return OSS_KEY::MIN_WIDTH;
    }
    else if(key == "max-width"){
        return OSS_KEY::MAX_WIDTH;
    }
    else if(key == "margin-top"){
        return OSS_KEY::MARGIN_TOP;
    }
    else if(key == "margin-right"){
        return OSS_KEY::MARGIN_RIGHT;
    }
    else if(key == "margin-bottom"){
        return OSS_KEY::MARGIN_BOTTOM;
    }
    else if(key == "margin-left"){
        return OSS_KEY::MARGIN_LEFT;
    }
    else if(key == "height"){
        return OSS_KEY::HEIGHT;
    }
    else if(key == "max-height"){
        return OSS_KEY::MAX_HEIGHT;
    }
    else if(key == "scale"){
        return OSS_KEY::SCALE;
    }
    else if(key == "top"){
        return OSS_KEY::TOP;
    }
    else if(key == "right"){
        return OSS_KEY::RIGHT;
    }
    else if(key == "bottom"){
        return OSS_KEY::BOTTOM;
    }
    else if(key == "left"){
        return OSS_KEY::LEFT;
    }
    else if(key == "position"){
        return OSS_KEY::POSITION;
    }
    else if(key == "font-family"){
        return OSS_KEY::FONT_FAMILY;
    }
    else if(key == "text-align"){
        return OSS_KEY::TEXT_ALIGN;
    }
    else if(key == "vertical-align"){
        return OSS_KEY::VERTICAL_ALIGN;
    }
    else if(key == "letter-spacing"){
        return OSS_KEY::LETTER_SPACING;
    }
    else if(key == "font-size"){
        return OSS_KEY::FONT_SIZE;
    }
    else if(key == "font-size-min"){
        return OSS_KEY::FONT_SIZE_MIN;
    }
    else if(key == "font-size-max"){
        return OSS_KEY::FONT_SIZE_MAX;
    }
    else if(key == "opacity"){
        return OSS_KEY::OPACITY;
    }
    else if(key == "text-transform"){
        return OSS_KEY::TEXT_TRANSFORM;
    }
    else if(key == "color"){
        return OSS_KEY::COLOR;
    }
    else if(key == "mask"){
        return OSS_KEY::MASK;
    }
    else if(key == "background-repeat"){
        return OSS_KEY::BACKGROUND_REPEAT;
    }
    else if(key == "display"){
        return OSS_KEY::DISPLAY;
    }
    else if(key == "background-gradient"){
        return OSS_KEY::BACKGROUND_GRADIENT;
    }
//    else if(key == "text-background-color"){
//        return OSS_KEY::TEXT_BACKGROUND_COLOR;
//    }
//    else if(key == "text-padding"){
//        return OSS_KEY::TEXT_PADDING;
//    }
    else if(key == "text-max-width"){
        return OSS_KEY::TEXT_MAX_WIDTH;
    }
    else if(key == "line-height"){
        return OSS_KEY::LINE_HEIGHT;
    }
    else if(key == "blur"){
        return OSS_KEY::BLUR;
    }
    else if(key == "fill"){
        return OSS_KEY::FILL;
    }
    else if(key == "stroke"){
        return OSS_KEY::STROKE;
    }
    else if(key == "stroke-miterlimit"){
        return OSS_KEY::STROKE_MITERLIMIT;
    }
    else if(key == "overflow"){
        return OSS_KEY::OSS_OVERFLOW;
    }
    
    else if(key == "border-width"){
        return OSS_KEY::BORDER_WIDTH;
    }
    else if(key == "border-radius"){
        return OSS_KEY::BORDER_RADIUS;
    }
    else if(key == "border-color"){
        return OSS_KEY::BORDER_COLOR;
    }
    else if(key == "pointer-events"){
        return OSS_KEY::POINTER_EVENTS;
    }
    else{
        return OSS_KEY::INVALID;
    }
}

string ofxOSS::getStringFromOssKey(OSS_KEY::ENUM key){
    string keyString;
    switch(key){
        case OSS_KEY::BACKGROUND_COLOR:
            keyString = "background-color";
            break;
        case OSS_KEY::BACKGROUND_VIDEO:
            keyString = "background-video";
            break;
        case OSS_KEY::BACKGROUND_IMAGE:
            keyString = "background-image";
            break;
        case OSS_KEY::BACKGROUND_SIZE:
            keyString = "background-size";
            break;
        case OSS_KEY::ORIGIN_X:
            keyString = "origin-x";
            break;
        case OSS_KEY::ORIGIN_Y:
            keyString = "origin-y";
            break;
        case OSS_KEY::BACKGROUND_POSITION:
            keyString = "background-position";
            break;
        case OSS_KEY::BACKGROUND_BLEND_MODE:
            keyString = "background-blend-mode";
            break;
        case OSS_KEY::WIDTH:
            keyString = "width";
            break;
        case OSS_KEY::MIN_WIDTH:
            keyString = "min-width";
            break;
        case OSS_KEY::MARGIN_TOP:
            keyString = "margin-top";
            break;
        case OSS_KEY::MARGIN_RIGHT:
            keyString = "margin-right";
            break;
        case OSS_KEY::MARGIN_BOTTOM:
            keyString = "margin-bottom";
            break;
        case OSS_KEY::MARGIN_LEFT:
            keyString = "margin-left";
            break;
        case OSS_KEY::MAX_WIDTH:
            keyString = "max-width";
            break;
        case OSS_KEY::HEIGHT:
            keyString = "height";
            break;
        case OSS_KEY::MAX_HEIGHT:
            keyString = "max-height";
            break;
        case OSS_KEY::TOP:
            keyString = "top";
            break;
        case OSS_KEY::RIGHT:
            keyString = "right";
            break;
        case OSS_KEY::BOTTOM:
            keyString = "bottom";
            break;
        case OSS_KEY::LEFT:
            keyString = "left";
            break;
        case OSS_KEY::POSITION:
            keyString = "position";
            break;
        case OSS_KEY::FONT_FAMILY:
            keyString = "font-family";
            break;
        case OSS_KEY::TEXT_ALIGN:
            keyString = "text-align";
            break;
        case OSS_KEY::VERTICAL_ALIGN:
            keyString = "vertical-align";
            break;
        case OSS_KEY::TEXT_TRANSFORM:
            keyString = "text-transform";
            break;
        case OSS_KEY::SCALE:
            keyString = "scale";
            break;
        case OSS_KEY::LETTER_SPACING:
            keyString = "letter-spacing";
            break;
        case OSS_KEY::FONT_SIZE:
            keyString = "font-size";
            break;
        case OSS_KEY::COLOR:
            keyString = "color";
            break;
        case OSS_KEY::OPACITY:
            keyString = "opacity";
            break;
        case OSS_KEY::MASK:
            keyString = "mask";
            break;
        case OSS_KEY::BACKGROUND_REPEAT:
            keyString = "background-repeat";
            break;
        case OSS_KEY::DISPLAY:
            keyString = "display";
            break;
        case OSS_KEY::BACKGROUND_GRADIENT:
            keyString = "background-gradient";
            break;
//        case OSS_KEY::TEXT_BACKGROUND_COLOR:
//            keyString = "text-background-color";
//            break;
//        case OSS_KEY::TEXT_PADDING:
//            keyString = "text-padding";
//            break;
        case OSS_KEY::TEXT_MAX_WIDTH:
            keyString = "text-max-width";
            break;
        case OSS_KEY::LINE_HEIGHT:
            keyString = "line-height";
            break;
        case OSS_KEY::BLUR:
            keyString = "blur";
            break;
        case OSS_KEY::FILL:
            keyString = "fill";
            break;
        case OSS_KEY::STROKE:
            keyString = "stroke";
            break;
        case OSS_KEY::STROKE_MITERLIMIT:
            keyString = "stroke-miterlimit";
            break;
        case OSS_KEY::OSS_OVERFLOW:
            keyString = "overflow";
            break;
        case OSS_KEY::BORDER_WIDTH:
            keyString = "border-width";
            break;
        case OSS_KEY::BORDER_COLOR:
            keyString = "border-color";
            break;
        case OSS_KEY::BORDER_RADIUS:
            keyString = "border-radius";
            break;
        case OSS_KEY::POINTER_EVENTS:
            keyString = "pointer-events";
            break;
        default:
            ofLogWarning("ofxOSS::getStringFromOssKey","No string key found for value provided.");
    }
    return keyString;
}

string ofxOSS::getStringFromOssValue(OSS_VALUE::ENUM value){
    string valueStr = "";
    switch(value){
            // GENERAL
        case OSS_VALUE::NONE:
            valueStr = "none";
            break;
        case OSS_VALUE::AUTO:
            valueStr = "auto";
            break;
            
        // DISPLAY
        case OSS_VALUE::BLOCK:
            valueStr = "block";
            break;
        case OSS_VALUE::VISIBLE:
            valueStr = "visible";
            break;
        case OSS_VALUE::HIDDEN:
            valueStr = "hidden";
            break;
            
            // POSITION / TEXT ALIGN
        case OSS_VALUE::CENTER:
            valueStr = "center";
            break;
        case OSS_VALUE::LEFT:
            valueStr = "left";
            break;
        case OSS_VALUE::RIGHT:
            valueStr = "right";
            break;
        case OSS_VALUE::TOP:
            valueStr = "top";
            break;
        case OSS_VALUE::BOTTOM:
            valueStr = "bottom";
            break;
            
        case OSS_VALUE::STATIC:
            valueStr = "static";
            break;
        case OSS_VALUE::ABSOLUTE:
            valueStr = "absolute";
            break;
        case OSS_VALUE::RELATIVE:
            valueStr = "relative";
            break;
        case OSS_VALUE::FIXED:
            valueStr = "fixed";
            break;
        case OSS_VALUE::FILL:
            valueStr = "fill";
            break;
            
            // SIZE
        case OSS_VALUE::COVER:
            valueStr = "cover";
            break;
        case OSS_VALUE::CONTAIN:
            valueStr = "contain";
            break;
            
            // BLEND MODES
        case OSS_VALUE::ALPHA:
            valueStr = "alpha";
            break;
        case OSS_VALUE::ADD:
            valueStr = "add";
            break;
        case OSS_VALUE::SUBTRACT:
            valueStr = "subtract";
            break;
        case OSS_VALUE::SCREEN:
            valueStr = "screen";
            break;
        case OSS_VALUE::MULTIPLY:
            valueStr = "multiply";
            break;
        case OSS_VALUE::DISABLED:
            valueStr = "disabled";
            break;
            
            // TEXT TRANSFORM
        case OSS_VALUE::UPPERCASE:
            valueStr = "uppercase";
            break;
        case OSS_VALUE::LOWERCASE:
            valueStr = "lowercase";
            break;
        case OSS_VALUE::CAPITALIZE:
            valueStr = "capitalize";
            break;
            
            // BACKGROUND REPEAT
        case OSS_VALUE::REPEAT:
            valueStr = "repeat";
            break;
        case OSS_VALUE::REPEAT_X:
            valueStr = "repeat-x";
            break;
        case OSS_VALUE::REPEAT_Y:
            valueStr = "repeat-y";
            break;
        case OSS_VALUE::FIT:
            valueStr = "fit";
            break;
        case OSS_VALUE::FIT_WORD:
            valueStr = "fit-word";
            break;
        default:
            ofLogWarning("ofxOSS::getStringFromOssValue","No string value found for OSS_VALUE provided.");
    }
    return valueStr;
}

OSS_VALUE::ENUM ofxOSS::getOssValueFromString(string value){
    
    // GENERAL
    if(value == "none"){
        return OSS_VALUE::NONE;
    }
    else if(value == "auto"){
        return OSS_VALUE::AUTO;
    }
    
    // DISPLAY
    else if(value == "block"){
        return OSS_VALUE::BLOCK;
    }
    else if(value == "visible"){
        return OSS_VALUE::VISIBLE;
    }
    else if(value == "hidden"){
        return OSS_VALUE::HIDDEN;
    }
    
    // POSITION / TEXT ALIGN
    else if(value == "center"){
        return OSS_VALUE::CENTER;
    }
    else if(value == "left"){
        return OSS_VALUE::LEFT;
    }
    else if(value == "right"){
        return OSS_VALUE::RIGHT;
    }
    else if(value == "top"){
        return OSS_VALUE::TOP;
    }
    else if(value == "bottom"){
        return OSS_VALUE::BOTTOM;
    }
    
    // POSITION TYPES
    else if(value == "static"){
        return OSS_VALUE::STATIC;
    }
    else if(value == "relative"){
        return OSS_VALUE::RELATIVE;
    }
    else if(value == "absolute"){
        return OSS_VALUE::ABSOLUTE;
    }
    else if(value == "fixed"){
        return OSS_VALUE::FIXED;
    }
    else if(value == "fill"){
        return OSS_VALUE::FILL;
    }

    // SIZE
    else if(value == "cover"){
        return OSS_VALUE::COVER;
    }
    else if(value == "contain"){
        return OSS_VALUE::CONTAIN;
    }
    
    // BLEND MODES
    else if(value == "alpha"){
        return OSS_VALUE::ALPHA;
    }
    else if(value == "add"){
        return OSS_VALUE::ADD;
    }
    else if(value == "subtract"){
        return OSS_VALUE::SUBTRACT;
    }
    else if(value == "screen"){
        return OSS_VALUE::SCREEN;
    }
    else if(value == "multiply"){
        return OSS_VALUE::MULTIPLY;
    }
    else if(value == "disabled"){
        return OSS_VALUE::DISABLED;
    }
    
    // TEXT TRANSFORM
    else if (value == "uppercase"){
        return OSS_VALUE::UPPERCASE;
    }
    else if (value == "lowercase"){
        return OSS_VALUE::LOWERCASE;
    }
    else if (value == "capitalize"){
        return OSS_VALUE::CAPITALIZE;
    }
    
    // BACKGROUND REPEAT
    else if (value == "repeat"){
        return OSS_VALUE::REPEAT;
    }
    else if (value == "repeat-x"){
        return OSS_VALUE::REPEAT_X;
    }
    else if (value == "repeat-y"){
        return OSS_VALUE::REPEAT_Y;
    }
    else if (value == "fit"){
        return OSS_VALUE::FIT;
    }
    else if (value == "fit-word"){
        return OSS_VALUE::FIT_WORD;
    }
    
    else{
        return OSS_VALUE::INVALID;
    }
}

OSS_TYPE::ENUM ofxOSS::getOssTypeFromOssKey(OSS_KEY::ENUM key){
    OSS_TYPE::ENUM type;
    switch (key) {
        case OSS_KEY::BACKGROUND_BLEND_MODE:
            type = OSS_TYPE::OSS_VALUE;
            break;
        case OSS_KEY::BACKGROUND_COLOR:
            type = OSS_TYPE::COLOR;
            break;
        case OSS_KEY::BACKGROUND_REPEAT:
            type = OSS_TYPE::OSS_VALUE;
            break;
        case OSS_KEY::COLOR:
            type = OSS_TYPE::COLOR;
            break;
        case OSS_KEY::OPACITY:
            type = OSS_TYPE::NUMBER;
            break;
        case OSS_KEY::SCALE:
            type = OSS_TYPE::NUMBER;
            break;
        case OSS_KEY::WIDTH:
            type = OSS_TYPE::NUMBER;
            break;
        case OSS_KEY::MIN_WIDTH:
            type = OSS_TYPE::NUMBER;
            break;
        case OSS_KEY::MAX_WIDTH:
            type = OSS_TYPE::NUMBER;
            break;
        case OSS_KEY::MAX_HEIGHT:
            type = OSS_TYPE::NUMBER;
            break;
        case OSS_KEY::HEIGHT:
            type = OSS_TYPE::NUMBER;
            break;
        case OSS_KEY::TOP:
            type = OSS_TYPE::NUMBER;
            break;
        case OSS_KEY::RIGHT:
            type = OSS_TYPE::NUMBER;
            break;
        case OSS_KEY::BOTTOM:
            type = OSS_TYPE::NUMBER;
            break;
        case OSS_KEY::LEFT:
            type = OSS_TYPE::NUMBER;
            break;
        case OSS_KEY::MARGIN_TOP:
            type = OSS_TYPE::NUMBER;
            break;
        case OSS_KEY::MARGIN_RIGHT:
            type = OSS_TYPE::NUMBER;
            break;
        case OSS_KEY::MARGIN_BOTTOM:
            type = OSS_TYPE::NUMBER;
            break;
        case OSS_KEY::MARGIN_LEFT:
            type = OSS_TYPE::NUMBER;
            break;
        case OSS_KEY::LETTER_SPACING:
        case OSS_KEY::FONT_SIZE:
        case OSS_KEY::FONT_SIZE_MIN:
        case OSS_KEY::FONT_SIZE_MAX:
        case OSS_KEY::ORIGIN_X:
        case OSS_KEY::ORIGIN_Y:
            type = OSS_TYPE::NUMBER;
            break;
        case OSS_KEY::LINE_HEIGHT:
            type = OSS_TYPE::NUMBER;
            break;
        case OSS_KEY::TEXT_MAX_WIDTH:
            type = OSS_TYPE::NUMBER;
            break;
//        case OSS_KEY::TEXT_BACKGROUND_COLOR:
//            type = OSS_TYPE::COLOR;
//            break;
        case OSS_KEY::TEXT_TRANSFORM:
            type = OSS_TYPE::OSS_VALUE;
            break;
        case OSS_KEY::FILL:
            type = OSS_TYPE::COLOR;
            break;
        case OSS_KEY::STROKE:
            type = OSS_TYPE::COLOR;
            break;
        case OSS_KEY::STROKE_MITERLIMIT:
            type = OSS_TYPE::NUMBER;
            break;
        case OSS_KEY::POSITION:
            type = OSS_TYPE::OSS_VALUE;
            break;
        case OSS_KEY::OSS_OVERFLOW:
            type = OSS_TYPE::OSS_VALUE;
            break;
        case OSS_KEY::BORDER_WIDTH:
            type = OSS_TYPE::NUMBER;
            break;
        case OSS_KEY::BORDER_COLOR:
            type = OSS_TYPE::COLOR;
            break;
        case OSS_KEY::BORDER_RADIUS:
            type = OSS_TYPE::NUMBER;
            break;
        case OSS_KEY::POINTER_EVENTS:
            type = OSS_TYPE::OSS_VALUE;
            break;
        case OSS_KEY::BLUR:
            type = OSS_TYPE::NUMBER;
            break;
        default:
            type = OSS_TYPE::NONE;
            break;
    }
    return type;
}

OSS_TYPE::ENUM ofxOSS::getOssTypeFromOssKey(string key){
    return getOssTypeFromOssKey(getOssKeyFromString(key));
}

/// |   Color Styles   | ///
/// | ---------------- | ///


ofColor ofxOSS::parseColor(string colorValue){
    ofColor color;
    
    // If given color in html hex form (#FF0000)
    if(ofIsStringInString(colorValue,"#")){
        int hex = ofHexToInt(ofSplitString(colorValue,"#")[1]);
        color = ofColor::fromHex(hex);
    }
    // If given color in rgb() or rgba() form
    else if(ofIsStringInString(colorValue,"rgb(") || ofIsStringInString(colorValue,"rgba(")){
        // Substring in C++ is annoying, starting index and length
        // Using the parenthesis and end of string
        int si = colorValue.find("(")+1;
        int l = colorValue.length()-si-1;
        
        // From 'rgb(255,0,128)', would get '255,0,128'
        string commaSeparatedChannels = colorValue.substr(si,l);
        
        color = parseColorChannels(commaSeparatedChannels);
    }
    else if(colorValue == "none"){
        color = ofColor(0,0,0,0);
    }
    else{
        ofLogWarning("ofxOSS::parseColor","Could not parse "+colorValue+" to ofColor, returning ofColor::black");
        color = ofColor::black;
    }
    return color;
}


ofColor ofxOSS::parseColorChannels(string colorChannels){
    ofColor color;
    //splitting string form '255,0,128' into array [255,0,128]
    vector<string> splitChannels = ofSplitString(colorChannels,",");
    
    // Color channels
    int r = ofToInt(splitChannels[0]);
    int g = ofToInt(splitChannels[1]);
    int b = ofToInt(splitChannels[2]);
    color.set(r,g,b);
    // Alpha support
    if(splitChannels.size() == 4){
        int a = ofToInt(splitChannels[3]);
        color = ofColor(color, a);
    }
    
    return color;
}

void ofxOSS::generateRule(string key, string value){
    return generateRule(getOssKeyFromString(key), value);
}

void ofxOSS::generateRule(OSS_KEY::ENUM key, string value){
    OSS_TYPE::ENUM type = getOssTypeFromOssKey(key);
    this->rules[key].setType(type);
    this->rules[key].setValue(value);
}

void ofxOSS::generateRule(OSS_KEY::ENUM key, OSS_VALUE::ENUM value){
    this->rules[key].setType(getOssTypeFromOssKey(key));
    this->rules[key].setOssValue(value);
}

void ofxOSS::generateRule(OSS_KEY::ENUM key, float value){
    this->rules[key].setType(getOssTypeFromOssKey(key));
    this->rules[key].setFloat(value);
}

void ofxOSS::generateRule(OSS_KEY::ENUM key, ofColor value){
    this->rules[key].setType(getOssTypeFromOssKey(key));
    this->rules[key].setColor(value);
}

string ofxOSS::stringifyColor(ofColor color){
    return "rgba("+ofJoinString(ofSplitString(ofToString(color), ", ", true, true),",")+")";
}