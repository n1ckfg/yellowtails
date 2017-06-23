#include "PolygonYt.h"

PolygonYt :: PolygonYt() {
    initPolygonYt(4);
}

PolygonYt :: PolygonYt(int n) {
    initPolygonYt(n);
}

void PolygonYt :: initPolygonYt(int n) {
    npoints = n;
   	xpoints.resize(npoints);
    xpoints = vector<int>(npoints, 0);
    ypoints.resize(npoints);
    ypoints = vector<int>(npoints, 0);
}
