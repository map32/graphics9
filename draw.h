#ifndef DRAW_H
#define DRAW_H

#include "matrix.h"

#define MAX_STEPS 100

color flat( struct matrix *poly, int i, color c, double *l, double *kr, double *kg, double *kb);
color gouraud( struct matrix *poly, color c, double *l, double *kr, double *kg, double *kb, double x0, double y0, double z0, double x1, double y1, double z1, double x2, double y2, double z2, double x, double y);
color phong( struct matrix *poly, color c, double *l, double *kr, double *kg, double *kb, double x0, double y0, double z0, double x1, double y1, double z1, double x2, double y2, double z2, double x, double y);
double * vertexnormal(struct matrix *poly, double x, double y, double z);
void draw_line(int x0, int y0,
	       int x1, int y1, 
	       screen s, color c);
void draw_line2(int x0, int y0, double z0,
		int x1, int y1, double z1,
		screen s, color c, zbuffer b);
void draw_line3(int x0, int y0, double z0,
		int x1, int y1, double z1,
		screen s, color c, color c2, zbuffer b);
void draw_line4(struct matrix *poly, color c, double *l,
		double *kr, double *kg, double *kb,
		double xa, double ya, double za, double xb,
		double yb, double zb, double xc, double yc,
		double zc,
	        int x0, int y0, double z0,
		int x1, int y1, double z1,
		screen s, zbuffer b);
void add_point( struct matrix * points, 
		 double x, double y, double z);
void add_edge( struct matrix * points, 
	       double x0, double y0, double z0, 
	       double x1, double y1, double z1);
void draw_lines( struct matrix * points, screen s, color c);
void draw_lines2( struct matrix * points, screen s, color c, zbuffer b);
void draw_polygons( struct matrix * points, screen s, color c);
void draw_polygons2( struct matrix * points, screen s, color c, zbuffer b);
void draw_polygons3( struct matrix * points, screen s, color c, double *l, double *kr, double *kg, double *kb, zbuffer b, int t);
void color_polygons( struct matrix * points, int i, screen s, color c);
void color_polygons2( struct matrix * points, int i, screen s, color c, zbuffer b);
void color_polygons3( struct matrix * points, int i, screen s, color c, double *l, double *kr, double *kg, double *kb, zbuffer b, int type);
color ambient( color c, double kr, double kg, double kb);
color diffuse( double *n, color c, double *l, double kr, double kg, double kb);
color specular(double *n, color c, double *l, double kr, double kg, double kb);

//advanced shapes
void add_circle( struct matrix * points, 
		 double cx, double cy, 
		 double r, double step );
void add_curve( struct matrix *points, 
		double x0, double y0,
		double x1, double y1,
		double x2, double y2,
		double x3, double y3,
		double step, int type );
void add_box( struct matrix *points,
	      double x, double y, double z,
	      double w, double h, double d);
void add_sphere( struct matrix * points, 
		 double cx, double cy, double cz, double r, 
		 int step );
void generate_sphere( struct matrix * points, 
		      double cx, double cy, double cz, double r, 
			   int step );
void add_torus( struct matrix * points, 
		double cx, double cy, double cz, double r1, double r2, 
		     int step );
void generate_torus( struct matrix * points, 
		     double cx, double cy, double cz, double r1, double r2, 
			   int step );
#endif
