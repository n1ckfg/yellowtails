#pragma once
#include "ofMain.h"
#include "PolygonYT.h"
#include "Vec3f.h"

class Gesture {
    
    public:
        Gesture();
        Gesture(int mw, int mh);
		virtual ~Gesture() {};

        void initGesture();
		void clear();
        void clearPolys();
        void addPoint(float x, float y);
        float distToLast(float ix, float iy);
        void compile();
        void smooth();

        vector<Vec3f> path;
        vector<int> crosses;
        vector<PolygonYT> polygons;
        int nPoints;
        int nPolys;
        
        float jumpDx, jumpDy;
        bool exists;
        float thickness = INIT_TH;
    
    private:
        float getPressureFromVelocity(float v);
        void setPressures();
    
        static double twoPi;
        static float radToDeg;
        static float damp;
        static float dampInv;
        static float damp1;
        static float INIT_TH;
    
        int w;
        int h;
        int capacity;
    
};
