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

File: image-io.c

Synopsis: Load and save images.

Status: This is EXPERIMENTAL code.  There is no guarantee.


Details: My images have the following format.

  image.bin  --  byte file with no header
  image.hdr  --  contains 
      
      <rows> <cols> <min val> <max val> <scale>


************************************************/

# include <stdio.h>
# include <math.h>
# include <string.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>


#include "robust.h"

#define ROWS 64
#define COLS 64

/*
Read in image from path and store in image.
This function reads:
  1) images saved by save_image where image name is path
  2) straight byte images named path.  This is of limited use
at the moment since the number of rows and cols of these images
is compiled in at 64X64.
*/

float *load_image(path, image)
     
     char *path;
     float *image;
{
  unsigned char *temp;
  char dataIn[MAXLEN], headIn[MAXLEN], buff[MAXLEN];
  int index, fdIn, i, j,  i0, sizeInput, sizeImage,err;
  int nx, ny;
  float min, max, scale;
  char *datatail=".bin", *headtail=".hdr";

  strcpy(headIn, "");
  strcpy(dataIn, "");
  strcat(strcat(headIn, path), headtail);
  printf("headIn: %s\n", headIn);
  if ( (fdIn = open(headIn,0)) == -1 ) {
    nx = ROWS;
    ny = COLS;
    scale = 1.0;
    min = 0.0;
  }
  else {
    err=read(fdIn, buff, MAXLEN);
    sscanf(buff, "%d %d %f %f %f", &nx, &ny, &min, &max, &scale);
    printf("nx: %d\n", nx);
    printf("ny: %d\n", ny);
  }

  strcat(strcat(dataIn, path), datatail);
  printf("dataIn: %s,  path: %s  \n\n", dataIn, path); 
  if ( (fdIn = open(dataIn,0)) == -1 ) {
    strcpy(dataIn, path);
    if ( (fdIn = open(path,0)) == -1 ) {
      error("Can't open input files, load_image", dataIn);
    }
  }

  sizeInput = nx * ny * sizeof( char );
  sizeImage = nx * ny * sizeof( float );
  if ((image = (float *) malloc((size_t) sizeImage)) == NULL)
    error(" Unable to allocate memory for ", "image");
  if ((temp = (unsigned char *) malloc((size_t) sizeInput)) == NULL)
    error(" Unable to allocate memory for ", "temp");
  if ( sizeInput != (i0 = read(fdIn,(char *) temp, sizeInput)) ) {
    fprintf(stderr,"error: unexpected end of file on %s\n", dataIn);
    exit(1);
    }

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      index = (i*nx) + j;
      image[index] = (((float) temp[index]) / scale) + (float) min;
    }
  }
  free(temp);
  close(fdIn);
  return(image);
}

void load_image2(path, image)
     
     char *path;
     float *image;
{
  unsigned char *temp;
  char dataIn[MAXLEN], headIn[MAXLEN], buff[MAXLEN];
  int index, fdIn, i, j, i0, sizeInput, sizeImage,err;
  int nx, ny;
  float min, max, scale;
  char *datatail=".bin", *headtail=".hdr";

  strcpy(headIn, "");
  strcpy(dataIn, "");
  strcat(strcat(headIn, path), headtail);
  printf("headIn: %s\n", headIn);
  if ( (fdIn = open(headIn,0)) == -1 ) {
    nx = ROWS;
    ny = COLS;
    scale = 1.0;
    min = 0.0;
  }
  else {
    err=read(fdIn, buff, MAXLEN);
    sscanf(buff, "%d %d %f %f %f", &nx, &ny, &min, &max, &scale);
    fprintf(stderr, "nx: %d\n", nx);
    fprintf(stderr, "ny: %d\n", ny);
  }

  strcat(strcat(dataIn, path), datatail);
  printf("dataIn: %s,  path: %s  \n\n", dataIn, path); 
  if ( (fdIn = open(dataIn,0)) == -1 ) {
    strcpy(dataIn, path);
    if ( (fdIn = open(path,0)) == -1 ) {
      error("Can't open input files, load_image ", dataIn);
    }
  }

  sizeInput = nx * ny * sizeof( char );
  sizeImage = nx * ny * sizeof( float );
  if ((temp = (unsigned char *) malloc((size_t) sizeInput)) == NULL)
    error(" Unable to allocate memory for ", "temp");
  if ( sizeInput != (i0 = read(fdIn,(char *) temp, sizeInput)) ) {
    fprintf(stderr,"error: unexpected end of file on %s\n", dataIn);
    exit(1);
    }

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      index = (i*nx) + j;
      image[index] = (((float) temp[index]) / scale) + (float) min;
    }
  }
  free(temp);
  close(fdIn);
}

