#ifndef MATHUTIL_H
#define MATHUTIL_H

typedef double vec[3];
typedef float mat4[16];

double randn(void);
double randf(void);

void vcopy(vec a, vec b);
void vmul(vec v, double a);
void vadd(vec a, vec b);
void vsub(vec a, vec b);
double vnormsq(vec a);

double vdot(vec a, vec b);
void vcross(vec v, vec a, vec b);

#endif
