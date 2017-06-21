#pragma once
#include "ofMain.h"

class PolygonYT {

	public:
		PolygonYT();
		PolygonYT(int n);
		virtual ~PolygonYT() {};

		int npoints;
		vector<int> xpoints;
		vector<int> ypoints;

};