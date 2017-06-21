#pragma once
#include "ofMain.h"
#include "vec3f.h"

class gesture {
    
    public:
        gesture();
        gesture(ofColor c);
        gesture(float s);
        gesture(ofColor c, float s);
        virtual ~gesture(){};

        void update();
        void draw();
        void run();
        void splitStroke();
        void smoothStroke();
        void refine();
    
        vector <ofVec3f *> points;
        float strokeSize = 10.0;
        ofColor strokeColor = ofColor(0);
        int splitReps = 2;
        int smoothReps = 10;
        bool drawMesh = true;

};
