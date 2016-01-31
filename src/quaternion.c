#include "quaternion.h"

#include <math.h>
#include <string.h>

void qadd(quat *a, quat *b) {
	a->s += b->s;

	vadd(a->v, b->v);
}

void qmul(quat *dest, quat *a, quat *b) {
	vec tmp;
	
	dest->s = a->s*b->s - vdot(a->v,b->v);
	
	memcpy(dest->v, a->v, sizeof(vec));
	vmul(dest->v, b->s);
	
	memcpy(tmp, b->v, sizeof(vec));
	vmul(tmp, a->s);
	
	vadd(dest->v, tmp);
	
	vcross(tmp, a->v, b->v);
	vadd(dest->v, tmp);
}

void rot_quat(quat *dest, double angle, vec naxis) {
	dest->s = cos(angle*0.5f);
	memcpy(dest->v, naxis, sizeof(vec));
	
	vmul(dest->v, sin(angle*0.5f));
}

void qconj(quat *dest) {
	vmul(dest->v,-1);
}

static float qlengthsquare(quat *q) {
	return q->s*q->s + q->v[0]*q->v[0] + q->v[1]*q->v[1] + q->v[2]*q->v[2];
}

void qinverse(quat *dest, quat *q) {
	memcpy(dest, q, sizeof(quat));
	qconj(dest);
	
	vmul(dest->v, 1.0/qlengthsquare(q));
}

void qrot_vec(vec dest, quat *q, vec x) {
	quat qc, qx, tmpdest;
	memcpy(&qc, q, sizeof(quat));
	qconj(&qc);
	
	qx.s = 0;
	memcpy(qx.v, x, sizeof(vec));
	
	qmul(&tmpdest, q, &qx);
	qmul(&qx, &tmpdest, &qc);
	
	memcpy(dest, qx.v, sizeof(vec));
}

void quat_to_matrix(quat *q, mat4 dest) {
	memset(dest,0,sizeof(mat4));
	dest[0] = 1-2*(q->v[1]*q->v[1] + q->v[2]*q->v[2]);
	dest[1] = 2*q->s*q->v[2] + 2*q->v[0]*q->v[1];
	dest[2] = -2*q->s*q->v[1] + 2*q->v[0]*q->v[2];
		
	dest[4] = -2*q->s*q->v[2] + 2* q->v[0]*q->v[1];
	dest[5] = 1-2*(q->v[0]*q->v[0] + q->v[2]*q->v[2]);
	dest[6] = 2*q->s*q->v[0] + 2*q->v[1]*q->v[2];
		
	dest[8] = 2*q->s*q->v[1] + 2*q->v[0]*q->v[2];
	dest[9] = -2*q->s*q->v[0] + 2*q->v[1]*q->v[2];
	dest[10] = 1-2*(q->v[0]*q->v[0] + q->v[1]*q->v[1]);
	dest[15] = 1;
}

inline float qlength(quat *q) {
	return sqrt(qlengthsquare(q));
}
