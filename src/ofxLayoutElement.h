#include "ofMain.h"
#include "ofxOSS.h"
#include "ofxXmlSettings.h"

class ofxLayoutElement{
    
public:
    void update();
    void draw();
    
    void setStyle(string styleKey, string styleValue);
    void setStyle(OSS_KEY::ENUM styleKey, string styleValue);
    
    string getStyle(string styleKey);
    string getStyle(OSS_KEY::ENUM styleKey);
    void applyStyles();
    
    void updateDimensions();
    void updatePosition();
    
    void addChildElement(ofxLayoutElement* childElement);
    
    void loadFromFile(string filename);
    void loadFromLayout(ofxXmlSettings* layout, int which = 0);
    
    ofxOSS stylesheet;
    
    string getID();
    void setID(string _ID);
private:
    ofRectangle boundary;
    ofRectangle parentBoundary;
    ofxLayoutElement* parentNode;
    vector<ofxLayoutElement*> childNodes;
    ofxOSS styles;
    string ID;
};