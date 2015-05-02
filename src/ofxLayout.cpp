#include "ofxLayout.h"
#include "ofGraphics.h"


/// |   Constructor/Destructor   | ///
/// | -------------------------- | ///

ofxLayout::ofxLayout(){
    mouseTransformation = ofMatrix4x4::newIdentityMatrix();
    init(0,0,ofGetViewportWidth(),ofGetViewportHeight());
}

ofxLayout::ofxLayout(ofPoint pos, ofRectangle dimensions){
    init(pos.x,pos.y,dimensions.width, dimensions.height);
}

ofxLayout::ofxLayout(int x, int y, int w, int h){
    init(x,y,w,h);
}

void ofxLayout::init(int x, int y, int w, int h){
    this->x = x;
    this->y = y;
    this->width = w;
    this->height = h;
    
    contextTreeRoot.setLayout(this);
    contextTreeRoot.styles = styleRulesRoot;
    
    
    
    assets.addBatch(IMAGES_BATCH);
}

void ofxLayout::enableMouseEvents(){
    ofAddListener(ofEvents().mouseMoved, this, &ofxLayout::mouseMoved);
    ofAddListener(ofEvents().mousePressed, this, &ofxLayout::mousePressed);
    ofAddListener(ofEvents().mouseReleased, this, &ofxLayout::mouseReleased);
    ofAddListener(ofEvents().mouseDragged, this, &ofxLayout::mouseDragged);
}

void ofxLayout::disableMouseEvents(){
    ofRemoveListener(ofEvents().mouseMoved, this, &ofxLayout::mouseMoved);
    ofRemoveListener(ofEvents().mousePressed, this, &ofxLayout::mousePressed);
    ofRemoveListener(ofEvents().mouseReleased, this, &ofxLayout::mouseReleased);
    ofRemoveListener(ofEvents().mouseDragged, this, &ofxLayout::mouseDragged);
}

map<string, ofxFontStash>* ofxLayout::getFonts(){
    return &fonts;
}

ofxLoaderSpool* ofxLayout::getAssets(){
    return &assets;
}

ofxLayoutElement* ofxLayout::hittest(ofPoint pt){
    ofxLayoutElement* hitElement = getBody();
    while(hitElement && !hitElement->children.empty()){
        ofxLayoutElement* element = NULL;
        for(int i = hitElement->children.size()-1; i >= 0; i--){
            ofxLayoutElement* child = hitElement->children[i];
            if(child->visible() && child->clickable() && child->hittest(pt)){
                element = child;
                break;
            }
        }
        if(element && hitElement != element){
            hitElement = element;
        }
        else{
            break;
        }
    }
    return hitElement;
}

void ofxLayout::setMouseTransformation(ofMatrix4x4 mouseTransformation){
    this->mouseTransformation = mouseTransformation;
}

void ofxLayout::mouseMoved(ofMouseEventArgs &args){
    ofPoint mousePt = ofPoint(args)*mouseTransformation;
    ofxLayoutElement* mouseMovedElement = hittest(mousePt);
    mouseMovedElement->mouseMoved(args);
    string evtStr = "mouseMoved";
    ofNotifyEvent(mouseMovedEvt, evtStr, mouseMovedElement);
}

void ofxLayout::mouseReleased(ofMouseEventArgs &args){
    ofPoint mousePt = ofPoint(args)*mouseTransformation;
    ofxLayoutElement* mouseReleasedElement = hittest(mousePt);
    mouseReleasedElement->mouseReleased(args);
    string evtStr = "mouseReleased";
    ofNotifyEvent(mouseReleasedEvt, evtStr, mouseReleasedElement);
}

void ofxLayout::mousePressed(ofMouseEventArgs &args){
    ofPoint mousePt = ofPoint(args)*mouseTransformation;
    ofxLayoutElement* mousePressedElement = hittest(mousePt);
    mousePressedElement->mousePressed(args);
    string evtStr = "mousePressed";
    ofNotifyEvent(mousePressedEvt, evtStr, mousePressedElement);
}

void ofxLayout::mouseDragged(ofMouseEventArgs &args){
    ofPoint mousePt = ofPoint(args)*mouseTransformation;
    ofxLayoutElement* mouseDraggedElement = hittest(mousePt);
    mouseDraggedElement->mouseDragged(args);
    string evtStr = "mouseDragged";
    ofNotifyEvent(mouseDraggedEvt, evtStr, mouseDraggedElement);
}

ofMatrix4x4 ofxLayout::getMouseTransformation(){
    return this->mouseTransformation;
}

ofxLayout::~ofxLayout(){
    disableMouseEvents();
    unload();
}

