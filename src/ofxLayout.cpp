#include "ofxLayout.h"

/// |   Constructor/Destructor   | ///
/// | -------------------------- | ///

ofxLayout::ofxLayout(){
    contextTreeRoot.setLayout(this);
    contextTreeRoot.styles = styleRulesRoot;
    
    ofAddListener(ofEvents().mouseMoved, this, &ofxLayout::mouseMoved);
    ofAddListener(ofEvents().mousePressed, this, &ofxLayout::mousePressed);
    ofAddListener(ofEvents().mouseReleased, this, &ofxLayout::mouseReleased);
    
    // This is so that the functionality can be overwritten in the case of adding new tag types
    init();
}

map<string, ofxFontStash>* ofxLayout::getFonts(){
    return &fonts;
}

ofxLoaderSpool* ofxLayout::getAssets(){
    return &assets;
}
void ofxLayout::mouseMoved(ofMouseEventArgs &args){
}

void ofxLayout::mouseReleased(ofMouseEventArgs &args){
}

void ofxLayout::mousePressed(ofMouseEventArgs &args){
}

ofxLayout::~ofxLayout(){
    ofRemoveListener(ofEvents().mouseMoved, this, &ofxLayout::mouseMoved);
    ofRemoveListener(ofEvents().mousePressed, this, &ofxLayout::mousePressed);
    ofRemoveListener(ofEvents().mouseReleased, this, &ofxLayout::mouseReleased);
    
    unload();
}

/// |   Cycle Functions  | ///
/// | ------------------ | ///

void ofxLayout::update(){
//    cout << ofGetFrameRate() << endl;
    assets.update();
//    animatableManager.update( 1.0f/ofGetTargetFrameRate() );
    contextTreeRoot.update();
    if(
       fbo.getWidth() != contextTreeRoot.getWidth() ||
       fbo.getHeight() != contextTreeRoot.getHeight()
    ){
        fbo.allocate(contextTreeRoot.getWidth(), contextTreeRoot.getHeight(), GL_RGBA);
    }
}

void ofxLayout::draw(){
    if(ready()){
        contextTreeRoot.draw();
    }
}

void ofxLayout::unload(){
    assets.getBatch("images")->clear();
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
            setData(key, value.asString());
        }
    }
    else{
        ofLogError("ofxLayout::loadData","Unable to parse data json "+dataFilename+".");
    }
}

void ofxLayout::setData(string key, string value){
    data[key] = value;
}

string ofxLayout::getData(string key){
    return data[key];
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
//        loadAnimationsFromOss(&ossStylesheet, &styleRulesRoot);
//        loadAnimationInstancesFromOss(&ossStylesheet, &styleRulesRoot);

        applyChanges();
    }
    else{
        ofLogError("ofxLayout::loadFromFile","Unable to parse OSS file "+ossFilename+".");
    }
}

