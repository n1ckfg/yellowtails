/**
 * Yellowtail
 * by Golan Levin (www.flong.com).
 * Translated to p5.js by Nick Fox-Gieg
 * Ported to three.js
 *
 * Click, drag, and release to create a kinetic gesture.
 *
 * Yellowtail (1998-2000) is an interactive software system for the gestural
 * creation and performance of real-time abstract animation. Yellowtail repeats
 * a user's strokes end-over-end, enabling simultaneous specification of a
 * line's shape and quality of movement. Each line repeats according to its
 * own period, producing an ever-changing and responsive display of lively,
 * worm-like textures.
 */

"use strict";

const PG_W = 800;
const PG_H = 800;

let gestureArray;
let nGestures;  // Number of gestures
let minMove;     // Minimum travel for a new point
let currentGestureID;

let localX, localY;
let mouseDown = false;

let renderer, scene, camera, mesh, positionAttr, positions;
let maxVertsPerGesture;

function setup() {
    gestureArray = [];
    nGestures = 36;  // Number of gestures
    minMove = 3;     // Minimum travel for a new point

    renderer = new THREE.WebGLRenderer({ antialias: false });
    renderer.setSize(PG_W, PG_H);
    renderer.setClearColor(0x000000, 1);
    renderer.domElement.style.width = "100%";
    renderer.domElement.style.height = "100%";
    document.body.appendChild(renderer.domElement);

    scene = new THREE.Scene();

    // y-down orthographic camera matching the p5 coordinate space
    camera = new THREE.OrthographicCamera(0, PG_W, 0, -PG_H, -1, 1);

    // 6 quads max per polygon (base + x-wrap x2 + y-wrap x2), 6 verts per quad
    maxVertsPerGesture = (600 - 1) * 6 * 6;

    const geometry = new THREE.BufferGeometry();
    positions = new Float32Array(nGestures * maxVertsPerGesture * 3);
    positionAttr = new THREE.BufferAttribute(positions, 3);
    positionAttr.setUsage(THREE.DynamicDrawUsage);
    geometry.setAttribute("position", positionAttr);
    geometry.setDrawRange(0, 0);

    const material = new THREE.MeshBasicMaterial({
        color: 0xfffff5,
        side: THREE.DoubleSide
    });

    mesh = new THREE.Mesh(geometry, material);
    mesh.frustumCulled = false;
    scene.add(mesh);

    currentGestureID = -1;
    gestureArray = new Array(nGestures);

    for (let i = 0; i < nGestures; i++) {
        gestureArray[i] = new Gesture(PG_W, PG_H);
    }

    clearGestures();

    addEvents();
    animate();
}

function animate() {
    requestAnimationFrame(animate);

    updateGeometry();
    renderGestures();
    renderer.render(scene, camera);
}

function addEvents() {
    const canvas = renderer.domElement;

    canvas.addEventListener("mousedown", (e) => {
        mouseDown = true;
        localMouse(e);
        currentGestureID = (currentGestureID + 1) % nGestures;
        let G = gestureArray[currentGestureID];
        G.clear();
        G.clearPolys();
        G.addPoint(localX, localY);
    });

    window.addEventListener("mousemove", (e) => {
        localMouse(e);
        if (mouseDown && currentGestureID >= 0) {
            let G = gestureArray[currentGestureID];
            if (G.distToLast(localX, localY) > minMove) {
                G.addPoint(localX, localY);
                G.smooth();
                G.compile();
            }
        }
    });

    window.addEventListener("mouseup", () => {
        mouseDown = false;
    });

    window.addEventListener("keydown", (e) => {
        if (e.key === "+" || e.key === "=") {
            if (currentGestureID >= 0) {
                let th = gestureArray[currentGestureID].thickness;
                gestureArray[currentGestureID].thickness = Math.min(96, th + 1);
                gestureArray[currentGestureID].compile();
            }
        } else if (e.key === "-") {
            if (currentGestureID >= 0) {
                let th = gestureArray[currentGestureID].thickness;
                gestureArray[currentGestureID].thickness = Math.max(2, th - 1);
                gestureArray[currentGestureID].compile();
            }
        } else if (e.key === " ") {
            clearGestures();
        }
    });
}

