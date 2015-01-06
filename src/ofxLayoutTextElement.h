#include "ofxLayoutElement.h"

class ofxLayoutTextElement : public ofxLayoutElement{
public:
    ofxLayoutTextElement();
    ~ofxLayoutTextElement();
    
protected:
    void drawTag();
};