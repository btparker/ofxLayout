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
            
            float fontSize;
            if(hasStyle(OSS_KEY::FONT_SIZE)){
                fontSize = ofToFloat(getStyle(OSS_KEY::FONT_SIZE));
            }
            
            ofRectangle fontBBox = fontPtr->getBBox(getValue(), fontSize, 0, 0);
            
            float x;
            float y = fontBBox.height;
            
            if(hasStyle(OSS_KEY::TEXT_ALIGN)){
                string textAlign = getStyle(OSS_KEY::TEXT_ALIGN);
                if(textAlign == "left"){
                    x = 0.0f;
                }
                else if(textAlign == "center"){
                    x = boundary.width/2-fontBBox.width/2;
                }
                else if(textAlign == "right"){
                    x = boundary.width - fontBBox.width;
                }
            }
            fontPtr->draw(getValue(), fontSize, x, y);
        }
    }
}