void ofxLayout::loadFromXmlLayout(ofxXmlSettings *xmlLayout, ofxLayoutElement* element, TAG::ENUM tagEnum, int which){
    element->setTag(tagEnum);
    string tag = ofxLayoutElement::getTagString(tagEnum);
    
    string id = xmlLayout->getAttribute(tag,"id", "", which);
    id = populateExpressions(id);
    element->setID(id);
    if(id != ""){
        idElementMap[id] = element;
    }
    
    string classes = xmlLayout->getAttribute(tag,"class", "", which);
    classes = populateExpressions(classes);
    element->setClasses(classes);
    for(string classname : ofSplitString(classes, " ",true,true)){
        vector<ofxLayoutElement*> cmap = classElementMap[classname];
        if(std::find(cmap.begin(), cmap.end(), element) != cmap.end()) {

        } else {
            /* v does not contain x */
            classElementMap[classname].push_back(element);
        }
    }
    
    string style = xmlLayout->getAttribute(tag,"style", "", which);
    style = populateExpressions(style);
    element->setInlineStyle(style);
    
    string value = xmlLayout->getValue(tag,"", which);
    value = populateExpressions(value);
    element->setValue(value);
    
    
    vector<string> attributes;
    
    xmlLayout->getAttributeNames(tag, attributes, which);
    
    for(int i = 0; i < attributes.size(); i++){
        string attribute = attributes[i];
        if(attribute != "id" && attribute != "class" && ofxOSS::validKey(attribute)){
            element->appendInlineStyle(" "+attribute+" : "+xmlLayout->getAttribute(tag, attribute, "", which)+";");
        }
    }
    
    string file = xmlLayout->getAttribute(tag,"file", "", which);
    file = populateExpressions(file);
    
    if(file != ""){
        ofxXmlSettings fileLayout;
        bool fileParsingSuccessful = fileLayout.loadFile(file);
        if(fileParsingSuccessful){
            loadFromXmlLayout(&fileLayout, element, tagEnum, which);
        }
        else{
            ofLogError("ofxLayout::loadFromFile","Unable to parse OFML file "+file+".");
        }
    }
    else{
        // Push into current element, and load all children of different valid tag types
        xmlLayout->pushTag(tag, which);
        loadTags(xmlLayout, element);
        xmlLayout->popTag();
    }
}

void ofxLayout::loadTags(ofxXmlSettings *xmlLayout, ofxLayoutElement* element){
    int numElements = xmlLayout->getNumTags(ofxLayoutElement::getTagString(TAG::DIV));
    for(int i = 0; i < numElements; i++){
        ofxLayoutElement* child = new ofxLayoutElement();
        element->addChild(child);
        loadFromXmlLayout(xmlLayout, child, TAG::DIV, i);
    }
    
    int numSvg = xmlLayout->getNumTags(ofxLayoutElement::getTagString(TAG::SVG));
    for(int i = 0; i < numSvg; i++){
        ofxLayoutElement* child = new ofxLayoutElement();
        element->addChild(child);
        loadFromXmlLayout(xmlLayout, child, TAG::SVG, i);
    }
    
    int numG = xmlLayout->getNumTags(ofxLayoutElement::getTagString(TAG::G));
    for(int i = 0; i < numG; i++){
        ofxLayoutElement* child = new ofxLayoutElement();
        element->addChild(child);
        loadFromXmlLayout(xmlLayout, child, TAG::G, i);
    }
    
    int numPolygons = xmlLayout->getNumTags(ofxLayoutElement::getTagString(TAG::POLYGON));
    for(int i = 0; i < numPolygons; i++){
        ofxLayoutElement* child = new ofxLayoutElement();
        element->addChild(child);
        ofPath* shape = child->getShape();
        shape->setCurveResolution(50);
        string ptStr = xmlLayout->getAttribute(ofxLayoutElement::getTagString(TAG::POLYGON),"points", "", i);
        vector<string> ptsStr = ofSplitString(ptStr, " ", true, true);
        for(int j = 0; j < ptsStr.size(); j++){
            vector<string> ptVec = ofSplitString(ptsStr[j],",", true, true);
            ofPoint pt(ofToFloat(ptVec[0]),ofToFloat(ptVec[1]));
            if(j==0){
                shape->moveTo(pt);
            }
            else{
                shape->lineTo(pt);
            }
        }
        shape->close();
        loadFromXmlLayout(xmlLayout, child, TAG::POLYGON, i);
    }
    
    // SVG Path Commands
    //    M = moveto
    //    L = lineto
    //    H = horizontal lineto
    //    V = vertical lineto
    //    C = curveto
    //    S = smooth curveto
    //    Q = quadratic Bézier curve
    //    T = smooth quadratic Bézier curveto
    //    A = elliptical Arc
    //    Z = closepath
    int numPaths = xmlLayout->getNumTags(ofxLayoutElement::getTagString(TAG::PATH));
    for(int i = 0; i < numPaths; i++){
        ofxLayoutElement* child = new ofxLayoutElement();
        element->addChild(child);
        ofPath* shape = child->getShape();
        string dStr = xmlLayout->getAttribute(ofxLayoutElement::getTagString(TAG::PATH),"d", "", i);
        ofxSVGPathParser pp(shape);
        pp.parse(dStr);
        loadFromXmlLayout(xmlLayout, child, TAG::PATH, i);
    }
}


