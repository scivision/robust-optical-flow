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

File: sor.c

Synopsis: Implements Simultaneous Over Relaxation.

Status: This is EXPERIMENTAL code.  There is no guarantee.


Details: Uses the Lorentzian robust estimator.
         To change estimators you would need to change the
	 defintion of "psi" and "sor-scale".


************************************************/

# include       <stdio.h>
# include       <math.h>
# include       <string.h>

#include "robust.h"

sor(image1, image2, iters, w, u, v, prev_u, prev_v, du, dv, 
    l1, l2, l3, s1, s2, s3, nx, ny,
    Ix, Iy, It, err, u_scale, v_scale)

     float *Ix, *Iy, *It, *u, *v, *u_scale, 
       *v_scale, *du, *dv, *prev_u, *prev_v, 
       *image1, *image2, *err;
     int iters, nx, ny;
     float w, l1, l2, l3;
     float *s1, *s2, *s3;

{
  float image_abs_max(), variance(), mean(), rms();
  int n;

    /* Compute derivatives */

    horn_derives(image1, image2, Ix, Iy, It, nx, ny);

    /* compute scale */
    sor_scale(u_scale, Ix, nx, ny, s1, s2, s3, l1, l2, l3);
    sor_scale(v_scale, Iy, nx, ny, s1, s2, s3, l1, l2, l3);

    /* minimize: update du, dv */
    for(n=0;n<iters;n++){
      fprintf(stderr, ".");
      inten_const(err, Ix, Iy, It, du, dv, nx, ny);
      sor_1_iter(Ix, err, du, u, prev_u, nx, ny, w, u_scale,
		 l1, l2, l3, s1, s2, s3);
      inten_const(err, Ix, Iy, It, du, dv, nx, ny);
      sor_1_iter(Iy, err, dv, v, prev_v, nx, ny, w, v_scale, 
		 l1, l2, l3, s1, s2, s3);
    }
  /* update u, v */
  add_image(u, du, nx, ny);
  add_image(v, dv, nx, ny);

}

sor_1_iter(grad, err, dflow, flow, prev_flow, nx, ny, 
	   om, scale, l1, l2, l3, s1, s2, s3)

     float *dflow, *flow, *prev_flow, 
       *grad, *err, *scale;
     float om, l1, l2, l3;
     float *s1, *s2, *s3;
     int nx, ny;
{
  int index, i,j;
  float new_flow, ts2;
  float psi2(), psi();

  /* update white sites */
  for(i=1;i<ny-1;i++){
    for(j=1+(i%2);j<nx-1;j=j+2){
      index = (i*nx) +j;
      new_flow = flow[index] + dflow[index];
      ts2 = s2[index];
      ts2 = 2.0*ts2*ts2;
      dflow[index] = 
	dflow[index] - 
	  ((om * scale[index]) * 
	   ((l1 * grad[index] * psi(err[index], s1[index])) +
	    (l2 * (psi2(new_flow - (flow[index-nx] + dflow[index-nx]), ts2)
		   + psi2(new_flow - (flow[index-1] + dflow[index-1]), ts2)
		   + psi2(new_flow - (flow[index+nx] + dflow[index+nx]), ts2)
		   + psi2(new_flow - (flow[index+1] + dflow[index+1]), ts2))) +
	    (l3 * psi(new_flow - prev_flow[index], s3[index]))));
    }
  }

  /* update black sites */
  for(i=1;i<ny-1;i++){
    for(j=2+(i%2);j<nx-1;j=j+2){
      index = (i*nx) +j;
      new_flow = flow[index] + dflow[index];
      ts2 = s2[index];
      ts2 = 2.0*ts2*ts2;
      dflow[index] = 
	dflow[index] - 
	  ((om * scale[index]) * 
	   ((l1 * grad[index] * psi(err[index], s1[index])) +
	    (l2 * (psi2(new_flow - (flow[index-nx] + dflow[index-nx]), ts2)
		   + psi2(new_flow - (flow[index-1] + dflow[index-1]), ts2)
		   + psi2(new_flow - (flow[index+nx] + dflow[index+nx]), ts2)
		   + psi2(new_flow - (flow[index+1] + dflow[index+1]), ts2))) +
	    (l3 * psi(new_flow - prev_flow[index], s3[index]))));
    }
  }
  fix_flow(dflow, nx, ny);
}
  

sor_scale(scale, grad, nx, ny, s1, s2, s3, l1, l2, l3)
     
     float *scale, *grad;
     float *s1, *s2, *s3;
     float l1, l2, l3;
     int nx, ny;
{
  int index, i, j;
  float tgrad, ts1, ts2, ts3;

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      index = (i*nx) + j;
      tgrad = grad[index];
      ts1 = s1[index];
      ts2 = s2[index];
      ts3 = s3[index];

      scale[index] = (1.0 / (((l1 * tgrad * tgrad)/
			     (ts1 * ts1)) +
			    ((4.0 * l2)/(ts2 * ts2)) +
			    (l3 / (ts3 * ts3))));

    }
  }
}


/* 
lorentzian 
*/
float psi(x, sigma)

     float x, sigma;
{
  float result;

  result = (2.0 * x) / ((2.0 * sigma * sigma) + (x * x)); 

  return(result);
}
  
float psi2(x, sigma2)

     float x, sigma2;
{
  float result;

  result = (2.0 * x) / (sigma2 + (x * x)); 

  return(result);
}
  


inten_const(err, Ix, Iy, It, u, v, nx, ny)

     float *err, *Ix, *Iy, *It, *u, *v;
     int nx, ny;
{
  int index, i, j;

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      index = (i*nx) + j;
      err[index] = Ix[index]*u[index] + Iy[index]*v[index] + It[index];
    }
  }
}

fix_flow(flow, nx, ny)

     float *flow;
     int nx, ny;
{
  int index, i, j;
  float tflow;

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){ 
      index = (i*nx) + j;
      tflow = flow[index];
      flow[index] = sign(tflow, 1.0) * min(abs(tflow), 1.0);
    }
  }
}

fix_flow2(flow, nx, ny, max_flow)

     float *flow, max_flow;
     int nx, ny;
{
  int index, i, j;
  float tflow;

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){ 
      index = (i*nx) + j;
      tflow = flow[index];
      flow[index] = sign(tflow, 1.0) * min(abs(tflow), max_flow);
    }
  }
}


