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

File: utils.c

Synopsis: A variety of utility functions.

Status: This is EXPERIMENTAL code.  There is no guarantee.

Details: 

************************************************/

# include       <stdio.h>
# include       <math.h>
# include       <string.h>
#include <stdlib.h>

#include "robust.h"
  

void pad_string(fnCurr, pad, im)
     char fnCurr[MAXLEN];
     int pad, im;
{
  if (pad > 0) {
    if (im==0)
      im++;
    if (im < pad)
      strcat(fnCurr, "0");
    if (im < pad/10)
      strcat(fnCurr, "0");
    if (im < pad/100)
      strcat(fnCurr, "0");
  }
}

float linear_interp(low_value, high_value, position)

     float low_value, high_value, position;
{
  float result;

  result = ((low_value * (1.0 - position))
	    + (high_value * position));
  return(result);
}


void reverse(s)
     char s[];
{
  int c, i, j;
  
  for (i=0,j=((int) strlen(s))-1;i<j;i++,j--) {
    c=s[i];
    s[i]=s[j];
    s[j]=c;
  }
}

static char s[MAXLEN];

char *itoa(n)
     int n;
{
  int i, sign_val;
  
  if ((sign_val = n) < 0)
    n= -n;
  i=0;
  do {
    s[i++] = n % 10 + '0';
  } while ((n /= 10) > 0);
  if (sign_val < 0)
    s[i++] = '-';
  s[i] = '\0';
  reverse(s);
  return(s);
}


void error(s1, s2)
 char *s1, *s2;
 {
  fprintf(stderr, "%s %s", s1, s2);
  exit(1);
 }

/* 
read an image and return a floating point image.
*/
void read_image(image, fdIn, fnIn, nx,ny)
     
     float *image;
     char fnIn[MAXLEN];
     int fdIn, nx, ny;
{
  unsigned char *temp;
  int index, i, j, iy, i0, sizeInput;

  sizeInput = nx * ny * sizeof( char );
  if ((temp = (unsigned char *) malloc((size_t) sizeInput)) == NULL)
    error(" Unable to allocate memory for ", "image");
  if ( sizeInput != (i0 = read(fdIn,(char *) temp, sizeInput)) ) {
    fprintf(stderr,"error: unexpected end of file on %s\n", fnIn);
    exit(1);
  }

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      index = (i * nx) + j;
      image[index] = ((float) temp[index]);
    }
  }
  free(temp);
}

void read_image_float(image, fnIn, nx,ny)

     float *image;
     char fnIn[MAXLEN];
     int nx, ny;
{
  int index, i, j, iy, i0, sizeInput;
  FILE *infile;

  infile = fopen(fnIn, "rb");
  if (infile == NULL) {
    fprintf(stderr, "infile: NULL\n");
  }

  sizeInput = nx * ny * sizeof( float );
  fread((float *) image, sizeInput, 1, infile);

  fclose(infile);
}


void read_image_float_strip(image, fdIn, fnIn, nx,ny, bytes)
     
     float *image;
     char fnIn[MAXLEN];
     int fdIn, nx, ny, bytes;
{
  unsigned char *c;
  int index, i, j, iy, i0, sizeInput;

  sizeInput = bytes * sizeof( char );
  if ((c = (unsigned char *) malloc((size_t) sizeInput)) == NULL)
    error(" Unable to allocate memory for ", "image");
  if ( sizeInput != (i0 = read(fdIn,(char *) c, sizeInput)) ) {
    fprintf(stderr,"error: unexpected end of file on %s\n", fnIn);
    exit(1);
  }

  sizeInput = nx * ny * sizeof( float );
  if ( sizeInput != (i0 = read(fdIn,(float *) image, sizeInput)) ) {
    fprintf(stderr,"error: unexpected end of file on %s\n", fnIn);
    exit(1);
  }
}

void read_image_strip_bytes(image, fnIn, nx,ny, bytes)
     
     float *image;
     char *fnIn;
     int nx, ny, bytes;
{
  unsigned char *temp, *c;
  FILE *infile;
  int index, i, j, iy, i0, sizeInput;

  infile = fopen(fnIn, "rb");
  if (infile == NULL) {
    fprintf(stderr, "infile: %s NULL\n", fnIn);
  }

  sizeInput = bytes * sizeof( char );
  if ((c = (unsigned char *) malloc((size_t) sizeInput)) == NULL)
    error(" Unable to allocate memory for ", "image");
  fread((char *) c, sizeInput, 1, infile);

  sizeInput = nx * ny * sizeof( char );
  if ((temp = (unsigned char *) malloc((size_t) sizeInput)) == NULL)
    error(" Unable to allocate memory for ", "image");
  fread((char *) temp, sizeInput, 1, infile);

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      index = (i * nx) + j;
      image[index] = ((float) temp[index]);
    }
  }
  free(temp);
  free(c);
  fclose(infile);
}

