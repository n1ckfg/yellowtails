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
    for (int i=0; i < nGestures; i++) {
        renderGesture(gestureArray[i], ofGetWidth(), ofGetHeight());
    }
    
    frameRateTitle();
}

void ofApp :: mousePressed(int x, int y, int button) {
    theMouseDown = true;
    currentGestureID = (currentGestureID + 1) % nGestures;
    gestureArray[currentGestureID].clear();
    gestureArray[currentGestureID].clearPolys();
    gestureArray[currentGestureID].addPoint((float)x, (float)y);
}

void ofApp :: mouseDragged(int x, int y, int button) {
    theMouseDown = true;
    if (currentGestureID >= 0) {
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
			PolygonYt *apoly;

			ofMesh mesh;
			mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);

            for (int i=0; i < gesture.nPolys; i++) {
                //cout << ofToString(gesture.polygons[i].xpoints) + " " + ofToString(gesture.polygons[i].xpoints) << endl;
				apoly = &gesture.polygons[i];

                mesh.addVertex(ofVec3f((float)apoly->xpoints[0], (float)apoly->ypoints[0]));
                mesh.addVertex(ofVec3f((float)apoly->xpoints[3], (float)apoly->ypoints[3]));
                mesh.addVertex(ofVec3f((float)apoly->xpoints[1], (float)apoly->ypoints[1]));
                mesh.addVertex(ofVec3f((float)apoly->xpoints[2], (float)apoly->ypoints[2]));
                
                /*
				if ((cr = gesture.crosses[i]) > 0) {
                    if ((cr & 3)>0) {
                        mesh.addVertex(ofVec3f((float)apoly->xpoints[0]+w, (float)apoly->ypoints[0]));
                        mesh.addVertex(ofVec3f((float)apoly->xpoints[3]+w, (float)apoly->ypoints[3]));
                        mesh.addVertex(ofVec3f((float)apoly->xpoints[1]+w, (float)apoly->ypoints[1]));
                        mesh.addVertex(ofVec3f((float)apoly->xpoints[2]+w, (float)apoly->ypoints[2]));
                        
                        mesh.addVertex(ofVec3f((float)apoly->xpoints[0]-w, (float)apoly->ypoints[0]));
                        mesh.addVertex(ofVec3f((float)apoly->xpoints[3]-w, (float)apoly->ypoints[3]));
                        mesh.addVertex(ofVec3f((float)apoly->xpoints[1]-w, (float)apoly->ypoints[1]));
                        mesh.addVertex(ofVec3f((float)apoly->xpoints[2]-w, (float)apoly->ypoints[2]));
                    }
                    if ((cr & 12)>0) {
                        mesh.addVertex(ofVec3f((float)apoly->xpoints[0], (float)apoly->ypoints[0]+h));
                        mesh.addVertex(ofVec3f((float)apoly->xpoints[3], (float)apoly->ypoints[3]+h));
                        mesh.addVertex(ofVec3f((float)apoly->xpoints[1], (float)apoly->ypoints[1]+h));
                        mesh.addVertex(ofVec3f((float)apoly->xpoints[2], (float)apoly->ypoints[2]+h));
                        
                        mesh.addVertex(ofVec3f((float)apoly->xpoints[0], (float)apoly->ypoints[0]-h));
                        mesh.addVertex(ofVec3f((float)apoly->xpoints[3], (float)apoly->ypoints[3]-h));
                        mesh.addVertex(ofVec3f((float)apoly->xpoints[1], (float)apoly->ypoints[1]-h));
                        mesh.addVertex(ofVec3f((float)apoly->xpoints[2], (float)apoly->ypoints[2]-h));
                    
                    // I have knowingly retained the small flaw of not
                    // completely dealing with the corner conditions
                    // (the case in which both of the above are true).
                }
				*/

            }
			mesh.drawWireframe();
        }
    }
}

void ofApp :: updateGeometry() {
    for (int i=0; i < nGestures; i++) {
        if (gestureArray[i].exists) {
            if (i != currentGestureID) {
                advanceGesture(gestureArray[i]);
            } else if (!theMouseDown) {
                advanceGesture(gestureArray[i]);
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
        float jx = gesture.jumpDx;
        float jy = gesture.jumpDy;
        
        if (nPts > 0) {
            for (int i=nPts1; i > 0; i--) {
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
    for (int i=0; i < nGestures; i++) {
        gestureArray[i].clear();
    }
}

//--------------------------------------------------------------
void ofApp :: frameRateTitle() {
    string s = ofToString(ofGetFrameRate(), 2) + "fps";
    ofSetWindowTitle(s + "  |  exists: " + ofToString(countActiveGestures()) + "  |  active: " + ofToString(currentGestureID));
}

