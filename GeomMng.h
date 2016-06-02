#pragma once
#include "Renderer.h"
#include <stdio.h>
#include <vector>

struct QuadInfo {
	float	x,y;
	float	w,h;
	col_t	r,g,b,a;
};

enum GeomLayer {
	GEOM_LAYER_BG,
	GEOM_LAYER_NORMAL,
	GEOM_LAYER_FG
};

class GeomMng {
	std::vector<QuadInfo> quads[3];
public:
	void FromFile(FILE *fp, int len);
	void Draw(GeomLayer depth);
	void Clear();
};
