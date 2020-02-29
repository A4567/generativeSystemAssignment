#pragma once

#include "ofMain.h"
#include "ofxFaceTracker2.h"
#include "ofxOpenCv.h"
#include "opencv2/opencv.hpp"
#include "ofxJSON.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    void face();
    void api();
    void setApi();
    void setFace();

    void keyPressed(int key);
		
    ofVideoGrabber cam;
    ofxFaceTracker2 tracker;
    
    vector<glm::vec2> facePoints;
    
    ofxFaceTracker2Landmarks::Feature faceFeature;
    
    ofImage camImg;
    ofPixels camPix;
    
    string url,apiKey,date;
    ofxJSONElement result;
    
    bool faceApi; // if true face if false Api
    
    ofPolyline line;
};