#include "ofxOSS.h"

ofxOSS::ofxOSS(){
    
}

void ofxOSS::setDefaults(){
    // Create defaults
    ofxOssRule* backgroundColorDefault = new ofxOssRule();
    backgroundColorDefault->type = OSS_TYPE::COLOR;
    backgroundColorDefault->value = "rgba(0,0,0,0)";
    this->rules[OSS_KEY::BACKGROUND_COLOR] = backgroundColorDefault;
    
    ofxOssRule* positionDefault = new ofxOssRule();
    positionDefault->type = OSS_TYPE::POSITION;
    positionDefault->value = "0";
    this->rules[OSS_KEY::POSITION] = positionDefault;
    
    ofxOssRule* widthDefault = new ofxOssRule();
    widthDefault->type = OSS_TYPE::NUMBER;
    widthDefault->value = "0";
    this->rules[OSS_KEY::WIDTH] = widthDefault;
    
    ofxOssRule* heightDefault = new ofxOssRule();
    heightDefault->type = OSS_TYPE::NUMBER;
    heightDefault->value = "0";
    this->rules[OSS_KEY::HEIGHT] = heightDefault;
}

ofxOSS::~ofxOSS(){
    
}

/// |   Setters/Getters   | ///
/// | ------------------- | ///

string ofxOSS::getStyle(string key){
    return getStyle(getEnumFromString(key));
}

string ofxOSS::getStyle(OSS_KEY::ENUM styleKey){
    return this->rules[styleKey]->value;
}

bool ofxOSS::validKey(string key){
    return getEnumFromString(key) != OSS_KEY::INVALID;
}

OSS_TYPE::ENUM ofxOSS::getType(OSS_KEY::ENUM key){
    OSS_TYPE::ENUM type;
    switch(key){
        case OSS_KEY::BACKGROUND_COLOR:
            type = OSS_TYPE::COLOR;
            break;
        case OSS_KEY::BACKGROUND_IMAGE:
            type = OSS_TYPE::IMAGE;
            break;
        case OSS_KEY::BACKGROUND_VIDEO:
            type = OSS_TYPE::IMAGE;
            break;
        case OSS_KEY::BACKGROUND_SIZE:
            type = OSS_TYPE::IMAGE;
            break;
        case OSS_KEY::BACKGROUND_POSITION:
            type = OSS_TYPE::POSITION;
            break;
        case OSS_KEY::WIDTH:
            type = OSS_TYPE::NUMBER;
            break;
        case OSS_KEY::HEIGHT:
            type = OSS_TYPE::NUMBER;
            break;
        case OSS_KEY::POSITION:
            type = OSS_TYPE::POSITION;
            break;
        default:
            ofLogWarning("ofxOSS::getType","No type found for value provided.");
            type = OSS_TYPE::INVALID;
    }
    return type;
}

OSS_TYPE::ENUM ofxOSS::getType(string key){
    return getType(getEnumFromString(key));
}


/// |   Utilities   | ///
/// | ------------- | ///

OSS_KEY::ENUM ofxOSS::getEnumFromString(string key){
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
    else if(key == "width"){
        return OSS_KEY::WIDTH;
    }
    else if(key == "height"){
        return OSS_KEY::HEIGHT;
    }
    else if(key == "position"){
        return OSS_KEY::POSITION;
    }
    else{
        ofLogWarning("ofxOSS::getEnumFromString","No enum for "+key+" found.");
        return OSS_KEY::INVALID;
    }
}

string ofxOSS::getStringFromEnum(OSS_KEY::ENUM key){
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
        case OSS_KEY::WIDTH:
            keyString = "width";
            break;
        case OSS_KEY::HEIGHT:
            keyString = "height";
            break;
        case OSS_KEY::POSITION:
            keyString = "position";
            break;
        default:
            ofLogWarning("ofxOSS::getEnumFromString","No string key found for value provided.");
    }
    return keyString;
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
    transform.setWidth(width);
    float height = getDimensionStyleValue(getStyle(OSS_KEY::HEIGHT), parentBoundary.height);
    transform.setHeight(height);
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
    
    if(isSizeStyleSingleRule && bgSizeStr == "auto"){
        // Don't bother doing anything
    }
    if(isSizeStyleSingleRule && (bgSizeStr == "cover" || bgSizeStr == "contain")){
        float wRatio = (dimensions.getWidth())/boundary.getWidth();
        float hRatio = (dimensions.getHeight())/boundary.getHeight();
        
        float scale;
        
        if(bgSizeStr == "cover"){
            scale = 1.0/min(wRatio,hRatio);
        }
        else{
            scale = 1.0/max(wRatio,hRatio);
        }
        
        result.scale(scale);
    }
    else{
        string wSizeStr = bgSizeStrSplit[0];
        string hSizeStr = isSizeStyleSingleRule ? wSizeStr : bgSizeStrSplit[1];
        
        result.setWidth(getDimensionStyleValue(wSizeStr, boundary.getWidth()));
        result.setHeight(getDimensionStyleValue(hSizeStr, boundary.getHeight()));
    }
    
    
    
    return result;
}

//bool ofxOSS::isBackgroundSizeDynamic(){
//    string bgSizeStr = getStyle(OSS_KEY::BACKGROUND_SIZE);
//    vector<string> bgSizeStrSplit = ofSplitString(bgSizeStr, " ",true, true);
//    
//    bool isSizeStyleSingleRule = bgSizeStrSplit.size() == 1;
//    // Doing this so there is no untrimmed space
//    
//    bgSizeStr = isSizeStyleSingleRule ? bgSizeStrSplit[0] : bgSizeStr;
//    
//    return isSizeStyleSingleRule && (bgSizeStr == "cover" || bgSizeStr == "contain");
//}

//ofRectangle ofxOSS::getBackgroundPosition(ofRectangle imageTransform, ofRectangle elementBoundary){
//    ofRectangle result = ofRectangle(imageTransform);
//    string bgPosStr = getStyle(OSS_KEY::BACKGROUND_POSITION);
//    vector<string> bgPosStrSplit = ofSplitString(bgPosStr, " ",true, true);
//    
//    bool isPosStyleSingleRule = bgPosStrSplit.size() == 1;
//    
//    // Doing this so there is no untrimmed space
//    bgPosStr = isPosStyleSingleRule ? bgPosStrSplit[0] : bgPosStr;
//    
//    string xPosStr = bgPosStrSplit[0];
//    string yPosStr = isPosStyleSingleRule ? xPosStr : bgPosStrSplit[1];
//    
//    if(isBackgroundSizeDynamic()){
//        result.setX(computeLeftPosition(xPosStr, imageTransform, elementBoundary));
//        result.setY(computeTopPosition(yPosStr, imageTransform, elementBoundary));
//    }
//    else{
//        result.setX(computeLeftPosition(xPosStr, imageTransform, elementBoundary));
//        result.setY(computeTopPosition(yPosStr, imageTransform, elementBoundary));
//    }
//    
//    return result;
//}


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

ofxOssRule* ofxOSS::generateRule(string key, string value){
    ofxOssRule* ossRule = new ofxOssRule();
    ossRule->type = ofxOSS::getType(key);
    ossRule->value = value;
    return ossRule;
}