function localMouse(e) {
    const rect = renderer.domElement.getBoundingClientRect();
    localX = (e.clientX - rect.left) / rect.width * PG_W;
    localY = (e.clientY - rect.top) / rect.height * PG_H;
}

function renderGestures() {
    const w = PG_W;
    const h = PG_H;

    let v = 0; // vertex cursor
    let f = 0; // float cursor

    for (let g = 0; g < nGestures; g++) {
        const gesture = gestureArray[g];
        if (!gesture.exists || gesture.nPolys <= 0) continue;

        const polygons = gesture.polygons;
        const crosses = gesture.crosses;
        const gnp = gesture.nPolys;

        for (let i = 0; i < gnp; i++) {
            const p = polygons[i];
            const xpts = p.xpoints;
            const ypts = p.ypoints;

            v = writeQuad(positions, f, xpts[0], ypts[0], xpts[1], ypts[1], xpts[2], ypts[2], xpts[3], ypts[3]);
            f = v * 3;

            const cr = crosses[i];
            if (cr > 0) {
                if ((cr & 3) > 0) {
                    v = writeQuad(positions, f, xpts[0] + w, ypts[0], xpts[1] + w, ypts[1], xpts[2] + w, ypts[2], xpts[3] + w, ypts[3]);
                    f = v * 3;
                    v = writeQuad(positions, f, xpts[0] - w, ypts[0], xpts[1] - w, ypts[1], xpts[2] - w, ypts[2], xpts[3] - w, ypts[3]);
                    f = v * 3;
                }
                if ((cr & 12) > 0) {
                    v = writeQuad(positions, f, xpts[0], ypts[0] + h, xpts[1], ypts[1] + h, xpts[2], ypts[2] + h, xpts[3], ypts[3] + h);
                    f = v * 3;
                    v = writeQuad(positions, f, xpts[0], ypts[0] - h, xpts[1], ypts[1] - h, xpts[2], ypts[2] - h, xpts[3], ypts[3] - h);
                    f = v * 3;
                }

                // I have knowingly retained the small flaw of not
                // completely dealing with the corner conditions
                // (the case in which both of the above are true).
            }
        }
    }

    mesh.geometry.setDrawRange(0, v);
    positionAttr.needsUpdate = true;
}

// writes one quad as two triangles, returns the new vertex count
function writeQuad(arr, f, x0, y0, x1, y1, x2, y2, x3, y3) {
    arr[f] = x0; arr[f + 1] = y0; arr[f + 2] = 0;
    arr[f + 3] = x1; arr[f + 4] = y1; arr[f + 5] = 0;
    arr[f + 6] = x2; arr[f + 7] = y2; arr[f + 8] = 0;
    arr[f + 9] = x0; arr[f + 10] = y0; arr[f + 11] = 0;
    arr[f + 12] = x2; arr[f + 13] = y2; arr[f + 14] = 0;
    arr[f + 15] = x3; arr[f + 16] = y3; arr[f + 17] = 0;
    return f / 3 + 6;
}

function updateGeometry() {
    let J;
    for (let g = 0; g < nGestures; g++) {
        if ((J = gestureArray[g]).exists) {
            if (g != currentGestureID) {
                advanceGesture(J);
            } else if (!mouseDown) {
                advanceGesture(J);
            }
        }
    }
}

function advanceGesture(gesture) {
    // Move a Gesture one step
    if (gesture.exists) {
        let nPts = gesture.nPoints;
        let nPts1 = nPts - 1;
        let path = [];
        let jx = gesture.jumpDx;
        let jy = gesture.jumpDy;

        if (nPts > 0) {
            path = gesture.path;

            for (let i = nPts1; i > 0; i--) {
                path[i].x = path[i - 1].x;
                path[i].y = path[i - 1].y;
            }

            path[0].x = path[nPts1].x - jx;
            path[0].y = path[nPts1].y - jy;
            gesture.compile();
        }
    }
}

function clearGestures() {
    for (let i = 0; i < nGestures; i++) {
        gestureArray[i].clear();
    }
}

class Gesture {