void read_binary_image_strip_bytes(image, fdIn, fnIn, nx,ny, bytes)
     
     float *image;
     char fnIn[MAXLEN];
     int fdIn, nx, ny, bytes;
{
  unsigned char *temp, *c;
  int index, i, j, iy, i0, sizeInput;

  sizeInput = bytes * sizeof( char );
  if ((c = (unsigned char *) malloc((size_t) sizeInput)) == NULL)
    error(" Unable to allocate memory for ", "image");
  if ( sizeInput != (i0 = read(fdIn,(char *) c, sizeInput)) ) {
    fprintf(stderr,"error: unexpected end of file on %s\n", fnIn);
    exit(1);
  }

  sizeInput = nx * ny * sizeof( char );
  if ((temp = (unsigned char *) malloc((size_t) sizeInput)) == NULL)
    error(" Unable to allocate memory for ", "image");
  if ( sizeInput != (i0 = read(fdIn,(char *) temp, sizeInput)) ) {
    fprintf(stderr,"error: unexpected end of file on %s\n", fnIn);
    exit(1);
  }

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      index = (i * nx) + j;
      image[index] = ((float) temp[index]);
    }
  }
  free(temp);
}


void read_float_image(image, fdIn, fnIn, nx,ny)
     
     float *image;
     char fnIn[MAXLEN];
     int fdIn, nx, ny;
{
  int i, j, iy, i0, sizeInput;

  sizeInput = nx * ny * sizeof( float );
  if ( sizeInput != (i0 = read(fdIn,(float *) image, sizeInput)) ) {
    fprintf(stderr,"error: unexpected end of file on %s\n", fnIn);
    exit(1);
  }
}

void read_float_image_skip_floats(image, fdIn, fnIn, nx,ny, floats)
     
     float *image;
     char fnIn[MAXLEN];
     int fdIn, nx, ny, floats;
{
  int i, j, iy, i0, sizeInput;
  float *f;

  sizeInput = floats * sizeof( float );
  if ((f = (float *) malloc((size_t) sizeInput)) == NULL)
    error(" Unable to allocate memory for ", "f");
  if ( sizeInput != (i0 = read(fdIn,(float *) f, sizeInput)) ) {
    fprintf(stderr,"error: unexpected end of file on %s\n", fnIn);
    exit(1);
  }

  sizeInput = nx * ny * sizeof( float );
  if ( sizeInput != (i0 = read(fdIn,(float *) image, sizeInput)) ) {
    fprintf(stderr,"error: unexpected end of file on %s\n", fnIn);
    exit(1);
  }
  free(f);
}

/*
copy image from to image to.
*/

void copy_image(to, from, nx, ny)

     float *to, *from;
     int nx, ny;
{
  int i, j;

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      to[i*nx + j] = from[i*nx + j];
    }
  }
}

void sub_image(to, from, x, y, dx, dy, nx, ny)

     float *to, *from;
     int x, y, dx, dy, nx, ny;
{
  int i, j;

  for(i=0;i<dy;i++){
    for(j=0;j<dx;j++){
      to[i*dx + j] = from[(i+y)*nx + j+x];
    }
  }
}

/*
intialize image to 0.0
*/
void zero_image(image, nx, ny)

     float *image;
     int nx, ny;
{
  int i, j;

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      image[i*nx + j] = 0.0;
    }
  }
}

void random_image(image, nx, ny)

     float *image;
     int nx, ny;
{
  int i, j;

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      image[i*nx + j] = (int)floor((((float) ((rand()>>4)&1023))/4.0));
    }
  }
}

/* 
   take an image of size nx x ny and return one of size
   ny x nx 
*/
void rotate_image_90(image, nx, ny)

     float *image;
     int nx, ny;
{
  int i, j, sizeInput;
  float *temp;

  sizeInput = nx * ny * sizeof( float );
  if ((temp = (float *) malloc((size_t) sizeInput)) == NULL)
    error(" Unable to allocate memory for ", "image");

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      temp[j*ny+i] = image[i*nx + j];
    }
  }
  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      image[i*nx+j] = temp[i*nx + j];
    }
  }
  free(temp);
}


/* 
add "add" to "image" 
*/
void add_image(image, add, nx, ny)

     float *image, *add;
     int nx, ny;
{
  int index, i, j;

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      index = i*nx + j;
      image[index] = image[index] + add[index];
    }
  }
}

void add_scalar(image, add, nx, ny)

     float *image, add;
     int nx, ny;
{
  int index, i, j;

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      index = i*nx + j;
      image[index] += add;
    }
  }
}

void sub_scalar(image, add, nx, ny)

     float *image, add;
     int nx, ny;
{
  int index, i, j;

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      index = i*nx + j;
      image[index] -= add;
    }
  }
}

