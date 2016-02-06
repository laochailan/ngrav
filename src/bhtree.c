#include "bhtree.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "parts.h"

enum {
	MAXLEVEL = 12
};

static BHNode *alloc_node(void) {
	BHNode *n = malloc(sizeof(BHNode));
	memset(n, 0, sizeof(BHNode));
	return n;
}

static int child_idx(vec divp, vec x) {
	int x0 = x[0] < divp[0];
	int x1 = x[1] < divp[1];
	int x2 = x[2] < divp[2];

	return x0+2*x1+4*x2;
}

static void add_node(Part *p, BHNode *n) {
	vec v;

	if(n->n == 1 && n->level < MAXLEVEL) { // split tree
		vec x;
		int i;
		
		for(i = 0; i < 8; i++) {
			n->children[i] = alloc_node();
			n->children[i]->width = n->width/2;
			n->children[i]->level = n->level+1;
			vcopy(n->children[i]->divp, n->divp);
						
			n->children[i]->divp[0] += n->width/4*(1-2*(i&1));
			n->children[i]->divp[1] += n->width/4*(1-2*(i&2)/2);
			n->children[i]->divp[2] += n->width/4*(1-2*(i&4)/4);
		}
		vcopy(x, n->cm);
		vmul(x,1/n->m);

		i = child_idx(n->divp, x);
		n->children[i]->n = 1;
		n->children[i]->m = n->m;
		vcopy(n->children[i]->cm, n->cm);
	}		

	n->n++;
	n->m += p->m;
	vcopy(v, p->x);
	vmul(v, p->m);
	vadd(n->cm, v);

	if(n->n < 2 || n->level >= MAXLEVEL)
		return;
	add_node(p, n->children[child_idx(n->divp, p->x)]);
}

static void norm_cm(BHNode *n) {
	//printf("w: %f, n: %d, m: %f\n",n->width, n->n, n->m);
	if(n->n == 0)
		return;

	vmul(n->cm,1/n->m);

	if(n->n == 1 || n->level >= MAXLEVEL)
		return;
	int i;
	for(i = 0; i < 8; i++)
		norm_cm(n->children[i]);

}

void bhtree_construct(Parts *p) {
	int i;

	vec topc;
	vec botc;

	p->bhtree = alloc_node();
	for(i = 0; i < 3; i++) {
		topc[i] = -INFINITY;
		botc[i] = INFINITY;
	}

	for(i = 0; i < p->N; i++) {
		int j;	
		for(j = 0; j < 3; j++) {
			if(topc[j] < p->parts[i].x[j])
				topc[j] = p->parts[i].x[j];
			if(botc[j] > p->parts[i].x[j])
				botc[j] = p->parts[i].x[j];
		}
	}

	p->bhtree->width = -INFINITY;
	for(i = 0; i <3; i++)
		if(topc[i] - botc[i] > p->bhtree->width)
			p->bhtree->width = topc[i] - botc[i];
	vadd(topc,botc);
	vmul(topc,0.5);
	vcopy(p->bhtree->divp,topc);

	for(i = 0; i < p->N; i++) {
		add_node(&p->parts[i], p->bhtree);
	}

	// normalize centers of mass
	norm_cm(p->bhtree);
}
	
void bhtree_free(BHNode *bh) {
	if(bh == 0)
		return;
	if(bh->n > 1 && bh->level < MAXLEVEL) {
		int i;
		for(i = 0; i < 8; i++) {
			bhtree_free(bh->children[i]);
			free(bh->children[i]);
		}
	}
}

static int evals = 0;

void force_rec(BHNode *n, vec f, vec x, double acc) {
	if(n->n == 0)
		return;
	vec d;
	double r;

	d[0] = n->cm[0]-x[0];
	d[1] = n->cm[1]-x[1];
	d[2] = n->cm[2]-x[2];

	r = sqrt(vnormsq(d));
	if(n->n == 1 || n->level >= MAXLEVEL || n->width/r < acc) {
		if(r >= 1e-15) {
			if(r < 1e-3) {
				double f = r/1e-3;
				f*=f;
				vmul(d,f*f);
			}
			
			vmul(d,n->m/r/r/r);
			vadd(f,d);
			evals++;
		}
		return;
	}
	
	if(n->n == 0) {
		return;
	}
	int i;
	for(i = 0; i < 8; i++)
		force_rec(n->children[i], f, x, acc);
}

void bhtree_force(BHNode *n, vec f, vec x, double acc) {
	memset(f,0,sizeof(vec));
	evals = 0;
	force_rec(n, f, x, acc);
}