    constructor(mw, mh) {
        this.damp = 5.0;
        this.dampInv = 1.0 / this.damp;
        this.damp1 = this.damp - 1;

        this.w = mw;
        this.h = mh;
        this.capacity = 600;

        this.path = new Array(this.capacity); // Vec3f
        this.polygons = new Array(this.capacity); // Polygon
        this.crosses = new Array(this.capacity); // int

        for (let i = 0; i < this.capacity; i++) {
            this.polygons[i] = new Polygon(4);
            this.path[i] = new Vec3f(0, 0, 0);
            this.crosses[i] = 0;
        }

        this.nPoints = 0;
        this.nPolys = 0;

        this.exists = false;
        this.jumpDx = 0;
        this.jumpDy = 0;

        this.INIT_TH = 14;
        this.thickness = this.INIT_TH;
    }

    clear() {
        this.nPoints = 0;
        this.exists = false;
        this.thickness = this.INIT_TH;
    }

    clearPolys() {
        this.nPolys = 0;
    }

    addPoint(x, y) {
        if (this.nPoints >= this.capacity) {
            // there are all sorts of possible solutions here,
            // but for abject simplicity, I don't do anything.
        } else {
            let v = this.distToLast(x, y);
            let p = this.getPressureFromVelocity(v);

            this.path[this.nPoints++].set(x, y, p);

            if (this.nPoints > 1) {
                this.exists = true;
                this.jumpDx = this.path[this.nPoints - 1].x - this.path[0].x;
                this.jumpDy = this.path[this.nPoints - 1].y - this.path[0].y;
            }
        }
    }

    getPressureFromVelocity(v) {
        let scale = 18;
        let minP = 0.02;
        let oldP = (this.nPoints > 0) ? this.path[this.nPoints - 1].p : 0;
        return ((minP + Math.max(0, 1.0 - v / scale)) + (this.damp1 * oldP)) * this.dampInv;
    }

    setPressures() {
        // pressures vary from 0...1
        let pressure;
        let t = 0;
        let u = 1.0 / (this.nPoints - 1) * Math.PI * 2;

        for (let i = 0; i < this.nPoints; i++) {
            pressure = Math.sqrt((1.0 - Math.cos(t)) * 0.5);
            this.path[i].p = pressure;
            t += u;
        }
    }

    distToLast(ix, iy) {
        if (this.nPoints > 0) {
            let v = this.path[this.nPoints - 1];
            let dx = v.x - ix;
            let dy = v.y - iy;
            return Math.sqrt(dx * dx + dy * dy);
        } else {
            return 30;
        }
    }

