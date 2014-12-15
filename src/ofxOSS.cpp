#include "ofxOSS.h"

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

bool ofxOSS::hasStyle(OSS_KEY::ENUM key){
    return stylesMap.count(key) > 0;
}

bool ofxOSS::hasStyle(string key){
    return hasStyle(getEnumFromString(key));
}

float ofxOSS::getDimensionStyleValue(OSS_KEY::ENUM dimensionKey, float parentDimension){
    string dimensionValue = getStyle(dimensionKey);
    
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

ofColor ofxOSS::getColorStyle(string key){
    return getColorStyle(getEnumFromString(key));
}

ofColor ofxOSS::getColorStyle(OSS_KEY::ENUM key){
    if(getType(key) == OSS_TYPE::COLOR){
        return parseColorChannels(stylesMap[key]);
    }
    else{
        ofLogWarning("ofxOSS::getColor","No color found for key provided.");
        return ofColor::black;
    }
}

string ofxOSS::getStyle(string key){
    return getStyle(getEnumFromString(key));
}

string ofxOSS::getStyle(OSS_KEY::ENUM key){
    return stylesMap[key];
}

void ofxOSS::setStyle(OSS_KEY::ENUM key, string value){
    switch (getType(key)) {
        case OSS_TYPE::COLOR:
            stylesMap[key] = ofToString(parseColor(value));
            break;
        default:
            stylesMap[key] = value;
    }
}

void ofxOSS::setStyle(string key, string value){
    setStyle(getEnumFromString(key), value);
}

OSS_TYPE::ENUM ofxOSS::getType(string key){
    return getType(getEnumFromString(key));
}

OSS_TYPE::ENUM ofxOSS::getType(OSS_KEY::ENUM key){
    OSS_TYPE::ENUM type;
    switch(key){
        case OSS_KEY::BACKGROUND_COLOR:
            type = OSS_TYPE::COLOR;
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

OSS_KEY::ENUM ofxOSS::getEnumFromString(string key){
    OSS_KEY::ENUM keyEnum;
    if(key == "background-color"){
        return OSS_KEY::BACKGROUND_COLOR;
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

float ofxOSS::computeLeftPosition(string xStr, ofRectangle boundary, ofRectangle parentBoundary){
    float x;
    if(xStr == "center"){
        x = (parentBoundary.width/2.0)-(boundary.width/2.0)+parentBoundary.x;
    }
    else if(ofIsStringInString(xStr,"%")){
        float percent = (ofToFloat(ofSplitString(xStr,"%")[0])/100.0);
        x = percent*parentBoundary.width;
    }
    else if(ofIsStringInString(xStr,"px")){
        x = ofToFloat(ofSplitString(xStr,"px")[0])+parentBoundary.x;
    }
    else{
        x = ofToFloat(xStr)+parentBoundary.x;
    }
    return x;
}

float ofxOSS::computeTopPosition(string yStr, ofRectangle boundary, ofRectangle parentBoundary){
    float y;
    
    if(yStr == "center"){
        y = (parentBoundary.height/2.0)-(boundary.height/2.0)+parentBoundary.y;
    }
    else if(ofIsStringInString(yStr,"%")){
        float percent = (ofToFloat(ofSplitString(yStr,"%")[0])/100.0);
        y = percent*parentBoundary.height;
    }
    else if(ofIsStringInString(yStr,"px")){
        y = ofToFloat(ofSplitString(yStr,"px")[0])+parentBoundary.y;
    }
    else{
        y = ofToFloat(yStr)+parentBoundary.y;
    }

    return y;
}