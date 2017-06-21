#pragma once
#include "ofMain.h"

class PolygonYT {

	public:
        PolygonYT();
		PolygonYT(int n);
		virtual ~PolygonYT() {};

        void initPolygonYT(int n);
    
		int npoints;
		vector<int> xpoints;
		vector<int> ypoints;

};
