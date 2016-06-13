/*========== my_main.c ==========

  This is the only file you need to modify in order
  to get a working mdl project (for now).

  my_main.c will serve as the interpreter for mdl.
  When an mdl script goes through a lexer and parser, 
  the resulting operations will be in the array op[].

  Your job is to go through each entry in op and perform
  the required action from the list below:

  frames: set num_frames (in misc_headers.h) for animation

  basename: set name (in misc_headers.h) for animation

  vary: manipluate knob values between two given frames
        over a specified interval

  set: set a knob to a given value
  
  setknobs: set all knobs to a given value

  push: push a new origin matrix onto the origin stack
  
  pop: remove the top matrix on the origin stack

  move/scale/rotate: create a transformation matrix 
                     based on the provided values, then 
		     multiply the current top of the
		     origins stack by it.

  box/sphere/torus: create a solid object based on the
                    provided values. Store that in a 
		    temporary matrix, multiply it by the
		    current top of the origins stack, then
		    call draw_polygons.

  line: create a line based on the provided values. Store 
        that in a temporary matrix, multiply it by the
	current top of the origins stack, then call draw_lines.

  save: call save_extension with the provided filename

  display: view the image live
  
  jdyrlandweaver
  =========================*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "parser.h"
#include "symtab.h"
#include "y.tab.h"

#include "misc_headers.h"
#include "matrix.h"
#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "stack.h"

/*======== void first_pass()) ==========
  Inputs:   
  Returns: 

  Checks the op array for any animation commands
  (frames, basename, vary)
  
  Should set num_frames and basename if the frames 
  or basename commands are present

  If vary is found, but frames is not, the entire
  program should exit.

  If frames is found, but basename is not, set name
  to some default value, and print out a message
  with the name being used.

  jdyrlandweaver
  ====================*/
void first_pass() {
  num_frames = 1;
  int v = 0;
  int i;
  strncpy(name,"",sizeof(name));
  for (i=0;i<lastop;i++) {
      switch (op[i].opcode) {
      case FRAMES:
	num_frames = op[i].op.frames.num_frames;
	break;
      case BASENAME:
	strcpy(name,op[i].op.basename.p->name);
	break;
      case VARY:
	v++;
	break;
      }
  }
  if (strcmp(name,"")==0){
    strcpy(name,"default");
  }
  if (v>0 && num_frames == 1){
    num_frames = -1;
  }
}

/*======== struct vary_node ** second_pass()) ==========
  Inputs:   
  Returns: An array of vary_node linked lists

  In order to set the knobs for animation, we need to keep
  a seaprate value for each knob for each frame. We can do
  this by using an array of linked lists. Each array index
  will correspond to a frame (eg. knobs[0] would be the first
  frame, knobs[2] would be the 3rd frame and so on).

  Each index should contain a linked list of vary_nodes, each
  node contains a knob name, a value, and a pointer to the
  next node.

  Go through the opcode array, and when you find vary, go 
  from knobs[0] to knobs[frames-1] and add (or modify) the
  vary_node corresponding to the given knob with the
  appropirate value. 

  05/17/12 09:29:31
  jdyrlandweaver
  ====================*/
struct vary_node ** second_pass() {
  struct vary_node ** knobs = (struct vary_node **) malloc(num_frames * sizeof(struct vary_node *));
  int i,i2;
  int sf, ef;
  double inc, val,sv,ev;
  char* n;
  for(i=0;i<num_frames;i++){
    knobs[i] = NULL;
  }
  
  for (i=0;i<lastop;i++) {
      switch (op[i].opcode) {
      case VARY:
	sf = op[i].op.vary.start_frame;
	ef = op[i].op.vary.end_frame;
	sv = op[i].op.vary.start_val;
	ev = op[i].op.vary.end_val;
	val = (double) sv;
	inc = (double)(ev-sv)/(double)(ef-sf);
	for(i2=0;i2<num_frames;i2++){
	  if (i2 > sf && i2 <= ef)
	    val += inc;
	  //printf("%d %s %lf",i2,p.name,p.value);
	  if (knobs[i2]==NULL){
	    knobs[i2] = (struct vary_node *)malloc(sizeof(struct vary_node));
	    knobs[i2]->value = val;
	    strcpy(knobs[i2]->name,op[i].op.vary.p->name);
	    knobs[i2]->next = NULL;
	    //printf("done.\n");
	  } else {
	    struct vary_node * cur = knobs[i2];
	    while(cur->next != NULL){
	      //printf("one.\n");
	      cur = cur->next;
	    }
	    cur->next = (struct vary_node *)malloc(sizeof(struct vary_node));
	    cur = cur->next;
	    cur->value = val;
	    strcpy(cur->name,op[i].op.vary.p->name);
	    cur->next = NULL;
	    //printf("alsodone.\n");
	  }
	  //printf("%d %s %lf\n",i2,knobs[i2]->name,knobs[i2]->value);
	}
      }
  }
  return knobs;
}

