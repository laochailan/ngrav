#ifndef QUATERNION_H
#define QUATERNION_H

#include "mathutil.h"

typedef struct quat quat;
struct quat {
	double s;
	double v[3];
};

void qadd(quat *a, quat *b);
void qmul(quat *dest, quat *a, quat *b);

void rot_quat(quat *dest, double angle, vec naxis);
void qconj(quat *dest);

void qinverse(quat *dest, quat *q);

void qrot_vec(vec dest, quat *q, vec x);

void quat_to_matrix(quat *q, mat4 dest);

float qlength(quat *q);

#endif
