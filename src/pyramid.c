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
at the address above.
---------------------------------------------------------------------

File: pyramid.c

Synopsis: Implements a Gaussian pyarmid.

Status: This is EXPERIMENTAL code.  There is no guarantee.


Details: The two main functions are project_image and
         reduce_image.

************************************************/

# include       <stdio.h>
# include       <math.h>
# include       <string.h>

#include "robust.h"

void sub_sample_image(image, result, nx, ny)
     float *image, *result;
     int nx, ny;
{
  int i, j;

  for(i=0;i<ny/2;i++){
    for(j=0;j<nx/2;j++){
      result[(i*(nx/2)) + j] = image[((i+i)*nx) + j+j];
    }
  }
}

void convolve3(input,nx,ny,filter,filter_size,output,sample)

  float *input, filter[3][3];
  float *output;
  int nx,ny, filter_size, sample;
  {
    float val;
    int i,ii, ir, j, jj, jr;// i0, j0, ik;
    int snx, sny, f_off, si, sj;

    f_off = (int)floor((filter_size - 1)/2.0);
    snx = (int)floor(nx/((float)sample)); sny = (int)floor(ny/((float)sample));

    si=0;
    for(i=1;i<ny-1;i=i+2){
      sj=0;
      for(j=1;j<nx-1;j=j+2){
        val = 0.0;
        for(ii= 0;ii<filter_size;ii++){
	  ir = (i + ii - f_off) % ny;
	  for(jj= 0;jj<filter_size;jj++){
	    jr = (j + jj - f_off) % nx;
	    val += filter[ii][jj] * input[(int) (ir*nx + jr)];
	  }
        }
        output[si*snx + sj] = val;
	sj++;
      }    
      si++;
    }

    if ((nx%2)==0) {
      for(i=0;i<sny;i++){
	output[(i*snx)+(snx-1)] = output[i*snx + (snx-2)];
      }
    }
    if ((ny%2)==0) {
      for(j=0;j<snx;j++){
	output[((sny-1)*snx)+j] = output[(sny-2)*snx + j]; 
      }
    }
  }

void reduce_image(image, reduced, nx, ny)

     float *image, *reduced;
     int nx, ny;
{
  float filter[3][3] = {{0.0625, 0.125, 0.0625},
			  {0.125, 0.25, 0.125},
			  {0.0625, 0.125, 0.0625}};
  int sample=2, filter_size=3;

  convolve3(image, nx, ny, filter, filter_size, reduced, sample);
}


void reduce_flow(input, output, nx, ny)

     float *input, *output;
     int nx, ny;
{
  float filter[3][3] = {{0.0625, 0.125, 0.0625},
			  {0.125, 0.25, 0.125},
			  {0.0625, 0.125, 0.0625}};
  float val;
  int i,ii, ir, j, jj, jr; // i0, j0, ik;
  int snx, sny, f_off, si, sj;
  int sample = 2, filter_size=3;

  f_off = (int)floor((filter_size - 1)/2.0);
  snx = (int)floor(nx/((float)sample)); sny = (int)floor(ny/((float)sample));

    si=0;
    for(i=1;i<ny-1;i=i+2){
      sj=0;
      for(j=1;j<nx-1;j=j+2){
        val = 0.0;
        for(ii= 0;ii<filter_size;ii++){
	  ir = (i + ii - f_off) % ny;
	  for(jj= 0;jj<filter_size;jj++){
	    jr = (j + jj - f_off) % nx;
	    val += filter[ii][jj] * input[(int) (ir*nx + jr)];
	  }
        }
        output[si*snx + sj] = val/2.0;
	sj++;
      }    
      si++;
    }

    if ((nx%2)==0) {
      for(i=0;i<sny;i++){
	output[(i*snx)+(snx-1)] = output[i*snx + (snx-2)];
      }
    }
    if ((ny%2)==0) {
      for(j=0;j<snx;j++){
	output[((sny-1)*snx)+j] = output[(sny-2)*snx + j];
      }
    }
}



/*
Project with interpolation.

This is a direct translation of my Connection Machine version and
as such is not as efficient as it could be.  The second loop should
be broken up into three loops corresponding to the three tests.
*/
void project_with_interp(image, proj, nx, ny, scale)

     float *image, *proj;
     float scale;
     int nx, ny;
{
  int index, i,j, px, py, nx_max, ny_max;
  int snx, sny, si=0, sj=0;

  snx = (int)floor(nx/2.0); sny = (int)floor(ny/2.0);
  fprintf(stderr, "projecting (%d %d) (%d %d)\n",
	  nx, ny, snx, sny);

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      proj[(i*nx) + j] = 0.0;
    }
  }
  for(i=1;i<ny;i+=2){
    sj=0;
    for(j=1;j<nx;j+=2){
      proj[(i*nx) + j] = image[si*snx + sj] * scale;
      sj++;
    }
    si++;
  }

  px = nx;
  py = ny;
  for(i=1;i<py;i++){
    for(j=1;j<px;j++){
      index = (i*px) + j;
      if ((i%2 == 0) && (j%2 == 0)) {
	proj[index] = (proj[(i-1)*px + (j-1)]
			  + proj[(i+1)*px + (j-1)] 
			  + proj[(i-1)*px + (j+1)] 
			  + proj[(i+1)*px + (j+1)])/4.0;
      } 
      
      else if (i%2 == 0) {
	proj[index] = (proj[(i-1)*px + j] + proj[(i+1)*px + j])/2.0;
      }
      
      else if (j%2 == 0) {
	proj[index] = (proj[index-1] + proj[index+1])/2.0;
      }
      
    }
  }

  ny_max = py-1;
  nx_max = px-1;
  for(i=0;i<py;i++){
    proj[i*px] = proj[i*px + 1];
    proj[i*px + nx_max] = proj[i*px + (nx_max-1)]; 
  }
  for(j=0;j<px;j++){
    proj[j] = proj[px + j];
   proj[(ny_max * px) + j] = proj[(ny_max-1)*px + j]; 
  }

}	  

void project_image(image, proj, nx, ny)

     float *image, *proj;
     int nx, ny;
{
  float scale = 1.0;

  project_with_interp(image, proj, nx, ny, scale);
}


void project_flow(image, proj, nx, ny)

     float *image, *proj;
     int nx, ny;
{
  project_with_interp(image, proj, nx, ny, 2.0);
}