/*
Save an image as raw 8-bit byte data with a header to allow it
to be reconstructed by load_image.
*/
void save_image(path, image, nx, ny)
     
     char *path;
     float *image;
     int nx, ny;
{
  char dataOut[MAXLEN], headOut[MAXLEN];
  int index, fdOut, iy, ix,sizeOutput, err;
  float min_val, max_val, scale, val;
  char *datatail=".bin", *headtail=".hdr";
  unsigned char *quant;
  FILE *outfile;
 
  strcpy(headOut, "");
  strcpy(dataOut, "");
  min_val = image[0];
  max_val = min_val;
  for(iy=0; iy<ny; iy++) 
    for(ix=0; ix<nx; ix++) {
      index = (int)((iy * nx) + ix);
      min_val = min(min_val, image[index]);
      max_val = max(max_val, image[index]);
    }
  
  if (min_val == max_val)
    scale = 1.0;
  else
    scale = 255.0 / (max_val - min_val);
      
  strcat(strcat(headOut, path), headtail);
/*  fprintf(stderr, "headOut: %s\n", headOut); */
  
  outfile = fopen(headOut, "w");
  if (outfile == NULL) {
    fprintf(stderr, "outfile: NULL\n");
  }
  fprintf(stderr, "%d %d %f %f %f\n", nx, ny, min_val, max_val, scale);
/*  fprintf(stderr, "writing header: %s\n", headOut); */
  fprintf(outfile, "%d %d %f %f %f", nx, ny, min_val, max_val, scale);
  fclose(outfile);

  strcat(strcat(dataOut, path), datatail);
/*  fprintf(stderr, "opening dataOut: %s\n", dataOut);  */
  if ( (fdOut = creat(dataOut,0644)) == -1 ) 
      error("Can't open output file, (in save_image)", dataOut);

/*  fprintf(stderr, "allocating\n");  */
  sizeOutput = nx * ny * sizeof( char );

  if ((quant = (unsigned char *) malloc((size_t) sizeOutput)) == NULL)
    error(" Unable to allocate memory for (in save_image)", "quant");


/*  fprintf(stderr, "quantizing\n");  */
  for(iy=0; iy<ny; iy++)
   for(ix=0; ix<nx; ix++) {
     index = (iy * nx) + ix;
     val = (image[index] - min_val) * scale;
     quant[index] = ROUND(val);
   }

/*  fprintf(stderr, "writing\n");  */
  err=write(fdOut, (char *) quant, sizeOutput);

/*  fprintf(stderr, "done\n");  */
  free(quant);
  close(fdOut);
}

void save_pgm(path, image, nx, ny)
     
     char *path;
     float *image;
     int nx, ny;
{
  char dataOut[MAXLEN], headOut[MAXLEN];
  int index, iy, ix, sizeOutput;
  float min_val, max_val, scale, val;
  char strcompress[2*MAXLEN];
  unsigned char *quant;
  FILE *outfile;
  int HEADER;
 
  strcpy(headOut, "");
  strcpy(dataOut, "");
  min_val = image[0];
  max_val = min_val;
  for(iy=0; iy<ny; iy++) 
    for(ix=0; ix<nx; ix++) {
      index = (int)((iy * nx) + ix);
      min_val = min(min_val, image[index]);
      max_val = max(max_val, image[index]);
    }
  
  if (min_val == max_val)
    scale = 1.0;
  else
    scale = 255.0 / (max_val - min_val);
      
  strcat(dataOut, path);
  outfile = fopen(dataOut, "wb");
  if (outfile == NULL) {
    fprintf(stderr, "outfile: NULL\n");
  }

  sizeOutput = nx * ny * sizeof( char );
  sprintf(strcompress,"P5\n%d %d\n255\n",nx,ny);
  HEADER=strlen(strcompress);
  sizeOutput+=HEADER;
  if ((quant = (unsigned char *) malloc((size_t) sizeOutput)) == NULL)
    error(" Unable to allocate memory for (in save_image)", "quant");
  strncpy(quant,strcompress,HEADER);

/*  fprintf(stderr, "quantizing\n");  */
  for(iy=0; iy<ny; iy++)
   for(ix=0; ix<nx; ix++) {
     index = (iy * nx) + ix;
     val = (image[index] - min_val) * scale;
     quant[index+HEADER] = min(max(ROUND(val),0),255);
   }

/*  fprintf(stderr, "writing\n");  */
  fwrite((char *) quant, sizeOutput, 1, outfile);

/*  fprintf(stderr, "done\n");  */
  free(quant);
  fclose(outfile);
}


