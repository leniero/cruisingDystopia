#include "ofApp.h"
//#include "helpers.h"


//--------------------------------------------------------------
void ofApp::setup(){
    ofEnableSmoothing();
    ofSetVerticalSync(true);
    
    video.setup(320, 240);
    finder.setup("haarcascade_frontalface_default.xml");
    
    
    //FOR USE WITH KINECT
    //finder.setup("haarcascade_frontalface_alt2.xml");
    //finder.setup("haarcascade_frontalface_alt_tree.xml");
    //ofSetLogLevel(OF_LOG_VERBOSE);
    //kinects.open();
    
    
    usePreview = false;
    
    previewCamera.setDistance(5.0f);
    previewCamera.setNearClip(0.01f);
    previewCamera.setFarClip(1000.0f);
    previewCamera.setPosition(0.0f, 0.0f, 0.0f);
    previewCamera.lookAt(glm::vec3(1.4f, 1.2f, 1.8f));
    
    headTrackedCamera.setNearClip(0.01f);
    headTrackedCamera.setFarClip(1000.0f);
    
    //defining the real world coordinates of the window which is being headtracked
    windowWidth = 0.3f;
    windowHeight = 0.2f;
    
    windowTopLeft = glm::vec3(-windowWidth / 2.0f,
                              +windowHeight / 2.0f,
                              0.0f);
    windowBottomLeft = glm::vec3(-windowWidth / 2.0f,
                                 - windowHeight / 2.0f,
                                 0.0f);
    windowBottomRight = glm::vec3(+windowWidth / 2.0f,
                                  -windowHeight / 2.0f,
                                  0.0f);
    
  
    viewerDistance = 0.15f;
    
    
    
    
    //SCENE
    ofDisableArbTex();
    ofEnableNormalizedTexCoords();
    
    greenOpen.set(500,500);
    
    
    heathVid.load("movies/cruisingDystopia.mov");
    heathVid2.load("movies/cruisingDystopia2.mov");
    heathVid.play();
    heathVid2.play();
    
   
    heathMat.setEmissiveColor(ofFloatColor(1.0, 1.0, 1.0));
    heathMat.setSpecularColor(ofFloatColor(0.1, 0.1, 0.1));
    heathMat.setAmbientColor(ofFloatColor(0.7, 0.7, 0.7));
    
    
    // Setup lights
    myLight.setDiffuseColor(ofFloatColor(1, 1, 0));
    myLight.setSpecularColor(ofFloatColor(1, 1, 0));
    myLight.setAreaLight(2,2);
    myLight.setPosition(vec3(0, -2, -10));
  
    
    
    //TOILET MESH
    toiletVid.load("movies/toilet.mov");
   
    
    //store the width and height for convenience
    int width = toiletVid.getWidth();
    int height = toiletVid.getHeight();
    
    //add one vertex to the mesh for each pixel
    for (int y = 0; y < height; y++){
        for (int x = 0; x<width; x++){
            toiletMesh.addVertex(glm::vec3(x,y,0));    // mesh index = x + y*width
        
            toiletMesh.addColor(ofFloatColor(0,0,0));
        }
    }
    
    for (int y = 0; y<height-1; y++){
        for (int x=0; x<width-1; x++){
            toiletMesh.addIndex(x+y*width);                // 0
            toiletMesh.addIndex((x+1)+y*width);            // 1
            toiletMesh.addIndex(x+(y+1)*width);            // 10
            
            toiletMesh.addIndex((x+1)+y*width);            // 1
            toiletMesh.addIndex((x+1)+(y+1)*width);        // 11
            toiletMesh.addIndex(x+(y+1)*width);            // 10
        }
    }
    
    //this determines how much we push the meshes out
    extrusionAmount = 300.0;
    
    
    //SOUND
    mySound.load("cruising.mp3");
    mySound.setLoop(true);
    mySound.play();
}

