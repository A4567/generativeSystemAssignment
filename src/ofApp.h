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
   
    void api();
    void setApi();
   
    void setContour();
    
    void keyPressed(int key);
	
    void contour();
    
    ofVideoGrabber cam;
    
    ofImage camImg;
    ofPixels camPix;
    
    string url,apiKey,date;
    ofxJSONElement result;
    
    bool faceApi; // if true face if false Api
    
    ofPolyline line;
   
    int camW,camH;
    ofxCvColorImage colorImg;
    ofxCvGrayscaleImage grayImage, grayBg, grayDiff;
    ofxCvContourFinder contourFinder;
    vector<ofVec3f> blobpoints;
    bool bLearnBackground;
    int thresh,contnum;
    int time;
};