void ofxLayout::allocateBlurFbo(int w, int h){
    ofFbo::Settings s;
    s.width = w;
    s.height = h;
    s.internalformat = GL_RGB;
    s.maxFilter = GL_LINEAR;
    mFboBlur.setup(s, 5, 15.0);
    mFboBlur.setBlurOffset(20);
    mFboBlur.setBlurPasses(4);
    mFboBlur.setNumBlurOverlays(1);
}

/// |   Cycle Functions  | ///
/// | ------------------ | ///

void ofxLayout::update(){
//    cout << ofGetFrameRate() << endl;
    width = ofGetViewportWidth();
    height = ofGetViewportHeight();
    
    am.update(1.0f/60.0f);
    assets.update();
    contextTreeRoot.update();
}

void ofxLayout::draw(){
    if(ready()){
        contextTreeRoot.draw();
    }
}

void ofxLayout::unload(){
    fonts.clear();
    assets.getBatch(IMAGES_BATCH)->clear();
}

/// |   Utilities   | ///
/// | ------------- | ///


bool ofxLayout::ready(){
    return assets.isBatchReady(IMAGES_BATCH);
}

bool ofxLayout::drawable(){
    return assets.isBatchDrawable(IMAGES_BATCH);
}

void ofxLayout::loadFromFile(string filename){
    ofxJSONElement layoutConfig;
    bool layoutParsingSuccessful = layoutConfig.open(filename);
    if(layoutParsingSuccessful){
        loadDataFromFile(filename);
        loadOfmlFromFile(layoutConfig["template"].asString()+"/index.ofml");
        loadOssFromFile(layoutConfig["template"].asString()+"/styles.oss");
        loadAnimationsFromFile(layoutConfig["template"].asString()+"/animations.json");
    }
    else{
        ofLogError("ofxLayout::loadFromFile","Unable to parse config file "+filename+".");
    }
}

void ofxLayout::loadFromTemplate(string templateFolder, ofxJSONElement data){
    loadData(data);
    loadOfmlFromFile(templateFolder+"/index.ofml");
    loadOssFromFile(templateFolder+"/styles.oss");
    loadAnimationsFromFile(templateFolder+"/animations.json");
}

void ofxLayout::loadAnimationsFromFile(string animationsFilename){
    ofxJSONElement animationData;
    if(animationData.open(animationsFilename)){
        populateJSON(&animationData);
        am.load(animationData);
        applyAnimations();
    }
    contextTreeRoot.setState("default",true, true);
}

void ofxLayout::loadDataFromFile(string dataFilename){
    ofxJSONElement jsonData;
    bool dataParsingSuccessful = jsonData.open(dataFilename);
    if(dataParsingSuccessful){
        loadData(jsonData);
    }
    else{
        ofLogError("ofxLayout::loadData","Unable to parse data json "+dataFilename+".");
    }
}

void ofxLayout::loadData(ofxJSONElement jsonData){
    vector<string> keys = jsonData.getMemberNames();
    for(int i = 0; i < keys.size(); i++){
        string key = keys[i];
        ofxJSONElement value = jsonData[key];
        setData(key, value.asString());
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
        populateXML(&xmlLayout);
        loadFromXmlLayout(&xmlLayout, &contextTreeRoot, TAG::BODY);
    }
    else{
        ofLogError("ofxLayout::loadFromFile","Unable to parse OFML file "+ofmlFilename+".");
    }
    applyChanges();
}

void ofxLayout::applyChanges(){
    applyStyles();
    assets.loadBatch(IMAGES_BATCH);
    if(x || y){
        getBody()->setPosition(ofPoint(x,y));
    }
}

void ofxLayout::loadOssFromFile(string ossFilename){
    styleRulesRoot.setDefaults();
    
    ofxJSONElement ossStylesheet;
    bool ossParsingSuccessful = ossStylesheet.open(ossFilename);
    if(ossParsingSuccessful){
        populateJSON(&ossStylesheet);
        loadFromOss(&ossStylesheet, &styleRulesRoot);
        applyChanges();
        allocateBlurFbo(width, height);
    }
    else{
        ofLogError("ofxLayout::loadFromFile","Unable to parse OSS file "+ossFilename+".");
    }
}

