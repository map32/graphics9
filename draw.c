#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "gmath.h"

color flat( struct matrix *poly, int i, color c, double *l, double *kr, double *kg, double *kb){
  color c1,c2,c3;
  //printf("flat %d %d %d\n",c.red,c.green,c.blue);
  double *n;
  c1 = ambient(c, kr[0], kg[0], kb[0]);
  n = normalize(normal(poly,i));
  c2 = diffuse(n,c,l,kr[1],kg[1],kb[1]);
  c3 = specular(n,c,l,kr[2],kg[2],kb[2]);
  c.red = c1.red + c2.red + c3.red;
  c.green = c1.green + c2.green + c3.green;
  c.blue = c1.blue + c2.blue + c3.blue;
  free(n);
  return c;
}

color gouraud( struct matrix *poly, color c, double *l, double *kr, double *kg, double *kb, double x0, double y0, double z0, double x1, double y1, double z1, double x2, double y2, double z2, double x, double y){
  color c1,c2,c3,q,w,e;
  c1 = ambient(c, kr[0], kg[0], kb[0]);
  double *n1, *n2, *n3, *t, m, g0,g1,g2;
  n1 = normalize(vertexnormal(poly, x0,y0,z0));
  n2 = normalize(vertexnormal(poly, x1,y1,z1));
  n3 = normalize(vertexnormal(poly, x2,y2,z2));
  q = diffuse(n1,c,l,kr[1],kg[1],kb[1]);
  w = diffuse(n2,c,l,kr[1],kg[1],kb[1]);
  e = diffuse(n3,c,l,kr[1],kg[1],kb[1]);
  m = mag(calculate_normal(x1-x0,y1-y0,0,x2-x0,y2-y0,0));
  g0 = mag(calculate_normal(x1-x,y1-y,0,x2-x,y2-y,0))/m;
  g1 = mag(calculate_normal(x2-x,y2-y,0,x0-x,y0-y,0))/m;
  g2 = mag(calculate_normal(x0-x,y0-y,0,x1-x,y1-y,0))/m;
  c2.red = q.red*g0+w.red*g1*0+e.red*g2*0;
  c2.green = q.green*g0+w.green*g1+e.green*g2;
  c2.blue = q.blue*g0+w.blue*g1+e.blue*g2;
  q = specular(n1,c,l,kr[2],kg[2],kb[2]);
  w = specular(n2,c,l,kr[2],kg[2],kb[2]);
  e = specular(n3,c,l,kr[2],kg[2],kb[2]);
  c3.red = q.red*g0+w.red*g1+e.red*g2;
  c3.green = q.green*g0+w.green*g1+e.green*g2;
  c3.blue = q.blue*g0+w.blue*g1+e.blue*g2;
  c.red = c1.red + c2.red + c3.red;
  //if (c.red>255){
  //  printf("%d %d %d %d",c1.red,c2.red,c3.red,c.red);
  //}
  //  printf("success\n");
  c.green = c1.green + c2.green + c3.green;
  c.blue = c1.blue + c2.blue + c3.blue;
  free(n1);
  free(n2);
  free(n3);
  return c;
}

color phong( struct matrix *poly, color c, double *l, double *kr, double *kg, double *kb, double x0, double y0, double z0, double x1, double y1, double z1, double x2, double y2, double z2, double x, double y){
  color c1,c2,c3,q,w,e;
  c1 = ambient(c, kr[0], kg[0], kb[0]);
  double *n1, *n2, *n3, m, g0,g1,g2;
  double *newn = (double *)malloc(3*sizeof(double));
  n1 = normalize(vertexnormal(poly, x0,y0,z0));
  n2 = normalize(vertexnormal(poly, x1,y1,z1));
  n3 = normalize(vertexnormal(poly, x2,y2,z2));
  m = mag(calculate_normal(x1-x0,y1-y0,0,x2-x0,y2-y0,0));
  g0 = mag(calculate_normal(x1-x,y1-y,0,x2-x,y2-y,0))/m;
  g1 = mag(calculate_normal(x2-x,y2-y,0,x0-x,y0-y,0))/m;
  g2 = mag(calculate_normal(x0-x,y0-y,0,x1-x,y1-y,0))/m;
  newn[0] = n1[0]*g0+n2[0]*g1+n3[0]*g2;
  newn[1] = n1[1]*g0+n2[1]*g1+n3[1]*g2;
  newn[2] = n1[2]*g0+n2[2]*g1+n3[2]*g2;
  newn = normalize(newn);
  c2 = diffuse(newn,c,l,kr[1],kg[1],kb[1]);
  c3 = specular(newn,c,l,kr[2],kg[2],kb[2]);
  c.red = c1.red + c2.red + c3.red;
  //if (c.red>255){
  //  printf("%d %d %d %d",c1.red,c2.red,c3.red,c.red);
  //}
  //  printf("success\n");
  c.green = c1.green + c2.green + c3.green;
  c.blue = c1.blue + c2.blue + c3.blue;
  free(n1);
  free(n2);
  free(n3);
  free(newn);
  return c;
}

double * vertexnormal( struct matrix *polygons, double x, double y, double z){
  int i, p;
  p=0;
  double *n = (double *) malloc(3*(sizeof(double)));
  double *s;
  n[0]=0;
  n[1]=0;
  n[2]=0;
  for( i=0; i < polygons->lastcol-2; i+=3 ) {
    if( (fabs(polygons->m[0][i] - x)<3 && fabs(polygons->m[1][i] - y)<3 && fabs(polygons->m[2][i] - z)<100)/** ||
	   (fabs(polygons->m[0][i+1] - x)<3 && fabs(polygons->m[1][i+1] - y)<3 && fabs(polygons->m[2][i+1] -z)<30) ||
	   (fabs(polygons->m[0][i+2] -x)<3 && fabs(polygons->m[1][i+2] - y)<3 && fabs(polygons->m[2][i+2] - z)<30)**/){
      s = normal(polygons,i);
      n[0] += s[0];
      n[1] += s[1];
      n[2] += s[2];
      p++;
    }
  }
  if (p!=0){
    n[0] = n[0]/p;
    n[1] = n[1]/p;
    n[2] = n[2]/p;
  }
  
  //free(s);
  return n;
}

color ambient(color c, double kr, double kg, double kb){
  c.red = c.red * kr;
  c.green = c.green * kg;
  c.blue *= kb;
  return c;
}
color diffuse( double *n, color c, double *l, double kr, double kg, double kb){
  double d = dot(n,normalize(l));
  d = (d > 0) ? d : 0;
  c.red *= d*kr;
  c.green *= d*kg;
  c.blue *= d*kb;
  return c;
}

