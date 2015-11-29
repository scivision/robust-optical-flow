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

File: warp.c

Synopsis: Warp an image based on the flow filed (u,v).

Status: This is EXPERIMENTAL code.  There is no guarantee.

Details: This does bi-linear interpolation.

************************************************/

# include       <stdio.h>
# include       <math.h>
# include       <string.h>

#include "robust.h"

#define default 0.0

/*
Backwards warp
*/


/*
Backwards warp of "image" by (u,v).  The result is stored in "warped".
Pixel values are determined by linearly interpolating the warped image.
While this is fairly fast, a better interpolation may be desirable.
*/
warp_image(image, warped, u, v, nx, ny)

     float *image, *warped, *u, *v;
     int nx, ny;
{
  float linear_interp(), new_i, new_j, remainder_i, remainder_j;
  int index, i, j, floor_i, floor_j, ceil_i, ceil_j;

  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      index = (i*nx) + j;
      /* fprintf(stderr, "%d %d   ", i, j); */
      new_i = ((float) i) - v[index];
      new_j = ((float) j) - u[index];

      if (new_j < 0 || new_j > (nx - 1) || new_i < 0 || new_i > (ny - 1)) 
	warped[index] = default;
      else{
	floor_i = ((int) floor(new_i)) % ny;
	ceil_i = ((int) ceil(new_i)) % ny;
	remainder_i = new_i - ((float) floor_i);
	floor_j = ((int) floor(new_j)) % nx;
	ceil_j = ((int) ceil(new_j)) % nx;
	remainder_j = new_j - ((float) floor_j);
      
	warped[index] = 
	  linear_interp
	  (linear_interp(image[(floor_i*nx)+floor_j],
			 image[(ceil_i*nx)+floor_j],
			 remainder_i),
	   linear_interp(image[(floor_i*nx)+ceil_j],
			 image[(ceil_i*nx)+ceil_j],
			 remainder_i),
	   remainder_j);
      }
    }
  }
}


 

