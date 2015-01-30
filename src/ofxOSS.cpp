#include "ofxOSS.h"

ofxOSS::ofxOSS(){
    
}

void ofxOSS::setDefaults(){
    // Create defaults
    ofxOssRule backgroundColorDefault(ofColor(0,0,0,0));
    this->rules[OSS_KEY::BACKGROUND_COLOR] = backgroundColorDefault;
    
    ofxOssRule positionDefault("0px 0px");
    this->rules[OSS_KEY::POSITION] = positionDefault;
    
    ofxOssRule bgPositionDefault(getStringFromOssValue(OSS_VALUE::AUTO));
    this->rules[OSS_KEY::BACKGROUND_POSITION] = bgPositionDefault;
    
    ofxOssRule widthDefault("100%");
    this->rules[OSS_KEY::WIDTH] = widthDefault;
    
    ofxOssRule heightDefault("100%");
    this->rules[OSS_KEY::HEIGHT] = heightDefault;
    
    ofxOssRule textAlignDefault(getStringFromOssValue(OSS_VALUE::LEFT));
    this->rules[OSS_KEY::TEXT_ALIGN] = textAlignDefault;
    
    ofxOssRule fontSizeDefault("50");
    this->rules[OSS_KEY::FONT_SIZE] = fontSizeDefault;
    
    ofxOssRule bgSizeDefault(getStringFromOssValue(OSS_VALUE::AUTO));
    this->rules[OSS_KEY::BACKGROUND_SIZE] = bgSizeDefault;
    
    ofxOssRule bgBlendModeDefault(getStringFromOssValue(OSS_VALUE::DISABLED));
    this->rules[OSS_KEY::BACKGROUND_BLEND_MODE] = bgBlendModeDefault;
    
    ofxOssRule opacityDefault("1.0f");
    this->rules[OSS_KEY::OPACITY] = opacityDefault;
    
    ofxOssRule textTransformDefault(getStringFromOssValue(OSS_VALUE::NONE));
    this->rules[OSS_KEY::TEXT_TRANSFORM] = textTransformDefault;
    
    ofxOssRule colorDefault(ofColor::black);
    this->rules[OSS_KEY::COLOR] = colorDefault;
}

ofxOSS::~ofxOSS(){
    rules.clear();
    idMap.clear();
    classMap.clear();
    tagMap.clear();
}

/// |   Setters/Getters   | ///
/// | ------------------- | ///

string ofxOSS::getStyle(string key){
    return getStyle(getOssKeyFromString(key));
}

