#include "mathutil.h"

#include <stdlib.h>
#include <math.h>
#include <float.h>

/* Box Muller transform (from Wikipedia) */
double randn(void) {
	static double z0, z1;
	static int generate = 0;
	double u1, u2;

	generate = !generate;

	if(!generate)
		return z1;

	do {
		u1 = randf();
		u2 = randf();
	} while(u1 <= DBL_MIN);

	z0 = sqrt(-2.*log(u1))*cos(2*M_PI*u2);
	z1 = sqrt(-2.*log(u1))*sin(2*M_PI*u2);

	return z0;
}


double randf(void) {
	return random()/(double)RAND_MAX;
}

void vcopy(vec a, vec b) {
	a[0] = b[0];
	a[1] = b[1];
	a[2] = b[2];
}

void vmul(vec v, double a) {
	v[0] *= a;
	v[1] *= a;
	v[2] *= a;
}

void vadd(vec a, vec b) {
	a[0] += b[0];
	a[1] += b[1];
	a[2] += b[2];
}

void vsub(vec a, vec b) {
	a[0] -= b[0];
	a[1] -= b[1];
	a[2] -= b[2];
}

double vnormsq(vec a) {
	return a[0]*a[0]+a[1]*a[1]+a[2]*a[2];
}

double vdot(vec a, vec b) {
	return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
}

void vcross(vec v, vec a, vec b) { 
	v[0] = a[1]*b[2]-a[2]*b[1];
	v[1] = a[2]*b[0]-a[0]*b[2];
	v[2] = a[0]*b[1]-a[1]*b[0];
}
