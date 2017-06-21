#include "vec3f.h"

vec3f :: vec3f() {
    set(0,0,0);
}

vec3f :: vec3f(float ix, float iy, float ip) {
    set(ix, iy, ip);
}

void vec3f :: set(float ix, float iy, float ip) {
    x = ix;
    y = iy;
    p = ip;
}