//--------------------------------------------------------------
void ofApp::update(){
    
    //KINECT
    //    kinects.update();
    //    if(kinects.isFrameNew()) {
    //        grayImage.setFromPixels(kinects.getIRPixels());
    //    }
    //grayImage.setup(320, 240);
    
    
    
    video.update();
    finder.findHaarObjects(video.getPixels());
    
    glm::vec3 headPosition(0,0,viewerDistance);
    
    if (finder.blobs.size() > 0) {
        //get the head position in camera pixel coordinates
        const ofxCvBlob & blob = finder.blobs.front();
        float cameraHeadX = blob.centroid.x;
        float cameraHeadY = blob.centroid.y;
        
     
        //since camera isn't mirrored, high x in camera means -ve x in world
        float worldHeadX = ofMap(cameraHeadX, 0, video.getWidth(), windowBottomRight.x, windowBottomLeft.x);
        
        //low y in camera is +ve y in world
        float worldHeadY = ofMap(cameraHeadY, 0, video.getHeight(), windowTopLeft.y, windowBottomLeft.y);
        
        //set position in a pretty arbitrary way
        headPosition = glm::vec3(worldHeadX, worldHeadY, viewerDistance);
    } else {
        if (!video.isInitialized()) {
            //if (!kinects.isFrameNew()) {
            //if video isn't working, just make something up
            headPosition = glm::vec3(0.5f * windowWidth * sin(ofGetElapsedTimef()), 0.5f * windowHeight * cos(ofGetElapsedTimef()), viewerDistance);
        }
    }
    
    headPositionHistory.push_back(headPosition);
    while (headPositionHistory.size() > 50.0f){
        headPositionHistory.pop_front();
    }
    
    //these 2 lines of code must be called every time the head position changes
    headTrackedCamera.setPosition(headPosition);
    headTrackedCamera.setupOffAxisViewPortal(windowTopLeft, windowBottomLeft, windowBottomRight);
    
    
    // Update the frames per second label in the GUI
    myFpsLabel = ofToString(ofGetFrameRate(), 2);
    
    //CRUISING
    heathVid.update();
    heathVid.getCurrentFrame();
    
    heathVid2.update();
    heathVid2.getCurrentFrame();
    
    if (heathVid.isFrameNew()){
        ofPixels pixelsH = heathVid.getPixels();
        heathTex.setFromPixels(pixelsH);
    }
    
    //SCENE CHANGES WITH TIME
    if (ofGetElapsedTimef() > 68){
        if (heathVid2.isFrameNew()){
            ofPixels pixelsH = heathVid2.getPixels();
            heathTex.setFromPixels(pixelsH);
        }
    }
    if (ofGetElapsedTimef() > 122){
        //TOILET
        //grab a new frame
        toiletVid.play();
        toiletVid.update();
        
        //update the mesh if we have a new frame
        if (toiletVid.isFrameNew()){
            //this determines how far we extrude the mesh
            for (int i=0; i<toiletVid.getWidth()*toiletVid.getHeight(); i++){
                
                ofFloatColor sampleColor(toiletVid.getPixels()[i*3]/255.f,                // r
                                         toiletVid.getPixels()[i*3+1]/255.f,            // g
                                         toiletVid.getPixels()[i*3+2]/255.f);            // b
                
                //now we get the vertex aat this position
                //we extrude the mesh based on it's brightness
                glm::vec3 tmpVec = toiletMesh.getVertex(i);
                tmpVec.z = sampleColor.getBrightness() * extrusionAmount;
                toiletMesh.setVertex(i, tmpVec);
                
                toiletMesh.setColor(i, sampleColor);
            }
        }
    }
    
    //Move light
    static float phase = 0.;
    phase += 0.01; if(phase>1) phase-=1;
    float px = cos(phase * 2 * 3.1415926535897932384626433832795);
    float pz = sin(phase * 2 * 3.1415926535897932384626433832795);
    myLight.setPosition(px*200, 100, pz*150);
    
    myLight.setDiffuseColor(ofFloatColor(0.5, 0.5, 0.5));
    myLight.setSpecularColor(ofFloatColor(0.1, 0.1, 0.1));
    
    if (ofGetElapsedTimef() > 64){
        myLight.setDiffuseColor(ofFloatColor(ofRandom(5,10), 0.1, 0.1));
        myLight.setSpecularColor(ofFloatColor(0.1, 0.1, 1));
        heathMat.setEmissiveColor(ofFloatColor(ofRandom(0,5)));
    }
    
    if (ofGetElapsedTimef() > 69){
        myLight.setDiffuseColor(ofFloatColor(ofRandom(5,10), 0.1, 0.1));
        myLight.setSpecularColor(ofFloatColor(0.1, 0.1, ofRandom(5,10)));
        heathMat.setEmissiveColor(ofFloatColor(1.0, 1.0, 1.0));
    }
    
    if (ofGetElapsedTimef() > 118){
        myLight.setDiffuseColor(ofFloatColor(ofRandom(1,5), ofRandom(1,5), 0.1));
        myLight.setSpecularColor(ofFloatColor(ofRandom(1,5), ofRandom(1,5), 0.1));
        heathMat.setEmissiveColor(ofFloatColor(ofRandom(0,5)));
    }
    
}

