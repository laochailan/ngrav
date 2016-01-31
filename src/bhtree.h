#ifndef BHTREE_H
#define BHTREE_H

#include "mathutil.h"

struct Parts;

typedef struct BHNode BHNode;
struct BHNode {
	int level;
	double width;
	vec divp; // division point

	int n; // number of particles in the node
	double m; // overall mass

	vec cm; // center of mass
	
	BHNode *children[8];
};

void bhtree_construct(struct Parts *p);
void bhtree_free(BHNode *bh);

void bhtree_force(BHNode *bh, vec f, vec x, double acc);

#endif
