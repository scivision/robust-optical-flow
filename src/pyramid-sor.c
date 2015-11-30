/***********************************************

Author contact information: 
    Michael J. Black
    Department of Computer Science
    Brown University 
    P.O. Box 1910
    Providence, RI 02912 

    http://www.cs.brown.edu/people/black/
    email: black@cs.brown.edu

    401-863-7637 (voice) 
    401-863-7657 (fax) 

---------------------------------------------------------------------
Copyright 1993, Michael J. Black

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any non-commercial purpose is hereby granted without
fee, provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear in
supporting documentation, and that the name of the author not be used
in advertising or publicity pertaining to distribution of the software
without specific, written prior permission.

THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
ANY PARTICULAR PURPOSE.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

For commercial use and licensing information contact the author
at the address above
---------------------------------------------------------------------

File: pyramid-sor.c

Synopsis: Perform coarse-to-fine Simultaneous Over Relaxation.

Status: This is EXPERIMENTAL code.  There is no guarantee.


	The code is recursive and currently it allocates
	memory for each recursive call.  This wastes time when
	used incrementally; for each image we end up reallocating
	memory.  This could be done once and used over and over.

Details:


************************************************/

# include       <stdio.h>
# include       <math.h>
# include       <string.h>
# include <stdlib.h>

#include "robust.h"

void pyramid_sor(float *image1, float *image2, int max_level, int min_level, int iters, float omega, float *u, float *v, 
	     float *prev_u, float *prev_v, float *du, float *dv, 
	     float l1, float l2, float l3, float *s1, float *s2, float *s3, int nx, int ny,
	     float *Ix, float *Iy, float *It, float *err, float *u_scale, float *v_scale, int filter)