color specular( double *n, color c, double *l, double kr, double kg, double kb){
  double *ll,v[3], d;
  v[0] = 0;
  v[1] = 0;
  v[2] = -1;
  n = normalize(n);
  ll = normalize(l);
  d = dot(ll,n)*2;
  n[0] *= d;
  n[1] *= d;
  n[2] *= d;
  n[0] -= ll[0];
  n[1] -= ll[1];
  n[2] -= ll[2];
  d = pow(dot(n,v),3);
  d = (d > 0) ? d : 0;
  c.red *= d*kr;
  c.green *= d*kg;
  c.blue *= d*kb;
  free(n);
  free(ll);
  return c;
}

void color_polygons2( struct matrix *polygons, int i, screen s, color c, zbuffer b) {
  
  double x0,x1,x2,y0,y1,y2;
  double z0,z1,z2;
  double dx0, dx1, dx2, dz0,dz1,dz2;
  if ( polygons->m[1][i+2] >= polygons->m[1][i+1] ) {
    if ( polygons->m[1][i] >= polygons->m[1][i+2] ) {
      y0 = polygons->m[1][i+1];
      y1 = polygons->m[1][i+2];
      y2 = polygons->m[1][i];
      x0 = polygons->m[0][i+1];
      x1 = polygons->m[0][i+2];
      x2 = polygons->m[0][i];
      z0 = polygons->m[2][i+1];
      z1 = polygons->m[2][i+2];
      z2 = polygons->m[2][i];
    } else if ( polygons->m[1][i] >= polygons->m[1][i+1]) {
      y0 = polygons->m[1][i+1];
      y1 = polygons->m[1][i];
      y2 = polygons->m[1][i+2];
      x0 = polygons->m[0][i+1];
      x1 = polygons->m[0][i];
      x2 = polygons->m[0][i+2];
      z0 = polygons->m[2][i+1];
      z1 = polygons->m[2][i];
      z2 = polygons->m[2][i+2];
    } else {
      y0 = polygons->m[1][i];
      y1 = polygons->m[1][i+1];
      y2 = polygons->m[1][i+2];
      x0 = polygons->m[0][i];
      x1 = polygons->m[0][i+1];
      x2 = polygons->m[0][i+2];
      z0 = polygons->m[2][i+1];
      z1 = polygons->m[2][i+2];
      z2 = polygons->m[2][i];
    }
  } else {
    if ( polygons->m[1][i] >= polygons->m[1][i+1] ) {
      y0 = polygons->m[1][i+2];
      y1 = polygons->m[1][i+1];
      y2 = polygons->m[1][i];
      x0 = polygons->m[0][i+2];
      x1 = polygons->m[0][i+1];
      x2 = polygons->m[0][i];
      z0 = polygons->m[2][i+2];
      z1 = polygons->m[2][i+1];
      z2 = polygons->m[2][i];
    } else if ( polygons->m[1][i] >= polygons->m[1][i+2]) {
      y0 = polygons->m[1][i+2];
      y1 = polygons->m[1][i];
      y2 = polygons->m[1][i+1];
      x0 = polygons->m[0][i+2];
      x1 = polygons->m[0][i];
      x2 = polygons->m[0][i+1];
      z0 = polygons->m[2][i+2];
      z1 = polygons->m[2][i];
      z2 = polygons->m[2][i+1];
    } else {
      y0 = polygons->m[1][i];
      y1 = polygons->m[1][i+2];
      y2 = polygons->m[1][i+1];
      x0 = polygons->m[0][i];
      x1 = polygons->m[0][i+2];
      x2 = polygons->m[0][i+1];
      z0 = polygons->m[2][i];
      z1 = polygons->m[2][i+2];
      z2 = polygons->m[2][i+1];
    }
  }
  dx0 = (x2-x0)/(y2-y0);
  dx1 = (x1-x0)/(y1-y0);
  dx2 = (x2-x1)/(y2-y1);
  dz0 = (z2-z0)/(y2-y0);
  dz1 = (z1-z0)/(y1-y0);
  dz2 = (z2-z1)/(y2-y1);
  x2 = x0;
  z2 = z0;
  while (y0 < y1) {
    draw_line2((int)x0, (int)y0, z0, (int)x2, (int)y0, z2, s, c, b);
    x0 = x0 + dx0;
    x2 = x2 + dx1;
    z0 += dz0;
    z2 += dz1;
    y0++;
  }
  //draw_line(x0, y0, x2, y0, s, c[(int)i%100]);
  //x0 += dx0;
  //y0++;
  x2 = x1;
  z2 = z1;
  while (y0 < y2) {
    draw_line2((int)x0,(int)y0,z0,(int)x2,(int)y0,z2,s,c,b);
    x0 = x0 + dx0;
    x2 = x2 + dx2;
    z0 += dz0;
    z2 += dz2;
    y0++;	
  }
}

