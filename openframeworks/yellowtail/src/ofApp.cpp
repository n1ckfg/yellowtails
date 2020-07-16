#include "ofApp.h"

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
	ofSetFrameRate(60);
	ofSetVerticalSync(false);
    
    currentGestureID = -1;
    for (int i=0; i<nGestures; i++){
        gestureArray.push_back(Gesture(ofGetWidth(), ofGetHeight()));
    }
    clearGestures();
}

void ofApp :: draw() {
    ofBackground(0);
    
    updateGeometry();
    ofFill();
    ofSetColor(255, 255, 245);
    for (int G=0; G<nGestures; G++) {
        renderGesture(gestureArray[G], ofGetWidth(), ofGetHeight());
    }
    
    frameRateTitle();
}

void ofApp :: mousePressed(int x, int y, int button) {
    theMouseDown = true;
    currentGestureID = (currentGestureID + 1) % nGestures;
    //Gesture G = gestureArray[currentGestureID];
    gestureArray[currentGestureID].clear();
    gestureArray[currentGestureID].clearPolys();
    gestureArray[currentGestureID].addPoint((float)x, (float)y);
}

void ofApp :: mouseDragged(int x, int y, int button) {
    theMouseDown = true;
    if (currentGestureID >= 0) {
        //Gesture G = gestureArray[currentGestureID];
        if (gestureArray[currentGestureID].distToLast(x, y) > minMove) {
            gestureArray[currentGestureID].addPoint((float)x, (float)y);
            gestureArray[currentGestureID].smooth();
            gestureArray[currentGestureID].compile();
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

void ofApp :: renderGesture(Gesture& gesture, int w, int h) {
    if (gesture.exists) {
        if (gesture.nPolys > 0) {
            int cr;

			ofMesh mesh;
			mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
			
			int gnp = gesture.nPolys;
            for (int i=0; i<gnp; i++) {
                cout << ofToString(gesture.polygons[i].xpoints) + " " + ofToString(gesture.polygons[i].xpoints) << endl;
                
                mesh.addVertex(ofVec3f((float)gesture.polygons[i].xpoints[0], (float)gesture.polygons[i].ypoints[0]));
                mesh.addVertex(ofVec3f((float)gesture.polygons[i].xpoints[1], (float)gesture.polygons[i].ypoints[1]));
                mesh.addVertex(ofVec3f((float)gesture.polygons[i].xpoints[2], (float)gesture.polygons[i].ypoints[2]));
                mesh.addVertex(ofVec3f((float)gesture.polygons[i].xpoints[3], (float)gesture.polygons[i].ypoints[3]));
                
                if ((cr = gesture.crosses[i]) > 0) {
                    if ((cr & 3)>0) {
                        mesh.addVertex(ofVec3f((float)gesture.polygons[i].xpoints[0]+w, (float)gesture.polygons[i].ypoints[0]));
                        mesh.addVertex(ofVec3f((float)gesture.polygons[i].xpoints[1]+w, (float)gesture.polygons[i].ypoints[1]));
                        mesh.addVertex(ofVec3f((float)gesture.polygons[i].xpoints[2]+w, (float)gesture.polygons[i].ypoints[2]));
                        mesh.addVertex(ofVec3f((float)gesture.polygons[i].xpoints[3]+w, (float)gesture.polygons[i].ypoints[3]));
                        
                        mesh.addVertex(ofVec3f((float)gesture.polygons[i].xpoints[0]-w, (float)gesture.polygons[i].ypoints[0]));
                        mesh.addVertex(ofVec3f((float)gesture.polygons[i].xpoints[1]-w, (float)gesture.polygons[i].ypoints[1]));
                        mesh.addVertex(ofVec3f((float)gesture.polygons[i].xpoints[2]-w, (float)gesture.polygons[i].ypoints[2]));
                        mesh.addVertex(ofVec3f((float)gesture.polygons[i].xpoints[3]-w, (float)gesture.polygons[i].ypoints[3]));
                    }
                    if ((cr & 12)>0) {
                        mesh.addVertex(ofVec3f((float)gesture.polygons[i].xpoints[0], (float)gesture.polygons[i].ypoints[0]+h));
                        mesh.addVertex(ofVec3f((float)gesture.polygons[i].xpoints[1], (float)gesture.polygons[i].ypoints[1]+h));
                        mesh.addVertex(ofVec3f((float)gesture.polygons[i].xpoints[2], (float)gesture.polygons[i].ypoints[2]+h));
                        mesh.addVertex(ofVec3f((float)gesture.polygons[i].xpoints[3], (float)gesture.polygons[i].ypoints[3]+h));
                        
                        mesh.addVertex(ofVec3f((float)gesture.polygons[i].xpoints[0], (float)gesture.polygons[i].ypoints[0]-h));
                        mesh.addVertex(ofVec3f((float)gesture.polygons[i].xpoints[1], (float)gesture.polygons[i].ypoints[1]-h));
                        mesh.addVertex(ofVec3f((float)gesture.polygons[i].xpoints[2], (float)gesture.polygons[i].ypoints[2]-h));
                        mesh.addVertex(ofVec3f((float)gesture.polygons[i].xpoints[3], (float)gesture.polygons[i].ypoints[3]-h));
                    }
                    
                    // I have knowingly retained the small flaw of not
                    // completely dealing with the corner conditions
                    // (the case in which both of the above are true).
                }
            }
			mesh.draw();
        }
    }
}

void ofApp :: updateGeometry() {
    Gesture J = Gesture(ofGetWidth(), ofGetHeight());
    for (int g=0; g<nGestures; g++) {
        if ((J = gestureArray[g]).exists) {
            if (g!=currentGestureID) {
                advanceGesture(J);
            } else if (!theMouseDown) {
                advanceGesture(gestureArray[g]);
            }
        }
    }
}

int ofApp :: countActiveGestures() {
    int counter = 0;
    for (int i=0; i<gestureArray.size(); i++) {
        if (gestureArray[i].exists) counter++;
    }
    return counter;
}

void ofApp :: advanceGesture(Gesture& gesture) {
    // move a Gesture one step
    if (gesture.exists) { // check
        int nPts = gesture.nPoints;
        int nPts1 = nPts-1;
        //vector<Vec3f> path;
        float jx = gesture.jumpDx;
        float jy = gesture.jumpDy;
        
        if (nPts > 0) {
            for (int i=nPts1; i>0; i--) {
                gesture.path[i].x = gesture.path[i-1].x;
                gesture.path[i].y = gesture.path[i-1].y;
            }
            gesture.path[0].x = gesture.path[nPts1].x - jx;
            gesture.path[0].y = gesture.path[nPts1].y - jy;
            gesture.compile();
        }
    }
}

void ofApp :: clearGestures() {
    for (int i=0; i<nGestures; i++) {
        gestureArray[i].clear();
    }
}

//--------------------------------------------------------------
void ofApp :: frameRateTitle() {
    string s = ofToString(ofGetFrameRate(), 2) + "fps";
    ofSetWindowTitle(s + "  |  exists: " + ofToString(countActiveGestures()) + "  |  active: " + ofToString(currentGestureID));
}

