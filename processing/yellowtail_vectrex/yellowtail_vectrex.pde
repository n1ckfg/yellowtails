/**
 * Yellowtail
 * by Golan Levin (www.flong.com). 
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

import ddf.minim.*; // minim req to gen audio
import xyscope.*;   // import XYscope
XYscope xy;         // create XYscope instance

boolean theMouseDown = false;

Gesture gestureArray[];
final int nGestures = 36;  // Number of gestures
final int minMove = 3;     // Minimum travel for a new point
int currentGestureID;

Polygon tempP;
int tmpXp[];
int tmpYp[];

void setup() {
  size(512, 512, P2D);
  background(0, 0, 0);
  noStroke();

  xy = new XYscope(this); // define XYscope instance
  //xy.getMixerInfo(); // lists all audio devices
  xy.vectrex(0); // 90 for landscape, 0 for portrait
  xy.ellipseDetail(30); // set detail of vertex ellipse
  /*
   If the SPOT-KILLER MOD was applied (z/brightness is always on),
   this auto sets the brightness (from way turned down) when the sketch runs.
   */
  //xy.z("MOTU 3-4"); // use custom 3rd channel audio device
  //xy.zRange(.5, 0);
  
  currentGestureID = -1;
  gestureArray = new Gesture[nGestures];
  for (int i=0; i<nGestures; i++){
    gestureArray[i] = new Gesture(width, height);
  }
  clearGestures();
}

void draw() {
  background(0);
  xy.clearWaves();
  
  updateGeometry();
  fill(255, 255, 245);
  for (int G=0; G<nGestures; G++) {
    renderGesture(gestureArray[G], width, height);
  }

  xy.buildWaves(); // build audio from shapes
}

void mousePressed() {
  theMouseDown = true;
  currentGestureID = (currentGestureID+1)%nGestures;
  Gesture G = gestureArray[currentGestureID];
  G.clear();
  G.clearPolys();
  G.addPoint(mouseX, mouseY);
}

void mouseDragged() {
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

void mouseReleased() {
  theMouseDown = false;
}

void keyPressed() {
  switch (key) {
    case '+':
    case '=':
    if (currentGestureID >= 0) {
      float th = gestureArray[currentGestureID].thickness;
      gestureArray[currentGestureID].thickness = Math.min(96, th+1);
      gestureArray[currentGestureID].compile();
    }
    break;
    case '-':
    if (currentGestureID >= 0) {
      float th = gestureArray[currentGestureID].thickness;
      gestureArray[currentGestureID].thickness = Math.max(2, th-1);
      gestureArray[currentGestureID].compile();
    }
    break;

    case ' ': clearGestures();
    break;
  }
}

void drawXyPoint(float x, float y) {
  vertex(x, y);
  xy.point(x, y);  
}

void renderGesture (Gesture gesture, int w, int h) {
  if (gesture.exists) {
    if (gesture.nPolys > 0) {
      Polygon polygons[] = gesture.polygons;
      int crosses[] = gesture.crosses;

      int xpts[];
      int ypts[];
      Polygon p;
      int cr;
      
      noStroke();
      beginShape(QUADS);
      int gnp = gesture.nPolys;
      for (int i=0; i<gnp; i++) {
        p = polygons[i];
        xpts = p.xpoints;
        ypts = p.ypoints;
        
        // We only send these points to the Vectrex because we're limited
        // by show many the beam can draw per frame
        drawXyPoint(xpts[0], ypts[0]);
        drawXyPoint(xpts[1], ypts[1]);
        drawXyPoint(xpts[2], ypts[2]);
        drawXyPoint(xpts[3], ypts[3]);

        if ((cr = crosses[i]) > 0) {
          if ((cr & 3)>0) {
            vertex(xpts[0]+w, ypts[0]);
            vertex(xpts[1]+w, ypts[1]);
            vertex(xpts[2]+w, ypts[2]);
            vertex(xpts[3]+w, ypts[3]);

            vertex(xpts[0]-w, ypts[0]);
            vertex(xpts[1]-w, ypts[1]);
            vertex(xpts[2]-w, ypts[2]);
            vertex(xpts[3]-w, ypts[3]);
          }
          if ((cr & 12)>0) {
            vertex(xpts[0], ypts[0]+h);
            vertex(xpts[1], ypts[1]+h);
            vertex(xpts[2], ypts[2]+h);
            vertex(xpts[3], ypts[3]+h);

            vertex(xpts[0], ypts[0]-h);
            vertex(xpts[1], ypts[1]-h);
            vertex(xpts[2], ypts[2]-h);
            vertex(xpts[3], ypts[3]-h);
          }

          // I have knowingly retained the small flaw of not
          // completely dealing with the corner conditions
          // (the case in which both of the above are true).
        }
      }
      endShape();
    }
  }
}

private void updateGeometry() {
  Gesture J;
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

void advanceGesture(Gesture gesture) {
  // move a Gesture one step
  if (gesture.exists) { // check
    int nPts = gesture.nPoints;
    int nPts1 = nPts-1;
    Vec3f path[];
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

void clearGestures() {
  for (int i=0; i<nGestures; i++) {
    gestureArray[i].clear();
  }
}