/*======== void print_knobs() ==========
Inputs:   
Returns: 

Goes through symtab and display all the knobs and their
currnt values

jdyrlandweaver
====================*/
void print_knobs() {
  
  int i;

  printf( "ID\tNAME\t\tTYPE\t\tVALUE\n" );
  for ( i=0; i < lastsym; i++ ) {

    if ( symtab[i].type == SYM_VALUE ) {
      printf( "%d\t%s\t\t", i, symtab[i].name );

      printf( "SYM_VALUE\t");
      printf( "%6.2f\n", symtab[i].s.value);
    }
  }
}


/*======== void my_main() ==========
  Inputs:
  Returns: 

  This is the main engine of the interpreter, it should
  handle most of the commadns in mdl.

  If frames is not present in the source (and therefore 
  num_frames is 1, then process_knobs should be called.

  If frames is present, the enitre op array must be
  applied frames time. At the end of each frame iteration
  save the current screen to a file named the
  provided basename plus a numeric string such that the
  files will be listed in order, then clear the screen and
  reset any other data structures that need it.

  Important note: you cannot just name your files in 
  regular sequence, like pic0, pic1, pic2, pic3... if that
  is done, then pic1, pic10, pic11... will come before pic2
  and so on. In order to keep things clear, add leading 0s
  to the numeric portion of the name. If you use sprintf, 
  you can use "%0xd" for this purpose. It will add at most
  x 0s in front of a number, if needed, so if used correctly,
  and x = 4, you would get numbers like 0001, 0002, 0011,
  0487

  05/17/12 09:41:35
  jdyrlandweaver
  ====================*/