void color_polygons3( struct matrix *polygons, int i, screen s, color c, double *l, double *kr, double *kg, double *kb, zbuffer b , int type) {
  
  double x0,x1,x2,y0,y1,y2,xb,yb,zb,xf,yf,zf;
  double z0,z1,z2;
  double dx0, dx1, dx2, dz0,dz1,dz2;
  color c0,c1;
  //printf("values: %d %d %d\n",c.red,c.green,c.blue);
  
  if ( polygons->m[1][i+2] >= polygons->m[1][i+1] ) {
    if ( polygons->m[1][i] >= polygons->m[1][i+2] ) {
      y0 = polygons->m[1][i+1];
      y1 = polygons->m[1][i+2];
      y2 = polygons->m[1][i];
      x0 = polygons->m[0][i+1];
      x1 = polygons->m[0][i+2];
      x2 = polygons->m[0][i];
      z0 = polygons->m[2][i+1];
      z1 = polygons->m[2][i+2];
      z2 = polygons->m[2][i];
    } else if ( polygons->m[1][i] >= polygons->m[1][i+1]) {
      y0 = polygons->m[1][i+1];
      y1 = polygons->m[1][i];
      y2 = polygons->m[1][i+2];
      x0 = polygons->m[0][i+1];
      x1 = polygons->m[0][i];
      x2 = polygons->m[0][i+2];
      z0 = polygons->m[2][i+1];
      z1 = polygons->m[2][i];
      z2 = polygons->m[2][i+2];
    } else {
      y0 = polygons->m[1][i];
      y1 = polygons->m[1][i+1];
      y2 = polygons->m[1][i+2];
      x0 = polygons->m[0][i];
      x1 = polygons->m[0][i+1];
      x2 = polygons->m[0][i+2];
      z0 = polygons->m[2][i+1];
      z1 = polygons->m[2][i+2];
      z2 = polygons->m[2][i];
    }
  } else {
    if ( polygons->m[1][i] >= polygons->m[1][i+1] ) {
      y0 = polygons->m[1][i+2];
      y1 = polygons->m[1][i+1];
      y2 = polygons->m[1][i];
      x0 = polygons->m[0][i+2];
      x1 = polygons->m[0][i+1];
      x2 = polygons->m[0][i];
      z0 = polygons->m[2][i+2];
      z1 = polygons->m[2][i+1];
      z2 = polygons->m[2][i];
    } else if ( polygons->m[1][i] >= polygons->m[1][i+2]) {
      y0 = polygons->m[1][i+2];
      y1 = polygons->m[1][i];
      y2 = polygons->m[1][i+1];
      x0 = polygons->m[0][i+2];
      x1 = polygons->m[0][i];
      x2 = polygons->m[0][i+1];
      z0 = polygons->m[2][i+2];
      z1 = polygons->m[2][i];
      z2 = polygons->m[2][i+1];
    } else {
      y0 = polygons->m[1][i];
      y1 = polygons->m[1][i+2];
      y2 = polygons->m[1][i+1];
      x0 = polygons->m[0][i];
      x1 = polygons->m[0][i+2];
      x2 = polygons->m[0][i+1];
      z0 = polygons->m[2][i];
      z1 = polygons->m[2][i+2];
      z2 = polygons->m[2][i+1];
    }
  }
  dx0 = (x2-x0)/(y2-y0);
  dx1 = (x1-x0)/(y1-y0);
  dx2 = (x2-x1)/(y2-y1);
  dz0 = (z2-z0)/(y2-y0);
  dz1 = (z1-z0)/(y1-y0);
  dz2 = (z2-z1)/(y2-y1);
  xb = x0;
  yb = y0;
  zb = z0;
  xf=x0;
  yf=y0;
  zf=z0;
  while (yb < y1) {
    if (type==0){
      c0 = flat(polygons,i,c,l,kr,kg,kb);
      draw_line2((int)xb, (int)yb, zb, (int)xf, (int)yb, zf, s, c0, b);
    } else if (type==1){
      c0 = gouraud(polygons,c,l,kr,kg,kb,x0,y0,z0,x1,y1,z1,x2,y2,z2,xb,yb);
      c1 = gouraud(polygons,c,l,kr,kg,kb,x0,y0,z0,x1,y1,z1,x2,y2,z2,xf,yb);
      draw_line3(xb, (int)yb, zb, xf, (int)yb, zf, s, c0,c1, b);
    } else if(type==2){
      draw_line4(polygons,c,l,kr,kg,kb,x0,y0,z0,x1,y1,z1,x2,y2,z2,xb, (int)yb, zb, xf, (int)yb, zf, s, b);
    }
    xb = xb + dx0;
    xf = xf + dx1;
    zb += dz0;
    zf += dz1;
    yb++;
  }
  //draw_line(x0, y0, x2, y0, s, c[(int)i%100]);
  //x0 += dx0;
  //y0++;
  xf = x1;
  zf = z1;
  while (yb < y2) {
    if (type==0){
      c0 = flat(polygons,i,c,l,kr,kg,kb);
      draw_line2((int)xb, (int)yb, zb, (int)xf, (int)yb, zf, s, c0, b);
    } else if (type==1){
      c0 = gouraud(polygons,c,l,kr,kg,kb,x0,y0,z0,x1,y1,z1,x2,y2,z2,xb,yb);
      c1 = gouraud(polygons,c,l,kr,kg,kb,x0,y0,z0,x1,y1,z1,x2,y2,z2,xf,yb);
      draw_line3(xb, (int)yb, zb, xf, (int)yb, zf, s, c0,c1, b);
    } else if(type==2){
      draw_line4(polygons,c,l,kr,kg,kb,x0,y0,z0,x1,y1,z1,x2,y2,z2,xb, (int)yb, zb, xf, (int)yb, zf, s, b);
    }
    xb = xb + dx0;
    xf = xf + dx2;
    zb += dz0;
    zf += dz2;
    yb++;	
  }
}

void color_polygons( struct matrix *polygons, int i, screen s, color c) {
  
  double x0,x1,x2,y0,y1,y2;
  double dx0, dx1, dx2, dz, dz1, dz2;
  if ( polygons->m[1][i+2] >= polygons->m[1][i+1] ) {
    if ( polygons->m[1][i] >= polygons->m[1][i+2] ) {
      y0 = polygons->m[1][i+1];
      y1 = polygons->m[1][i+2];
      y2 = polygons->m[1][i];
      x0 = polygons->m[0][i+1];
      x1 = polygons->m[0][i+2];
      x2 = polygons->m[0][i];
    } else if ( polygons->m[1][i] >= polygons->m[1][i+1]) {
      y0 = polygons->m[1][i+1];
      y1 = polygons->m[1][i];
      y2 = polygons->m[1][i+2];
      x0 = polygons->m[0][i+1];
      x1 = polygons->m[0][i];
      x2 = polygons->m[0][i+2];
    } else {
      y0 = polygons->m[1][i];
      y1 = polygons->m[1][i+1];
      y2 = polygons->m[1][i+2];
      x0 = polygons->m[0][i];
      x1 = polygons->m[0][i+1];
      x2 = polygons->m[0][i+2];
    }
  } else {
    if ( polygons->m[1][i] >= polygons->m[1][i+1] ) {
      y0 = polygons->m[1][i+2];
      y1 = polygons->m[1][i+1];
      y2 = polygons->m[1][i];
      x0 = polygons->m[0][i+2];
      x1 = polygons->m[0][i+1];
      x2 = polygons->m[0][i];
    } else if ( polygons->m[1][i] >= polygons->m[1][i+2]) {
      y0 = polygons->m[1][i+2];
      y1 = polygons->m[1][i];
      y2 = polygons->m[1][i+1];
      x0 = polygons->m[0][i+2];
      x1 = polygons->m[0][i];
      x2 = polygons->m[0][i+1];
    } else {
      y0 = polygons->m[1][i];
      y1 = polygons->m[1][i+2];
      y2 = polygons->m[1][i+1];
      x0 = polygons->m[0][i];
      x1 = polygons->m[0][i+2];
      x2 = polygons->m[0][i+1];
    }
  }
      
  dx0 = (x2-x0)/(y2-y0);
  dx1 = (x1-x0)/(y1-y0);
  dx2 = (x2-x1)/(y2-y1);
  x2 = x0;
  while (y0 < y1) {
    draw_line(x0, y0, x2, y0, s, c);
    x0 = x0 + dx0;
    x2 = x2 + dx1;
    y0++;
  }
  //draw_line(x0, y0, x2, y0, s, c[(int)i%100]);
  //x0 += dx0;
  //y0++;
  x2 = x1;
  while (y0 < y2) {
    draw_line(x0,y0,x2,y0,s,c);
    x0 = x0 + dx0;
    x2 = x2 + dx2;
    y0++;	
  }
    
  
}