//void ofxLayout::loadAnimationsFromOss(ofxJSONElement* jsonElement, ofxOSS* styleObject){
//    vector<string> keys = jsonElement->getMemberNames();
//    for(int i = 0; i < keys.size(); i++){
//        string key = keys[i];
//        ofxJSONElement value = (*jsonElement)[key];
//        if(ofStringTimesInString(key, "@keyframes") > 0){
//            string animationName = ofSplitString(key, "@keyframes ", true, true)[0];
//            ofxAnimation* animation = animatableManager.addAnimation(animationName);
//            vector<string> keyframes = value.getMemberNames();
//            for(int j = 0; j < keyframes.size(); j++){
//                string keyframeKey = keyframes[j];
//                ofxAnimationKeyframe* keyframe = animation->addKeyframe(keyframeKey);
//                ofxJSONElement keyframeValue = (*jsonElement)[key][keyframeKey];
//                vector<string> keyframeValueKeys = keyframeValue.getMemberNames();
//                for(int k = 0; k < keyframeValueKeys.size(); k++){
//                    OSS_TYPE::ENUM type = ofxOSS::getOssTypeFromOssKey(keyframeValueKeys[k]);
//                    if(type == OSS_TYPE::COLOR){
//                        keyframe->setValue(keyframeValueKeys[k], ofxOSS::parseColor(keyframeValue[keyframeValueKeys[k]].asString()));
//                    }
//                    if(type == OSS_TYPE::NUMBER){
//                        keyframe->setValue(keyframeValueKeys[k], ofToFloat(keyframeValue[keyframeValueKeys[k]].asString()));
//                    }
//                }
//                
//            }
//        }
//    }
//}

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
//        else if(ofStringTimesInString(key,"animation") > 0 || ofStringTimesInString(key, "@keyframes") > 0){
//            continue;
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

