#include "ofxLayout.h"

/// |   Constructor/Destructor   | ///
/// | -------------------------- | ///

ofxLayout::ofxLayout(){
    contextTreeRoot.setAssets(&assets);
    contextTreeRoot.setFonts(&fonts);
    contextTreeRoot.setLayout(this);
    contextTreeRoot.styles = styleRulesRoot;
    
    // This is so that the functionality can be overwritten in the case of adding new tag types
    init();
}

ofxLayout::~ofxLayout(){
    unload();
}

/// |   Cycle Functions  | ///
/// | ------------------ | ///

void ofxLayout::update(){
    assets.update();
    animatableManager.update( 1.0f/ofGetTargetFrameRate() );
    contextTreeRoot.update();
}

void ofxLayout::draw(){
    ofBackground(0);
    if(drawable()){
        contextTreeRoot.draw();
    }
}

void ofxLayout::unload(){
    assets.getBatch("images")->clear();
    idElementMap.clear();
}
/// |   Utilities   | ///
/// | ------------- | ///
void ofxLayout::init(){
    assets.addBatch("images");
}

bool ofxLayout::ready(){
    return assets.isBatchReady("images");
}

bool ofxLayout::drawable(){
    return assets.isBatchDrawable("images");
}

void ofxLayout::loadFromFile(string filename){
    ofxJSONElement layoutConfig;
    bool layoutParsingSuccessful = layoutConfig.open(filename);
    if(layoutParsingSuccessful){
        loadDataFromFile(filename);
        loadOfmlFromFile(layoutConfig["template"].asString()+"/index.ofml");
        loadOssFromFile(layoutConfig["template"].asString()+"/styles.oss");
    }
    else{
        ofLogError("ofxLayout::loadFromFile","Unable to parse config file "+filename+".");
    }
}
void ofxLayout::loadDataFromFile(string dataFilename){
    ofxJSONElement jsonData;
    bool dataParsingSuccessful = jsonData.open(dataFilename);
    if(dataParsingSuccessful){
        vector<string> keys = jsonData.getMemberNames();
        for(int i = 0; i < keys.size(); i++){
            string key = keys[i];
            ofxJSONElement value = jsonData[key];
            data[key] = value.asString();
        }
    }
    else{
        ofLogError("ofxLayout::loadData","Unable to parse data json "+dataFilename+".");
    }
}

