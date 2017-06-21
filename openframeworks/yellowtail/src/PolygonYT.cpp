#include "PolygonYT.h"

PolygonYT :: PolygonYT() {

}

PolygonYT :: PolygonYT(int n) {
	npoints = n;
	xpoints.resize(n);
	ypoints.resize(n);
}