/*======== void add_polygon() ==========
Inputs:   struct matrix *surfaces
         double x0
         double y0
         double z0
         double x1
         double y1
         double z1
         double x2
         double y2
         double z2  
Returns: 
Adds the vertices (x0, y0, z0), (x1, y1, z1)
and (x2, y2, z2) to the polygon matrix. They
define a single triangle surface.

04/16/13 13:05:59
jdyrlandweaver
====================*/
void add_polygon( struct matrix *polygons, 
		  double x0, double y0, double z0, 
		  double x1, double y1, double z1, 
		  double x2, double y2, double z2 ) {
  add_point(polygons, x0, y0, z0);
  add_point(polygons, x1, y1, z1);
  add_point(polygons, x2, y2, z2);
}

/*======== void draw_polygons() ==========
Inputs:   struct matrix *polygons
          screen s
          color c  
Returns: 
Goes through polygons 3 points at a time, drawing 
lines connecting each points to create bounding
triangles

04/16/13 13:13:27
jdyrlandweaver
====================*/
void draw_polygons( struct matrix *polygons, screen s, color c ) {
  
  int i;  
  for( i=0; i < polygons->lastcol-2; i+=3 ) {

    if ( calculate_dot( polygons, i ) < 0 ) {
      draw_line( polygons->m[0][i],
		 polygons->m[1][i],
		 polygons->m[0][i+1],
		 polygons->m[1][i+1],
		 s, c);
      draw_line( polygons->m[0][i+1],
		 polygons->m[1][i+1],
		 polygons->m[0][i+2],
		 polygons->m[1][i+2],
		 s, c);
      draw_line( polygons->m[0][i+2],
		 polygons->m[1][i+2],
		 polygons->m[0][i],
		 polygons->m[1][i],
		 s, c);
      color_polygons(polygons,i,s,c);
    }
  }
}

void draw_polygons2( struct matrix *polygons, screen s, color c, zbuffer b) {
  
  int i;  
  for( i=0; i < polygons->lastcol-2; i+=3 ) {

    if ( calculate_dot( polygons, i ) < 0 ) {
      draw_line2( polygons->m[0][i],
		 polygons->m[1][i],
		 polygons->m[2][i],
		 polygons->m[0][i+1],
		 polygons->m[1][i+1],
		 polygons->m[2][i+1],
		  s, c,b);
      draw_line2( polygons->m[0][i+1],
		 polygons->m[1][i+1],
		 polygons->m[2][i+1],
		 polygons->m[0][i+2],
		 polygons->m[1][i+2],
		 polygons->m[2][i+2],
		  s, c,b);
      draw_line2( polygons->m[0][i+2],
		 polygons->m[1][i+2],
		 polygons->m[2][i+2],
		 polygons->m[0][i],
		 polygons->m[1][i],
		 polygons->m[2][i],
		  s, c,b);
    }
    color_polygons2(polygons,i,s,c,b);
  }
}

void draw_polygons3( struct matrix *polygons, screen s, color c, double *l, double *kr, double *kg, double *kb, zbuffer b, int t) {
  
  int i;  
  for( i=0; i < polygons->lastcol-2; i+=3 ) {

    /**if ( calculate_dot( polygons, i ) < 0 ) {
      draw_line2( polygons->m[0][i],
		 polygons->m[1][i],
		 polygons->m[2][i],
		 polygons->m[0][i+1],
		 polygons->m[1][i+1],
		 polygons->m[2][i+1],
		 s, c, b);
      draw_line2( polygons->m[0][i+1],
		 polygons->m[1][i+1],
		 polygons->m[2][i+1],
		 polygons->m[0][i+2],
		 polygons->m[1][i+2],
		 polygons->m[2][i+2],
		 s, c, b);
      draw_line2( polygons->m[0][i+2],
		 polygons->m[1][i+2],
		 polygons->m[2][i+2],
		 polygons->m[0][i],
		 polygons->m[1][i],
		 polygons->m[2][i],
		 s, c, b);
      
		 }**/
    color_polygons3(polygons,i,s,c,l,kr,kg,kb,b,t);
  }
}


/*======== void add_sphere() ==========
  Inputs:   struct matrix * points
            double cx
	    double cy
	    double r
	    double step  
  Returns: 

  adds all the points for a sphere with center 
  (cx, cy) and radius r.

  should call generate_sphere to create the
  necessary points

  jdyrlandweaver
  ====================*/
void add_sphere( struct matrix * points, 
		 double cx, double cy, double cz, double r, 
		 int step ) {

  struct matrix * temp;
  int lat, longt;
  int index;
  int num_steps, num_points;
  double px0, px1, px2, px3;
  double py0, py1, py2, py3;
  double pz0, pz1, pz2, pz3;

  num_steps = MAX_STEPS / step;
  num_points = num_steps * (num_steps + 1);
  
  temp = new_matrix( 4, num_points);
  //generate the points on the sphere
  generate_sphere( temp, cx, cy, cz, r, step );

  int latStop, longStop, latStart, longStart;
  latStart = 0;
  latStop = num_steps;
  longStart = 0;
  longStop = num_steps;

  num_steps++;

  for ( lat = latStart; lat < latStop; lat++ ) {
    for ( longt = longStart; longt < longStop; longt++ ) {
      
      index = lat * num_steps + longt;

      px0 = temp->m[0][ index ];
      py0 = temp->m[1][ index ];
      pz0 = temp->m[2][ index ];
      
      px1 = temp->m[0][ (index + num_steps) % num_points ];
      py1 = temp->m[1][ (index + num_steps) % num_points ];
      pz1 = temp->m[2][ (index + num_steps) % num_points ];

      px3 = temp->m[0][ index + 1 ];
      py3 = temp->m[1][ index + 1 ];
      pz3 = temp->m[2][ index + 1 ];

      if (longt != longStop - 1) {
	px2 = temp->m[0][ (index + num_steps + 1) % num_points ];
	py2 = temp->m[1][ (index + num_steps + 1) % num_points ];
	pz2 = temp->m[2][ (index + num_steps + 1) % num_points ];
      }
      else {
	px2 = temp->m[0][ (index + 1) % num_points ];
	py2 = temp->m[1][ (index + 1) % num_points ];
	pz2 = temp->m[2][ (index + 1) % num_points ];
      }

      if (longt != 0)
	add_polygon( points, px0, py0, pz0, px1, py1, pz1, px2, py2, pz2 );
      if (longt != longStop - 1)
	add_polygon( points, px2, py2, pz2, px3, py3, pz3, px0, py0, pz0 );
    }
  }
}

/*======== void generate_sphere() ==========
  Inputs:   struct matrix * points
            double cx
	    double cy
	    double r
	    double step  
  Returns: 

  Generates all the points along the surface of a 
  sphere with center (cx, cy) and radius r

  Adds these points to the matrix parameter

  03/22/12 11:30:26
  jdyrlandweaver
  ====================*/
