#pragma once
#include "ofMain.h"

class PolygonYt {

	public:
        PolygonYt();
		PolygonYt(int n);
		virtual ~PolygonYt() {};

        void initPolygonYt(int n);
    
		int npoints;
		vector<int> xpoints;
		vector<int> ypoints;

};
