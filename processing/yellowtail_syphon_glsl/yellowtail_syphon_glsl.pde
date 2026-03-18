/**
 * Yellowtail 
 * by Golan Levin. 
 * 
 * Click, drag, and release to create a kinetic gesture.
 */

import java.awt.*;
import processing.opengl.*;
import codeanticode.syphon.*;

PShader[] filters;
int selFilter = 0;

int sW = 960;
int sH = 540;
int fps = 24;

SyphonServer server;
String syphonServerName = "Simple Server";
PGraphics canvas;

int myW;
int myH;
boolean theMouseDown = false;

Gesture gestureArray[];
final int nGestures = 36;  // Number of gestures
final int minMove = 3;     // Minimum travel for a new point
int currentGestureID;

Polygon tempP;
int tmpXp[];
int tmpYp[];

void settings() {
  size(sW, sH, P3D);
}

void setup(){
  frameRate(fps);
  canvas = createGraphics(sW,sH,P3D);
  myW = width;
  myH = height;
  background(0, 0, 0);
  noStroke();

  currentGestureID = -1;
  gestureArray = new Gesture[nGestures];
  for (int i=0; i<nGestures; i++){
    gestureArray[i] = new Gesture(myW, myH);
  }
  clearGestures();
  
  server = new SyphonServer(this, syphonServerName);
  
  filters = new PShader[7];  
  filters[0] = loadShader("crosshatch.frag", "basic.vert");
  filters[0].set("hatch_y_offset", 5.0f);
  filters[0].set("lum_threshold_1", 1.0f);
  filters[0].set("lum_threshold_2", 0.7f);
  filters[0].set("lum_threshold_3", 0.5f);
  filters[0].set("lum_threshold_4", 0.3f);
  
  filters[1] = loadShader("thermal.frag", "basic.vert");
  
  filters[2] = loadShader("toon.frag", "basic.vert");
  filters[2].set("HueLevels", new float[]{0.0f, 80.0f, 160.0f, 240.0f, 320.0f, 360.0f});
  filters[2].set("SatLevels", new float[]{0.0f, 0.15f, 0.3f, 0.45f, 0.6f, 0.8f, 1.0f});
  filters[2].set("ValLevels", new float[]{0.0f, 0.3f, 0.6f, 1.0f});

  filters[3] = loadShader("dream.frag", "basic.vert");
  
  filters[4] = loadShader("crossstitch.frag", "basic.vert");
  filters[4].set("stitching_size", 5.0f);
  filters[4].set("invert", 1);
  
  filters[5] = loadShader("freichen.frag", "basic.vert");
  
  filters[6] = loadShader("sobel.frag", "basic.vert");
  
  //textureMode(NORMALIZED);
}

void draw(){
  canvas.beginDraw(); //1.  begin draw loop
  canvas.background(0);

  updateGeometry();
  canvas.fill(255, 255, 245);
  for (int G=0; G<nGestures; G++){
    renderGesture(gestureArray[G],myW,myH);
  }
  
  // Apply the currently selected filter to the canvas directly
  canvas.filter(filters[selFilter]);
  
  canvas.endDraw(); //2.  end draw loop
  
  //server.sendImage(canvas); //3.  canvas goes to Syphon server
  image(canvas, 0, 0);  //4.  canvas is displayed in app
    
  noStroke();
  
  beginShape(QUADS);
  texture(canvas);
  vertex(mouseX, 0, float(mouseX)/width, 0);
  vertex(width, 0, 1, 0); 
  vertex(width, height, 1, 1); 
  vertex(mouseX, height, float(mouseX)/width, 1);  
  endShape();
  
  ellipse(mouseX,mouseY,20,20);  //5.  extra elements that aren't sent to Syphon
}

void mousePressed()
{
  theMouseDown = true;
  currentGestureID = (currentGestureID+1)%nGestures;
  Gesture G = gestureArray[currentGestureID];
  G.clear();
  G.clearPolys();
  G.addPoint(mouseX, mouseY);
}

void mouseDragged(){
  theMouseDown = true;
  if (currentGestureID >= 0){
    Gesture G = gestureArray[currentGestureID];
    if (G.distToLast(mouseX, mouseY) > minMove) {
      G.addPoint(mouseX, mouseY);
      G.smooth();
      G.compile();
    }
  }
}