void generate_sphere( struct matrix * points, 
		      double cx, double cy, double cz, double r, 
		      int step ) {


  int circle, rotation;
  double x, y, z, circ, rot;

  int rotStart = step * 0;
  int rotStop = MAX_STEPS;
  int circStart = step * 0;
  int circStop = MAX_STEPS;
  
  for ( rotation = rotStart; rotation < rotStop; rotation += step ) {
    rot = (double)rotation / MAX_STEPS;
    for ( circle = circStart; circle <= circStop; circle+= step ) {

      circ = (double)circle / MAX_STEPS;
      x = r * cos( M_PI * circ ) + cx;
      y = r * sin( M_PI * circ ) *
	cos( 2 * M_PI * rot ) + cy;
      z = r * sin( M_PI * circ ) *
	sin( 2 * M_PI * rot ) + cz;

      add_point( points, x, y, z);
    }
  }
}    


/*======== void add_torus() ==========
  Inputs:   struct matrix * points
            double cx
	    double cy
	    double r1
	    double r2
	    double step  
  Returns: 

  adds all the points required to make a torus
  with center (cx, cy) and radii r1 and r2.

  should call generate_torus to create the
  necessary points

  03/22/12 13:34:03
  jdyrlandweaver
  ====================*/
void add_torus( struct matrix * points, 
		double cx, double cy, double cz, double r1, double r2, 
		int step ) {

  struct matrix * temp;
  int lat, longt;
  int index;
  int num_steps;
  
  num_steps = MAX_STEPS / step;

  temp = new_matrix( 4, num_steps * num_steps );
  //generate the points on the torus
  generate_torus( temp, cx, cy, cz, r1, r2, step );
  int num_points = temp->lastcol;

  int latStop, longtStop, latStart, longStart;
  latStart = 0;
  longStart = 0;
  latStop = num_steps;
  longtStop = num_steps;
  for ( lat = latStart; lat < latStop; lat++ )
    for ( longt = longStart; longt < longtStop; longt++ ) {

      index = lat * num_steps + longt;

      if ( longt != num_steps-1) {
	add_polygon( points, temp->m[0][index],
		     temp->m[1][index],
		     temp->m[2][index],
		     temp->m[0][(index+num_steps+1) % num_points],
		     temp->m[1][(index+num_steps+1) % num_points],
		     temp->m[2][(index+num_steps+1) % num_points],
		     temp->m[0][index+1],
		     temp->m[1][index+1],
		     temp->m[2][index+1] );
	add_polygon( points, temp->m[0][index],
		     temp->m[1][index],
		     temp->m[2][index],
		     temp->m[0][(index+num_steps) % num_points],
		     temp->m[1][(index+num_steps) % num_points],
		     temp->m[2][(index+num_steps) % num_points],
		     temp->m[0][(index+num_steps) % num_points + 1],
		     temp->m[1][(index+num_steps) % num_points + 1],
		     temp->m[2][(index+num_steps) % num_points + 1]);
      }
      else {
	add_polygon( points, temp->m[0][index],
		     temp->m[1][index],
		     temp->m[2][index],
		     temp->m[0][(index+1) % num_points],
		     temp->m[1][(index+1) % num_points],
		     temp->m[2][(index+1) % num_points],
		     temp->m[0][index+1-num_steps],
		     temp->m[1][index+1-num_steps],
		     temp->m[2][index+1-num_steps] );
	add_polygon( points, temp->m[0][index],
		     temp->m[1][index],
		     temp->m[2][index],
		     temp->m[0][(index+num_steps) % num_points],
		     temp->m[1][(index+num_steps) % num_points],
		     temp->m[2][(index+num_steps) % num_points],
		     temp->m[0][(index+1) % num_points],
		     temp->m[1][(index+1) % num_points],
		     temp->m[2][(index+1) % num_points]);
      }

    }
}

/*======== void generate_torus() ==========
  Inputs:   struct matrix * points
            double cx
	    double cy
	    double r
	    double step  
  Returns: 

  Generates all the points along the surface of a 
  tarus with center (cx, cy) and radii r1 and r2

  Adds these points to the matrix parameter

  03/22/12 11:30:26
  jdyrlandweaver
  ====================*/
void generate_torus( struct matrix * points, 
		     double cx, double cy, double cz, double r1, double r2, 
		     int step ) {

  double x, y, z, circ, rot;
  int circle, rotation;

  double rotStart = step * 0;
  double rotStop = MAX_STEPS;
  double circStart = step * 0;
  double circStop = MAX_STEPS;

  for ( rotation = rotStart; rotation < rotStop; rotation += step ) {

    rot = (double)rotation / MAX_STEPS;
    for ( circle = circStart; circle < circStop; circle+= step ) {

      circ = (double)circle / MAX_STEPS;
      x = cos( 2 * M_PI * rot ) *
	( r1 * cos( 2 * M_PI * circ ) + r2 ) + cx;
      y = r1 * sin( 2 * M_PI * circ ) + cy;
      z = sin( 2 * M_PI * rot ) *
	( r1 * cos( 2 * M_PI * circ ) + r2 ) + cz;

      add_point( points, x, y, z );
    }
  }
}

/*======== void add_box() ==========
  Inputs:   struct matrix * points
            double x
	    double y
	    double z
	    double width
	    double height
	    double depth
  Returns: 

  add the points for a rectagular prism whose 
  upper-left corner is (x, y, z) with width, 
  height and depth dimensions.

  jdyrlandweaver
  ====================*/
void add_box( struct matrix * polygons,
	      double x, double y, double z,
	      double width, double height, double depth ) {


  
  double x2, y2, z2;
  x2 = x + width;
  y2 = y - height;
  z2 = z - depth;
  printf("%lf %lf %lf\n",x2,y2,z2);
  //front
  add_polygon( polygons, 
	       x, y, z, 
	       x, y2, z,
	       x2, y2, z);
  add_polygon( polygons, 
	       x2, y2, z, 
	       x2, y, z,
	       x, y, z);
  //back
  add_polygon( polygons, 
	       x2, y, z2, 
	       x2, y2, z2,
	       x, y2, z2);
  add_polygon( polygons, 
	       x, y2, z2, 
	       x, y, z2,
	       x2, y, z2);
  //top
  add_polygon( polygons, 
	       x, y, z2, 
	       x, y, z,
	       x2, y, z);
  add_polygon( polygons, 
	       x2, y, z, 
	       x2, y, z2,
	       x, y, z2);
  //bottom
  add_polygon( polygons, 
	       x2, y2, z2, 
	       x2, y2, z,
	       x, y2, z);
  add_polygon( polygons, 
	       x, y2, z, 
	       x, y2, z2,
	       x2, y2, z2);
  //right side
  add_polygon( polygons, 
	       x2, y, z, 
	       x2, y2, z,
	       x2, y2, z2);
  add_polygon( polygons, 
	       x2, y2, z2, 
	       x2, y, z2,
	       x2, y, z);
  //left side
  add_polygon( polygons, 
	       x, y, z2, 
	       x, y2, z2,
	       x, y2, z);
  add_polygon( polygons, 
	       x, y2, z, 
	       x, y, z,
	       x, y, z2); 
}
  
