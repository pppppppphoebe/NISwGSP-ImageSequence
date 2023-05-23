#include "WarpingMesh.h"

WarpingMesh::WarpingMesh(int _nw, int _nh, vector<Point2> _vertices) {
	nw = _nw;
	nh = _nh;
	vertices = _vertices;
};

vector<Point2> WarpingMesh::getVertices() {
	return vertices;
}

int WarpingMesh::getQuadMeshCount() {
	return nw * nh;
}

bool WarpingMesh::isPointinQuadMesh(Point2 A, Point2 B, Point2 C, Point2 D, Point2 pt) {
	Point2 AB(B.x - A.x, -(B.y - A.y));  float C1 = -1 * (AB.y * A.x + AB.x * A.y); float D1 = (AB.y * pt.x + AB.x * pt.y) + C1;
	Point2 AD(D.x - A.x, -(D.y - A.y));  float C2 = -1 * (AD.y * A.x + AD.x * A.y); float D2 = (AD.y * pt.x + AD.x * pt.y) + C2;
	Point2 BC(C.x - B.x, -(C.y - B.y));  float C3 = -1 * (BC.y * B.x + BC.x * B.y); float D3 = (BC.y * pt.x + BC.x * pt.y) + C3;
	Point2 CD(D.x - C.x, -(D.y - C.y));  float C4 = -1 * (CD.y * C.x + CD.x * C.y); float D4 = (CD.y * pt.x + CD.x * pt.y) + C4;
	return     0 >= D1 && 0 >= D4 && 0 <= D2 && 0 >= D3;
}

int WarpingMesh::findPointNearestMeshQuadIndex(Point2 pt) {
	int quadId = -1;
	bool isinside = false;
	const vector<Point2> nexts = {
		Point2(0, 0), Point2(0, 1), Point2(1, 1), Point2(1, 0)
	};
	vector<Point2> neighbours(nexts.size(), Point2());

	for (int i = 0; i < nw * nh; i++) {
		int gridy = i / nw;
		int gridx = i % nw;
		
		for (int n = 0; n < nexts.size(); n++) {
			int v_id = (gridy + nexts[n].y) * (nw+1) + (gridx + nexts[n].x);		
			neighbours[n] = vertices[v_id];
		}

		isinside = isPointinQuadMesh(neighbours[0], neighbours[1], neighbours[2], neighbours[3], pt);

		if (isinside) {
			quadId = i;
			break;
		}
	}
	return quadId;
}

vector<float> WarpingMesh::getInterpolateVertexWeights(Point2 pt,int quadId) {
	int gridy = quadId / nw;
	int gridx = quadId % nw;

	vector<float> weights(4,0);

	const vector<Point2> nexts = {
		Point2(0, 0), Point2(0, 1), Point2(1, 1), Point2(1, 0)
	};

	float totaldis = 0;
	for (int n = 0; n < nexts.size(); n++) {

		int v_id = (gridy + nexts[n].y) * (nw+1) + (gridx + nexts[n].x);
		Point2 p2 = vertices[v_id];

		float dis = norm(pt - p2);
		weights[n] = dis;
		totaldis += dis;
	}

	for (int n = 0; n < nexts.size(); n++) {
		int v_id = (gridy + nexts[n].y) * (nw + 1) + (gridx + nexts[n].x);
		weights[n] = weights[n] / totaldis ;
	}
	return weights;
}

