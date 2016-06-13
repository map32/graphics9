#ifndef GMATH_H
#define GMATH_H

#include "matrix.h"

double * cross(double *a, double *b);

double * calculate_normal( double a1, double a2, double a3,
			   double b1, double b2, double b3 );
double calculate_dot( struct matrix *points, int i );

double * normal( struct matrix *points, int i);

double mag( double *v);

double * normalize( double *v);

double dot(double *a, double *b);

#endif
