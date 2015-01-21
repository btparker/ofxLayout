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
            string text = getValue();
            if(hasStyle(OSS_KEY::TEXT_TRANSFORM)){
                vector<string> words = ofSplitString(text, " ",true,true);
                OSS_VALUE::ENUM textTransform = ofxOSS::getOssValueFromString(getStyle(OSS_KEY::TEXT_TRANSFORM));
                switch(textTransform){
                    case OSS_VALUE::NONE:
                        //do nothing
                        break;
                    case OSS_VALUE::UPPERCASE:
                        text = ofToUpper(text);
                        break;
                    case OSS_VALUE::LOWERCASE:
                        text = ofToLower(text);
                        break;
                    case OSS_VALUE::CAPITALIZE:
                        for(int i = 0; i < words.size(); i++) {
                            words[i] = ofToLower(words[i]);
                            words[i] = ofToUpper(words[i].substr(0,1))+words[i].substr(1,words[i].size());
                        }
                        text = ofJoinString(words, " ");
                        break;
                    default:
                        text = text;
                }
            }
            
            
            
            
            float fontSize;
            if(hasStyle(OSS_KEY::FONT_SIZE)){
                fontSize = ofToFloat(getStyle(OSS_KEY::FONT_SIZE));
            }
            
            ofRectangle fontBBox = fontsPtr->at(fontFilename).getBBox(text, fontSize, 0, 0);
            
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
            
            ofFill();
            if(hasStyle(OSS_KEY::COLOR)){
                string colorStr = getStyle(OSS_KEY::COLOR);
                ofColor fontColor = ofxOSS::parseColor(colorStr);
                ofSetColor(fontColor);
            }
            fontsPtr->at(fontFilename).draw(text, fontSize, x, y);
        }
    }
}