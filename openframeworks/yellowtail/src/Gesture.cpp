#include "Gesture.h"

Gesture :: Gesture() {
    w = ofGetWidth();
    h = ofGetHeight();
    initGesture(w, h);
}

Gesture :: Gesture(int mw, int mh) {
    w = mw;
    h = mh;
    initGesture(w, h);
}

void Gesture :: initGesture(int mw, int mh) {
    twoPi = PI * 2.0;
    radToDeg = (float) (360.0 / (2.0 * PI)); // 57.2957795131;
    damp = 5;
    dampInv = 1.0 / damp;
    damp1 = damp -1;
    INIT_TH = 14;
    
    capacity = 600;
    for (int i=0; i<capacity; i++) {
        polygons.push_back(PolygonYt(4));
        path.push_back(Vec3f());
        crosses.push_back(0);
    }
    nPoints = 0;
    nPolys = 0;
    
    exists = false;
    jumpDx = 0;
    jumpDy = 0;
}

void Gesture :: clear() {
    nPoints = 0;
    exists = false;
    thickness = INIT_TH;
}

void Gesture :: clearPolys() {
    nPolys = 0;
}

void Gesture :: addPoint(float x, float y) {
    if (nPoints >= capacity) {
        // there are all sorts of possible solutions here,
        // but for abject simplicity, I don't do anything.
    } else {
        float v = distToLast(x, y);
        float p = getPressureFromVelocity(v);
        path[nPoints++].set(x,y,p);
        
        if (nPoints > 1) {
            exists = true;
            jumpDx = path[nPoints-1].x - path[0].x;
            jumpDy = path[nPoints-1].y - path[0].y;
        }
    }    
}

float Gesture :: getPressureFromVelocity(float v) {
    float scale = 18;
    float minP = 0.02;
    float oldP = (nPoints > 0) ? path[nPoints-1].p : 0;
    return ((minP + MAX(0, 1.0 - v / scale)) + (damp1 * oldP)) * dampInv;
}

void Gesture :: setPressures() {
    // pressures vary from 0...1
    float pressure;
    Vec3f tmp;
    double t = 0;
    
    double u = 1.0f/(double)(nPoints - 1)*twoPi;
    for (int i=0; i<nPoints; i++) {
        pressure = (float) sqrt((1.0 - cos(t)) * 0.5);
        path[i].p = pressure;
        t += u;
    }
}

float Gesture :: distToLast(float ix, float iy) {
    if (nPoints > 0) {
        Vec3f v = path[nPoints-1];
        float dx = v.x - ix;
        float dy = v.y - iy;
        return (float) sqrt(dx * dx + dy * dy);
    } else {
        return 30;
    }
}

