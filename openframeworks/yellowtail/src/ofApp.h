#pragma once

#include "ofMain.h"
#include "Gesture.h"
#include "PolygonYT.h"

class ofApp : public ofBaseApp {

	public:
		void setup();
		void update();
		void draw();

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
		
        void frameRateTitle();
    
        int pmouseX = 0;
        int pmouseY = 0;
    
        bool theMouseDown = false;
        
        vector<Gesture> gestureArray;
        const int nGestures = 36;  // Number of gestures
        const int minMove = 3;     // Minimum travel for a new point
        int currentGestureID;
        
        PolygonYT tempP;
        vector<int> tmpXp;
        vector<int> tmpYp;
        
        void renderGesture (Gesture gesture, int w, int h);
        
        void updateGeometry();
        
        void advanceGesture(Gesture gesture);
        
        void clearGestures();
    
};