{
  float *p_im1, *p_im2, *p_u, *p_v, *p2_u, *p2_v, *p_du, *p_dv;
  float *lap1, *lap2, *p_prev_u, *p_prev_v, *warp;
  float *p_s1, *p_s2, *p_s3;
  int index, i, j, sizeImage, smallsize, snx, sny;
  float image_abs_max();

  fprintf(stderr," nx %d\n", nx); 

  sizeImage = nx * ny * sizeof ( float );
  if ((lap1 = (float *) malloc((size_t) sizeImage)) == NULL)
    error(" Unable to allocate memory for ", "lap1");
  if ((lap2 = (float *) malloc((size_t) sizeImage)) == NULL)
    error(" Unable to allocate memory for ", "lap2");
  if ((warp = (float *) malloc((size_t) sizeImage)) == NULL)
    error(" Unable to allocate memory for ", "warp");

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      index = (i*nx) + j;
      lap1[index] = 0.0;
      lap2[index] = 0.0;
      warp[index] = 0.0;
    }
  }

  if (max_level <= min_level) {
    
    warp_image(image1, warp, u, v, nx, ny);
    if (filter == 1) {
      laplacian2(warp, lap1, nx, ny);
      laplacian2(image2, lap2, nx, ny);
    }
    else {
      copy_image(lap1, warp, nx, ny);
      copy_image(lap2, image2, nx, ny);
    }
    for(i=0;i<ny;i++){
      for(j=0;j<nx;j++)
      {
      index = (i*nx) + j;
      du[index] = 0.0;
      dv[index] = 0.0;
      }
    }
    

    /* side effect u,v */
    sor(lap1, lap2, iters, omega, u, v, 
	    prev_u, prev_v, du, dv, 
	    l1, l2, l3, s1, s2, s3, nx, ny,
	    Ix, Iy, It, err, u_scale, v_scale);
      

    fprintf(stderr, "Max u, %f, level %d\n", image_abs_max(u,nx,ny), nx);

  }
  else {
    snx = (int)floor(nx/2.0); sny = (int)floor(ny/2.0);
    smallsize = snx * sny * sizeof( float );
    if ((p_im1 = (float *) malloc((size_t) smallsize)) == NULL)
      error(" Unable to allocate memory for ", "p_im1");
    if ((p_im2 = (float *) malloc((size_t) smallsize)) == NULL)
      error(" Unable to allocate memory for ", "p_im2");
    if ((p_du = (float *) malloc((size_t) smallsize)) == NULL)
      error(" Unable to allocate memory for ", "p_du");
    if ((p_dv = (float *) malloc((size_t) smallsize)) == NULL)
      error(" Unable to allocate memory for ", "p_dv");
    if ((p_u = (float *) malloc((size_t) smallsize)) == NULL)
      error(" Unable to allocate memory for ", "p_u");
    if ((p_v = (float *) malloc((size_t) smallsize)) == NULL)
      error(" Unable to allocate memory for ", "p_v");
    if ((p_prev_u = (float *) malloc((size_t) smallsize)) == NULL)
      error(" Unable to allocate memory for ", "p_prev_u");
    if ((p_prev_v = (float *) malloc((size_t) smallsize)) == NULL)
      error(" Unable to allocate memory for ", "p_prev_v");
    if ((p_s1 = (float *) malloc((size_t) smallsize)) == NULL)
      error(" Unable to allocate memory for ", "p_s1");
    if ((p_s2 = (float *) malloc((size_t) smallsize)) == NULL)
      error(" Unable to allocate memory for ", "p_s2");
    if ((p_s3 = (float *) malloc((size_t) smallsize)) == NULL)
      error(" Unable to allocate memory for ", "p_s3");

    if ((p2_u = (float *) malloc((size_t) sizeImage)) == NULL)
      error(" Unable to allocate memory for ", "p2_u");
    if ((p2_v = (float *) malloc((size_t) sizeImage)) == NULL)
      error(" Unable to allocate memory for ", "p2_v");

    for(i=0;i<sny;i++){
      for(j=0;j<snx;j++){
	index = (i*snx) + j;
	p_im1[index] = 0.0;
	p_im2[index] = 0.0;
	p_u[index] = 0.0;
	p_v[index] = 0.0;
	p_du[index] = 0.0;
	p_dv[index] = 0.0;
	p_prev_u[index] = 0.0;
	p_prev_v[index] = 0.0;
	p_s1[index] = 0.0;
	p_s2[index] = 0.0;
	p_s3[index] = 0.0;
      }
    }

    for(i=0;i<ny;i++){
      for(j=0;j<nx;j++){
	index = (i*nx) + j;
	p2_u[index] = 0.0;
	p2_v[index] = 0.0;
      }
    }

    reduce_image(image1, p_im1, nx, ny);
    reduce_image(image2, p_im2, nx, ny);
    reduce_flow(u, p_u, nx, ny);
    reduce_flow(v, p_v, nx, ny);
    reduce_flow(du, p_du, nx, ny);
    reduce_flow(dv, p_dv, nx, ny);
    reduce_flow(prev_u, p_prev_u, nx, ny);
    reduce_flow(prev_v, p_prev_v, nx, ny);
    reduce_image(s1, p_s1, nx, ny);
    reduce_flow(s2, p_s2, nx, ny); /* divide by two */
    reduce_flow(s3, p_s3, nx, ny); /* divide by two */
    pyramid_sor(p_im1, p_im2, (max_level - 1), min_level, iters, omega, 
		p_u, p_v, 
		p_prev_u, p_prev_v, 
		p_du, p_dv, 
		l1, l2, l3, p_s1, p_s2, p_s3, 
		snx, sny,
		Ix, Iy, It, err, u_scale, v_scale, filter);

    /* when it returns... */
    project_flow(p_u, p2_u, nx, ny);
    project_flow(p_v, p2_v, nx, ny);

    fprintf(stderr, "Max p_u (pre project), %f, level %d\n", image_abs_max(p_u,(int)floor(nx/2.0),(int)floor(ny/2.0)), (int)floor(nx/2.0));
    fprintf(stderr, "Max p2_u (post project), %f, level %d\n", image_abs_max(p2_u,nx,ny), nx);
    fprintf(stderr, "Max u, %f, level %d\n", image_abs_max(u,nx,ny), nx);

    /* sanity check */
    for(i=0;i<ny;i++){
      for(j=0;j<nx;j++){
	index = (i*nx) + j;
	if ((abs(p2_u[index] - u[index]) >= 0.5) || 
	    (abs(p2_v[index] - v[index]) >= 0.5)) 
	  {
	    u[index] = p2_u[index];
	    v[index] = p2_v[index];
	  }
      }
    }

    warp_image(image1, warp, u, v, nx, ny);
    if (filter == 1) {
      laplacian2(warp, lap1, nx, ny);
      laplacian2(image2, lap2, nx, ny);
    }
    else {
      copy_image(lap1, warp, nx, ny);
      copy_image(lap2, image2, nx, ny);
    }
    for(i=0;i<ny;i++){
      for(j=0;j<nx;j++){
	index = (i*nx) + j;
	du[index] = 0.0;
	dv[index] = 0.0;
      }
    }

    /* side effect u,v */
    sor(lap1, lap2, iters, omega, u, v, 
	    prev_u, prev_v, du, dv, 
	    l1, l2, l3, s1, s2, s3, nx, ny,
	    Ix, Iy, It, err, u_scale, v_scale);

    fprintf(stderr, "Max u, %f, level %d\n", image_abs_max(u,nx,ny), nx);

    free(p_im1); free(p_im2);
    free(p_u); free(p_v);
    free(p_du); free(p_dv);
    free(p2_u); free(p2_v);
    free(p_prev_u); free(p_prev_v);
    free(p_s1); free(p_s2); free(p_s3);

  }

  free(lap1); free(lap2);
  free(warp);

}


