#include "ofxOSS.h"

ofxOSS::ofxOSS(){
    
}

void ofxOSS::setDefaults(){
    // Create defaults
    ofxOssRule backgroundColorDefault;
    backgroundColorDefault.value = "rgba(0,0,0,0)";
    this->rules[OSS_KEY::BACKGROUND_COLOR] = backgroundColorDefault;
    
    ofxOssRule positionDefault;
    positionDefault.value = "0px 0px";
    this->rules[OSS_KEY::POSITION] = positionDefault;
    
    
    ofxOssRule bgPositionDefault;
    bgPositionDefault.value = "center";
    this->rules[OSS_KEY::BACKGROUND_POSITION] = bgPositionDefault;
    
    ofxOssRule widthDefault;
    widthDefault.value = "100%";
    this->rules[OSS_KEY::WIDTH] = widthDefault;
    
    ofxOssRule heightDefault;
    heightDefault.value = "100%";
    this->rules[OSS_KEY::HEIGHT] = heightDefault;
    
    ofxOssRule textAlignDefault;
    textAlignDefault.value = "left";
    this->rules[OSS_KEY::TEXT_ALIGN] = textAlignDefault;
    
    ofxOssRule fontSizeDefault;
    fontSizeDefault.value = "50";
    this->rules[OSS_KEY::FONT_SIZE] = fontSizeDefault;
    
    ofxOssRule bgSizeDefault;
    bgSizeDefault.value = "cover";
    this->rules[OSS_KEY::BACKGROUND_SIZE] = bgSizeDefault;
    
    ofxOssRule bgBlendModeDefault;
    bgBlendModeDefault.value = getStringFromBlendMode(OSS_BLEND_MODE::DISABLED);
    this->rules[OSS_KEY::BACKGROUND_BLEND_MODE] = bgBlendModeDefault;
    
    ofxOssRule opacityDefault;
    opacityDefault.value = "1.0f";
    this->rules[OSS_KEY::OPACITY] = opacityDefault;
    
    ofxOssRule textTransformDefault;
    textTransformDefault.value = "none";
    this->rules[OSS_KEY::TEXT_TRANSFORM] = textTransformDefault;
    
    ofxOssRule colorDefault;
    colorDefault.value = "rgb(0,0,0)";
    this->rules[OSS_KEY::COLOR] = colorDefault;
    
    ofxOssRule maskDefault;
    maskDefault.value = "";
    this->rules[OSS_KEY::MASK] = maskDefault;
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
    return this->rules[styleKey].value;
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
        default:
            ofLogWarning("ofxOSS::getEnumFromString","No string key found for value provided.");
    }
    return keyString;
}

OSS_BLEND_MODE::ENUM ofxOSS::getBlendModeFromString(string blendMode){
    if(blendMode == "disabled"){
        return OSS_BLEND_MODE::DISABLED;
    }
    else if(blendMode == "alpha"){
        return OSS_BLEND_MODE::ALPHA;
    }
    else if(blendMode == "add"){
        return OSS_BLEND_MODE::ADD;
    }
    else if(blendMode == "subtract"){
        return OSS_BLEND_MODE::SUBTRACT;
    }
    else if(blendMode == "screen"){
        return OSS_BLEND_MODE::SCREEN;
    }
    else if(blendMode == "multiply"){
        return OSS_BLEND_MODE::MULTIPLY;
    }
//    else if(blendMode == "overlay"){
//        return OSS_BLEND_MODE::OVERLAY;
//    }
//    else if(blendMode == "darken"){
//        return OSS_BLEND_MODE::DARKEN;
//    }
//    else if(blendMode == "lighten"){
//        return OSS_BLEND_MODE::LIGHTEN;
//    }
//    else if(blendMode == "color-dodge"){
//        return OSS_BLEND_MODE::COLOR_DODGE;
//    }
//    else if(blendMode == "color-burn"){
//        return OSS_BLEND_MODE::COLOR_BURN;
//    }
//    else if(blendMode == "hard-light"){
//        return OSS_BLEND_MODE::HARD_LIGHT;
//    }
//    else if(blendMode == "soft-light"){
//        return OSS_BLEND_MODE::SOFT_LIGHT;
//    }
//    else if(blendMode == "difference"){
//        return OSS_BLEND_MODE::DIFFERENCE;
//    }
//    else if(blendMode == "exclusion"){
//        return OSS_BLEND_MODE::EXCLUSION;
//    }
//    else if(blendMode == "hue"){
//        return OSS_BLEND_MODE::HUE;
//    }
//    else if(blendMode == "saturation"){
//        return OSS_BLEND_MODE::SATURATION;
//    }
//    else if(blendMode == "color"){
//        return OSS_BLEND_MODE::COLOR;
//    }
//    else if(blendMode == "luminosity"){
//        return OSS_BLEND_MODE::LUMINOSITY;
//    }
    else{
        ofLogWarning("ofxOSS::getBlendModeFromString","No blend-mode for "+blendMode+" found.");
        return OSS_BLEND_MODE::INVALID_BLEND_MODE;
    }
}

