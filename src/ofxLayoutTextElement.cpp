#include "ofxLayoutTextElement.h"

ofxLayoutTextElement::ofxLayoutTextElement(){
    ofxLayoutElement::ofxLayoutElement();
}

ofxLayoutTextElement::~ofxLayoutTextElement(){
    ofxLayoutElement::~ofxLayoutElement();
}

void ofxLayoutTextElement::drawTag(){
    
    ofEnableAlphaBlending();
    if(hasStyle(OSS_KEY::FONT_FAMILY)){
        ofRectangle drawBox;
        
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
            
            float lineHeight;
            if(hasStyle(OSS_KEY::LINE_HEIGHT)){
                lineHeight = ofToFloat(getStyle(OSS_KEY::LINE_HEIGHT))/100.0f;
            }
            else{
                lineHeight = 1.0f;
            }
            
            fontsPtr->at(fontFilename).setLineHeight(lineHeight);
            
            float textMaxWidth = boundary.width;
            if(hasStyle(OSS_KEY::TEXT_MAX_WIDTH)){
                textMaxWidth = ofToFloat(getStyle(OSS_KEY::TEXT_MAX_WIDTH));
            }
            
            
            
            
            int numLines;
            ofRectangle fontBBox = fontsPtr->at(fontFilename).drawMultiLineColumn(text, fontSize, 0, 0, textMaxWidth,numLines, true, 0, true);
            drawBox.width = fontBBox.width;
            drawBox.height = fontBBox.height;
            
            
            float x;
            float y = fontSize*fontsPtr->at(fontFilename).getLineHeight();
            
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
            
            drawBox.x = x;
            drawBox.y = 0;
            
            if(hasStyle(OSS_KEY::TEXT_PADDING)){
                float paddingTop, paddingRight, paddingBottom, paddingLeft = 0.0f;
                vector<string> paddings = ofSplitString(getStyle(OSS_KEY::TEXT_PADDING), " ");
                if(paddings.size() == 1){
                    float padding = ofToFloat(paddings[0]);
                    paddingTop = padding;
                    paddingRight = padding;
                    paddingBottom = padding;
                    paddingLeft = padding;
                }
                else if(paddings.size() == 2){
                    float paddingV = ofToFloat(paddings[0]);
                    float paddingH = ofToFloat(paddings[1]);
                    paddingTop = paddingV;
                    paddingRight = paddingH;
                    paddingBottom = paddingV;
                    paddingLeft = paddingH;
                }
                
                else if(paddings.size() == 4){
                    paddingTop = ofToFloat(paddings[0]);
                    paddingRight = ofToFloat(paddings[1]);
                    paddingBottom = ofToFloat(paddings[2]);
                    paddingLeft = ofToFloat(paddings[3]);
                }
                
                x += paddingLeft;
                
                drawBox.width += paddingLeft+paddingRight;
                drawBox.height += paddingTop+paddingBottom;
                
            }
            
            ofFill();
            if(hasStyle(OSS_KEY::TEXT_BACKGROUND_COLOR)){
                string colorStr = getStyle(OSS_KEY::TEXT_BACKGROUND_COLOR);
                ofColor textBackgroundColor = ofxOSS::parseColor(colorStr);
                ofSetColor(textBackgroundColor);
                ofDrawRectangle(drawBox);
            }
            
            ofFill();
            if(hasStyle(OSS_KEY::COLOR)){
                string colorStr = getStyle(OSS_KEY::COLOR);
                ofColor fontColor = ofxOSS::parseColor(colorStr);
                ofSetColor(fontColor);
            }
            fontsPtr->at(fontFilename).drawMultiLineColumn(text, fontSize, x, y, textMaxWidth,numLines, false, 0, true);
        }
        
        ofDisableAlphaBlending();
    }
}