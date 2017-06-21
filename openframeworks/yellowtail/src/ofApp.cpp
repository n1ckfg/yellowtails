#include "ofApp.h"
#include "Gesture.h"
#include "PolygonYT.h"

/**
 * Yellowtail
 * by Golan Levin (www.flong.com).
 * translated to openFrameworks by Nick Fox-Gieg
 *
 * Click, drag, and release to create a kinetic gesture.
 *
 * Yellowtail (1998-2000) is an interactive software system for the gestural
 * creation and performance of real-time abstract animation. Yellowtail repeats
 * a user's strokes end-over-end, enabling simultaneous specification of a
 * line's shape and quality of movement. Each line repeats according to its
 * own period, producing an ever-changing and responsive display of lively,
 * worm-like textures.
 */

void ofApp :: setup() {
    ofBackground(0);
    //noStroke();
    
    currentGestureID = -1;
    gestureArray.resize(nGestures);
    for (int i=0; i<nGestures; i++){
        gestureArray[i] = Gesture(ofGetWidth(), ofGetHeight());
    }
    clearGestures();
}

void ofApp :: draw() {
    ofBackground(0);
    
    updateGeometry();
    //ofFill(255, 255, 245);
    for (int G=0; G<nGestures; G++) {
        renderGesture(gestureArray[G], ofGetWidth(), ofGetHeight());
    }
}

void ofApp :: mousePressed(int x, int y, int button) {
    theMouseDown = true;
    currentGestureID = (currentGestureID+1)%nGestures;
    Gesture G = gestureArray[currentGestureID];
    G.clear();
    G.clearPolys();
    G.addPoint(mouseX, mouseY);
}

void ofApp :: mouseDragged(int x, int y, int button) {
    theMouseDown = true;
    if (currentGestureID >= 0) {
        Gesture G = gestureArray[currentGestureID];
        if (G.distToLast(mouseX, mouseY) > minMove) {
            G.addPoint(mouseX, mouseY);
            G.smooth();
            G.compile();
        }
    }
}

void ofApp :: mouseReleased(int x, int y, int button) {
    theMouseDown = false;
}


void ofApp :: keyPressed(int key) {
    switch (key) {
        case '+':
        case '=':
            if (currentGestureID >= 0) {
                float th = gestureArray[currentGestureID].thickness;
                gestureArray[currentGestureID].thickness = MIN(96, th+1);
                gestureArray[currentGestureID].compile();
            }
            break;
        case '-':
            if (currentGestureID >= 0) {
                float th = gestureArray[currentGestureID].thickness;
                gestureArray[currentGestureID].thickness = MAX(2, th-1);
                gestureArray[currentGestureID].compile();
            }
            break;
            
        case ' ': clearGestures();
            break;
    }
}

void ofApp :: renderGesture(Gesture gesture, int w, int h) {
    if (gesture.exists) {
        if (gesture.nPolys > 0) {
            vector<PolygonYT> polygons = gesture.polygons;
            vector<int> crosses = gesture.crosses;
            
            vector<int> xpts;
            vector<int> ypts;
            PolygonYT p = PolygonYT(4);
            int cr;
            
            //noStroke();
			//ofMesh mesh;
			//mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
            ofBeginShape();
            int gnp = gesture.nPolys;
            for (int i=0; i<gnp; i++) {
                p = polygons[i];
                xpts = p.xpoints;
                ypts = p.ypoints;
                
                ofVertex((float) xpts[0], (float)ypts[0], 0);
                ofVertex((float)xpts[1], (float)ypts[1], 0);
                ofVertex((float)xpts[2], (float)ypts[2], 0);
                ofVertex((float)xpts[3], (float)ypts[3], 0);
                
                if ((cr = crosses[i]) > 0) {
                    if ((cr & 3)>0) {
                        ofVertex((float)xpts[0]+w, (float)ypts[0], 0);
                        ofVertex((float)xpts[1]+w, (float)ypts[1], 0);
                        ofVertex((float)xpts[2]+w, (float)ypts[2], 0);
                        ofVertex((float)xpts[3]+w, (float)ypts[3], 0);
                        
                        ofVertex((float)xpts[0]-w, (float)ypts[0], 0);
                        ofVertex((float)xpts[1]-w, (float)ypts[1], 0);
                        ofVertex((float)xpts[2]-w, (float)ypts[2], 0);
                        ofVertex((float)xpts[3]-w, (float)ypts[3], 0);
                    }
                    if ((cr & 12)>0) {
                        ofVertex((float)xpts[0], (float)ypts[0]+h, 0);
                        ofVertex((float)xpts[1], (float)ypts[1]+h, 0);
                        ofVertex((float)xpts[2], (float)ypts[2]+h, 0);
                        ofVertex((float)xpts[3], (float)ypts[3]+h, 0);
                        
                        ofVertex((float)xpts[0], (float)ypts[0]-h, 0);
                        ofVertex((float)xpts[1], (float)ypts[1]-h, 0);
                        ofVertex((float)xpts[2], (float)ypts[2]-h, 0);
                        ofVertex((float)xpts[3], (float)ypts[3]-h, 0);
                    }
                    
                    // I have knowingly retained the small flaw of not
                    // completely dealing with the corner conditions
                    // (the case in which both of the above are true).
                }
            }
            ofEndShape();
        }
    }
}

