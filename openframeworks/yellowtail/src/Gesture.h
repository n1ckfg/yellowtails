#pragma once
#include "ofMain.h"
#include "Vec3f.h"

class Gesture {
    
    public:
        Gesture(int mw, int mh);
        void clear();
        void clearPolys();
        void addPoint(float x, float y);
        float distToLast(float ix, float iy);
        void compile();
        void smooth();

        vector <Vec3f *> path;
        vector <int *> crosses;
        vector <Polygon *> polygons;
        int nPoints;
        int nPolys;
        
        float jumpDx, jumpDy;
        Boolean exists;
        float thickness = INIT_TH;
    
    private:
        float getPressureFromVelocity(float v);
        void setPressures();
    
        static double twoPi;
        static float radToDeg;
        static float damp;
        static float dampInv;
        static float damp1;
        
        int w;
        int h;
        int capacity;
        static float INIT_TH;
    
};
