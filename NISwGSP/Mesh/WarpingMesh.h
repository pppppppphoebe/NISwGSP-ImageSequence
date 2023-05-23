#pragma once
#include "../Configure.h"

class WarpingMesh {
public:
	WarpingMesh(int _cols, int _rows, vector<Point2> _vertices);
	vector<Point2> getVertices() ;

	int getQuadMeshCount();
	bool isPointinQuadMesh(Point2 A, Point2 B, Point2 C, Point2 D, Point2 pt);
	int findPointNearestMeshQuadIndex(Point2 pt);
	vector<float> getInterpolateVertexWeights(Point2 pt, int quadId);

	vector<Point2>vertices;
	int nw;
	int nh;


};