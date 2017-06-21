#pragma once
#include "ofMain.h"

class vec3f {
    
    public:
        vec3f();
        vec3f(float ix, float iy, float ip);
        virtual ~vec3f(){};
    
        void set(float ix, float iy, float ip);
    
        float x;
        float y;
        float p; // pressure
    
};