void my_main( int polygons ) {

  int i, f, j;
  double step;
  double xval, yval, zval, knob_value;
  double kr[3], kg[3], kb[3];
  struct matrix *transform;
  struct matrix *tmp;
  struct stack *s;
  screen t;
  color g;
  color *g2;
  zbuffer b;

  struct vary_node **knobs;
  struct vary_node *vn;
  char frame_name[128];

  num_frames = 1;
  step = 5;
 
  g.red = 0;
  g.green = 255;
  g.blue = 255;
  double l[3];
  
  first_pass();
  knobs = second_pass();
  if (num_frames == -1)
    return;
  
  for(j=0;j<num_frames;j++){
    s = new_stack();
    tmp = new_matrix(4,1000);
    clear_screen(t);
    clear_buffer(b);
    vn = knobs[j];
    while (vn != NULL){
      //printf("%d %s %lf\n", j, vn->name, vn->value);
      lookup_symbol(vn->name)->s.value = vn->value;
      vn = vn->next;
    }
    for (i=0;i<lastop;i++) {
      switch (op[i].opcode) {

      case SPHERE:
	add_sphere( tmp,op[i].op.sphere.d[0], //cx
		    op[i].op.sphere.d[1],  //cy
		    op[i].op.sphere.d[2],  //cz
		    op[i].op.sphere.r,
		    step);
	//apply the current top origin
	matrix_mult( s->data[ s->top ], tmp );
	draw_polygons3( tmp, t, g, l, kr, kg, kb, b,2);
	tmp->lastcol = 0;
	break;

      case TORUS:
	add_torus( tmp, op[i].op.torus.d[0], //cx
		   op[i].op.torus.d[1],     //cy
		   op[i].op.torus.d[2],    //cz
		   op[i].op.torus.r0,
		   op[i].op.torus.r1,
		   step);
	matrix_mult( s->data[ s->top ], tmp );
	//printf("torus\n");
	//color_polygons2( tmp, t, g2, b );
	draw_polygons3( tmp, t, g, l, kr, kg, kb, b, 2);
	tmp->lastcol = 0;
	break;

      case BOX:
	add_box( tmp, op[i].op.box.d0[0],
		 op[i].op.box.d0[1],
		 op[i].op.box.d0[2],
		 op[i].op.box.d1[0],
		 op[i].op.box.d1[1],
		 op[i].op.box.d1[2]);
	matrix_mult( s->data[ s->top ], tmp );
        draw_polygons3( tmp, t, g, l, kr, kg, kb, b, 2);
	tmp->lastcol = 0;
	break;

      case LINE:
	add_edge( tmp, op[i].op.line.p0[0],
		  op[i].op.line.p0[1],
		  op[i].op.line.p0[1],
		  op[i].op.line.p1[0],
		  op[i].op.line.p1[1],
		  op[i].op.line.p1[1]);
	draw_lines2( tmp, t, g, b );
	printf("linedrawn\n");
	tmp->lastcol = 0;
	break;

      case MOVE:
	//get the factors
	xval = op[i].op.move.d[0];
	yval =  op[i].op.move.d[1];
	zval = op[i].op.move.d[2];
	if (op[i].op.move.p != NULL){
	  knob_value = op[i].op.move.p->s.value;
	  xval *= knob_value;
	  yval *= knob_value;
	  zval *= knob_value;
	}
      
	transform = make_translate( xval, yval, zval );
	//multiply by the existing origin
	matrix_mult( s->data[ s->top ], transform );
	//put the new matrix on the top
	copy_matrix( transform, s->data[ s->top ] );
	free_matrix( transform );
	break;

      case SCALE:
	xval = op[i].op.scale.d[0];
	yval = op[i].op.scale.d[1];
	zval = op[i].op.scale.d[2];
        if (op[i].op.scale.p != NULL){
	  knob_value = op[i].op.scale.p->s.value;
	  xval *= knob_value;
	  yval *= knob_value;
	  zval *= knob_value;
	}
      
	transform = make_scale( xval, yval, zval );
	matrix_mult( s->data[ s->top ], transform );
	//put the new matrix on the top
	copy_matrix( transform, s->data[ s->top ] );
	free_matrix( transform );
	break;

      case ROTATE:
	xval = op[i].op.rotate.degrees * ( M_PI / 180 );
        if (op[i].op.rotate.p != NULL){
	  knob_value = op[i].op.rotate.p->s.value;
	  xval *= knob_value;
	}
	//get the axis
	if ( op[i].op.rotate.axis == 0 ) 
	  transform = make_rotX( xval );
	else if ( op[i].op.rotate.axis == 1 ) 
	  transform = make_rotY( xval );
	else if ( op[i].op.rotate.axis == 2 ) 
	  transform = make_rotZ( xval );

	matrix_mult( s->data[ s->top ], transform );
	//put the new matrix on the top
	copy_matrix( transform, s->data[ s->top ] );
	free_matrix( transform );
	break;

      case PUSH:
	push( s );
	break;
      case POP:
	pop( s );
	break;
      case SAVE:
	sprintf(frame_name,"%s_%04d",op[i].op.save.p->name,j);
	save_extension( t, op[i].op.save.p->name );
	break;
      case DISPLAY:
	display( t );
	break;
      case AMBIENT:
	g.red = op[i].op.ambient.c[0];
	g.green = op[i].op.ambient.c[1];
	g.blue = op[i].op.ambient.c[2];
	break;
      case CONSTANTS:
	kr[0] = op[i].op.constants.p->s.c->r[0];
	kr[1] = op[i].op.constants.p->s.c->r[1];
	kr[2] = op[i].op.constants.p->s.c->r[2];
        kg[0] = op[i].op.constants.p->s.c->g[0];
	kg[1] = op[i].op.constants.p->s.c->g[1];
	kg[2] = op[i].op.constants.p->s.c->g[2];
	kb[0] = op[i].op.constants.p->s.c->b[0];
	kb[1] = op[i].op.constants.p->s.c->b[1];
	kb[2] = op[i].op.constants.p->s.c->b[2];
	break;
      case LIGHT:
	l[0] = op[i].op.light.p->s.l->l[0];
	l[1] = op[i].op.light.p->s.l->l[1];
	l[2] = op[i].op.light.p->s.l->l[2];
	g.red = op[i].op.light.p->s.l->c[0];
	g.green = op[i].op.light.p->s.l->c[1];
	g.blue = op[i].op.light.p->s.l->c[2];
	break;
      }
    }
    print_knobs();
    sprintf(frame_name,"%s_%04d",name,j);
    save_extension(t,frame_name);
    free_stack( s );
    free_matrix( tmp );
    //free_matrix( transform );
  }
}
