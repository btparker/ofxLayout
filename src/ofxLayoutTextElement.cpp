#include "ofxLayoutTextElement.h"

ofxLayoutTextElement::ofxLayoutTextElement(){
    ofxLayoutElement::ofxLayoutElement();
}

ofxLayoutTextElement::~ofxLayoutTextElement(){
    ofxLayoutElement::~ofxLayoutElement();
}

void ofxLayoutTextElement::drawTag(){
    if(hasStyle(OSS_KEY::FONT_FAMILY)){
        string fontFilename = getStyle(OSS_KEY::FONT_FAMILY);
        if(fontsPtr->count(fontFilename) > 0){
            ofFill();
            ofSetColor(255,255,255);
            ofxFontStash* fontPtr = fontsPtr->at(fontFilename);
            float fontSize = 50.0f;
            float x = 0.0f;
            float y = fontSize;
            fontPtr->draw(getValue(), fontSize, x, y);
        }
    }
}