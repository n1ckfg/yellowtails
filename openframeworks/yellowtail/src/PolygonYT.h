#pragma once
#include "ofMain.h"

class PolygonYT {

	public:
		PolygonYT(int n);
		virtual ~PolygonYT() {};

		int npoints;
		vector<int> xpoints;
		vector<int> ypoints;

};