void ofxLayout::loadFromXmlLayout(ofxXmlSettings *xmlLayout, ofxLayoutElement* element, TAG::ENUM tagEnum, int which){
    element->setTag(tagEnum);
    string tag = ofxLayoutElement::getTagString(tagEnum);
    
    string id = xmlLayout->getAttribute(tag,"id", "", which);
    element->setID(id);
    if(id != ""){
        idElementMap[id] = element;
    }
    
    string classes = xmlLayout->getAttribute(tag,"class", "", which);
    element->setClasses(classes);
    for(string classname : ofSplitString(classes, " ",true,true)){
        set<ofxLayoutElement*> cmap = classElementMap[classname];
        if(std::find(cmap.begin(), cmap.end(), element) != cmap.end()) {

        } else {
            /* v does not contain x */
            classElementMap[classname].insert(element);
        }
    }
    
    string style = xmlLayout->getAttribute(tag,"style", "", which);
    element->setInlineStyle(style);
    
    string value = xmlLayout->getValue(tag,"", which);
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
//            ofxSVGPathParser pp(shape);
//            pp.parse(dStr);
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
        ofxLoaderBatch* imagesBatch = assets.getBatch(IMAGES_BATCH);
        if(ofStringTimesInString(imageFilename, ":")){
            vector<string> ids = ofSplitString(imageFilename, ":");
            imagesBatch = assets.getBatch(IMAGES_BATCH)->getBatch(ids[0]);
            imageFilename = ids[1];
        }
        ofFile file(ofToDataPath(imageFilename));
        string bgImgExt = file.getExtension();
        if(bgImgExt == "svg" && ofFile::doesFileExist(imageFilename)){
            element->loadSvg(imageFilename);
        }
        else if(!imageFilename.empty() && !imagesBatch->hasTexture(imageFilename)){
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
                                      2048,
                                      true,
                                      8,
                                      4.0f
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

set<ofxLayoutElement*> ofxLayout::getElementsByClass(string classname){
    if(classElementMap.count(classname) == 0){
        return set<ofxLayoutElement*>();
    }
    else{
        return classElementMap[classname];
    }
}

void ofxLayout::populateExpressions(string& value){
    while(ofStringTimesInString(value, "{{") > 0){
        string leftDeliminator = "{{";
        string rightDeliminator = "}}";
        
        int leftDeliminatorPos = value.find(leftDeliminator);
        int rightDeliminatorPos = value.find(rightDeliminator);
        
        int dataKeyPos = leftDeliminatorPos+leftDeliminator.length();
        int dataKeyLength = rightDeliminatorPos-dataKeyPos;

        string dataKey = value.substr(dataKeyPos, dataKeyLength);
        
        if(data.count(dataKey) > 0){
            ofStringReplace(value, leftDeliminator+dataKey+rightDeliminator, data[dataKey]);
        }
        else{
            ofLogWarning("ofxLayout::populateExpressions","Could not find data value for key '{{"+dataKey+"}}', replaced with ''.");
            ofStringReplace(value, leftDeliminator+dataKey+rightDeliminator, "");
        }
    }
}

void ofxLayout::populateXML(ofxXmlSettings* xml){
    string xmlStr;
    xml->copyXmlToString(xmlStr);
    populateExpressions(xmlStr);
    xml->clear();
    xml->loadFromBuffer(xmlStr);
}

void ofxLayout::populateJSON(ofxJSONElement* json){
    string jsonStr = json->toStyledString();
    populateExpressions(jsonStr);
    json->clear();
    json->parse(jsonStr);
}

ofxLayoutElement* ofxLayout::getBody(){
    return &contextTreeRoot;
}

void ofxLayout::applyAnimations(){
    for(pair<string, ofxAnimationInstance*> it : *am.getAnimationInstances()){
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
            if(element && !element->hasState(state)){
                element->addState(state, am.cloneAnimationInstance(it.second->getID()));
            }
        }
        else if(isClass){
            set<ofxLayoutElement*> classElements = getElementsByClass(selector);
            for(ofxLayoutElement* element : classElements){
                if(!element->hasState(state)){
                    element->addState(state, am.cloneAnimationInstance(it.second->getID()));
                }
            }
        }
    }
}

int ofxLayout::getWidth(){
    return width;
}


int ofxLayout::getHeight(){
    return height;
}

ofPoint ofxLayout::getPosition(){
    return ofPoint(x,y);
}

void ofxLayout::removeElement(ofxLayoutElement* element){
    ofxLayoutElement* p = element->getParent();
    string id = element->getID();
    if(idElementMap[id]){
        idElementMap.erase(id);
    }
    for(string className : ofSplitString(element->getClasses()," ")){
        if(classElementMap[className].size() > 0){
            for(ofxLayoutElement* classEl : classElementMap[className]){
                if(classEl == element){
                    classElementMap[className].erase(element);
                    break;
                }
            }
        }
    }
    for(int i = 0; i < p->children.size(); i++){
        if(p->children[i] == element){
            p->children.erase(p->children.begin()+i);
        }
    }
}