/*======== void add_circle() ==========
  Inputs:   struct matrix * points
            double cx
	    double cy
	    double y
	    double step  
  Returns: 


  03/16/12 19:53:52
  jdyrlandweaver
  ====================*/
void add_circle( struct matrix * points, 
		 double cx, double cy, 
		 double r, double step ) {
  
  double x0, y0, x, y, t;
  
  x0 = cx + r;
  y0 = cy;

  for ( t = step; t <= 1; t+= step ) {
    
    x = r * cos( 2 * M_PI * t ) + cx;
    y = r * sin( 2 * M_PI * t ) + cy;
    
    add_edge( points, x0, y0, 0, x, y, 0 );
    x0 = x;
    y0 = y;
  }

  add_edge( points, x0, y0, 0, cx + r, cy, 0 );
}

/*======== void add_curve() ==========
Inputs:   struct matrix *points
         double x0
         double y0
         double x1
         double y1
         double x2
         double y2
         double x3
         double y3
         double step
         int type  
Returns: 

Adds the curve bounded by the 4 points passsed as parameters
of type specified in type (see matrix.h for curve type constants)
to the matrix points

03/16/12 15:24:25
jdyrlandweaver
====================*/
void add_curve( struct matrix *points, 
		double x0, double y0, 
		double x1, double y1, 
		double x2, double y2, 
		double x3, double y3, 
		double step, int type ) {

  double x, y, t;
  struct matrix * xcoefs;
  struct matrix * ycoefs;
  
  //generate the coeficients
  if ( type == BEZIER_MODE ) {
    ycoefs = generate_curve_coefs(y0, y1, y2, y3, BEZIER_MODE);
    xcoefs = generate_curve_coefs(x0, x1, x2, x3, BEZIER_MODE);
  }

  else {
    xcoefs = generate_curve_coefs(x0, x1, x2, x3, HERMITE_MODE);
    ycoefs = generate_curve_coefs(y0, y1, y2, y3, HERMITE_MODE);
  }

  /*
  printf("a = %lf b = %lf c = %lf d = %lf\n", xcoefs->m[0][0],
         xcoefs->m[1][0], xcoefs->m[2][0], xcoefs->m[3][0]);
  */

  for (t=step; t <= 1; t+= step) {
    
    x = xcoefs->m[0][0] * t * t * t + xcoefs->m[1][0] * t * t
      + xcoefs->m[2][0] * t + xcoefs->m[3][0];

    y = ycoefs->m[0][0] * t * t * t + ycoefs->m[1][0] * t * t
      + ycoefs->m[2][0] * t + ycoefs->m[3][0];

    add_edge(points, x0, y0, 0, x, y, 0);
    x0 = x;
    y0 = y;
  }

  free_matrix(xcoefs);
  free_matrix(ycoefs);
}

/*======== void add_point() ==========
Inputs:   struct matrix * points
         int x
         int y
         int z 
Returns: 
adds point (x, y, z) to points and increment points.lastcol
if points is full, should call grow on points
====================*/
void add_point( struct matrix * points, double x, double y, double z) {
  
  if ( points->lastcol == points->cols )
    grow_matrix( points, points->lastcol + 100 );

  points->m[0][points->lastcol] = x;
  points->m[1][points->lastcol] = y;
  points->m[2][points->lastcol] = z;
  points->m[3][points->lastcol] = 1;

  points->lastcol++;
}

/*======== void add_edge() ==========
Inputs:   struct matrix * points
          int x0, int y0, int z0, int x1, int y1, int z1
Returns: 
add the line connecting (x0, y0, z0) to (x1, y1, z1) to points
should use add_point
====================*/
void add_edge( struct matrix * points, 
	       double x0, double y0, double z0, 
	       double x1, double y1, double z1) {
  add_point( points, x0, y0, z0 );
  add_point( points, x1, y1, z1 );
}

/*======== void draw_lines() ==========
Inputs:   struct matrix * points
         screen s
         color c 
Returns: 
Go through points 2 at a time and call draw_line to add that line
to the screen
====================*/
void draw_lines( struct matrix * points, screen s, color c) {

  int i;
 
  if ( points->lastcol < 2 ) {
    
    printf("Need at least 2 points to draw a line!\n");
    return;
  }

  for ( i = 0; i < points->lastcol - 1; i+=2 ) {

    draw_line( points->m[0][i], points->m[1][i], 
	       points->m[0][i+1], points->m[1][i+1], s, c);
    //FOR DEMONSTRATION PURPOSES ONLY
    //draw extra pixels so points can actually be seen    
    /*
    draw_line( points->m[0][i]+1, points->m[1][i], 
	       points->m[0][i+1]+1, points->m[1][i+1], s, c);
    draw_line( points->m[0][i], points->m[1][i]+1, 
	       points->m[0][i+1], points->m[1][i+1]+1, s, c);
    draw_line( points->m[0][i]-1, points->m[1][i], 
	       points->m[0][i+1]-1, points->m[1][i+1], s, c);
    draw_line( points->m[0][i], points->m[1][i]-1, 
	       points->m[0][i+1], points->m[1][i+1]-1, s, c);
    draw_line( points->m[0][i]+1, points->m[1][i]+1, 
	       points->m[0][i+1]+1, points->m[1][i+1]+1, s, c);
    draw_line( points->m[0][i]-1, points->m[1][i]+1, 
	       points->m[0][i+1]-1, points->m[1][i+1]+1, s, c);
    draw_line( points->m[0][i]-1, points->m[1][i]-1, 
	       points->m[0][i+1]-1, points->m[1][i+1]-1, s, c);
    draw_line( points->m[0][i]+1, points->m[1][i]-1, 
	       points->m[0][i+1]+1, points->m[1][i+1]-1, s, c);
    */
  } 	       
}

void draw_lines2( struct matrix * points, screen s, color c, zbuffer b) {

  int i;
 
  if ( points->lastcol < 2 ) {
    
    printf("Need at least 2 points to draw a line!\n");
    return;
  }

  for ( i = 0; i < points->lastcol - 1; i+=2 ) {

    draw_line2( points->m[0][i], points->m[1][i], points->m[2][i],
	       points->m[0][i+1], points->m[1][i+1], points->m[2][i+2], s, c, b);
    //FOR DEMONSTRATION PURPOSES ONLY
    //draw extra pixels so points can actually be seen    
    /*
    draw_line( points->m[0][i]+1, points->m[1][i], 
	       points->m[0][i+1]+1, points->m[1][i+1], s, c);
    draw_line( points->m[0][i], points->m[1][i]+1, 
	       points->m[0][i+1], points->m[1][i+1]+1, s, c);
    draw_line( points->m[0][i]-1, points->m[1][i], 
	       points->m[0][i+1]-1, points->m[1][i+1], s, c);
    draw_line( points->m[0][i], points->m[1][i]-1, 
	       points->m[0][i+1], points->m[1][i+1]-1, s, c);
    draw_line( points->m[0][i]+1, points->m[1][i]+1, 
	       points->m[0][i+1]+1, points->m[1][i+1]+1, s, c);
    draw_line( points->m[0][i]-1, points->m[1][i]+1, 
	       points->m[0][i+1]-1, points->m[1][i+1]+1, s, c);
    draw_line( points->m[0][i]-1, points->m[1][i]-1, 
	       points->m[0][i+1]-1, points->m[1][i+1]-1, s, c);
    draw_line( points->m[0][i]+1, points->m[1][i]-1, 
	       points->m[0][i+1]+1, points->m[1][i+1]-1, s, c);
    */
  } 	       
}

