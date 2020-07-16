#include "PolygonYt.h"

PolygonYt :: PolygonYt() {
    initPolygonYt(4);
}

PolygonYt :: PolygonYt(int n) {
    initPolygonYt(n);
}

void PolygonYt :: initPolygonYt(int n) {
    npoints = n;
	for (int i = 0; i < npoints; i++) {
		xpoints.push_back(0);
		ypoints.push_back(0);
	}
}
