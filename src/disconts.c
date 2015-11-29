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

File: disconts.c

Synopsis: Compute where there are spatial and data outliers.

Status: This is EXPERIMENTAL code.  There is no guarantee.

Details: 

************************************************/

# include       <stdio.h>
# include       <math.h>
# include       <string.h>

#include "robust.h"


disconts(u, v, prev_u, prev_v, du, dv, 
	 s1, s2, s3, nx, ny,
	 Ix, Iy, It, err, discont)

     float *Ix, *Iy, *It, *u, *v,
       *du, *dv, *prev_u, *prev_v, *err,
       *discont;
     int nx, ny;
     float *s1, *s2, *s3;

{
  float image_abs_max(), variance(), mean();
  int index, nx_max, ny_max, i, j;
  float t1, t2, t3;

  /* compute disconts */
  inten_const(err, Ix, Iy, It, du, dv, nx, ny);
  for(i=1;i<ny-1;i++){
    for(j=1;j<nx-1;j++){
      index = i*nx + j;
      t1 = s1[index] * ROOT2;
      t2 = s2[index] * ROOT2;
      t3 = s3[index] * ROOT2;
      /* data and temporal disconts */
      if ((abs(err[index]) >= t1) ||
	  (abs(u[index] - prev_u[index]) >= t3) ||
	  (abs(v[index] - prev_v[index]) >= t3))
	discont[index] = 0.0;
      else
	discont[index] = 1.0;
    }
  }
  ny_max = ny-1;
  nx_max = nx-1;
  for(i=0;i<ny;i++){
    discont[(i*nx)] = 1.0;
    discont[(i*nx) + nx_max] = 0.0;
  }
  for(j=0;j<nx;j++){
    discont[j] = 1.0;
    discont[(ny_max*nx)+j] = 0.0;
  }

}

temporal_outliers(u, v, prev_u, prev_v, du, dv, 
	 s1, s2, s3, nx, ny,
	 Ix, Iy, It, err, discont)

     float *Ix, *Iy, *It, *u, *v,
       *du, *dv, *prev_u, *prev_v, *err,
       *discont;
     int nx, ny;
     float *s1, *s2, *s3;

{
  float image_abs_max(), variance(), mean();
  int index, nx_max, ny_max, i, j;
  float t1, t2, t3;

  for(i=1;i<ny-1;i++){
    for(j=1;j<nx-1;j++){
      index = i*nx + j;
      t3 = s3[index] * ROOT2;
      /* temporal disconts */
      if ((abs(u[index] - prev_u[index]) >= t3) ||
	  (abs(v[index] - prev_v[index]) >= t3))
	discont[index] = 0.0;
      else
	discont[index] = 1.0;
    }
  }
  ny_max = ny-1;
  nx_max = nx-1;
  for(i=0;i<ny;i++){
    discont[(i*nx)] = 1.0;
    discont[(i*nx) + nx_max] = 1.0;
  }
  for(j=0;j<nx;j++){
    discont[j] = 1.0;
    discont[(ny_max*nx)+j] = 1.0;
  }

}

data_disconts(image1, image2, u, v, s1, nx, ny, warp, It, discont)

     float *image1, *image2, *It, *u, *v, *warp, *discont;
     int nx, ny;
     float *s1;

{
  int index, nx_max, ny_max, i, j;
  float t1;
  char *fnout;

  /* compute disconts */
  warp_image(image1, warp, u, v, nx, ny);
  horn_dt(warp, image2, It, nx, ny);
  for(i=1;i<ny-1;i++){
    for(j=1;j<nx-1;j++){
      index = i*nx + j;
      t1 = s1[index] * ROOT2;
      /* data and temporal disconts */
      if (abs(It[index]) >= t1) 
	discont[index] = 0.0;
      else
	discont[index] = 1.0;
    }
  }
  ny_max = ny-1;
  nx_max = nx-1;
  for(i=0;i<ny;i++){
    discont[i*nx] = 0.0;
    discont[i*nx + nx_max] = 0.0;
  }
  for(j=0;j<nx;j++){
    discont[j] = 0.0;
    discont[ny_max*nx + j] = 0.0;
  }

}



spatial_disconts(u, v, s2, nx, ny, discont)

     float *u, *v, *discont;
     int nx, ny;
     float *s2;

{
  float image_abs_max(), variance(), mean();
  int index, nx_max, ny_max, i, j;
  float t2;

  /* compute spatial disconts */
  for(i=1;i<ny-1;i++){
    for(j=1;j<nx-1;j++){
      index = i*nx + j;
      t2 = s2[index] * ROOT2;
      /* data and temporal disconts */
      if ((abs(u[index] - u[index-nx]) >= t2) ||
	  (abs(u[index] - u[index-1]) >= t2) ||
	  (abs(v[index] - v[index-nx]) >= t2) ||
	  (abs(v[index] - v[index-1]) >= t2))	
	discont[index] = 0.0;
      else
	discont[index] = 1.0;
    }
  }
  ny_max = ny-1;
  nx_max = nx-1;
  for(i=0;i<ny;i++){
    discont[i*nx] = 0.0;
    discont[i*nx + nx_max] = 0.0;
  }
  for(j=0;j<nx;j++){
    discont[j] = 0.0;
    discont[ny_max*nx + j] = 0.0;
  }

}