void draw_line4(struct matrix *poly, color c, double *l,
		double *kr, double *kg, double *kb,
		double xa, double ya, double za, double xb,
		double yb, double zb, double xc, double yc,
		double zc,int x0, int y0, double z0, int x1, int y1, double z1, screen s, zbuffer b) {
 
  int x, y, d, dx, dy;
  double z, dz;
  double dr, dg, db;
  color c1;
  
  //if (y0 != y1) {
  //  printf("%d,%d",y0,y1);
  //  return;
  //}
  x = x0;
  y = y0;
  z = z0;
  
  //swap points so we're always draing left to right
  if ( x0 > x1 ) {
    x = x1;
    y = y1;
    z = z1;
    x1 = x0;
    y1 = y0;
    z1 = z0;
  }

  //need to know dx and dy for this version
  dx = (x1 - x) * 2;
  dy = (y1 - y) * 2;
  dz = z1 - z;
  //printf("a\n");
  //positive slope: Octants 1, 2 (5 and 6)
  if ( dy > 0 ) {
    //printf("b\n");
    //slope < 1: Octant 1 (5)
    if ( dx > dy ) {
      d = dy - ( dx / 2 );
      dz = dz / (x1 - x);
      //printf("c\n");
      while ( x <= x1 ) {
	c1 = phong(poly,c,l,kr,kg,kb,xa,ya,za,xb,yb,zb,xc,yc,zc,x,y);
	plot2(s, c1, x, y,z,b);

	if ( d < 0 ) {
	  x = x + 1;
	  z = z + dz;
	  d = d + dy;
	}
	else {
	  x = x + 1;
	  y = y + 1;
	  z = z + dz;
	  d = d + dy - dx;
	}
      }
    }

    //slope > 1: Octant 2 (6)
    else {
      d = ( dy / 2 ) - dx;
      dz = dz / (y1 - y);
      while ( y <= y1 ) {
	c1 = phong(poly,c,l,kr,kg,kb,xa,ya,za,xb,yb,zb,xc,yc,zc,x,y);
	plot2(s, c1, x, y,z,b);
	if ( d > 0 ) {
	  y = y + 1;
	  z = z + dz;
	  d = d - dx;
	}
	else {
	  y = y + 1;
	  x = x + 1;
	  z = z + dz;
	  d = d + dy - dx;
	}
      }
    }
  }

  //negative slope: Octants 7, 8 (3 and 4)
  else { 

    //slope > -1: Octant 8 (4)
    if ( dx > abs(dy) ) {

      d = dy + ( dx / 2 );
      dz = dz / (x1 - x);
      while ( x <= x1 ) {
	c1 = phong(poly,c,l,kr,kg,kb,xa,ya,za,xb,yb,zb,xc,yc,zc,x,y);
	plot2(s, c1, x, y,z,b);

	if ( d > 0 ) {
	  x = x + 1;
	  z = z + dz;
	  d = d + dy;
	}
	else {
	  x = x + 1;
	  y = y - 1;
	  z = z + dz;
	  d = d + dy + dx;
	}
      }
    }

    //slope < -1: Octant 7 (3)
    else {

      d =  (dy / 2) + dx;
      dz = dz / (y1 - y);
      while ( y >= y1 ) {
	c1 = phong(poly,c,l,kr,kg,kb,xa,ya,za,xb,yb,zb,xc,yc,zc,x,y);
	plot2(s, c1, x, y,z,b);
	if ( d < 0 ) {
	  y = y - 1;
	  z = z - dz;
	  d = d + dx;
	}
	else {
	  y = y - 1;
	  x = x + 1;
	  z = z - dz;
	  d = d + dy + dx;
	}
      }
    }
  }
}

void draw_line3(int x0, int y0, double z0, int x1, int y1, double z1, screen s, color c, color c2, zbuffer b) {
 
  int x, y, d, dx, dy;
  double z, dz;
  double dr, dg, db;

  //if (y0 != y1) {
  //  printf("%d,%d",y0,y1);
  //  return;
  //}
  x = x0;
  y = y0;
  z = z0;
  
  //swap points so we're always draing left to right
  if ( x0 > x1 ) {
    color t;
    x = x1;
    y = y1;
    z = z1;
    x1 = x0;
    y1 = y0;
    z1 = z0;
    t = c;
    c = c2;
    c2 = t;
  }

  //need to know dx and dy for this version
  dx = (x1 - x) * 2;
  dy = (y1 - y) * 2;
  dz = z1 - z;
  dr = c2.red-c.red;
  dg = c2.green-c.green;
  db = c2.blue-c.blue;
  //printf("a\n");
  //positive slope: Octants 1, 2 (5 and 6)
  if ( dy > 0 ) {
    //printf("b\n");
    //slope < 1: Octant 1 (5)
    if ( dx > dy ) {
      d = dy - ( dx / 2 );
      dz = dz / (x1 - x);
      dr = dr / (x1 - x);
      dg = dg / (x1 - x);
      db = db / (x1 - x);
      //printf("c\n");
      while ( x <= x1 ) {
	plot2(s, c, x, y,z,b);

	if ( d < 0 ) {
	  x = x + 1;
	  z = z + dz;
	  c.red += dr;
	  c.green += dg;
	  c.blue += db;
	  d = d + dy;
	}
	else {
	  x = x + 1;
	  y = y + 1;
	  z = z + dz;
	  c.red += dr;
	  c.green += dg;
	  c.blue += db;
	  d = d + dy - dx;
	}
      }
    }

    //slope > 1: Octant 2 (6)
    else {
      d = ( dy / 2 ) - dx;
      dz = dz / (y1 - y);
      dr = dr / (y1 - y);
      dg = dg / (y1 - y);
      db = db / (y1 - y);
      while ( y <= y1 ) {
	plot2(s, c, x, y,z,b);
	if ( d > 0 ) {
	  y = y + 1;
	  z = z + dz;
	  c.red += dr;
	  c.green += dg;
	  c.blue += db;
	  d = d - dx;
	}
	else {
	  y = y + 1;
	  x = x + 1;
	  z = z + dz;
	  c.red += dr;
	  c.green += dg;
	  c.blue += db;
	  d = d + dy - dx;
	}
      }
    }
  }

  //negative slope: Octants 7, 8 (3 and 4)
  else { 

    //slope > -1: Octant 8 (4)
    if ( dx > abs(dy) ) {

      d = dy + ( dx / 2 );
      dz = dz / (x1 - x);
      dr = dr / (x1 - x);
      dg = dg / (x1 - x);
      db = db / (x1 - x);
      while ( x <= x1 ) {

	plot2(s, c, x, y,z,b);

	if ( d > 0 ) {
	  x = x + 1;
	  z = z + dz;
	  c.red += dr;
	  c.green += dg;
	  c.blue += db;
	  d = d + dy;
	}
	else {
	  x = x + 1;
	  y = y - 1;
	  z = z + dz;
	  c.red += dr;
	  c.green += dg;
	  c.blue += db;
	  d = d + dy + dx;
	}
      }
    }

    //slope < -1: Octant 7 (3)
    else {

      d =  (dy / 2) + dx;
      dz = dz / (y1 - y);
      dr = dr / (y1 - y);
      dg = dg / (y1 - y);
      db = db / (y1 - y);
      while ( y >= y1 ) {
	
	plot2(s, c, x, y,z,b);
	if ( d < 0 ) {
	  y = y - 1;
	  z = z - dz;
	  c.red -= dr;
	  c.green -= dg;
	  c.blue -= db;
	  d = d + dx;
	}
	else {
	  y = y - 1;
	  x = x + 1;
	  z = z - dz;
	  c.red -= dr;
	  c.green -= dg;
	  c.blue -= db;
	  d = d + dy + dx;
	}
      }
    }
  }
}

