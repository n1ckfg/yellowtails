#include "Vec3f.h"

Vec3f :: Vec3f() {
    set(0,0,0);
}

Vec3f :: Vec3f(float ix, float iy, float ip) {
    set(ix, iy, ip);
}

void Vec3f :: set(float ix, float iy, float ip) {
    x = ix;
    y = iy;
    p = ip;
}
