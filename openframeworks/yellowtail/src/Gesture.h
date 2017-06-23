#pragma once
#include "ofMain.h"
#include "PolygonYt.h"
#include "Vec3f.h"

class Gesture {
    
    public:
        Gesture();
        Gesture(int mw, int mh);
		virtual ~Gesture() {};

        void initGesture(int mw, int mh);
		void clear();
        void clearPolys();
        void addPoint(float x, float y);
        float distToLast(float ix, float iy);
        void compile();
        void smooth();

        vector<Vec3f> path;
        vector<int> crosses;
        vector<PolygonYt> polygons;
        int nPoints;
        int nPolys;
        
        float jumpDx, jumpDy;
        bool exists;
        float thickness = INIT_TH;
    
    private:
        float getPressureFromVelocity(float v);
        void setPressures();
    
        double twoPi;
        float radToDeg;
        float damp;
        float dampInv;
        float damp1;
        float INIT_TH;
    
        int w;
        int h;
        int capacity;
    
};