string ofxOSS::getStringFromBlendMode(OSS_BLEND_MODE::ENUM blendMode){
    string blendModeStr;
    switch(blendMode){
        case OSS_BLEND_MODE::DISABLED:
            blendModeStr = "disabled";
            break;
        case OSS_BLEND_MODE::ALPHA:
            blendModeStr = "alpha";
            break;
        case OSS_BLEND_MODE::ADD:
            blendModeStr = "add";
            break;
        case OSS_BLEND_MODE::SUBTRACT:
            blendModeStr = "subtract";
            break;
        case OSS_BLEND_MODE::SCREEN:
            blendModeStr = "screen";
            break;
        case OSS_BLEND_MODE::MULTIPLY:
            blendModeStr = "multiply";
            break;
//        case OSS_BLEND_MODE::OVERLAY:
//            blendModeStr = "overlay";
//            break;
//        case OSS_BLEND_MODE::DARKEN:
//            blendModeStr = "darken";
//            break;
//        case OSS_BLEND_MODE::LIGHTEN:
//            blendModeStr = "lighten";
//            break;
//        case OSS_BLEND_MODE::COLOR_DODGE:
//            blendModeStr = "color-dodge";
//            break;
//        case OSS_BLEND_MODE::COLOR_BURN:
//            blendModeStr = "color-burn";
//            break;
//        case OSS_BLEND_MODE::HARD_LIGHT:
//            blendModeStr = "hard-light";
//            break;
//        case OSS_BLEND_MODE::SOFT_LIGHT:
//            blendModeStr = "soft-light";
//            break;
//        case OSS_BLEND_MODE::DIFFERENCE:
//            blendModeStr = "difference";
//            break;
//        case OSS_BLEND_MODE::EXCLUSION:
//            blendModeStr = "exclusion";
//            break;
//        case OSS_BLEND_MODE::HUE:
//            blendModeStr = "hue";
//            break;
//        case OSS_BLEND_MODE::SATURATION:
//            blendModeStr = "saturation";
//            break;
//        case OSS_BLEND_MODE::COLOR:
//            blendModeStr = "color";
//            break;
//        case OSS_BLEND_MODE::LUMINOSITY:
//            blendModeStr = "luminosity";
//            break;
        default:
            ofLogWarning("ofxOSS::getStringFromBlendMode","No string for given OSS_BLEND_MODE found.");
    }
    return blendModeStr;
}


OSS_TEXT_TRANSFORM::ENUM ofxOSS::getTextTransformFromString(string textTransform){
    if(textTransform == "none"){
        return OSS_TEXT_TRANSFORM::NONE;
    }
    else if (textTransform == "uppercase"){
        return OSS_TEXT_TRANSFORM::UPPERCASE;
    }
    else if (textTransform == "lowercase"){
        return OSS_TEXT_TRANSFORM::LOWERCASE;
    }
    else if (textTransform == "capitalize"){
        return OSS_TEXT_TRANSFORM::CAPITALIZE;
    }
    else{
        ofLogWarning("ofxOSS::getTextTransformFromString","No text-transform for "+textTransform+" found.");
        return OSS_TEXT_TRANSFORM::INVALID_TEXT_TRANSFORM;
    }
}

string ofxOSS::getStringFromTextTransform(OSS_TEXT_TRANSFORM::ENUM textTransform){
    string textTransformStr;
    switch(textTransform){
        case OSS_TEXT_TRANSFORM::NONE:
            textTransformStr = "none";
            break;
        case OSS_TEXT_TRANSFORM::UPPERCASE:
            textTransformStr = "uppercase";
            break;
        case OSS_TEXT_TRANSFORM::LOWERCASE:
            textTransformStr = "lowercase";
            break;
        case OSS_TEXT_TRANSFORM::CAPITALIZE:
            textTransformStr = "capitalize";
            break;
        default:
            ofLogWarning("ofxOSS::getStringFromTextTransform","No string for given OSS_TEXT_TRANSFORM found.");
    }
    return textTransformStr;
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

ofxOssRule ofxOSS::generateRule(string key, string value){
    ofxOssRule ossRule;
    ossRule.value = value;
    return ossRule;
}

