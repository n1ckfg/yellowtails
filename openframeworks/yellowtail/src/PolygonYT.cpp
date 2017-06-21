#include "PolygonYT.h"

PolygonYT :: PolygonYT() {
    initPolygonYT(4);
}

PolygonYT :: PolygonYT(int n) {
    initPolygonYT(n);
}

void PolygonYT :: initPolygonYT(int n) {
    npoints = n;
   	xpoints.resize(npoints);
    xpoints = vector<int>(npoints, 0);
    ypoints.resize(npoints);
    ypoints = vector<int>(npoints, 0);
}
