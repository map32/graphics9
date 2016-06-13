#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "matrix.h"
#include "gmath.h"



/*======== double * calculate_normal() ==========
  Inputs:   double ax
            double ay
	    double az
	    double bx
	    double by
	    double bz  
  Returns: A double arry of size 3 representing the 
           cross product of <ax, ay, az> and <bx, by, bz>

  04/17/12 16:46:30
  jonalf
  ====================*/

double * cross( double *a, double *b){
  return calculate_normal(a[0],a[1],a[2],b[0],b[1],b[2]);
}

double * calculate_normal( double ax, double ay, double az,	
			   double bx, double by, double bz ) {
  
  double* norm;
  norm = (double *)malloc(3 * sizeof(double));

  norm[0] = ay*bz - az*by;
  norm[1] = az*bx - ax*bz;
  norm[2] = ax*by - ay*bx;

  return norm;
}

/*======== double calculate_dot() ==========
  Inputs:   struct matrix *points
            int i  
  Returns: The dot product of a surface normal and
           a view vector
  
  calculates the dot product of the surface normal to
  triangle points[i], points[i+1], points[i+2] and a 
  view vector (use <0, 0, -1> to start.

  04/17/12 16:38:34
  jonalf
  ====================*/
double calculate_dot( struct matrix *points, int i ) {

  double *norm;
  double vx, vy, vz;
  double dot;

  norm = normal(points,i);

  //set up view vector
  vx = 0;
  vy = 0;
  vz = -1;

  //calculate dot product
  dot = norm[0] * vx + norm[1] * vy + norm[2] * vz;

  free(norm);  
  return dot;
}

double * normal( struct matrix *points, int i){
  double ax, ay, az, bx, by, bz;
  double *norm;

  //calculate A and B vectors
  ax = points->m[0][i+1] - points->m[0][i];
  ay = points->m[1][i+1] - points->m[1][i];
  az = points->m[2][i+1] - points->m[2][i];

  bx = points->m[0][i+2] - points->m[0][i];
  by = points->m[1][i+2] - points->m[1][i];
  bz = points->m[2][i+2] - points->m[2][i];

  //get the surface normal
  norm = calculate_normal( ax, ay, az, bx, by, bz );
  return norm;
}

double mag( double *v){
  return sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
}

double * normalize( double *v) {
  double f = mag(v);
  double *n = (double*)malloc(3*sizeof(double));
  n[0] = v[0]/f;
  n[1] = v[1]/f;
  n[2] = v[2]/f;
  return n;
}

double dot(double *a, double *b){
  return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
}