void ofApp :: updateGeometry() {
    Gesture J = Gesture(ofGetWidth(), ofGetHeight());
    for (int g=0; g<nGestures; g++) {
        if ((J=gestureArray[g]).exists) {
            if (g!=currentGestureID) {
                advanceGesture(J);
            } else if (!theMouseDown) {
                advanceGesture(J);
            }
        }
    }
}

void ofApp :: advanceGesture(Gesture gesture) {
    // move a Gesture one step
    if (gesture.exists) { // check
        int nPts = gesture.nPoints;
        int nPts1 = nPts-1;
        vector<Vec3f> path;
        float jx = gesture.jumpDx;
        float jy = gesture.jumpDy;
        
        if (nPts > 0) {
            path = gesture.path;
            for (int i=nPts1; i>0; i--) {
                path[i].x = path[i-1].x;
                path[i].y = path[i-1].y;
            }
            path[0].x = path[nPts1].x - jx;
            path[0].y = path[nPts1].y - jy;
            gesture.compile();
        }
    }
}

void ofApp :: clearGestures() {
    for (int i=0; i<nGestures; i++) {
        gestureArray[i].clear();
    }
}

/*

//--------------------------------------------------------------
void ofApp :: setup() {
    //
}

//--------------------------------------------------------------
void ofApp :: update() {
    //
}

//--------------------------------------------------------------
void ofApp :: draw() {
    ofBackground(bgColor);
    if (ofGetMousePressed()) {
        if (strokes[strokes.size()-1].points.size() < 1 || ofDist(mouseX, mouseY, pmouseX, pmouseY) > 2) {
            if (pmouseX !=0 && pmouseY != 0) {
                strokes[strokes.size()-1].points.push_back(new ofVec3f(mouseX, mouseY, 0));
            }
        }
    }
    
    for (int i=0; i<strokes.size(); i++) {
        strokes[i].run();
    }
    
    frameRateTitle();
    pmouseX = mouseX;
    pmouseY = mouseY;
}

//--------------------------------------------------------------
void ofApp :: frameRateTitle() {
    string s = ofToString(ofGetFrameRate(), 2) + "fps";
    ofSetWindowTitle(s);
}

//--------------------------------------------------------------
void ofApp :: keyPressed(int key) {
    strokes.clear();
}

//--------------------------------------------------------------
void ofApp :: keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp :: mouseMoved(int x, int y ) {

}

//--------------------------------------------------------------
void ofApp :: mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp :: mousePressed(int x, int y, int button) {
    strokes.push_back(new gesture(fgColor));
}

//--------------------------------------------------------------
void ofApp :: mouseReleased(int x, int y, int button) {
    strokes[strokes.size()-1].refine();
}

//--------------------------------------------------------------
void ofApp :: mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp :: mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp :: windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp :: gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp :: dragEvent(ofDragInfo dragInfo) { 

}
*/