void draw_line2(int x0, int y0, double z0, int x1, int y1, double z1, screen s, color c, zbuffer b) {
 
  int x, y, d, dx, dy;
  double z, dz;

  //if (y0 != y1) {
  //  printf("%d,%d",y0,y1);
  //  return;
  //}
  x = x0;
  y = y0;
  z = z0;
  
  //swap points so we're always draing left to right
  if ( x0 > x1 ) {
    x = x1;
    y = y1;
    z = z1;
    x1 = x0;
    y1 = y0;
    z1 = z0;
  }

  //need to know dx and dy for this version
  dx = (x1 - x) * 2;
  dy = (y1 - y) * 2;
  dz = z1 - z;
  //printf("a\n");
  //positive slope: Octants 1, 2 (5 and 6)
  if ( dy > 0 ) {
    //printf("b\n");
    //slope < 1: Octant 1 (5)
    if ( dx > dy ) {
      d = dy - ( dx / 2 );
      dz = dz / (x1 - x);
      //printf("c\n");
      while ( x <= x1 ) {
	plot2(s, c, x, y,z,b);

	if ( d < 0 ) {
	  x = x + 1;
	  z = z + dz;
	  d = d + dy;
	}
	else {
	  x = x + 1;
	  y = y + 1;
	  z = z + dz;
	  d = d + dy - dx;
	}
      }
    }

    //slope > 1: Octant 2 (6)
    else {
      d = ( dy / 2 ) - dx;
      dz = dz / (y1 - y);
      
      while ( y <= y1 ) {
	plot2(s, c, x, y,z,b);
	if ( d > 0 ) {
	  y = y + 1;
	  z = z + dz;
	  d = d - dx;
	}
	else {
	  y = y + 1;
	  x = x + 1;
	  z = z + dz;
	  d = d + dy - dx;
	}
      }
    }
  }

  //negative slope: Octants 7, 8 (3 and 4)
  else { 

    //slope > -1: Octant 8 (4)
    if ( dx > abs(dy) ) {

      d = dy + ( dx / 2 );
      dz = dz / (x1 - x);
  
      while ( x <= x1 ) {

	plot2(s, c, x, y,z,b);

	if ( d > 0 ) {
	  x = x + 1;
	  z = z + dz;
	  d = d + dy;
	}
	else {
	  x = x + 1;
	  y = y - 1;
	  z = z + dz;
	  d = d + dy + dx;
	}
      }
    }

    //slope < -1: Octant 7 (3)
    else {

      d =  (dy / 2) + dx;
      dz = dz / (y1 - y);

      while ( y >= y1 ) {
	
	plot2(s, c, x, y,z,b);
	if ( d < 0 ) {
	  y = y - 1;
	  z = z - dz;
	  d = d + dx;
	}
	else {
	  y = y - 1;
	  x = x + 1;
	  z = z - dz;
	  d = d + dy + dx;
	}
      }
    }
  }
}

void draw_line(int x0, int y0, int x1, int y1, screen s, color c) {
 
  int x, y, d, dx, dy;
  
  x = x0;
  y = y0;
  
  //swap points so we're always draing left to right
  if ( x0 > x1 ) {
    x = x1;
    y = y1;
    x1 = x0;
    y1 = y0;
  }

  //need to know dx and dy for this version
  dx = (x1 - x) * 2;
  dy = (y1 - y) * 2;

  //positive slope: Octants 1, 2 (5 and 6)
  if ( dy > 0 ) {

    //slope < 1: Octant 1 (5)
    if ( dx > dy ) {
      d = dy - ( dx / 2 );
      
      while ( x <= x1 ) {
	plot(s, c, x, y);

	if ( d < 0 ) {
	  x = x + 1;
	  d = d + dy;
	}
	else {
	  x = x + 1;
	  y = y + 1;
	  d = d + dy - dx;
	}
      }
    }

    //slope > 1: Octant 2 (6)
    else {
      d = ( dy / 2 ) - dx;
      while ( y <= y1 ) {

	plot(s, c, x, y );
	if ( d > 0 ) {
	  y = y + 1;
	  d = d - dx;
	}
	else {
	  y = y + 1;
	  x = x + 1;
	  d = d + dy - dx;
	}
      }
    }
  }

  //negative slope: Octants 7, 8 (3 and 4)
  else { 

    //slope > -1: Octant 8 (4)
    if ( dx > abs(dy) ) {

      d = dy + ( dx / 2 );
  
      while ( x <= x1 ) {

	plot(s, c, x, y);

	if ( d > 0 ) {
	  x = x + 1;
	  d = d + dy;
	}
	else {
	  x = x + 1;
	  y = y - 1;
	  d = d + dy + dx;
	}
      }
    }

    //slope < -1: Octant 7 (3)
    else {

      d =  (dy / 2) + dx;

      while ( y >= y1 ) {
	
	plot(s, c, x, y );
	if ( d < 0 ) {
	  y = y - 1;
	  d = d + dx;
	}
	else {
	  y = y - 1;
	  x = x + 1;
	  d = d + dy + dx;
	}
      }
    }
  }
}
