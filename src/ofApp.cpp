#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    faceApi = false;
    
    if(faceApi){
        ofSetBackgroundAuto(false);
        ofSetBackgroundColor(0);
        setFace();
        setApi();
    }else{
        ofSetBackgroundAuto(false);
        ofSetBackgroundColor(0);
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
    
    thresh = 20;
}
void ofApp::setFace(){
    cam.setup(ofGetWidth(), ofGetHeight());
    tracker.setup();
    
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
    line.clear();
    if(faceApi){
        facePoints.clear();
        cam.update();
        if(cam.isFrameNew()){
            //tracker.update(cam);
            camPix = cam.getPixels();
            camImg.setFromPixels(camPix);
            camImg.mirror(false, true);
            camImg.setImageType(OF_IMAGE_COLOR);
            tracker.update(camImg);
        }
    }else{
        blobpoints.clear();
        // line.clear();
        cam.update();
        if (cam.isFrameNew()){
            colorImg.setFromPixels(cam.getPixels());
            grayImage = colorImg; // convert our color image to a grayscale image
            if (bLearnBackground == true) {
                grayBg = grayImage; // update the background image
                bLearnBackground = false;
            }
            grayDiff.absDiff(grayBg, grayImage);
            grayDiff.threshold(thresh);
            contourFinder.findContours(grayDiff, 500, (camW*camH)/4, 2, false, true);
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if(faceApi){
        face();
    }else{
        contour();
        
        //        contourFinder.draw(0,0,camW,camH);
        //        ofSetColor(255);
        //        ofDrawBitmapString(ofToString(thresh), 10,ofGetHeight()-10);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'b'){
        ofClear(0);
    }
    if(key == 'w'){
        ofClear(255);
    }
    if(key == ' '){
        bLearnBackground = true;
    }
    if(key == 'p'){
        thresh++;
    }
    if(key == 'q'){
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
        }  int time = ofGetElapsedTimef();ofSetColor(255);
               ofDrawBitmapString(ofToString(time), 10, ofGetHeight()-10);
        for (int j = 0; j < blobpoints.size(); j++) {
            
            ofColor faceColor;
            
            float pointAvg = (contourFinder.blobs[0].centroid.x + contourFinder.blobs[0].centroid.y)/2;//(blobpoints[j].y + blobpoints[j].x)/2;
            
            float screenAvg = (ofGetHeight() + ofGetWidth())/2;
            
            float dist = ofDist(blobpoints[j].x, blobpoints[j].y, 0, 0);
            float hue = ofMap(pointAvg, 0, screenAvg, 0, 255);
            
            
            faceColor.setHsb(hue, 255, 255, 10);
            ofSetColor(faceColor);
            line.addVertex(blobpoints[j]);
        }
        line.close();
       
        if(time % 3 == 0){
            line.draw();
        }
    }
}


//--------------------------------------------------------------
void ofApp::face(){
    api();
    
    for(int i = 0; i < tracker.instances.size(); i++){
        faceFeature = tracker.instances[i].landmarks.ALL_FEATURES;
        for(int j = 0; j < tracker.instances[i].landmarks.getFeatureIndices(faceFeature).size(); j++){
            int nbp = tracker.instances[i].landmarks.getFeatureIndices(faceFeature)[j];
            glm::vec2 pointF = tracker.instances[i].landmarks.getImagePoint(nbp);
            facePoints.push_back(pointF);
        }
    }
    
    for (int d = 0; d < facePoints.size(); d++){
        
        ofPixelsRef facepix = camImg.getPixels();
        ofColor faceColor;
        float pointAvg = (facePoints[d].y + facePoints[d].x)/2;
        float screenAvg = (ofGetHeight() + ofGetWidth())/2;
        
        float dist = ofDist(facePoints[d].x, facePoints[d].y, 0, 0);
        float hue = ofMap(pointAvg, 0, screenAvg, 0, 255);//ofMap(dist, 0, screenAvg, 0, 255);
        
        //        faceColor = facepix.getColor(facePoints[d].x, facePoints[d].y);
        faceColor.setHsb(hue, 255, 255, 10);
        ofSetColor(faceColor);
        
        //ofDrawCircle(facePoints[d].x, facePoints[d].y, 5);
        //line.curveTo(facePoints[d].x,facePoints[d].y);
        
        //line.addVertex(ofVec3f(cos(d*sin(facePoints[d].x))*200+ofGetWidth()/2,sin(cos(facePoints[d].y)*d)*200+ofGetHeight()/2,cos(d*7)*200));
        if(tracker.instances.size() > 0){
            line.addVertex(ofVec3f(facePoints[d].x,facePoints[d].y,0));
        }
    }
    
    
    line.close();
    line.draw();
    // ofSetColor(255);
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
        
        float posX = (i*1.3) + ofGetWidth()/2;
        float posY = (i*1.4) + ofGetHeight()/2;
        
        float speed;
        if(tracker.instances.size()>0){
            speed = v/15;
        }else{
            speed = v/100;
        }
        
        float radius = d*miss;
        
        ofVec2f point;
        point.x = ((ofGetWidth()*2) * ofNoise(ofGetElapsedTimef() * speed + posX)) - ofGetWidth()/2;
        point.y = ((ofGetHeight()*2) * ofNoise(ofGetElapsedTimef()  * speed + posY)) - ofGetHeight()/2;
        
        ofSetCircleResolution(360);
        // ofDrawCircle(point, radius);
        
        
        //line.addVertex(ofVec3f(point.x,point.y,sin(speed)));
        line.curveTo(point.x, point.y);
        
    }
    //    line.close();
    //    line.draw();
}