    compile() {
        // compute the polygons from the path of Vec3f's
        if (this.exists) {
            this.clearPolys();

            let p0, p1, p2;
            let radius0, radius1;
            let ax, bx, cx, dx;
            let ay, by, cy, dy;
            let axi, bxi, cxi, dxi, axip, axid;
            let ayi, byi, cyi, dyi, ayip, ayid;
            let p1x, p1y;
            let dx01, dy01, hp01, si01, co01;
            let dx02, dy02, hp02, si02, co02;
            let taper = 1.0;

            let nPathPoints = this.nPoints - 1;
            let lastPolyIndex = nPathPoints - 1;
            let npm1finv = 1.0 / Math.max(1, nPathPoints - 1);

            // handle the first point
            p0 = this.path[0];
            p1 = this.path[1];
            radius0 = p0.p * this.thickness;
            dx01 = p1.x - p0.x;
            dy01 = p1.y - p0.y;
            hp01 = Math.sqrt(dx01 * dx01 + dy01 * dy01);

            if (hp01 == 0) {
                hp01 = 0.0001;
            }

            co01 = radius0 * dx01 / hp01;
            si01 = radius0 * dy01 / hp01;
            ax = p0.x - si01;
            ay = p0.y + co01;
            bx = p0.x + si01;
            by = p0.y - co01;

            let LC = 20;
            let RC = this.w - LC;
            let TC = 20;
            let BC = this.h - TC;
            let mint = 0.618;
            let tapow = 0.4;

            // handle the middle points
            let apoly; // Polygon

            for (let i = 1; i < nPathPoints; i++) {
                taper = Math.pow((lastPolyIndex - i) * npm1finv, tapow);

                p0 = this.path[i - 1];
                p1 = this.path[i];
                p2 = this.path[i + 1];
                p1x = p1.x;
                p1y = p1.y;
                radius1 = Math.max(mint, taper * p1.p * this.thickness);

                // assumes all segments are roughly the same length...
                dx02 = p2.x - p0.x;
                dy02 = p2.y - p0.y;
                hp02 = Math.sqrt(dx02 * dx02 + dy02 * dy02);

                if (hp02 != 0) {
                    hp02 = radius1 / hp02;
                }

                co02 = dx02 * hp02;
                si02 = dy02 * hp02;

                // translate the integer coordinates to the viewing rectangle
                axi = axip = Math.floor(ax);
                ayi = ayip = Math.floor(ay);
                axi = (axi < 0) ? (this.w - ((-axi) % this.w)) : axi % this.w;
                axid = axi - axip;
                ayi = (ayi < 0) ? (this.h - ((-ayi) % this.h)) : ayi % this.h;
                ayid = ayi - ayip;

                // set the vertices of the polygon

                apoly = this.polygons[this.nPolys++];

                let xpts = apoly.xpoints;
                let ypts = apoly.ypoints;
                xpts[0] = axi = axid + axip;
                xpts[1] = bxi = axid + Math.floor(bx);
                xpts[2] = cxi = axid + Math.floor((cx = p1x + si02));
                xpts[3] = dxi = axid + Math.floor((dx = p1x - si02));
                ypts[0] = ayi = ayid + ayip;
                ypts[1] = byi = ayid + Math.floor(by);
                ypts[2] = cyi = ayid + Math.floor((cy = p1y - co02));
                ypts[3] = dyi = ayid + Math.floor((dy = p1y + co02));

                // keep a record of where we cross the edge of the screen
                this.crosses[i] = 0;
                if ((axi <= LC) || (bxi <= LC) || (cxi <= LC) || (dxi <= LC)) {
                    this.crosses[i] |= 1;
                }
                if ((axi >= RC) || (bxi >= RC) || (cxi >= RC) || (dxi >= RC)) {
                    this.crosses[i] |= 2;
                }
                if ((ayi <= TC) || (byi <= TC) || (cyi <= TC) || (dyi <= TC)) {
                    this.crosses[i] |= 4;
                }
                if ((ayi >= BC) || (byi >= BC) || (cyi >= BC) || (dyi >= BC)) {
                    this.crosses[i] |= 8;
                }

                //swap data for next time
                ax = dx;
                ay = dy;
                bx = cx;
                by = cy;
            }

            // handle the last point
            p2 = this.path[nPathPoints];

            apoly = this.polygons[this.nPolys++];

            let xpts = apoly.xpoints;
            let ypts = apoly.ypoints;

            xpts[0] = Math.floor(ax);
            xpts[1] = Math.floor(bx);
            xpts[2] = Math.floor((p2.x));
            xpts[3] = Math.floor((p2.x));

            ypts[0] = Math.floor(ay);
            ypts[1] = Math.floor(by);
            ypts[2] = Math.floor((p2.y));
            ypts[3] = Math.floor((p2.y));
        }
    }

    smooth() {
        // average neighboring points
        let weight = 18;
        let scale = 1.0 / (weight + 2);
        let nPointsMinusTwo = this.nPoints - 2;
        let lower, upper, center;

        for (let i = 1; i < nPointsMinusTwo; i++) {
            lower = this.path[i - 1];
            center = this.path[i];
            upper = this.path[i + 1];

            center.x = (lower.x + weight * center.x + upper.x) * scale;
            center.y = (lower.y + weight * center.y + upper.y) * scale;
        }
    }

}

class Polygon {

    constructor(n) {
        this.npoints = n;
        this.xpoints = new Array(n);
        this.ypoints = new Array(n);
    }

}

class Vec3f {

    constructor(ix, iy, ip) {
        this.x = 0;
        this.y = 0;
        this.p = 0; // Pressure

        this.set(ix, iy, ip);
    }

    set(ix, iy, ip) {
        this.x = ix;
        this.y = iy;
        this.p = ip;
    }

}

setup();
