#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    faceApi = false;
    
    if(faceApi){
        ofSetBackgroundAuto(false);
        ofSetBackgroundColor(255);
        setFace();
    }else{
        //ofSetBackgroundAuto(false);
        ofSetBackgroundColor(255);
        setApi();
    }
    
    
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
        line.clear();
        api();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if(faceApi){
        face();
    }else{
        api();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::face(){
    for(int i = 0; i < tracker.instances.size(); i++){
        faceFeature = tracker.instances[i].landmarks.LEFT_EYE;
        
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
        float hue = ofMap(pointAvg, 0, screenAvg, 0, 255);
        //faceColor = facepix.getColor(facePoints[d].x, facePoints[d].y);
        faceColor.setHsb(hue, 255, 255, 20);
        ofSetColor(faceColor);
        ofDrawCircle(facePoints[d].x, facePoints[d].y, 5);
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
        colour.setHsb(hue,125,125,120);
        ofSetColor(colour);
        
        string dMax = result["near_earth_objects"][date][i]["estimated_diameter"]["meters"]["estimated_diameter_max"].asString();
        string dMin = result["near_earth_objects"][date][i]["estimated_diameter"]["meters"]["estimated_diameter_min"].asString();
        float d = (ofToFloat(dMin) + ofToFloat(dMax))/2;
        
        string missDist = result["near_earth_objects"][date][i]["close_approach_data"][0]["miss_distance"]["astronomical"].asString();
        float miss = ofToFloat(missDist);
        
        float posX = (i*1.3) + ofGetWidth()/2;
        float posY = (i*1.4) + ofGetHeight()/2;
        
        float speed = v/100;
        float radius = d*miss;
        
        ofVec2f point;
        point.x = ((ofGetWidth()*2) * ofNoise(ofGetElapsedTimef() * speed + posX)) - ofGetWidth()/2;
        point.y = ((ofGetHeight()*2) * ofNoise(ofGetElapsedTimef()  * speed + posY)) - ofGetHeight()/2;
        
        ofSetCircleResolution(360);
        //ofDrawCircle(point, radius);
        
        
        ofDrawCircle(point, radius);
        line.curveTo(point.x, point.y);
            
    }
    line.close();
    line.draw();
}