void ofxLayout::loadOfmlFromFile(string ofmlFilename){
    ofxXmlSettings xmlLayout;
    bool ofmlParsingSuccessful = xmlLayout.loadFile(ofmlFilename);
    if(ofmlParsingSuccessful){
        loadFromXmlLayout(&xmlLayout, &contextTreeRoot, TAG::BODY);
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
    styleRulesRoot.setDefaults();
    
    ofxJSONElement ossStylesheet;
    bool ossParsingSuccessful = ossStylesheet.open(ossFilename);
    if(ossParsingSuccessful){
        loadFromOss(&ossStylesheet, &styleRulesRoot);
        
        loadAnimationsFromOss(&ossStylesheet, &styleRulesRoot);
        loadAnimationInstancesFromOss(&ossStylesheet, &styleRulesRoot);

        applyChanges();
    }
    else{
        ofLogError("ofxLayout::loadFromFile","Unable to parse OSS file "+ossFilename+".");
    }
}

void ofxLayout::loadFromXmlLayout(ofxXmlSettings *xmlLayout, ofxLayoutElement* element, TAG::ENUM tagEnum, int which){
    string tag = ofxLayoutElement::getTagString(tagEnum);
    
    string id = xmlLayout->getAttribute(tag,"id", "", which);
    id = populateExpressions(id);
    element->setID(id);
    
    string classes = xmlLayout->getAttribute(tag,"class", "", which);
    classes = populateExpressions(classes);
    element->setClasses(classes);
    
    string style = xmlLayout->getAttribute(tag,"style", "", which);
    style = populateExpressions(style);
    element->setInlineStyle(style);
    
    string value = xmlLayout->getValue(tag,"", which);
    value = populateExpressions(value);
    element->setValue(value);
    // Push into current element, and load all children of different valid tag types
    xmlLayout->pushTag(tag, which);
    loadTags(xmlLayout, element);
    xmlLayout->popTag();
}

void ofxLayout::loadTags(ofxXmlSettings *xmlLayout, ofxLayoutElement* element){
    int numElements = xmlLayout->getNumTags(ofxLayoutElement::getTagString(TAG::ELEMENT));
    for(int i = 0; i < numElements; i++){
        ofxLayoutElement* child = new ofxLayoutElement();
        element->addChild(child);
        loadFromXmlLayout(xmlLayout, child, TAG::ELEMENT, i);
    }
}
void ofxLayout::loadAnimationsFromOss(ofxJSONElement* jsonElement, ofxOSS* styleObject){
    vector<string> keys = jsonElement->getMemberNames();
    for(int i = 0; i < keys.size(); i++){
        string key = keys[i];
        ofxJSONElement value = (*jsonElement)[key];
        if(ofStringTimesInString(key, "@keyframes") > 0){
            string animationName = ofSplitString(key, "@keyframes ", true, true)[0];
            ofxAnimation* animation = animatableManager.addAnimation(animationName);
            vector<string> keyframes = value.getMemberNames();
            for(int j = 0; j < keyframes.size(); j++){
                string keyframeKey = keyframes[j];
                ofxAnimationKeyframe* keyframe = animation->addKeyframe(keyframeKey);
                ofxJSONElement keyframeValue = (*jsonElement)[key][keyframeKey];
                vector<string> keyframeValueKeys = keyframeValue.getMemberNames();
                for(int k = 0; k < keyframeValueKeys.size(); k++){
                    OSS_TYPE::ENUM type = ofxOSS::getOssTypeFromOssKey(keyframeValueKeys[k]);
                    if(type == OSS_TYPE::COLOR){
                        keyframe->setValue(keyframeValueKeys[k], ofxOSS::parseColor(keyframeValue[keyframeValueKeys[k]].asString()));
                    }
                    if(type == OSS_TYPE::NUMBER){
                        keyframe->setValue(keyframeValueKeys[k], ofToFloat(keyframeValue[keyframeValueKeys[k]].asString()));
                    }
                }
                
            }
        }
    }
}

void ofxLayout::loadFromOss(ofxJSONElement* jsonElement, ofxOSS* styleObject){
    vector<string> keys = jsonElement->getMemberNames();
    for(int i = 0; i < keys.size(); i++){
        string key = keys[i];
        ofxJSONElement value = (*jsonElement)[key];
        bool keyIsId = key.substr(0,1) == "#";
        bool keyIsClass = key.substr(0,1) == ".";
        // ID
        if(keyIsId){
            string idName = key.substr(1);
            bool idExists = styleObject->idMap.count(idName) > 0;
            
            if(!idExists){
                styleObject->idMap[idName] = ofxOSS();
            }
            loadFromOss(&value, &(styleObject->idMap[idName]));
        }
        // Class
        else if(keyIsClass){
            string className = key.substr(1);
            bool classExists = styleObject->classMap.count(className) > 0;
            
            if(!classExists){
                styleObject->classMap[className] = ofxOSS();
            }

            loadFromOss(&value, &(styleObject->classMap[className]));
        }
//        else if(key == "animation" || ofStringTimesInString(key, "@keyframes") > 0){
//            return;
//        }
        // Style Key
        else if(ofxOSS::validKey(key)){
            string value = (*jsonElement)[key].asString();
            value = populateExpressions(value);
            styleObject->generateRule(key, value);
        }
        else{
            ofLogWarning("ofxLayout::loadFromOss","Unable to parse key "+key+".");
        }
    }
}

void ofxLayout::loadAnimationInstancesFromOss(ofxJSONElement* jsonElement, ofxOSS* styleObject){
    vector<string> keys = jsonElement->getMemberNames();
    for(int i = 0; i < keys.size(); i++){
        string key = keys[i];
        ofxJSONElement value = (*jsonElement)[key];
        bool keyIsId = key.substr(0,1) == "#";
        bool keyIsClass = key.substr(0,1) == ".";
        // ID
        if(keyIsId){
            string idName = key.substr(1);
            loadAnimationInstancesFromOss(&value, &(styleObject->idMap[idName]));
        }
        // Class
        else if(keyIsClass){
            string className = key.substr(1);
            loadAnimationInstancesFromOss(&value, &(styleObject->classMap[className]));
        }
        else if(key == "animation"){
            vector<string> animationParams = ofSplitString((*jsonElement)[key].asString(), " ");
            string animationName = animationParams[0];
            string animationID = animationParams[1];
            float duration = ofToFloat(animationParams[2]);
            float delay = ofToFloat(animationParams[3]);
            AnimCurve curve = ofxAnimatableManager::getCurveFromName(animationParams[4]);
            
            animationName = populateExpressions(animationName);
            
            if(animatableManager.hasAnimation(animationName)){
                ofxAnimationInstance* animationInstance = animatableManager.generateAnimationInstance(animationName, animationID);
                animationInstance->setDuration(duration);
                animationInstance->setDelay(delay);
                animationInstance->setCurve(curve);
                set<string> keyframeKeys = animatableManager.getAnimation(animationName)->getKeyframeSequence()[0]->getKeys();
                
                for(auto k : keyframeKeys) {
                    OSS_TYPE::ENUM type = ofxOSS::getOssTypeFromOssKey(k);
                    if(type == OSS_TYPE::COLOR){
                        animationInstance->setAnimatable(k, styleObject->getStyle(ofxOSS::getOssKeyFromString(k))->getAnimatableColor());
                    }
                    if(type == OSS_TYPE::NUMBER){
                        animationInstance->setAnimatable(k, styleObject->getStyle(ofxOSS::getOssKeyFromString(k))->getAnimatableFloat());
                    }
                }
                animationInstance->play();
            }
        }
    }
}

void ofxLayout::applyStyles(ofxLayoutElement* element, ofxOSS* styleObject){
    if(element == NULL){
        element = &contextTreeRoot;
    }
    if(styleObject == NULL){
        styleObject = &styleRulesRoot;
    }
    
    // Order is important! Styling override order is [ CLASS, ID, INLINE ]
    vector<string> classes = ofSplitString(element->getClasses(), " ");
    for(int i = 0; i < classes.size(); i++){
        if(styleRulesRoot.classMap.count(classes[i])){
            element->overrideStyles(&styleRulesRoot.classMap[classes[i]]);
        }
    }
    
    string id = element->getID();
    if(styleRulesRoot.idMap.count(id)){
        element->overrideStyles(&styleRulesRoot.idMap[id]);
    }
    
    ofxOSS inlineStyles = element->getInlineStyles();
    element->overrideStyles(&inlineStyles);
    
    // Get assets
    if(element->hasStyle(OSS_KEY::BACKGROUND_IMAGE)){
        string imageFilename = element->getStyle(OSS_KEY::BACKGROUND_IMAGE);
        ofxLoaderBatch* imagesBatch = assets.getBatch("images");
        if(!imagesBatch->hasTexture(imageFilename)){
            imagesBatch->addTexture(imageFilename, imageFilename);
        }
    }
    
    // Get fonts
    if(element->hasStyle(OSS_KEY::FONT_FAMILY)){
        string fontFilename = element->getStyle(OSS_KEY::FONT_FAMILY);
        
        if(fonts.count(fontFilename) == 0){
            fonts[fontFilename] = ofxFontStash();
            fonts[fontFilename].setup(fontFilename);
        }
    }
    
    for(int i = 0; i < element->children.size(); i++){
        applyStyles(element->children[i], &styleRulesRoot);
    }
}

ofxLayoutElement* ofxLayout::getElementById(string ID){
    if(idElementMap.count(ID) == 0){
        return NULL;
    }
    else{
        return idElementMap[ID];
    }
}

string ofxLayout::populateExpressions(string input){
    string value = input;
    while(ofStringTimesInString(value, "{{") > 0){
        string leftDeliminator = "{{";
        string rightDeliminator = "}}";
        
        int leftDeliminatorPos = input.find(leftDeliminator);
        int rightDeliminatorPos = input.find(rightDeliminator);
        
        int dataKeyPos = leftDeliminatorPos+leftDeliminator.length();
        int dataKeyLength = rightDeliminatorPos-dataKeyPos;

        string dataKey = input.substr(dataKeyPos, dataKeyLength);
        
        if(data.count(dataKey) > 0){
            ofStringReplace(value, leftDeliminator+dataKey+rightDeliminator, data[dataKey]);
        }
        else{
            ofLogWarning("ofxLayout::populateExpressions","Could not find data value for key '{{"+dataKey+"}}', replaced with ''.");
            ofStringReplace(value, leftDeliminator+dataKey+rightDeliminator, "");
        }
    }
    return value;
}