void save_image_no_scale(path, image, nx, ny)
     
     char *path;
     float *image;
     int nx, ny;
{
  char dataOut[MAXLEN], headOut[MAXLEN];
  int index, iy, ix, sizeOutput;
  char *datatail=".bin";
  unsigned char *quant;
  FILE *outfile;
 
  strcpy(headOut, "");
  strcpy(dataOut, "");

  strcat(strcat(dataOut, path), datatail);

  /*  fprintf(stderr, "opening dataOut: %s\n", dataOut);  */
  outfile = fopen(dataOut, "wb");
  if (outfile == NULL) {
    fprintf(stderr, "outfile: NULL\n");
  }

  /*  fprintf(stderr, "allocating\n");  */
  sizeOutput = nx * ny * sizeof( char );

  if ((quant = (unsigned char *) malloc((size_t) sizeOutput)) == NULL)
    error(" Unable to allocate memory for (in save_image)", "quant");


  /*  fprintf(stderr, "quantizing\n");  */
  for(iy=0; iy<ny; iy++)
   for(ix=0; ix<nx; ix++) {
     index = (iy * nx) + ix;
     quant[index] = min(max(ROUND(image[index]),0),255);
   }

  /*  fprintf(stderr, "writing\n");  */
  fwrite((char *) quant, sizeOutput, 1, outfile);

  /*  fprintf(stderr, "done\n");  */
  free(quant);
  fclose(outfile);
}

void save_pgm_no_scale(path, image, nx, ny)
     
     char *path;
     float *image;
     int nx, ny;
{
  char dataOut[MAXLEN];
  int index, iy, ix, sizeOutput;
  char strcompress[2*MAXLEN];
  unsigned char *quant;
  FILE *outfile;
  int HEADER;
 
  strcpy(dataOut, "");
  strcat(dataOut, path);

  outfile = fopen(dataOut, "wb");
  if (outfile == NULL) {
    fprintf(stderr, "outfile: NULL\n");
  }

  sizeOutput = nx * ny * sizeof( char );
  sprintf(strcompress,"P5\n%d %d\n255\n",nx,ny);
  HEADER=strlen(strcompress);
  sizeOutput+=HEADER;
  if ((quant = (unsigned char *) malloc((size_t) sizeOutput)) == NULL)
    error(" Unable to allocate memory for (in save_image)", "quant");
  strncpy(quant,strcompress,HEADER);

/*  fprintf(stderr, "quantizing\n");  */
  for(iy=0; iy<ny; iy++)
   for(ix=0; ix<nx; ix++) {
     index = (iy * nx) + ix;
     quant[index+HEADER] = min(max(ROUND(image[index]),0),255);
   }

/*  fprintf(stderr, "writing\n");  */
  fwrite((char *) quant, sizeOutput, 1, outfile);

/*  fprintf(stderr, "done\n");  */
  free(quant);
  fclose(outfile);
}

void save_pgm_scale(path, image, nx, ny, min_val, max_val)
     
     char *path;
     float *image, min_val, max_val;
     int nx, ny;
{
  char dataOut[MAXLEN], headOut[MAXLEN];
  int index, fdOut, iy, ix, sizeOutput, err;
  float scale, val;
  char strcompress[2*MAXLEN];
  unsigned char *quant;
  int HEADER;
 
  strcpy(headOut, "");
  strcpy(dataOut, "");
/*
  for(iy=0; iy<ny; iy++) {
    for(ix=0; ix<nx; ix++) {
      index = (int)((iy * nx) + ix);
      if (image[index] > max_val)
	image[index] = max_val;
      else if (image[index] < min_val)
	image[index] = min_val;
    }
  }
*/
  if (min_val == max_val)
    scale = 1.0;
  else
    scale = 255.0 / (max_val - min_val);
      
  strcat(dataOut, path);
  if ( (fdOut = creat(dataOut,0644)) == -1 ) 
      error("Can't open output file, (in save_image)", dataOut);

  sizeOutput = nx * ny * sizeof( char );
  sprintf(strcompress,"P5\n%d %d\n255\n",nx,ny);
  HEADER=strlen(strcompress);
  sizeOutput+=HEADER;
  if ((quant = (unsigned char *) malloc((size_t) sizeOutput)) == NULL)
    error(" Unable to allocate memory for (in save_image)", "quant");
  strncpy(quant,strcompress,HEADER);

/*  fprintf(stderr, "quantizing\n");  */
  for(iy=0; iy<ny; iy++)
   for(ix=0; ix<nx; ix++) {
     index = (iy * nx) + ix;
     val = (image[index] - min_val) * scale;
     quant[index+HEADER] = min(max(ROUND(val),0),255);
   }

/*  fprintf(stderr, "writing\n");  */
 err= write(fdOut, (char *) quant, sizeOutput);

/*  fprintf(stderr, "done\n");  */
  free(quant);
  close(fdOut);
}


