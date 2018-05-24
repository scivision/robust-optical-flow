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

File: gnc.c

Synopsis: Perform graduated non-convexity to recover
          optical flow for two images.

Status: This is EXPERIMENTAL code.  There is no guarantee.
        I'm not a C hacker, so if I am doing stupid things
	please enlighten me.

Details:

This is the main program (complied into a file called "gnc") for
estimating optical flow.  It takes the following required inputs from
standard input:

  gnc <first-frame> <second-frame> <max-pyr> <min-pyr> <in-path> <out-path>

where:
 
  * first-frame and second-frame are integer frame numbers (assuming
    that your image sequences is numbered).

  * max and min pyramid really could be a single number (max - min)
    which says how many layers you want in the pyramid.  So for
    a three level pyramid, you would have max = 4, min = 2, for
    example.  

  * in-path and out-path give the paths for the input and output files.
    Input files should be byte files with no header.  The frame number
    will be concatenated to the path to give <inpath><frame> as the
    input file name.

    The output files saved are <outpath>-*-<frame>, where * is
       "u": horizontal flow component
       "v": vertical flow
       "discont": spatial discontinuities

There are a number of optional command line arguments:

   -l1, -l2, -l3: These weight the data, spatial, and temporal
                  terms respectively.  Float.  
		  Usually l1=l2=l3=1.0.

   -S1, -S2, -S3: These are the INTIAL scale parameters for the
                  robust estimator (for the data, spatial, and 
		  temporal terms respectively).  These should be
		  chosen large enough to ensure that the objective
		  function is convex.
	       
   -s1, -s2, -s3: These are the FINAL scale parameters for the
                  robust estimator (for the data, spatial, and 
		  temporal terms respectively).  These should be
		  chosen to reflect what is considered an outlier.

   -f:  A float that indicates the rate that S2, S2, and S3 are 
        decreased each frame.  So S1 at time t is S1 * "f" at t-1.

   -nx, -ny:  These are the x and y dimensions of the images (integer).

   -F: If 1, then the images are prefiltered with a Laplacian, otherwise
       they are not.

   -w: The "overrelaxation" parameter (0 < w < 2).  Typically
       w = 1.995.

   -iters:  The number of iterations of relaxation per frame.
            Usually between 1 and 20 for most sequences.  If you
	    have just a two image sequence this will probably be
	    much higher.

   -end: If your input files don't look like <in-path><frame>
         but have a tail, then this is the value of end.  So, if
	 end is specified the input path is <in-path><frame><end>.

Example:

  gnc 0 1 4 4 
      /usr/people/black/data/pepsi/pepsi- 
      foo- 
      -l1 1.0 -l2 1.0 
      -S1 10.0 -S2 1.0 
      -s1 1.0 -s2 0.05 
      -nx 64 -ny 64 
      -F 0 -w 1.995 
      -iters 50

************************************************/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "robust.h"

