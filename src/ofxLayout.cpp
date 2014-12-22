#include "ofxLayout.h"

/// |   Constructor/Destructor   | ///
/// | -------------------------- | ///

ofxLayout::ofxLayout(){
    contextTreeRoot = new ofxLayoutElement();
    stylesheet = new ofxOSS();
}

ofxLayout::~ofxLayout(){
    delete contextTreeRoot;
    delete stylesheet;
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

void ofxLayout::loadFromFile(string ofmlFilename, string ossFilename){
    ofxXmlSettings xmlLayout;
    xmlLayout.loadFile(ofmlFilename);
    loadFromXmlLayout(&xmlLayout, contextTreeRoot);
    if (ossFilename != "") {
        stylesheet->loadFromFile(ossFilename);
        contextTreeRoot->setStylesheet(stylesheet);
    }
}

void ofxLayout::loadFromXmlLayout(ofxXmlSettings *xmlLayout, ofxLayoutElement* element, int which){
    string id = xmlLayout->getAttribute("element","id", "", which);
    element->setID(id);
    
    string style = xmlLayout->getAttribute("element","style", "", which);
    vector<string> styles;
    if(style != ""){
        styles = ofSplitString(style, ";", true, false);
    }
    for(int i = 0; i < styles.size(); i++){
        vector<string> stylePair = ofSplitString(styles[i], ":", true, true);
        element->setStyle(stylePair[0], stylePair[1]);
    }
    
    xmlLayout->pushTag("element",which);
    int numChildren = xmlLayout->getNumTags("element");
    for(int i = 0; i < numChildren; i++){
        ofxLayoutElement* child = new ofxLayoutElement();
        element->addChildElement(child);
        loadFromXmlLayout(xmlLayout,child,i);
    }
    xmlLayout->popTag();
}


