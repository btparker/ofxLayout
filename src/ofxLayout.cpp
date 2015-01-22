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
    contextTreeRoot.update();
}

void ofxLayout::draw(){
    if(drawable()){
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
        applyChanges();
    }
    else{
        ofLogError("ofxLayout::loadFromFile","Unable to parse OSS file "+ossFilename+".");
    }
}

void ofxLayout::loadFromXmlLayout(ofxXmlSettings *xmlLayout, ofxLayoutElement* element, TAG::ENUM tagEnum, int which){
    string tag = ofxLayoutElement::getTagString(tagEnum);
    
    string id = xmlLayout->getAttribute(tag,"id", "", which);
    element->setID(id);
    
    string classes = xmlLayout->getAttribute(tag,"class", "", which);
    element->setClasses(classes);
    
    string style = xmlLayout->getAttribute(tag,"style", "", which);
    element->setInlineStyle(style);
    
    string value = populateValueExpressions(xmlLayout->getValue(tag,"", which));
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
    
    int numTextElements = xmlLayout->getNumTags(ofxLayoutElement::getTagString(TAG::TEXT));
    for(int i = 0; i < numTextElements; i++){
        ofxLayoutTextElement* child = new ofxLayoutTextElement();
        element->addChild(child);
        loadFromXmlLayout(xmlLayout, child, TAG::TEXT, i);
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
                styleObject->idMap[idName] = ofxOSS();
            }
            
            loadFromOss(&value, &(styleObject->idMap[idName]));
        }
        else if(keyIsClass){
            string className = key.substr(1);
            bool classExists = styleObject->classMap.count(className) > 0;
            
            if(!classExists){
                styleObject->classMap[className] = ofxOSS();
            }
            
            loadFromOss(&value, &(styleObject->classMap[className]));
        }
        else if(ofxOSS::validKey(key)){
            string value = populateValueExpressions((*jsonElement)[key].asString());
            ofxOssRule ossRule = ofxOSS::generateRule(key, value);
            
            styleObject->rules[ofxOSS::getOssKeyFromString(key)] = ossRule;
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

void ofxLayout::computeFbo(ofFbo* fboPtr, vector<string>* filters, ofxLayoutElement* startElement){
    if(startElement == NULL){
        startElement = &contextTreeRoot;
    }
    fboPtr->allocate(startElement->getFbo()->getWidth(), startElement->getFbo()->getHeight());
    ofPushMatrix();
    ofTranslate(-1*startElement->getBoundary().getX(), -1*startElement->getBoundary().getY());
    fboPtr->begin();
    ofBackground(255, 0, 0);
    ofEnableAlphaBlending();
    filterElements(filters, startElement);
    ofClearAlpha();
    fboPtr->end();
    ofPopMatrix();
}

void ofxLayout::filterElements(vector<string> *filters, ofxLayoutElement *element){
    vector<string> classes = ofSplitString(element->getClasses()," ",true,true);
    string elementID = element->getID();
    bool hasFilterID = false;
    bool hasFilterClass = false;
    for(vector<string>::iterator filter = filters->begin(); filter != filters->end(); ++filter) {
        bool filterIsId = filter->substr(0,1) == "#";
        bool filterIsClass = filter->substr(0,1) == ".";
        
        if(filterIsId){
            string idFilter = filter->substr(1,filter->length());
            hasFilterID = elementID == idFilter;
        }
        if(filterIsClass){
            string classFilter = filter->substr(1,filter->length());
            if (find(classes.begin(), classes.end(), classFilter) != classes.end()){
                hasFilterClass = true;
            }
        }
    }
    
    bool noFilters = filters->size() == 0;
    bool drawElement = noFilters || hasFilterID || hasFilterClass;
   
    if(drawElement){
        element->getFbo()->draw(0,0);
    }
    for(int i = 0 ; i < element->children.size(); i++){
        filterElements(filters, element->children[i]);
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

string ofxLayout::populateValueExpressions(string input){
    string value = input;
    if(ofStringTimesInString(input, "{{") > 0){
        string dataKey = input;
        string dataValue = "";
        string leftValue = "";
        string rightValue = "";
        vector<string> left = ofSplitString(dataKey, "{{", true);
        if(left.size() == 1){
            dataKey = left[0];
        }
        else{
            leftValue = left[0];
            left.erase(left.begin());
            dataKey = ofJoinString(left, "");
        }
        vector<string> right = ofSplitString(dataKey, "}}", true);
        dataKey = right[0];
        
        if(data.count(dataKey) > 0){
            dataValue = data[dataKey];
        }
        
        if(right.size() > 1){
            right.erase(right.begin());
            rightValue = populateValueExpressions(ofJoinString(right, ""));
        }
        value = leftValue + dataValue + rightValue;
    }
    return value;
}