int main(int argc, char **argv)
{
  float lambda1=1.0, lambda2=1.0, lambda3=0.0, l3=0.05;
  float s1=1.0, s2=1.0, s3=1.0;
  float s1_end=1.0, s2_end=1.0, s3_end=1.0;
  float factor=0.75;
//  float s1_rate, s2_rate, s3_rate;
  float omega=1.95;
  char *inpath, *outpath, *flag, *end;
  int index, from, to, by=1, max_level, min_level;
  int iters = 5, nx=128, ny=128, filter=1, skip=0;
  int stages = 1, st;
  float s1_factor, s2_factor;

//  unsigned char *uOut, *vOut;
  float *dx, *dy, *dt, *u, *v;
  float *u_scale, *v_scale, *du, *dv;
  float *prev_u, *prev_v, *prev_du, *prev_dv;
  float *prev, *curr, *err;//*lap1, *lap2, 
  float *discont, *outliers, *sigma1, *sigma2, *sigma3;
  int sizeDeriv, sizeOutput;
  int i, j;
  char fnCurr[MAXLEN];
  char fnOutu[MAXLEN], fnOutv[MAXLEN], fnOutdiscont[MAXLEN];
  float image_abs_max(), *load_image(); 
  double atof(); 
  char *itoa();
  void reverse();

  if (argc==1){
  fprintf(stderr,"you must specify command-line arguments, or use the Python wrapper\n");
  return EXIT_FAILURE;
  }


  end = "";
  from = atoi(*++argv);
  to = atoi(*++argv);
  fprintf(stderr, "from: %d, to: %d\n", from, to);

  max_level = atoi(*++argv);
  min_level = atoi(*++argv);
  fprintf(stderr, "max_level: %d, min_level: %d\n", max_level, min_level);

  inpath = *++argv;
  outpath = *++argv;
  fprintf(stderr, "in: %s, out: %s\n", inpath, outpath);
  
  argc = argc - 6;
  while (--argc > 0 && (*++argv)[0] == '-') {
    flag = ++argv[0];
    if (strcmp(flag, "l1") == 0) {
      lambda1 = (float) atof(*++argv);
      printf("l1: %f\n", lambda1);
    }
    else if (strcmp(flag, "l2") == 0) {
      lambda2 =  (float) atof(*++argv);
      printf("l2: %f\n", lambda2);
    }
    else if (strcmp(flag, "l3") == 0) {
      lambda3 =  (float) atof(*++argv);
      printf("l3: %f\n", lambda3);
    }
      /* sigma start */
    else if (strcmp(flag, "S1") == 0) {
      s1 =  (float) atof(*++argv) / ROOT2;
      printf("S1: %f\n", s1);
    }
    else if (strcmp(flag, "S2") == 0) {
      s2 =  (float) atof(*++argv) / ROOT2;
      printf("S2: %f\n", s2);
    }
    else if (strcmp(flag, "S3") == 0) {
      s3 =  (float) atof(*++argv) / ROOT2;
      printf("S3: %f\n", s3);
    }
      /*  sigma end */
    else if (strcmp(flag, "s1") == 0) {
      s1_end =  (float) atof(*++argv) / ROOT2;
      printf("s1: %f\n", s1_end);
    }
    else if (strcmp(flag, "s2") == 0) {
      s2_end =  (float) atof(*++argv) / ROOT2;
      printf("s2: %f\n", s2_end);
    }
    else if (strcmp(flag, "s3") == 0) {
      s3_end =  (float) atof(*++argv) / ROOT2;
      printf("s3: %f\n", s3_end);
    }
      /* other */
    else if (strcmp(flag, "by") == 0) {
      by = atoi(*++argv);
      fprintf(stderr, "by: %d\n", by);
    }
    else if (strcmp(flag, "w") == 0) {
      omega =  (float) atof(*++argv);
      fprintf(stderr, "omega: %f\n", omega);
    }
    else if (strcmp(flag, "nx") == 0) {
      nx = atoi(*++argv);
      printf("nx: %d\n", nx);
    }
    else if (strcmp(flag, "ny") == 0) {
      ny = atoi(*++argv);
      printf("ny: %d\n", ny);
    }
    else if (strcmp(flag, "iters") == 0) {
      iters = atoi(*++argv);
      printf("iters: %d\n", iters);
    }
    else if (strcmp(flag, "stages") == 0) {
      stages = atoi(*++argv);
      printf("stages: %d\n", stages);
    }
    else if (strcmp(flag, "end") == 0) {
      end = *++argv;
      printf("end: %s\n", end);
    }
    else if (strcmp(flag, "f") == 0) {
      factor =  (float) atof(*++argv);
      printf("scale factor: %f\n", factor);
    }
    else if (strcmp(flag, "F") == 0) {
      filter = atoi(*++argv);
      printf("filter: %d\n", filter);
    }
    else if (strcmp(flag, "skip") == 0) {
      skip = atoi(*++argv);
      printf("skip: %d\n", skip);
    }
    argc = argc -1;
  }


  printf("allocating memory\n");

  sizeDeriv = nx * ny * sizeof( float );
  sizeOutput = nx * ny * sizeof( char );

  if ((err = (float *) malloc((size_t) sizeDeriv)) == NULL)
      error(" Unable to allocate memory for ", "err");
  if ((discont = (float *) malloc((size_t) sizeDeriv)) == NULL)
      error(" Unable to allocate memory for ", "discont");
  if ((outliers = (float *) malloc((size_t) sizeDeriv)) == NULL)
      error(" Unable to allocate memory for ", "outliers");
  if ((sigma1 = (float *) malloc((size_t) sizeDeriv)) == NULL)
      error(" Unable to allocate memory for ", "sigma1");
  if ((sigma2 = (float *) malloc((size_t) sizeDeriv)) == NULL)
      error(" Unable to allocate memory for ", "sigma2");
  if ((sigma3 = (float *) malloc((size_t) sizeDeriv)) == NULL)
      error(" Unable to allocate memory for ", "sigma3");
  if ((prev = (float *) malloc((size_t) sizeDeriv)) == NULL)
      error(" Unable to allocate memory for ", "prev");
  if ((curr = (float *) malloc((size_t) sizeDeriv)) == NULL)
      error(" Unable to allocate memory for ", "curr");
  if ((u = (float *) malloc((size_t) sizeDeriv)) == NULL)
      error(" Unable to allocate memory for ", "u");
  if ((v = (float *) malloc((size_t) sizeDeriv)) == NULL)
      error(" Unable to allocate memory for ", "v");
  if ((du = (float *) malloc((size_t) sizeDeriv)) == NULL)
      error(" Unable to allocate memory for ", "du");
  if ((dv = (float *) malloc((size_t) sizeDeriv)) == NULL)
      error(" Unable to allocate memory for ", "dv");
  if ((prev_u = (float *) malloc((size_t) sizeDeriv)) == NULL)
      error(" Unable to allocate memory for ", "prev_u");
  if ((prev_v = (float *) malloc((size_t) sizeDeriv)) == NULL)
      error(" Unable to allocate memory for ", "prev_v");
  if ((prev_du = (float *) malloc((size_t) sizeDeriv)) == NULL)
      error(" Unable to allocate memory for ", "prev_du");
  if ((prev_dv = (float *) malloc((size_t) sizeDeriv)) == NULL)
      error(" Unable to allocate memory for ", "prev_dv");
  if ((u_scale = (float *) malloc((size_t) sizeDeriv)) == NULL)
      error(" Unable to allocate memory for ", "u_scale");
  if ((v_scale = (float *) malloc((size_t) sizeDeriv)) == NULL)
      error(" Unable to allocate memory for ", "v_scale");
  if ((dx = (float *) malloc((size_t) sizeDeriv)) == NULL)
      error(" Unable to allocate memory for ", "dx");
  if ((dy = (float *) malloc((size_t) sizeDeriv)) == NULL)
      error(" Unable to allocate memory for ", "dy");
  if ((dt = (float *) malloc((size_t) sizeDeriv)) == NULL)
      error(" Unable to allocate memory for ", "dt");


  /* initialize u, v */
  /* this should be fixed to allow prev_u and prev_v to be
     entered from a file
     */
  fprintf(stderr, "initializing u,v,du,dv\n");
  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      index = (i * nx) + j;
      prev[index] = 0.0;
      curr[index] = 0.0;
      u[index] = 0.0;
      v[index] = 0.0;
      du[index] = 0.0;
      dv[index] = 0.0;
      prev_u[index] = 0.0;
      prev_v[index] = 0.0;
      prev_du[index] = 0.0;
      prev_dv[index] = 0.0;
      outliers[index] = 0.0;
      sigma1[index] = s1;
      sigma2[index] = s2;
      sigma3[index] = s3;
    }
  }


  fprintf(stderr, "reading first image ");
  strcpy(fnCurr,inpath);
  strcat(strcat(fnCurr, itoa(from)),end);

  read_image_strip_bytes(prev, fnCurr, nx, ny,skip);

  strcpy(fnCurr,inpath);
  strcat(strcat(fnCurr, itoa(to)),end);

  read_image_strip_bytes(curr, fnCurr, nx, ny,skip);

  /* no need for the temporal constraint */
  l3 = 0.0;

  s1_factor = pow((s1_end/s1), (1.0/ (float)(stages -1)));
  s2_factor = pow((s2_end/s2), (1.0/ (float)(stages -1)));
  for(st=0; st<stages; st++) {
    fprintf(stderr, "Stage: %d\n  s1: %f\n  s2: %f\n\n",
	    st, s1, s2);
    /* update flow */
    pyramid_sor(prev, curr, max_level, min_level, iters, omega, u, v, 
		prev_u, prev_v, du, dv, lambda1, lambda2,
		lambda3, sigma1, sigma2, sigma3, nx, ny, dx, dy, dt, err,
		u_scale, v_scale, filter);

    /* save results */
    strcpy(fnOutu,outpath);
    strcat(strcat(strcat(fnOutu, "u-"), itoa(st)),".pgm");
    strcpy(fnOutv,outpath);
    strcat(strcat(strcat(fnOutv, "v-"), itoa(st)),".pgm");
    save_pgm(fnOutu, u, nx, ny);
    save_pgm(fnOutv, v, nx, ny);

    strcpy(fnOutu,outpath);
    strcat(strcat(fnOutu, "u-"), itoa(st));
    strcpy(fnOutv,outpath);
    strcat(strcat(fnOutv, "v-"), itoa(st));
    save_image_float(fnOutu, u, nx, ny);
    save_image_float(fnOutv, v, nx, ny);

    data_disconts(prev, curr, u, v, sigma1, nx, ny, err, dt, outliers);
    strcpy(fnOutdiscont,outpath);
    strcat(strcat(strcat(fnOutdiscont, "data-"), itoa(st)),".pgm");
    save_pgm(fnOutdiscont, outliers, nx, ny);

    spatial_disconts(u, v, sigma2, nx, ny, discont); 
    strcpy(fnOutdiscont,outpath);
    strcat(strcat(strcat(fnOutdiscont, "spatial-"), itoa(st)),".pgm");
    save_pgm(fnOutdiscont, discont, nx, ny);

    warp_image(prev, err, u, v, nx, ny);
    strcpy(fnOutdiscont,outpath);
    strcat(strcat(strcat(fnOutdiscont, "stable-"), itoa(st)),".pgm");
    save_pgm_no_scale(fnOutdiscont, err, nx, ny);

    s1 = max((s1 * s1_factor), s1_end);
    s2 = max((s2 * s2_factor), s2_end);
    for(i=0;i<ny;i++){
      for(j=0;j<nx;j++){
	index = (i*nx) +j;
	sigma1[index] = s1;
	sigma2[index] = s2;
      }
    }

    copy_image(prev_u, u, nx, ny);
    copy_image(prev_v, v, nx, ny);
  }

}

