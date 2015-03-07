#include "ofxLayout.h"
#include "MiscUtils.h"
#include "ofGraphics.h"


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
    loadTagElements(TAG::DIV, xmlLayout, element);
    loadTagElements(TAG::SVG, xmlLayout, element);
    loadTagElements(TAG::G, xmlLayout, element);
    loadTagElements(TAG::POLYGON, xmlLayout, element);
    loadTagElements(TAG::PATH, xmlLayout, element);
    loadTagElements(TAG::CIRCLE, xmlLayout, element);
}

void ofxLayout::loadTagElements(TAG::ENUM tag, ofxXmlSettings *xmlLayout, ofxLayoutElement* element){
    int numElements = xmlLayout->getNumTags(ofxLayoutElement::getTagString(tag));
    for(int i = 0; i < numElements; i++){
        ofxLayoutElement* child = new ofxLayoutElement();
        element->addChild(child);
        
        if(tag == TAG::PATH){
            ofPath* shape = child->initShape();
            string dStr = xmlLayout->getAttribute(ofxLayoutElement::getTagString(TAG::PATH),"d", "", i);
            
            // Good ol' ofxSVGPathParser, saving me here
            ofxSVGPathParser pp(shape);
            pp.parse(dStr);
        }
        else if(tag == TAG::POLYGON){
            ofPath* shape = child->initShape();
            
            // TODO: maybe not hard code this
            shape->setCurveResolution(50);
            string ptStr = xmlLayout->getAttribute(ofxLayoutElement::getTagString(TAG::POLYGON),"points", "", i);
            
            // Getting the points as a vector, then translating
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
        }
        else if(tag == TAG::CIRCLE){
            ofPath* shape = child->initShape();
            float x = xmlLayout->getAttribute(ofxLayoutElement::getTagString(TAG::CIRCLE),"cx", 0.0f, i);
            
            float y = xmlLayout->getAttribute(ofxLayoutElement::getTagString(TAG::CIRCLE),"cy", 0.0f, i);
            
            float r = xmlLayout->getAttribute(ofxLayoutElement::getTagString(TAG::CIRCLE),"r", 0.0f, i);
            
            shape->circle(0, 0, 5);
        }
        loadFromXmlLayout(xmlLayout, child, tag, i);
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
            if(startElement->children[i]->clickable()){
                hittest(pt,returnedElements,startElement->children[i]);
            }
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

void ofxLayout::applyAnimations(ofxAnimatableManager *am){
    for(pair<string, ofxAnimationInstance*> it : *am->getAnimationInstances()){
        bool hasState = false;
        bool isID = false;
        bool isClass = false;
        string selector = "";
        string state = "default";
        
        char c = it.first[0];
        
        if(c == '#'){
            isID = true;
        }
        else if(c == '.'){
            isClass = true;
        }
        
        for(char c : it.first){
            if(c == '#' || c == '.'){
                continue;
            }
            else if(c == ':'){
                state = "";
                hasState = true;
                continue;
            }
            
            if(hasState){
                state += c;
            }
            else{
                selector += c;
            }
        }
        
        if(isID){
            ofxLayoutElement* element = getElementById(selector);
            element->addState(state, it.second);
        }
        else if(isClass){
            for(ofxLayoutElement* element : getElementsByClass(selector)){
                element->addState(state, it.second);
            }
        }
    }
}