string ofxOSS::getStyle(OSS_KEY::ENUM styleKey){
    return this->rules[styleKey].getString();
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
    else if(key == "background-position"){
        return OSS_KEY::BACKGROUND_POSITION;
    }
    else if(key == "background-blend-mode"){
        return OSS_KEY::BACKGROUND_BLEND_MODE;
    }
    else if(key == "width"){
        return OSS_KEY::WIDTH;
    }
    else if(key == "height"){
        return OSS_KEY::HEIGHT;
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
    else if(key == "font-size"){
        return OSS_KEY::FONT_SIZE;
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
    else if(key == "text-background-color"){
        return OSS_KEY::TEXT_BACKGROUND_COLOR;
    }
    else if(key == "text-padding"){
        return OSS_KEY::TEXT_PADDING;
    }
    else if(key == "text-max-width"){
        return OSS_KEY::TEXT_MAX_WIDTH;
    }
    else if(key == "line-height"){
        return OSS_KEY::LINE_HEIGHT;
    }
    else{
        ofLogWarning("ofxOSS::getOssKeyFromString","No enum for "+key+" found.");
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
        case OSS_KEY::BACKGROUND_POSITION:
            keyString = "background-position";
            break;
        case OSS_KEY::BACKGROUND_BLEND_MODE:
            keyString = "background-blend-mode";
            break;
        case OSS_KEY::WIDTH:
            keyString = "width";
            break;
        case OSS_KEY::HEIGHT:
            keyString = "height";
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
        case OSS_KEY::TEXT_TRANSFORM:
            keyString = "text-transform";
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
        case OSS_KEY::TEXT_BACKGROUND_COLOR:
            keyString = "text-background-color";
            break;
        case OSS_KEY::TEXT_PADDING:
            keyString = "text-padding";
            break;
        case OSS_KEY::TEXT_MAX_WIDTH:
            keyString = "text-max-width";
            break;
        case OSS_KEY::LINE_HEIGHT:
            keyString = "line-height";
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
    
    else{
        ofLogWarning("ofxOSS::getOssValueFromString","No OSS_VALUE found for "+value+".");
        return OSS_VALUE::INVALID;
    }
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
    else{
        ofLogWarning("ofxOSS::getColorFromString","Could not parse "+colorValue+" to ofColor, returning ofColor::black");
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

/// |   Transformation Styles   | ///
/// | ------------------------- | ///
ofRectangle ofxOSS::computeElementTransform(ofRectangle parentBoundary){
    ofRectangle transform = ofRectangle();
    float width = getDimensionStyleValue(getStyle(OSS_KEY::WIDTH), parentBoundary.width);
    transform.width = width;
    float height = getDimensionStyleValue(getStyle(OSS_KEY::HEIGHT), parentBoundary.height);
    transform.height = height;
    ofPoint pos = getPosition(transform, parentBoundary);
    transform.setPosition(pos);
    return transform;
}

float ofxOSS::getDimensionStyleValue(OSS_KEY::ENUM dimensionKey, float parentDimension){
    string dimensionValue = getStyle(dimensionKey);
    return getDimensionStyleValue(dimensionValue, parentDimension);
}

float ofxOSS::getDimensionStyleValue(string dimensionValue, float parentDimension){
    // If dimension not found, return parent's
    if(dimensionValue == ""){
        return parentDimension;
    }
    
    if(ofIsStringInString(dimensionValue,"%")){
        float percent = (ofToFloat(ofSplitString(dimensionValue,"%")[0])/100.0);
        return percent*parentDimension;
    }
    else if(ofIsStringInString(dimensionValue,"px")){
        return ofToFloat(ofSplitString(dimensionValue,"px")[0]);
    }
    else{
        return ofToFloat(dimensionValue);
    }
}

ofPoint ofxOSS::getPosition(ofRectangle boundary, ofRectangle parentBoundary){
    string posString = getStyle(OSS_KEY::POSITION);
    return computePosition(posString,boundary, parentBoundary);
}


ofPoint ofxOSS::getBackgroundPosition(ofRectangle boundary, ofRectangle parentBoundary){
    string posString = getStyle(OSS_KEY::BACKGROUND_POSITION);
    return computePosition(posString,boundary, parentBoundary);
}

ofPoint ofxOSS::computePosition(string posString, ofRectangle boundary, ofRectangle parentBoundary){
    if(posString.length() > 0){
        ofPoint posPt;
        vector<string> posPieces = ofSplitString(posString, " ");
        string xStr, yStr;
        float x,y;
        if(posPieces.size() == 2){
            xStr = posPieces[0];
            yStr = posPieces[1];
        }
        else{
            xStr = yStr = posPieces[0];
        }
        
        x = computeLeftPosition(xStr, boundary, parentBoundary);
        y = computeTopPosition(yStr, boundary, parentBoundary);
        
        return ofPoint(x,y);
    }
    else{
        return ofPoint();
    }
}

ofRectangle ofxOSS::computeBackgroundTransform(ofRectangle dimensions, ofRectangle boundary){
    ofRectangle result = ofRectangle(dimensions);
    
    string bgSizeStr = getStyle(OSS_KEY::BACKGROUND_SIZE);
    
    
    vector<string> bgSizeStrSplit = ofSplitString(bgSizeStr, " ",true, true);
    
    bool isSizeStyleSingleRule = bgSizeStrSplit.size() == 1;
    // Doing this so there is no untrimmed space
    
    bgSizeStr = isSizeStyleSingleRule ? bgSizeStrSplit[0] : bgSizeStr;
    
    if(isSizeStyleSingleRule && bgSizeStr == getStringFromOssValue(OSS_VALUE::AUTO)){
        //does nothing, the result being the dimensions
    }
    else if(isSizeStyleSingleRule && (bgSizeStr == getStringFromOssValue(OSS_VALUE::COVER) || bgSizeStr == getStringFromOssValue(OSS_VALUE::CONTAIN))){
        float wRatio = (dimensions.getWidth())/boundary.getWidth();
        float hRatio = (dimensions.getHeight())/boundary.getHeight();
        
        float scale;
        
        if(bgSizeStr == getStringFromOssValue(OSS_VALUE::COVER)){
            scale = 1.0/min(wRatio,hRatio);
        }
        else{
            scale = 1.0/max(wRatio,hRatio);
        }
        
        result.scale(scale);
    }
    // example: "50px 75%"
    else{
        string wSizeStr = bgSizeStrSplit[0];
        string hSizeStr = isSizeStyleSingleRule ? wSizeStr : bgSizeStrSplit[1];
        
        result.setWidth(getDimensionStyleValue(wSizeStr, boundary.getWidth()));
        result.setHeight(getDimensionStyleValue(hSizeStr, boundary.getHeight()));
    }
    
    return result;
}


/// |   Private Functions   | ///
/// | --------------------- | ///

float ofxOSS::computeLeftPosition(string xStr, ofRectangle boundary, ofRectangle parentBoundary){
    float x;
    if(xStr == "center"){
        x = (parentBoundary.width/2.0)-(boundary.width/2.0);
    }
    else if(ofIsStringInString(xStr,"%")){
        float percent = (ofToFloat(ofSplitString(xStr,"%")[0])/100.0);
        x = percent*parentBoundary.width;
    }
    else if(ofIsStringInString(xStr,"px")){
        x = ofToFloat(ofSplitString(xStr,"px")[0]);
    }
    else{
        x = ofToFloat(xStr);
    }
    return x;
}

float ofxOSS::computeTopPosition(string yStr, ofRectangle boundary, ofRectangle parentBoundary){
    float y;
    
    if(yStr == "center"){
        y = (parentBoundary.height/2.0)-(boundary.height/2.0);
    }
    else if(ofIsStringInString(yStr,"%")){
        float percent = (ofToFloat(ofSplitString(yStr,"%")[0])/100.0);
        y = percent*parentBoundary.height;
    }
    else if(ofIsStringInString(yStr,"px")){
        y = ofToFloat(ofSplitString(yStr,"px")[0]);
    }
    else{
        y = ofToFloat(yStr);
    }
    
    return y;
}

ofxOssRule ofxOSS::generateRule(string key, string value){
    ofxOssRule ossRule(value);
    return ossRule;
}

void ofxOSS::parseBackgroundGradient(string bgGradientStr, ofColor* firstColor, ofColor* secondColor, bool* vertical){
    vector<string> pieces = ofSplitString(bgGradientStr, " ", true, true);
    firstColor->set(parseColor(pieces[0]));
    secondColor->set(parseColor(pieces[1]));
    *vertical = !(pieces.size() > 2 && pieces[2] == "horizontal");
}

string ofxOSS::stringifyColor(ofColor color){
    return
    "rgba("+
        ofToString(color.r)+","+
        ofToString(color.g)+","+
        ofToString(color.b)+","+
        ofToString(color.a)+
    ")";
}