void average_images(image1, image2, mean, nx, ny)

     float *image1, *image2, *mean;
     int nx, ny;
{
  int index, i, j;

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      index = i*nx + j;
      mean[index] = (image1[index] + image2[index])/2.0;
    }
  }
}

/* subtract "sub" from "image" */
void subtract_image(image, sub, nx, ny)

     float *image, *sub;
     int nx, ny;
{
  int index, i, j;

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      index = i*nx + j;
      image[index] -= sub[index];
    }
  }
}

void abs_image(image, nx, ny)

     float *image;
     int nx, ny;
{
  int index, i, j;

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      index = i*nx + j;
      image[index] = abs(image[index]);
    }
  }
}

void square_image(image, nx, ny)

     float *image;
     int nx, ny;
{
  int index, i, j;

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      index = i*nx + j;
      image[index] *= image[index];
    }
  }
}

void sqrt_image(image, nx, ny)

     float *image;
     int nx, ny;
{
  int index, i, j;

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      index = i*nx + j;
      image[index] = sqrt(image[index]);
    }
  }
}


void one_over_image(image, nx, ny)

     float *image;
     int nx, ny;
{
  int index, i, j;
  register float tmp;

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      index = i*nx + j;
      tmp = image[index];
      if (abs(tmp)>0.0) 
	image[index] = 1.0/tmp;
      else
	image[index] = 0.0;
    }
  }
}


void invert_image(image, nx, ny)

     float *image;
     int nx, ny;
{
  int index, i, j;
  register float tmp;

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      index = i*nx + j;
      image[index] = abs(image[index] - 1.0);
    }
  }
}


void invert_image2(image, nx, ny, maxval)

     float *image, maxval;
     int nx, ny;
{
  int index, i, j;

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      index = i*nx + j;
      image[index] = abs(image[index] - maxval);
    }
  }
}


/* 
divide image by a float
*/
void divide_image(image, divisor, nx, ny)

     float *image, divisor;
     int nx, ny;
{
  int index, i, j;

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      index = i*nx + j;
      image[index] = image[index] / divisor;
    }
  }
}


void mul_image_val(image1, val, nx, ny)

     float *image1, val;
     int nx, ny;
{
  int index, i, j;

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      index = i*nx + j;
      image1[index] *= val;
    }
  }
}

void mul_images(image1, image2, nx, ny)

     float *image1, *image2;
     int nx, ny;
{
  int index, i, j;

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      index = i*nx + j;
      image1[index] = image1[index] * image2[index];
    }
  }
}

void mask_image(image, mask, mval, dval, nx, ny)

     float *image, *mask, mval, dval;
     int nx, ny;
{
  int index, i, j;

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      index = i*nx + j;
      if (mask[index] < mval)
	image[index] = dval;
    }
  }
}


  
void quantize(image, nx, ny, quant, maxAmp)
 float *image, *maxAmp;
 unsigned char *quant;
 int nx,ny;
 {
  float val, dc, scl;
  float im_min, im_max;
  int i,ix,iy;
  unsigned char colour();
 
  *maxAmp = 0.0;
  for(iy=0; iy<ny; iy++) 
   for(ix=0; ix<nx; ix++) {
    *maxAmp = max( *maxAmp, abs(image[iy*nx + ix]));
   }

  /* Rescale output, and quantize to 8 bits */
  scl = 254.0/(2.0*(*maxAmp)); /* 254 is used so response is symmetric about 0 */
  for(iy=0; iy<ny; iy++)
   for(ix=0; ix<nx; ix++) {
    val = image[iy*nx + ix];
    quant[iy*nx + ix] = colour( val, - (*maxAmp), scl);
   }
 }

  
void scale_image(image, nx, ny, quant)
 float *image;
 unsigned char *quant;
 int nx,ny;
 {
  float val, dc, scl, min_val, max_val;
  float im_min, im_max;
  int i,ix,iy;
  unsigned char colour();
 
  min_val = image[0];
  max_val = min_val;
  for(iy=0; iy<ny; iy++) 
   for(ix=0; ix<nx; ix++) {
     min_val = min(min_val, image[iy*nx + ix]);
     max_val = max(max_val, image[iy*nx + ix]);
   }

  if (min_val == max_val)
    scl = 1.0;
  else
    scl = 255.0 / (max_val - min_val);

  for(iy=0; iy<ny; iy++)
   for(ix=0; ix<nx; ix++) {
    val = image[iy*nx + ix];
    quant[iy*nx + ix] = colour(val, min_val, scl);
   }
 }

unsigned char colour( v, minV, scl)
 float v, minV, scl;
 {
  float val;
  int iVal;
  val = (v-minV)*scl;
  iVal = ROUND(val);
  iVal = min( 255, iVal);
  iVal = max(0, iVal);
  return((unsigned char) iVal);
 }
 

