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

File: image_stats.c

Synopsis: A collection of functions for computing image statistics.

Status: This is EXPERIMENTAL code.  There is no guarantee.

Details: 

************************************************/

# include       <stdio.h>
# include       <math.h>
# include       <string.h>

#include "robust.h"

#define BIG_POS_NUM 1.0e38
#define BIG_NEG_NUM -1.0e38

float image_abs_max(image, nx, ny)

     float *image;
     int nx, ny;
{
  float result;
  int i, j;

  result = 0.0;
  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      result = max(result,abs(image[i*nx + j]));
    }
  }
  return(result);
}
  

float image_max(image, nx, ny)

     float *image;
     int nx, ny;
{
  float result;
  int i, j;

  result = BIG_NEG_NUM;
  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      result = max(result,abs(image[i*nx + j]));
    }
  }
  return(result);
}
  

float image_min(image, nx, ny)

     float *image;
     int nx, ny;
{
  float result;
  int i, j;

  result = BIG_POS_NUM;
  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      result = min(result,abs(image[i*nx + j]));
    }
  }
  return(result);
}
  
float mean(image, nx, ny)

     float *image;
     int nx, ny;
{
  float result;
  int i, j;

  result = 0.0;
  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      result = result + image[i*nx + j];
    }
  }
  result = result / ((float) (nx * ny));
  return(result);
}
  
float rms(image, nx, ny)

     float *image;
     int nx, ny;
{
  float result, tmp;
  int i, j;

  result = 0.0;
  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      tmp = image[i*nx + j];
      result = result + (tmp * tmp);
    }
  }
  result =  sqrt(result) / ((float) (nx * ny));
  return(result);
}
  
float variance(image, mean, nx, ny)

     float *image;
     float mean;
     int nx, ny;
{
  float result, tmp;
  int i, j;

  result = 0.0;
  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      tmp = mean - image[i*nx + j];
      result = result + (tmp * tmp);
    }
  }
  result = result / ((float) (nx * ny));
  return(result);
}
  