void Gesture :: compile() {
    // compute the polygons from the path of Vec3f's
    if (exists) {
        clearPolys();
        
        Vec3f p0, p1, p2;
        float radius0, radius1;
        float ax, bx, cx, dx;
        float ay, by, cy, dy;
        int   axi, bxi, cxi, dxi, axip, axid;
        int   ayi, byi, cyi, dyi, ayip, ayid;
        float p1x, p1y;
        float dx01, dy01, hp01, si01, co01;
        float dx02, dy02, hp02, si02, co02;
        float dx13, dy13, hp13, si13, co13;
        float taper = 1.0;
        
        int  nPathPoints = nPoints - 1;
        int  lastPolyIndex = nPathPoints - 1;
        float npm1finv =  1.0 / (float) (MAX(1, nPathPoints - 1));
        
        // handle the first point
        p0 = path[0];
        p1 = path[1];
        radius0 = p0.p * thickness;
        dx01 = p1.x - p0.x;
        dy01 = p1.y - p0.y;
        hp01 = (float) sqrt(dx01 * dx01 + dy01 * dy01);
        if (hp01 == 0) {
            hp02 = 0.0001;
        }
        co01 = radius0 * dx01 / hp01;
        si01 = radius0 * dy01 / hp01;
        ax = p0.x - si01; ay = p0.y + co01;
        bx = p0.x + si01; by = p0.y - co01;
               
        int LC = 20;
        int RC = w-LC;
        int TC = 20;
        int BC = h-TC;
        float mint = 0.618;
        double tapow = 0.4;
        
		PolygonYt *apoly;

        // handle the middle points
        for (int i=1; i<nPathPoints; i++) {
            taper = (float) (pow((lastPolyIndex - i) * npm1finv, tapow));
            
            p0 = path[i-1];
            p1 = path[i];
            p2 = path[i+1];
            p1x = p1.x;
            p1y = p1.y;
            radius1 = MAX(mint, taper * p1.p * thickness);
            
            // assumes all segments are roughly the same length...
            dx02 = p2.x - p0.x;
            dy02 = p2.y - p0.y;
            hp02 = (float) sqrt(dx02 * dx02 + dy02 * dy02);
            if (hp02 != 0) {
                hp02 = radius1/hp02;
            }
            co02 = dx02 * hp02;
            si02 = dy02 * hp02;
            
            // translate the integer coordinates to the viewing rectangle
            axi = axip = (int)ax;
            ayi = ayip = (int)ay;
            axi=(axi<0)?(w-((-axi)%w)):axi%w;
            axid = axi-axip;
            ayi=(ayi<0)?(h-((-ayi)%h)):ayi%h;
            ayid = ayi-ayip;
            
			apoly = &polygons[nPolys++];

            // set the vertices of the polygon
			apoly->xpoints[0] = axi = axid + axip;
			apoly->xpoints[1] = bxi = axid + (int) bx;
			apoly->xpoints[2] = cxi = axid + (int) (cx = p1x + si02);
			apoly->xpoints[3] = dxi = axid + (int) (dx = p1x - si02);
            
			apoly->ypoints[0] = ayi = ayid + ayip;
			apoly->ypoints[1] = byi = ayid + (int) by;
			apoly->ypoints[2] = cyi = ayid + (int) (cy = p1y - co02);
			apoly->ypoints[3] = dyi = ayid + (int) (dy = p1y + co02);
            
            // keep a record of where we cross the edge of the screen
			crosses[i] = 0;
            if ((axi<=LC) || (bxi<=LC) || (cxi<=LC) || (dxi<=LC)) crosses[i] |= 1;
            if ((axi>=RC) || (bxi>=RC) || (cxi>=RC) || (dxi>=RC)) crosses[i] |= 2;
            if ((ayi<=TC) || (byi<=TC) || (cyi<=TC) || (dyi<=TC)) crosses[i] |= 4;
            if ((ayi>=BC) || (byi>=BC) || (cyi>=BC) || (dyi>=BC)) crosses[i] |= 8;
            
            //swap data for next time
            ax = dx; ay = dy;
            bx = cx; by = cy;		
		}
        
        // handle the last point
		p2 = path[nPathPoints];
		apoly = &polygons[nPolys++];

		apoly->xpoints[0] = (int) ax;
		apoly->xpoints[1] = (int) bx;
		apoly->xpoints[2] = (int) p2.x;
		apoly->xpoints[3] = (int) p2.x;
        
		apoly->ypoints[0] = (int) ay;
		apoly->ypoints[1] = (int) by;
		apoly->ypoints[2] = (int) p2.y;
		apoly->ypoints[3] = (int) p2.y;
    }
}

void Gesture :: smooth() {
    // average neighboring points
    float weight = 18;
    float scale  = 1.0/(weight + 2);
    int nPointsMinusTwo = nPoints - 2;
    Vec3f *lower, *upper, *center;
    
    for (int i=1; i<nPointsMinusTwo; i++) {
        lower = &path[i-1];
        center = &path[i];
        upper = &path[i+1];
        
        center->x = (lower->x + weight * center->x + upper->x) * scale;
        center->y = (lower->y + weight * center->y + upper->y) * scale;
    }
}
