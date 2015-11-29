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

File: 

Synopsis: 

Status: This is EXPERIMENTAL code.  There is no guarantee.


************************************************/
# include       <stdio.h>
# include       <math.h>
# include       <string.h>

#include "robust.h"

float image_mean(image, nx, ny)

     float *image;
     int nx, ny;
{
  int index, i, j;
  float sum;

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      index = i*nx + j;
      sum = sum + image[index];
    }
  }
  sum = sum / (float)(nx * ny);
  return(sum);
}

subtract_image_value(image, sub, nx, ny)

     float *image, sub;
     int nx, ny;
{
  int index, i, j;

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      index = i*nx + j;
      image[index] = image[index] - sub;
    }
  }
}


add_image_value(image, add, nx, ny)

     float *image, add;
     int nx, ny;
{
  int index, i, j;

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      index = i*nx + j;
      image[index] = image[index] - add;
    }
  }
}


band_pass_image(image, nx, ny)

     float *image;
     int nx, ny;
{
  subtract_image_value(image, image_mean(image, nx, ny), nx, ny);
}


average_neighborhood(input,output,nx,ny,size)

  float *input, *output;
  int nx,ny, size;
  {
    float val;
    int i,ii, ir, j, jj, jr, i0, j0, ik;
    int f_off, si, sj;

    f_off = (int)floor((size - 1)/2.0);

    for(i=0;i<ny;i++){
      for(j=0;j<nx;j++){
        val = 0.0;
        for(ii= 0;ii<size;ii++){
	  ir = (i + ii - f_off) % ny;
	  for(jj= 0;jj<size;jj++){
	    jr = (j + jj - f_off) % nx;
	    val += input[(int) (ir*nx + jr)];
	  }
        }
        output[i*nx + j] = val/(float)(size*size);
      }    
    }
  }


div_images(image1, image2, nx, ny)

     float *image1, *image2;
     int nx, ny;
{
  int index, i, j;

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      index = i*nx + j;
      image1[index] = image1[index] / image2[index];
    }
  }
}