//--------------------------------------------------------------
void ofApp::drawScene(bool isPreview){
    
    ofEnableDepthTest();
    
    if (isPreview) {
        ofPushStyle();
        ofSetColor(150, 100, 100);
        ofDrawGrid(1.0f, 5.0f, true);
        
        ofSetColor(255);
        
        
        //draw camera preview
        headTrackedCamera.transformGL();
        
        ofPushMatrix();
        ofScale(0.002f, 0.002f, 0.002f);
        ofNode().draw();
        ofPopMatrix();
        
        ofMultMatrix(glm::inverse(headTrackedCamera.getProjectionMatrix()));
        
        ofNoFill();
        ofDrawBox(2.0f);
        
        headTrackedCamera.restoreTransformGL();
        
        ofPopStyle();
    }
    
    
    //CRUISING
    ofEnableLighting();
    myLight.enable();
    //myLight.draw();
    
    //cruising
    heathMat.begin();
    heathTex.getTexture().bind();
    
    
    float spinX = sin(ofGetElapsedTimef()*.55f);
    float spinY = cos(ofGetElapsedTimef()*.095f);
    
    greenOpen.rotateDeg(spinX, 1.0, 0.0, 0.0);
    greenOpen.rotateDeg(spinY, 0, 1.0, 0.0);
    greenOpen.drawFaces();
    
    heathTex.getTexture().unbind();
    heathMat.end();
    
    //TOILET
    if (ofGetElapsedTimef() > 122){
        heathVid.stop();
        heathVid2.stop();
        greenOpen.setRadius(0);
        
        myLight.enable();
        
        ofTranslate(-480, 270,-250);
        ofScale(0.5);
        //ofRotateDeg(90, 0, 0, 1);
        ofRotateDeg(180, 1, 0, 0);
        toiletMesh.drawFaces();
    }
    
    myLight.disable();
    ofDisableDepthTest();
}
//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackgroundGradient(ofColor(50), ofColor(0));
    
    if (usePreview){
        previewCamera.begin();
    }
    else{
        headTrackedCamera.begin();
    }
    
    
    drawScene(usePreview);
    
    if (usePreview){
        previewCamera.end();
    }
    else{
        headTrackedCamera.end();
    }
    
    
    //KINECT
    //    ofPushMatrix();
    //    ofScale(0.5);
    //   // grayImage.draw(0, 0);
    //    ofPopMatrix();
    
    
    //MAC CAMERA
    ofPushMatrix();
    ofDisableNormalizedTexCoords();
    ofScale(0.5);
    video.draw((ofGetWidth()*2)-video.getWidth(), 0);
    ofPopMatrix();
    
    ofEnableNormalizedTexCoords();
    
    ofPushStyle();
    ofNoFill();
    
    for(unsigned int i = 0; i < finder.blobs.size(); i++) {
        ofRectangle cur = finder.blobs[i].boundingRect;
        ofPushMatrix();
        ofScale(0.5);
        ofTranslate((ofGetWidth()*2)-video.getWidth(), 0);
        ofDrawRectangle(cur.x, cur.y, cur.width, cur.height);
        ofPopMatrix();
    }
    ofPopStyle();
    
    stringstream message;
    message << "[SPACE] = User preview camera [" << (usePreview ? 'x' : ' ') << "]";
    ofDrawBitmapString(message.str(), 20, 40);
    
    stringstream ss;
    ss << "FPS: " << ofToString(ofGetFrameRate(),0) <<endl<<endl;
    ofDrawBitmapString(ss.str().c_str(), 20, 20);
    
    
    
    if (usePreview){
        ofRectangle bottomLeft(0, ofGetHeight() - 200.0f, 300.0f, 200.0f);
        
        ofPushStyle();
        ofSetColor(0);
        ofDrawRectangle(bottomLeft);
        ofPopStyle();
        
        headTrackedCamera.begin(bottomLeft);
        drawScene(false);
        headTrackedCamera.end();
    }
    //
    //------
}

//--------------------------------------------------------------
void ofApp::exit(){
    //kinects.close();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == ' ')
        usePreview = !usePreview;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
}
