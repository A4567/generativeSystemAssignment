#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    faceApi = false;
    
    if(faceApi){
        ofSetBackgroundAuto(false);
        ofSetBackgroundColor(0);
       
        setApi();
    }else{
        ofSetBackgroundAuto(false);
        ofSetBackgroundColor(255);
        setContour();
        setApi();
    }
    
    
}
void ofApp::setContour(){
    camW = 1280;//640;//cam.getWidth();
    camH = 720;//480;//cam.getHeight();
    cam.setup(camW, camH);
    
    colorImg.allocate(camW,camH);
    grayImage.allocate(camW,camH);
    grayBg.allocate(camW,camH);
    grayDiff.allocate(camW,camH);
    
    thresh = 30;
    contnum = 1;
}

void ofApp::setApi(){
    apiKey = "7mVxnXbbyWVMm4fXm4giYXGMb7WpTTmy8zAPuBRg";
    string year = ofToString(ofGetYear());
    string month = ofToString(ofGetMonth());
    if(month.length() < 2){
        month.append("0");
        reverse(month.begin(), month.end());
    }
    string day = ofToString(ofGetDay());
    if(day.length() < 2){
        day.append("0");
        reverse(day.begin(), day.end());
    }
    date = year + "-" + month + "-" + day;
    
    url = "https://api.nasa.gov/neo/rest/v1/feed?start_date="+ date +"&end_date="+ date +"&api_key="+apiKey;
    result.open(url);
}

//--------------------------------------------------------------
void ofApp::update(){
    time = ofGetElapsedTimef();
    line.clear();
    if(faceApi){
       
    }else{
        blobpoints.clear();
        // line.clear();
        cam.update();
        if (cam.isFrameNew()){
            colorImg.setFromPixels(cam.getPixels());
            colorImg.mirror(false,true);
            grayImage = colorImg; // convert our color image to a grayscale image
            if (bLearnBackground == true) {
                grayBg = grayImage; // update the background image
                bLearnBackground = false;
            }
            grayDiff.absDiff(grayBg, grayImage);
            grayDiff.threshold(thresh);
            contourFinder.findContours(grayDiff, 100, (camW*camH)/4, contnum, false, true);
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    //ofEnableBlendMode(OF_BLENDMODE_MULTIPLY);
    //ofDisableAlphaBlending();
    if(faceApi){
        
    }else{
        
        //colorImg.draw(0,0,camW,camH);
        ofPushMatrix();
        ofScale(1.5, 1.5);
        contour();
        ofPopMatrix();
               // contourFinder.draw(0,0,camW,camH);
                ofSetColor(255);
                ofDrawBitmapString(ofToString(contnum), 10,ofGetHeight()-10);
        ofDrawBitmapString(ofToString(thresh), 10,ofGetHeight()-20);
         ofDrawBitmapString(ofToString(time), ofGetWidth()-20,ofGetHeight()-20);
    }
    if(time % 30 == 0){
        ofSetColor(0, 0, 0, 5);
        ofDrawRectangle(0,0, ofGetWidth(), ofGetHeight());
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'b'){
        ofClear(0);
    }
    if(key == 'v'){
        ofClear(255);
    }
    if(key == ' '){
        bLearnBackground = true;
    }
    if(key == 'p'){
        contnum++;
    }
    if(key == 'q'){
        contnum--;
    }
    if(key == 'o'){
        thresh++;
    }
    if(key == 'w'){
        thresh--;
    }
}
//---
void ofApp::contour(){
     api();
    if(contourFinder.nBlobs > 0){
        for(int k = 0; k < contourFinder.nBlobs; k ++){
            for(int i = 0; i < contourFinder.blobs[k].pts.size(); i++){
                ofVec3f point(contourFinder.blobs[k].pts[i]);
                blobpoints.push_back(point);
            }
        }
        time = ofGetElapsedTimef();

        for (int j = 0; j < blobpoints.size(); j++) {
            
            ofColor faceColor;
            
            float pointAvg = (contourFinder.blobs[0].centroid.x + contourFinder.blobs[0].centroid.y)/2;//(blobpoints[j].y + blobpoints[j].x)/2;
            
            float screenAvg = (ofGetHeight() + ofGetWidth())/2;
            float camAvg = (cam.getHeight() + cam.getWidth())/2;
            float dist = ofDist(blobpoints[j].x, blobpoints[j].y, 0, 0);
            float hue = ofMap(pointAvg, 0, camAvg, 0, 255);
            

            faceColor.setHsb(hue, hue, 255, 5);
            ofSetColor(faceColor);

            line.curveTo(blobpoints[j]);
        }
        line.close();
       

           line.draw();
        
//        }
    }
}



//--------------------------------------------------------------
void ofApp::api(){
    
    for(int i = 0; i < result["near_earth_objects"][date].size(); i++){
        
        string kps = result["near_earth_objects"][date][i]["close_approach_data"][0]["relative_velocity"]["kilometers_per_second"].asString();
        float v = ofToFloat(kps);
        
        string cad = result["near_earth_objects"][date][i]["close_approach_data"][0]["close_approach_date_full"].asString();
        if(cad == ""){
            cad = ofToString(ofGetYear()) + "-" + "xxx" + "-" + ofToString(ofGetDay()) + " " + ofToString(ofGetHours()) + ":" + ofToString(ofGetMinutes());
        }
        cad.replace(0, 12, "");
        cad.erase(2,1);
        float hue = ofMap(ofToFloat(cad), 0000, 2359, 0, 255);
        ofColor colour;
        colour.setHsb(0,0,255,10);
        //ofSetColor(colour);
        
        string dMax = result["near_earth_objects"][date][i]["estimated_diameter"]["meters"]["estimated_diameter_max"].asString();
        string dMin = result["near_earth_objects"][date][i]["estimated_diameter"]["meters"]["estimated_diameter_min"].asString();
        float d = (ofToFloat(dMin) + ofToFloat(dMax))/2;
        
        string missDist = result["near_earth_objects"][date][i]["close_approach_data"][0]["miss_distance"]["astronomical"].asString();
        float miss = ofToFloat(missDist);
        
        float posX = (i*1.3) + cam.getWidth()/2;
        float posY = (i*1.4) + cam.getHeight()/2;
        
        float speed;
       
        
        float radius = d*miss;
        
        ofVec2f point;
        point.x = ((cam.getWidth()*2) * ofNoise(ofGetElapsedTimef() * speed + posX)) - cam.getWidth()/2;
        point.y = ((cam.getHeight()*2) * ofNoise(ofGetElapsedTimef()  * speed + posY)) - cam.getHeight()/2;
        
        ofSetCircleResolution(360);
        // ofDrawCircle(point, radius);
        
        
        //line.addVertex(ofVec3f(point.x,point.y,sin(speed)));
        line.curveTo(point.x, point.y);
        
    }
    //    line.close();
    //    line.draw();
}
