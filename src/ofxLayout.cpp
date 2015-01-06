#include "ofxLayout.h"

/// |   Constructor/Destructor   | ///
/// | -------------------------- | ///

ofxLayout::ofxLayout(){
    contextTreeRoot = new ofxLayoutElement(&assets);
    styleRulesRoot = new ofxOSS();
    contextTreeRoot->styles = styleRulesRoot;
    
    // This is so that the functionality can be overwritten in the case of adding new tag types
    init();
}

ofxLayout::~ofxLayout(){
    delete contextTreeRoot;
    delete styleRulesRoot;
}

/// |   Cycle Functions  | ///
/// | ------------------ | ///

void ofxLayout::update(){
    contextTreeRoot->update();
}

void ofxLayout::draw(){
    contextTreeRoot->draw();
}

/// |   Utilities   | ///
/// | ------------- | ///
void ofxLayout::init(){
    assets.addBatch("images");
}

string ofxLayout::getTagString(TAG::ENUM tagEnum){
    string tag = "";
    switch (tagEnum) {
        case TAG::BODY:
            tag = "body";
            break;
        case TAG::ELEMENT:
            tag = "element";
            break;
        case TAG::TEXT:
            tag = "text";
            break;
        default:
            ofLogWarning("ofxLayout::getTagString","Can't find corresponding string for enum");
            break;
    }
    return tag;
}

TAG::ENUM ofxLayout::getTagEnum(string tagString){
    TAG::ENUM tag = TAG::INVALID;
    if(tagString == "body"){
        return TAG::BODY;
    }
    else if(tagString == "element") {
        return TAG::ELEMENT;
    }
    else if(tagString == "text") {
        return TAG::TEXT;
    }
    else{
        ofLogWarning("ofxLayout::getTagString","Can't find corresponding enum for tag string '"+tagString+"'");
        return TAG::INVALID;
    }
}

void ofxLayout::loadOfmlFromFile(string ofmlFilename){
    ofxXmlSettings xmlLayout;
    bool ofmlParsingSuccessful = xmlLayout.loadFile(ofmlFilename);
    if(ofmlParsingSuccessful){
        loadFromXmlLayout(&xmlLayout, contextTreeRoot, TAG::BODY);
    }
    else{
        ofLogError("ofxLayout::loadFromFile","Unable to parse OFML file "+ofmlFilename+".");
    }
    applyChanges();
}

void ofxLayout::applyChanges(){
    applyStyles();
    assets.loadBatch("images");
}

void ofxLayout::loadOssFromFile(string ossFilename){
    styleRulesRoot->setDefaults();
    
    ofxJSONElement ossStylesheet;
    bool ossParsingSuccessful = ossStylesheet.open(ossFilename);
    if(ossParsingSuccessful){
        loadFromOss(&ossStylesheet, styleRulesRoot);
        applyChanges();
    }
    else{
        ofLogError("ofxLayout::loadFromFile","Unable to parse OSS file "+ossFilename+".");
    }
}

void ofxLayout::loadFromXmlLayout(ofxXmlSettings *xmlLayout, ofxLayoutElement* element, TAG::ENUM tagEnum, int which){
    string tag = getTagString(tagEnum);
    
    string id = xmlLayout->getAttribute(tag,"id", "", which);
    element->setID(id);
    
    string classes = xmlLayout->getAttribute(tag,"class", "", which);
    element->setClasses(classes);
    
    string style = xmlLayout->getAttribute(tag,"style", "", which);
    element->setInlineStyle(style);
    
    // Push into current element, and load all children of different valid tag types
    xmlLayout->pushTag(tag, which);
    loadTags(xmlLayout, element);
    xmlLayout->popTag();
}

void ofxLayout::loadTags(ofxXmlSettings *xmlLayout, ofxLayoutElement* element){
    int numElements = xmlLayout->getNumTags(getTagString(TAG::ELEMENT));
    for(int i = 0; i < numElements; i++){
        ofxLayoutElement* childElement = new ofxLayoutElement(&assets);
        element->addChild(childElement);
        loadFromXmlLayout(xmlLayout, childElement, TAG::ELEMENT, i);
    }
    
    int numTextElements = xmlLayout->getNumTags(getTagString(TAG::TEXT));
    for(int i = 0; i < numTextElements; i++){
        ofxLayoutElement* childElement = new ofxLayoutElement(&assets);
        element->addChild(childElement);
        loadFromXmlLayout(xmlLayout, childElement, TAG::TEXT, i);
    }
}

void ofxLayout::loadFromOss(ofxJSONElement* jsonElement, ofxOSS* styleObject){
    vector<string> keys = jsonElement->getMemberNames();
    for(int i = 0; i < keys.size(); i++){
        string key = keys[i];
        ofxJSONElement value = (*jsonElement)[key];
        bool keyIsId = key.substr(0,1) == "#";
        bool keyIsClass = key.substr(0,1) == ".";
        if(keyIsId){
            string idName = key.substr(1);
            bool idExists = styleObject->idMap.count(idName) > 0;
            
            if(!idExists){
                styleObject->idMap[idName] = new ofxOSS();
            }
            
            loadFromOss(&value, styleObject->idMap[idName]);
        }
        else if(keyIsClass){
            string className = key.substr(1);
            bool classExists = styleObject->classMap.count(className) > 0;
            
            if(!classExists){
                styleObject->classMap[className] = new ofxOSS();
            }
            
            loadFromOss(&value, styleObject->classMap[className]);
        }
        else if(ofxOSS::validKey(key)){
            ofxOssRule* ossRule = ofxOSS::generateRule(key, (*jsonElement)[key].asString());
            
            styleObject->rules[ofxOSS::getEnumFromString(key)] = ossRule;
        }
        else{
            ofLogWarning("ofxLayout::loadFromOss","Unable to parse key "+key+".");
        }
    }
}

void ofxLayout::applyStyles(ofxLayoutElement* element, ofxOSS* styleObject){
    if(element == NULL){
        element = contextTreeRoot;
    }
    if(styleObject == NULL){
        styleObject = styleRulesRoot;
    }
    
    // Order is important! Styling override order is [ CLASS, ID, INLINE ]
    vector<string> classes = ofSplitString(element->getClasses(), " ");
    for(int i = 0; i < classes.size(); i++){
        if(styleRulesRoot->classMap.count(classes[i])){
            element->overrideStyles(styleRulesRoot->classMap[classes[i]]);
        }
    }
    
    string id = element->getID();
    if(styleRulesRoot->idMap.count(id)){
        element->overrideStyles(styleRulesRoot->idMap[id]);
    }
    
    ofxOSS* inlineStyles = element->getInlineStyles();
    element->overrideStyles(inlineStyles);
    
    // Get assets
    if(element->hasStyle(OSS_KEY::BACKGROUND_IMAGE)){
        string imageFilename = element->getStyle(OSS_KEY::BACKGROUND_IMAGE);
        ofxLoaderBatch* imagesBatch = assets.getBatch("images");
        if(!imagesBatch->hasTexture(imageFilename)){
            imagesBatch->addTexture(imageFilename, imageFilename);
        }
    }
    
    for(int i = 0; i < element->children.size(); i++){
        applyStyles(element->children[i], styleRulesRoot);
    }
}