#include "ofMain.h"
#include "ofxOSS.h"

class ofxLayoutElement{
    
public:
    void update();
    void draw();
    
    void setStyle(string styleKey, string styleValue);
    void setStyle(OSS_KEY::ENUM styleKey, string styleValue);
    
    string getStyle(string styleKey);
    string getStyle(OSS_KEY::ENUM styleKey);
    void applyStyles();
    
    ofxOSS stylesheet;
private:
    ofRectangle boundary;
    ofxLayoutElement* parentNode;
    vector<ofxLayoutElement*> childNodes;
    ofxOSS styles;
};