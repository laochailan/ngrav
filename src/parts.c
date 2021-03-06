#include "parts.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bhtree.h"

void rand_sphere(vec x, double r) {
	do {
		x[0] = r*(2*randf()-1);
		x[1] = r*(2*randf()-1);
		x[2] = r*(2*randf()-1);
	} while(vnormsq(x) > r*r);
}

void rand_cube(vec x, double r) {
	x[0] = r*(2*randf()-1);
	x[1] = 0.3*r*(2*randf()-1);
	x[2] = 0.3*r*(2*randf()-1);
}

void rand_disk(vec x, double r) {
	do {
		x[0] = r*(2*randf()-1);
		x[1] = r*(2*randf()-1);
		x[2] = 0.01*r*(2*randf()-1);
	} while(vnormsq(x) > r*r);
}

int parts_init(Parts *p) {
	int i;

	memset(p,0,sizeof(Parts));
	p->dt = 0.01;
	p->N = 20000;
	p->parts = calloc(p->N, sizeof(Part));
	memset(p->parts,0,sizeof(Part)*p->N);

	if(!p->parts) {
		return 1;
	}

	for(i = 0; i < p->N; i++) {
		rand_disk(p->parts[i].x,0.3);

		p->parts[i].m = 10e-7;

		p->parts[i].v[0] = -p->parts[i].x[1];
		p->parts[i].v[1] = p->parts[i].x[0];
		double r = sqrt(vnormsq(p->parts[i].v));
		vmul(p->parts[i].v, 1.3*sqrt(p->N*p->parts[i].m/0.3/0.3/0.3));

	}
	return 0;
}

void parts_deinit(Parts *p) {
	free(p->parts);
	bhtree_free(p->bhtree);
}

void parts_step(Parts *p) {
	int i;
	
/*
#pragma omp parallel for
	for(i = 0; i < p->N; i++) {
		int j;
		vec a;
		memset(a,0,sizeof(vec));
		
		for(j = 0; j < p->N; j++) {
			double r;

			if(i == j)
				continue;
			vec dx;
			vcopy(dx,p->parts[j].x);
			vsub(dx,p->parts[i].x);
			r = sqrt(vnormsq(dx));
			if(r < 2e-3) {
				double f = r/2e-3;
				f*=f;
				vmul(dx,f*f);
			}
			
			vmul(dx,p->parts[j].m/r/r/r);
			vadd(a,dx);
		}
		vmul(a,p->dt);
		vadd(p->parts[i].v,a);
	}
*/
	bhtree_free(p->bhtree);
	bhtree_construct(p);
#pragma omp parallel for
	for(i = 0; i < p->N; i++) {
		vec a;
		bhtree_force(p->bhtree, a, p->parts[i].x, 1);
		vmul(a,p->dt);
		vadd(p->parts[i].v,a);
	}
#pragma omp parallel for
	for(i = 0; i < p->N; i++) {
		vec dx;
		vcopy(dx,p->parts[i].v);
		vmul(dx,p->dt);
		vadd(p->parts[i].x, dx);

		double r = vnormsq(p->parts[i].x);
		if(r > 12)
			vmul(p->parts[i].x,-1);
	}
/*
	vec P;
	P[0] = 0;
	P[1] = 0;
	P[2] = 0;
	for(i = 0; i < p->N; i++) {
		P[0] += p->parts[i].v[0];
		P[1] += p->parts[i].v[0];
		P[2] += p->parts[i].v[0];
	}
	printf("Momentum %f %f %f\n", P[0], P[1], P[2]);
*/
	p->t += p->dt;
}