void save_image_scale(path, image, nx, ny, imin, imax)
     
     char *path;
     float *image, imin, imax;
     int nx, ny;
{
  char dataOut[MAXLEN], headOut[MAXLEN];
  int index, fdOut, iy, ix, sizeOutput,err;
  float scale, val;
  char *datatail=".bin", *headtail=".hdr";
  unsigned char *quant;
  FILE *outfile;
 
  strcpy(headOut, "");
  strcpy(dataOut, "");

/*
  for(iy=0; iy<ny; iy++) {
    for(ix=0; ix<nx; ix++) {
      index = (int)((iy * nx) + ix);
      if (image[index] > 255.0)
	image[index] = 255.0;
      else if (image[index] < 0.0)
	image[index] = 0.0;
    }
  }
*/

  for(iy=0; iy<ny; iy++) {
    for(ix=0; ix<nx; ix++) {
      index = (int)((iy * nx) + ix);
      if (image[index] > imax)
	image[index] = imax;
      else if (image[index] < imin)
	image[index] = imin;
    }
  }
  if (imin == imax)
    scale = 1.0;
  else
    scale = 255.0 / (imax - imin);
   
  strcat(strcat(headOut, path), headtail);
  
  outfile = fopen(headOut, "w");
  if (outfile == NULL) {
    fprintf(stderr, "outfile: NULL\n");
  }
  fprintf(stderr, "%d %d %f %f %f\n", nx, ny, imin, imax, scale);
  fprintf(outfile, "%d %d %f %f %f", nx, ny, imin, imax, scale);
  fclose(outfile);

  strcat(strcat(dataOut, path), datatail);
  if ( (fdOut = creat(dataOut,0644)) == -1 ) 
      error("Can't open output file, (in save_image)", dataOut);

  sizeOutput = nx * ny * sizeof( char );

  if ((quant = (unsigned char *) malloc((size_t) sizeOutput)) == NULL)
    error(" Unable to allocate memory for (in save_image)", "quant");
/*
  for(iy=0; iy<ny; iy++)
   for(ix=0; ix<nx; ix++) {
     index = (iy * nx) + ix;
     val = image[index];
     quant[index] = ROUND(val);
   }
*/
  for(iy=0; iy<ny; iy++)
   for(ix=0; ix<nx; ix++) {
     index = (iy * nx) + ix;
     val = (image[index] - imin) * scale;
     quant[index] = ROUND(val);
   }

  err=write(fdOut, (char *) quant, sizeOutput);

  free(quant);
  close(fdOut);
}
 
void save_image_float(path, image, nx, ny)
     
     char *path;
     float *image;
     int nx, ny;
{
  char dataOut[MAXLEN], headOut[MAXLEN];
  int index, fdOut, iy, ix, sizeOutput, err;
  float min_val, max_val;
  char *datatail=".flt", *headtail=".hdr";
  FILE *outfile;
 
  strcpy(headOut, "");
  strcpy(dataOut, "");
  min_val = image[0];
  max_val = min_val;
  for(iy=0; iy<ny; iy++) 
    for(ix=0; ix<nx; ix++) {
      index = (int)((iy * nx) + ix);
      min_val = min(min_val, image[index]);
      max_val = max(max_val, image[index]);
    }
        
  strcat(strcat(headOut, path), headtail);
  
  outfile = fopen(headOut, "w");
  if (outfile == NULL) {
    fprintf(stderr, "outfile: NULL\n");
  }
  fprintf(stderr, "%d %d %f %f %f\n", nx, ny, min_val, max_val, 1.0);
  fprintf(outfile, "%d %d %f %f %f", nx, ny, min_val, max_val, 1.0);
  fclose(outfile);

  sizeOutput=nx*ny*sizeof(float);

  strcat(strcat(dataOut, path), datatail);
/*  fprintf(stderr, "opening dataOut: %s\n", dataOut);  */
  if ( (fdOut = creat(dataOut,0644)) == -1 ) 
      error("Can't open output file, (in save_image)", dataOut);

/*  fprintf(stderr, "writing\n");  */
  err=write(fdOut, (float *) image , sizeOutput);

  close(fdOut);
}


