#include "ofxLayout.h"

/// |   Constructor/Destructor   | ///
/// | -------------------------- | ///

ofxLayout::ofxLayout(){
    contextTreeRoot = new ofxLayoutElement(&assets);
    styleRulesRoot = new ofxOSS();
    contextTreeRoot->styles = styleRulesRoot;
    assets.addBatch("images");
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

void ofxLayout::loadOfmlFromFile(string ofmlFilename){
    ofxXmlSettings xmlLayout;
    bool ofmlParsingSuccessful = xmlLayout.loadFile(ofmlFilename);
    if(ofmlParsingSuccessful){
        loadFromXmlLayout(&xmlLayout, contextTreeRoot);
    }
    else{
        ofLogError("ofxLayout::loadFromFile","Unable to parse OFML file "+ofmlFilename+".");
    }
    
    applyStyles();
    assets.loadBatch("images");
}

void ofxLayout::loadOssFromFile(string ossFilename){
    styleRulesRoot->setDefaults();
    
    ofxJSONElement ossStylesheet;
    bool ossParsingSuccessful = ossStylesheet.open(ossFilename);
    if(ossParsingSuccessful){
        loadFromOss(&ossStylesheet, styleRulesRoot);
        applyStyles();
        assets.loadBatch("images");
    }
    else{
        ofLogError("ofxLayout::loadFromFile","Unable to parse OSS file "+ossFilename+".");
    }
}

void ofxLayout::loadFromXmlLayout(ofxXmlSettings *xmlLayout, ofxLayoutElement* element, int which){
    string tag = "element";
    
    string id = xmlLayout->getAttribute(tag,"id", "", which);
    element->setID(id);
    
    string classes = xmlLayout->getAttribute(tag,"class", "", which);
    element->setClasses(classes);
    
    string style = xmlLayout->getAttribute(tag,"style", "", which);
    element->setInlineStyle(style);
    
    xmlLayout->pushTag(tag);
    int numElements = xmlLayout->getNumTags(tag);
    for(int i = 0; i < numElements; i++){
        ofxLayoutElement* childElement = new ofxLayoutElement(&assets);
        element->addChild(childElement);
        loadFromXmlLayout(xmlLayout, childElement,i);
    }
    xmlLayout->popTag();
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
    
    string tag = "element";
    
    vector<string> classes = ofSplitString(element->getClasses(), " ");
    for(int i = 0; i < classes.size(); i++){
        if(styleObject->classMap.count(classes[i])){
            element->overrideStyles(styleObject->classMap[classes[i]]);
        }
    }
    
    string id = element->getID();
    if(styleObject->idMap.count(id)){
        element->overrideStyles(styleObject->idMap[id]);
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
        applyStyles(element->children[i], element->styles);
    }
}