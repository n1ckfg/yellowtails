#pragma once
#include "ofMain.h"

class Vec3f {
    
    public:
        Vec3f();
        Vec3f(float ix, float iy, float ip);
        virtual ~Vec3f(){};
    
        void set(float ix, float iy, float ip);
    
        float x;
        float y;
        float p; // pressure
    
};