//void ofxLayout::loadAnimationInstancesFromOss(ofxJSONElement* jsonElement, ofxOSS* styleObject, ofxLayoutElement* element){
//    vector<string> keys = jsonElement->getMemberNames();
//    for(int i = 0; i < keys.size(); i++){
//        string key = keys[i];
//        ofxJSONElement value = (*jsonElement)[key];
//        bool keyIsId = key.substr(0,1) == "#";
//        bool keyIsClass = key.substr(0,1) == ".";
//        // ID
//        if(keyIsId){
//            string idName = key.substr(1);
//            loadAnimationInstancesFromOss(&value, &(styleObject->idMap[idName]), idElementMap[idName]);
//        }
//        // Class
//        else if(keyIsClass){
//            string className = key.substr(1);
//            for(auto classElement : getElementsByClass(className)){
//                loadAnimationInstancesFromOss(&value, &(styleObject->classMap[className]), classElement);
//            }
//        }
//        else if(ofStringTimesInString(key, "animation") > 0){
//            if(element == NULL){
//                continue;
//            }
//            string animation = (*jsonElement)[key].asString();
//            string animationID;
//            string animationStateID;
//            if(ofStringTimesInString(key, "animation-")>0){
//                animationStateID = key.substr(string("animation-").length());
//                
//            }
//            else{
//                animationStateID = "default";
//            }
//            
//            animationID = ofToString(element)+":"+animationStateID;
//            cout << animationID << endl;
//
//            
//            animation = populateExpressions(animation);
//            vector<string> animationParams = ofSplitString(animation, " ");
//            string animationName = animationParams[0];
//            
//            float duration = ofToFloat(animationParams[1]);
//            float delay = ofToFloat(animationParams[2]);
//            AnimCurve curve = ofxAnimatableManager::getCurveFromName(animationParams[3]);
//            
//            animationName = populateExpressions(animationName);
//            
//            if(animatableManager.hasAnimation(animationName)){
//                element->copyStyles(styleObject);
//                ofxAnimationInstance* animationInstance = animatableManager.generateAnimationInstance(animationName, animationID);
//                animationInstance->setStateID(animationStateID);
//                animationInstance->setDuration(duration);
//                animationInstance->setDelay(delay);
//                animationInstance->setCurve(curve);
//                set<string> keyframeKeys = animatableManager.getAnimation(animationName)->getKeyframeSequence()[0]->getKeys();
//                
//                for(auto k : keyframeKeys) {
//                    OSS_TYPE::ENUM type = ofxOSS::getOssTypeFromOssKey(k);
//                    if(type == OSS_TYPE::COLOR){
//                        animationInstance->setAnimatable(k, element->getStyle(ofxOSS::getOssKeyFromString(k))->getAnimatableColor());
//                    }
//                    if(type == OSS_TYPE::NUMBER){
//                        animationInstance->setAnimatable(k, element->getStyle(ofxOSS::getOssKeyFromString(k))->getAnimatableFloat());
//                    }
//                }
//                element->addAnimationState(animationStateID,animationInstance);
//            }
////            if(animationStateID == "default"){
////                element->setState(animationStateID);
////            }
//        }
//    }
//}

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
    element->copyStyles(&inlineStyles);
    
    // Get assets
    if(element->hasStyle(OSS_KEY::BACKGROUND_IMAGE)){
        string imageFilename = element->getStringStyle(OSS_KEY::BACKGROUND_IMAGE);
        ofxLoaderBatch* imagesBatch = assets.getBatch("images");
        if(!imagesBatch->hasTexture(imageFilename)){
            imagesBatch->addTexture(imageFilename, imageFilename);
        }
    }
    
    // Get fonts
    if(element->hasStyle(OSS_KEY::FONT_FAMILY)){
        string fontFilename = element->getStringStyle(OSS_KEY::FONT_FAMILY);
        
        if(fonts.count(fontFilename) == 0){
            fonts[fontFilename] = ofxFontStash();
            fonts[fontFilename].setup(fontFilename,
                                      1.0,
                                      1024,
                                      true,
                                      8,
                                      2.0f
                                      );
        }
    }
    
    if(element->getTag() == TAG::POLYGON || element->getTag() == TAG::G || element->getTag() == TAG::PATH){
        element->getStyle(OSS_KEY::POSITION)->setOssValue(OSS_VALUE::ABSOLUTE);
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

vector<ofxLayoutElement*> ofxLayout::getElementsByClass(string classname){
    if(classElementMap.count(classname) == 0){
        return vector<ofxLayoutElement*>();
    }
    else{
        return classElementMap[classname];
    }
}

ofxLayoutElement* ofxLayout::hittest(ofPoint pt, vector<ofxLayoutElement*>* returnedElements, ofxLayoutElement* startElement){
    if(returnedElements == NULL){
        returnedElements = new vector<ofxLayoutElement*>();
    }
    if(startElement == NULL){
        startElement = &contextTreeRoot;
    }
    // If intersects
    if(startElement->getBoundary().inside(pt)){
        returnedElements->push_back(startElement);
        for(int i = 0; i < startElement->children.size(); i++){
            hittest(pt,returnedElements,startElement->children[i]);
        }
    }
    if(returnedElements->size() > 0){
        return returnedElements->at(returnedElements->size()-1);
    }
    else{
        return NULL;
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

ofxLayoutElement* ofxLayout::getBody(){
    return &contextTreeRoot;
}