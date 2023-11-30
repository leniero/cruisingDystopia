#pragma once

#include "ofMain.h"
#include "ofxCvHaarFinder.h"
#include "ofxKinectV2.h"
#include "ofxGui.h"
#include "ofxAssimpModelLoader.h"
#include "ofxOpenCv.h"
#include "ofVboMesh.h"



using namespace glm;

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void exit();
        void drawScene(bool isPreview);
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

        ofVideoGrabber video;
       ofxCvHaarFinder finder;


        glm::vec3 windowTopLeft;
        glm::vec3 windowBottomLeft;
        glm::vec3 windowBottomRight;
        ofCamera headTrackedCamera;
        ofEasyCam previewCamera;

        bool usePreview;
        float windowWidth;
        float windowHeight;
        float viewerDistance;

        deque<glm::vec3> headPositionHistory;

        ofVboMesh window;

    
    //KINECT
//    ofxKinectV2 kinects;
//    ofxCvGrayscaleImage grayImage;
    
    
    //CRUISING
    ofSpherePrimitive greenOpen;
  
    ofVideoPlayer heathVid;
    ofVideoPlayer heathVid2;
    
    ofImage heathTex;
    ofMaterial heathMat;
    
    ofxLabel myFpsLabel;
    ofxPanel myGui;
          
    ofLight myLight;
    
 
    //TOILET
    float extrusionAmount;
    ofVboMesh toiletMesh;
    ofVideoPlayer toiletVid;
    
    //SOUND
    ofSoundPlayer   mySound;
};