void mouseMoved(){
  theMouseDown = false;
}

void mouseReleased(){
  theMouseDown = false;
}


void keyPressed(){
  switch (key){
    case '+':      
    case '=':
      if (currentGestureID >= 0){
        float th = gestureArray[currentGestureID].thickness;
        gestureArray[currentGestureID].thickness = Math.min(96, th+1);
        gestureArray[currentGestureID].compile();
      }
      break;
      
    case '-':
      if (currentGestureID >= 0){
        float th = gestureArray[currentGestureID].thickness;
        gestureArray[currentGestureID].thickness = Math.max(2, th-1);
        gestureArray[currentGestureID].compile();
      }
      break;

    case ' ': 
      clearGestures();
      break;
    
    case '1':
      selFilter = 0;
      break;

    case '2':
      selFilter = 1;
      break;

    case '3':
      selFilter = 2;
      break;

    case '4':
      selFilter = 3;
      break;

    case '5':
      selFilter = 4;
      break;

    case '6':
      selFilter = 5;
      break;

    case '7':
      selFilter = 6;
      break;  
  }    
}


void renderGesture (Gesture gesture, int w, int h){
  if (gesture.exists){
    if (gesture.nPolys > 0){
      Polygon polygons[] = gesture.polygons;
      int crosses[] = gesture.crosses;

      int xpts[];
      int ypts[];
      Polygon p;
      int cr;
      
      canvas.noStroke();
      canvas.beginShape(QUADS);
      int gnp = gesture.nPolys;
      for (int i=0; i<gnp; i++){

        p = polygons[i];
        xpts = p.xpoints;
        ypts = p.ypoints;

        canvas.vertex(xpts[0], ypts[0]);
        canvas.vertex(xpts[1], ypts[1]);
        canvas.vertex(xpts[2], ypts[2]);
        canvas.vertex(xpts[3], ypts[3]);

        if ((cr = crosses[i]) > 0){
          if ((cr & 3)>0){
            canvas.vertex(xpts[0]+w, ypts[0]);
            canvas.vertex(xpts[1]+w, ypts[1]);
            canvas.vertex(xpts[2]+w, ypts[2]);
            canvas.vertex(xpts[3]+w, ypts[3]);

            canvas.vertex(xpts[0]-w, ypts[0]);
            canvas.vertex(xpts[1]-w, ypts[1]);
            canvas.vertex(xpts[2]-w, ypts[2]);
            canvas.vertex(xpts[3]-w, ypts[3]);
          }
          if ((cr & 12)>0){
            canvas.vertex(xpts[0], ypts[0]+h);
            canvas.vertex(xpts[1], ypts[1]+h);
            canvas.vertex(xpts[2], ypts[2]+h);
            canvas.vertex(xpts[3], ypts[3]+h);

            canvas.vertex(xpts[0], ypts[0]-h);
            canvas.vertex(xpts[1], ypts[1]-h);
            canvas.vertex(xpts[2], ypts[2]-h);
            canvas.vertex(xpts[3], ypts[3]-h);
          }
        }
      }
      canvas.endShape();
    }
  }
}

private void updateGeometry(){
  Gesture J;
  for (int g=0; g<nGestures; g++){
    if ((J=gestureArray[g]).exists){
      if (g!=currentGestureID){
        advanceGesture(J);
      } else if (!theMouseDown){
        advanceGesture(J);
      }
    }
  }
}

void advanceGesture(Gesture gesture){
  if (gesture.exists){
    int nPts = gesture.nPoints;
    int nPts1 = nPts-1;
    Vec3f path[];
    float jx = gesture.jumpDx;
    float jy = gesture.jumpDy;

    if (nPts > 0){
      path = gesture.path;
      for (int i=nPts1; i>0; i--){
        path[i].x = path[i-1].x;
        path[i].y = path[i-1].y;
      }
      path[0].x = path[nPts1].x - jx;
      path[0].y = path[nPts1].y - jy;
      gesture.compile();
    }
  }
}

void clearGestures(){
  for (int i=0; i<nGestures; i++){
    gestureArray[i].clear();
  }
}
