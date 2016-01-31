#ifndef PARTS_H
#define PARTS_H

#include "mathutil.h"

struct BHNode;

typedef struct Part Part;
struct Part {
	vec x;
	vec v;
	double m;
};


typedef struct Parts Parts;
struct Parts {
	int N;
	Part *parts;
	struct BHNode *bhtree;

	double dt;
	double t;
};

int parts_init(Parts *p);
void parts_deinit(Parts *p);

void parts_step(Parts *p);

#endif
